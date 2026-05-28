from collections import deque
import socket
import time
import threading
import logging

from . import message
from zeroconf import Zeroconf, ServiceListener, ServiceBrowser


class PlutoController:
    def __init__(self, target_ip: str = "", target_port: int = 4242):
        self.target_addr = (target_ip, target_port) if target_ip else None
        self.default_port = target_port

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(1.0)

        self.session_token = 0
        self.sequence_number = 0
        self.is_connected = False

        self._lock = threading.Lock()
        self._stop_event = threading.Event()
        self.received_messages = deque(maxlen=100)

    def scan_for_robot(self, timeout_seconds: float = 3.0) -> bool:
        """Scans the network for the ESP32 via mDNS and updates target_addr."""
        found_event = threading.Event()
        discovered_ip = None
        discovered_port = None

        class PlutoListener(ServiceListener):
            def add_service(self, zc: Zeroconf, type_: str, name: str) -> None:
                nonlocal discovered_ip, discovered_port
                info = zc.get_service_info(type_, name)
                if info and info.parsed_addresses():
                    discovered_ip = info.parsed_addresses()[0]
                    discovered_port = info.port
                    found_event.set()

            def update_service(self, zc: Zeroconf, type_: str, name: str) -> None:
                pass

            def remove_service(self, zc: Zeroconf, type_: str, name: str) -> None:
                pass

        zc = Zeroconf()
        browser = ServiceBrowser(zc, "_pluto._udp.local.", PlutoListener())

        found_event.wait(timeout_seconds)
        zc.close()

        if discovered_ip:
            self.target_addr = (discovered_ip, discovered_port or self.default_port)
            logging.info(f"Discovered Pluto at {self.target_addr}")
            return True

        logging.warning("Failed to discover Pluto via mDNS.")
        return False

    def send_heartbeat(self):
        """Sends an INFO_REQUEST_IS_ALIVE message to prevent session timeout."""
        msg = message.Message(
            family=message.MessageFamilyKind.KIND_INFO,
            kind=message.MessageInfoKind.INFO_REQUEST_IS_ALIVE,
            event_clock_or_duration=0,
        )
        self.send_messages([msg])

    def send_behavior(
        self, behavior_kind: message.MessageBehaviorKind, duration_ms: int = 0
    ):
        """Sends a high-level behavior command."""
        msg = message.create_behavior(behavior_kind, duration_ms)
        self.send_messages([msg])

    def get_latest_messages(self):
        """Returns and clears all received messages."""
        with self._lock:
            msgs = list(self.received_messages)
            self.received_messages.clear()
            return msgs

    def connect(self) -> bool:
        """Performs the handshake to acquire a session token."""
        if not self.target_addr:
            logging.error("No target address set. Cannot connect.")
            return False

        handshake_packet = message.UDPPacket(
            session_token=0,
            sequence_number=self.sequence_number,
            timestamp=int(time.time() * 1000) & 0xFFFFFFFF,
        )

        try:
            self.sock.sendto(handshake_packet.pack(), self.target_addr)
            data, _ = self.sock.recvfrom(2048)

            response = message.UDPPacket.unpack(data)
            self.session_token = response.session_token
            self.is_connected = True

            # Start background listeners and heartbeats
            threading.Thread(target=self._listen_loop, daemon=True).start()
            self._start_heartbeat()

            return True
        except (socket.timeout, ValueError) as e:
            logging.error(f"Connection failed: {e}")
            return False

    def send_messages(self, messages: list[message.Message]):
        """Wraps messages in a UDPPacket and sends them."""
        if not self.is_connected:
            return

        with self._lock:
            self.sequence_number = (self.sequence_number + 1) & 0xFFFFFFFF
            packet = message.UDPPacket(
                session_token=self.session_token,
                sequence_number=self.sequence_number,
                timestamp=int(time.time() * 1000) & 0xFFFFFFFF,
                messages=messages,
            )
            try:
                assert self.target_addr is not None
                self.sock.sendto(packet.pack(), self.target_addr)
            except Exception as e:
                logging.error(f"Failed to send packet: {e}")

    def _start_heartbeat(self):
        """Recursive timer for heartbeat maintenance."""
        if not self._stop_event.is_set() and self.is_connected:
            self.send_heartbeat()
            threading.Timer(2.0, self._start_heartbeat).start()

    def _listen_loop(self):
        """Background thread to handle incoming packets from the ESP."""
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
                if not self._stop_event.is_set():
                    logging.error(f"Listener error: {e}")

    def disconnect(self):
        """Safely stops background tasks and closes the socket."""
        self._stop_event.set()
        self.is_connected = False
        try:
            self.sock.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass
        self.sock.close()
