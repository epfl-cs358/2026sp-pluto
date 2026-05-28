"""
Contains the message formats for communication between the controller and pluto.
This file should ALWAYS be in sync with `message.h`.
"""

import struct
import zlib
from enum import IntEnum
from typing import List


class MessageFamilyKind(IntEnum):
    KIND_INFO = 0
    KIND_MOVE = 1
    KIND_SENSOR = 2
    KIND_BEHAVIOR = 3


class MessageInfoKind(IntEnum):
    INFO_REQUEST_IS_ALIVE = 0
    INFO_REQUEST_EVENT_CLOCK = 1
    INFO_REQUEST_SENSOR = 2
    INFO_ACKNOWLEDGE = 3


class MessageMoveKind(IntEnum):
    MOVE_CONTROL_BEGIN_FOR = 0
    MOVE_CONTROL_END = 1
    MOVE_STOP_FOR = 2
    MOVE_BY = 3


class MessageSensorKind(IntEnum):
    SENSOR_DISTANCE = 0
    SENSOR_MICROPHONE = 1


class SensorMicrophone(IntEnum):
    MIC_NONE = 0
    MIC_SIT = 1
    MIC_GIVE_PAW = 2


class MessageBehaviorKind(IntEnum):
    BEHAVIOR_FLIP = 0
    BEHAVIOR_BOW = 1
    BEHAVIOR_SIT = 2
    BEHAVIOR_GIVE_PAW = 3


class Message:
    """Represents an exactly 8-byte message block."""

    def __init__(
        self,
        family: int,
        kind: int,
        event_clock_or_duration: int,
        payload_bytes: bytes = b"\x00\x00\x00\x00",
    ):
        self.family = family
        self.kind = kind
        self.event_clock_or_duration = event_clock_or_duration

        # payload must be exactly 4 bytes to match the C++ union
        if len(payload_bytes) != 4:
            raise ValueError(f"Payload must be exactly 4 bytes, got {len(payload_bytes)}")
        self.payload_bytes = payload_bytes

    def pack(self) -> bytes:
        # esp8266 is little-endian
        # family: 2 bits (0-1)
        # kind: 4 bits (2-5)
        # event_clock_or_duration: 26 bits (6-31)
        header = (
            (self.family & 0x03)
            | ((self.kind & 0x0F) << 2)
            | ((self.event_clock_or_duration & 0x3FFFFFF) << 6)
        )
        return struct.pack("<I4s", header, self.payload_bytes)

    @classmethod
    def unpack(cls, data: bytes) -> "Message":
        if len(data) != 8:
            raise ValueError("Message data must be exactly 8 bytes")

        header, payload_bytes = struct.unpack("<I4s", data)

        family = header & 0x03
        kind = (header >> 2) & 0x0F
        event_clock_or_duration = (header >> 6) & 0x3FFFFFF

        return cls(family, kind, event_clock_or_duration, payload_bytes)


MAX_MESSAGES_PER_PACKET = 64
PACKET_HEADER_SIZE = 17  # crc32(4) + token(4) + seq(4) + timestamp(4) + count(1)


class UDPPacket:
    """Represents the transport envelope containing up to MAX_MESSAGES_PER_PACKET messages."""

    def __init__(
        self,
        session_token: int,
        sequence_number: int,
        timestamp: int,
        messages: List[Message] = [],
    ):
        self.session_token = session_token
        self.sequence_number = sequence_number
        self.timestamp = timestamp
        self.messages = messages

    def pack(self) -> bytes:
        count = len(self.messages)
        if count > MAX_MESSAGES_PER_PACKET:
            raise ValueError(f"Cannot pack more than {MAX_MESSAGES_PER_PACKET} messages")

        # pack the messages array
        messages_bytes = b"".join(msg.pack() for msg in self.messages)

        # pack header (excluding CRC for now)
        # format: token(I), seq(I), ts(I), count(B)
        header_no_crc = struct.pack(
            "<IIIB", self.session_token, self.sequence_number, self.timestamp, count
        )

        # calculate CRC over the payload (skipping the first 4 bytes)
        payload_start = header_no_crc + messages_bytes
        crc32 = zlib.crc32(payload_start) & 0xFFFFFFFF
        return struct.pack("<I", crc32) + payload_start

    @classmethod
    def unpack(cls, data: bytes) -> "UDPPacket":
        if len(data) < PACKET_HEADER_SIZE:
            raise ValueError("Packet too small to contain header")

        # unpack the 17-byte header
        crc32, token, seq, ts, count = struct.unpack("<IIIIB", data[:PACKET_HEADER_SIZE])

        expected_size = PACKET_HEADER_SIZE + (8 * count)
        if len(data) != expected_size:
            raise ValueError(
                f"Packet size mismatch. Expected {expected_size}, got {len(data)}"
            )

        payload_start = data[4:expected_size]
        computed_crc = zlib.crc32(payload_start) & 0xFFFFFFFF
        if crc32 != computed_crc:
            raise ValueError(f"CRC mismatch! Expected {crc32}, computed {computed_crc}")

        messages = []
        for i in range(count):
            start_idx = PACKET_HEADER_SIZE + (i * 8)
            end_idx = start_idx + 8
            messages.append(Message.unpack(data[start_idx:end_idx]))

        return cls(token, seq, ts, messages)


def create_move_by(forward_back: int, left_right: int, duration_ms: int) -> Message:
    # <hh packs two signed 16-bit integers
    payload = struct.pack("<hh", forward_back, left_right)
    return Message(
        MessageFamilyKind.KIND_MOVE, MessageMoveKind.MOVE_BY, duration_ms, payload
    )


def create_control_begin(lease_duration_ms: int) -> Message:
    # payload is ignored, zero fill
    return Message(
        MessageFamilyKind.KIND_MOVE,
        MessageMoveKind.MOVE_CONTROL_BEGIN_FOR,
        lease_duration_ms,
        b"\x00\x00\x00\x00",
    )


def create_acknowledge(sequence_number: int) -> Message:
    # <I packs one unsigned 32-bit integer
    payload = struct.pack("<I", sequence_number)
    return Message(
        MessageFamilyKind.KIND_INFO, MessageInfoKind.INFO_ACKNOWLEDGE, 0, payload
    )


def create_sensor_distance(distance_mm: int, current_millis: int) -> Message:
    # <I packs one unsigned 32-bit integer
    payload = struct.pack("<I", distance_mm)
    return Message(
        MessageFamilyKind.KIND_SENSOR,
        MessageSensorKind.SENSOR_DISTANCE,
        current_millis,
        payload,
    )


def create_behavior(behavior_kind: MessageBehaviorKind, duration_ms: int = 0) -> Message:
    return Message(
        family=MessageFamilyKind.KIND_BEHAVIOR,
        kind=behavior_kind,
        event_clock_or_duration=duration_ms,
        payload_bytes=b"\x00\x00\x00\x00",  # Empty payload as kind defines the action
    )
