import socket
import time
import threading

import message


class PlutoController:
    def __init__(self, target_ip: str, target_port: int = 4242):
        self.target_addr = (target_ip, target_port)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(1.0)

        self.session_token = 0
        self.sequence_number = 0
        self.is_connected = False

        self._lock = threading.Lock()
        self._stop_event = threading.Event()
        self.received_messages = []

    def connect(self) -> bool:
        """Performs the handshake to acquire a session token."""
        # Initial packet with SESSION_REQUEST_TOKEN (0)
        handshake_packet = message.UDPPacket(
            session_token=0,
            sequence_number=self.sequence_number,
            timestamp=int(time.time() * 1000),
        )

        try:
            self.sock.sendto(handshake_packet.pack(), self.target_addr)
            data, _ = self.sock.recvfrom(2048)

            response = message.UDPPacket.unpack(data)
            self.session_token = response.session_token
            self.is_connected = True

            # Start background listener for ACKs and sensor data
            threading.Thread(target=self._listen_loop, daemon=True).start()
            return True
        except (socket.timeout, ValueError) as e:
            print(f"Connection failed: {e}")
            return False

    def send_messages(self, messages: list[message.Message]):
        """Wraps messages in a UDPPacket and sends them."""
        if not self.is_connected:
            return

        with self._lock:
            self.sequence_number += 1
            packet = message.UDPPacket(
                session_token=self.session_token,
                sequence_number=self.sequence_number,
                timestamp=int(time.time() * 1000),
                messages=messages,
            )
            self.sock.sendto(packet.pack(), self.target_addr)

    def _listen_loop(self):
        """Background thread to handle incoming packets from the ESP32."""
        self.sock.settimeout(0.5)
        while not self._stop_event.is_set():
            try:
                data, _ = self.sock.recvfrom(2048)
                packet = message.UDPPacket.unpack(data)

                with self._lock:
                    for m in packet.messages:
                        self.received_messages.append(m)
            except socket.timeout:
                continue
            except Exception as e:
                print(f"Listener error: {e}")

    def disconnect(self):
        self._stop_event.set()
        self.sock.close()
