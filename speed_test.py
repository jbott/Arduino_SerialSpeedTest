#!/usr/bin/env python
"""
Speed test an attached Arduino's USB speed.
"""
from enum import IntEnum
import argparse
import time
import timeit
import struct

from cobs import cobs
import serial

class PacketType(IntEnum):
    CMD = 1
    LOG = 2
    DATA = 3

uint16_t = struct.Struct("<H")
uint32_t = struct.Struct("<I")

parser = argparse.ArgumentParser(description="Speed test a serial port")
parser.add_argument("test", choices=["transmit"], help="Which test to run")
parser.add_argument("port", help="Serial port name or path")
parser.add_argument(
    "--baud",
    type=int,
    default=115200,
    help="Baud rate (ignored for virtual serial ports, default: 115200)",
)
parser.add_argument(
    "--rate",
    type=int,
    default=1000,
    help="Rate of which to send packets (Hz, default: 1000)",
)
parser.add_argument(
    "--chunk-size", type=int, default=256, help="Packet size (bytes, default: 256)"
)
parser.add_argument("--packet", action="store_true", help="Use packet based comms")


class SpeedTestDevice:
    def __init__(self, serial, packet=False):
        self.serial = serial
        self.packet = packet
        self._buf = bytearray()

        if self.cmd("x") != "$NONE":
            raise ValueError("Failed to initialize!")

    def send(self, buf):
        raw = buf.encode("ascii")

        packet_type = uint16_t.pack(1)
        packet_length = uint16_t.pack(len(raw))
        crc32 = uint32_t.pack(0xDEADBEEF)

        framed = packet_type + packet_length + raw + crc32;

        if self.packet:
            pkt = cobs.encode(framed) + b"\0"

        else:
            pkt = raw + b"\n"

        self.serial.write(pkt)

    def read(self, size=-1):
        return self.serial.read(size)

    def read_packets(self):
        while True:
            # Read as much data as possible and append to internal buffer
            # print(self.serial.in_waiting, len(self._buf))
            self._buf += self.serial.read(1024)

            count = 0
            while count < 100:
                # Split at the first null character, leaving extra in self._buf
                split = self._buf.split(b"\0", maxsplit=1)

                if len(split) == 2:
                    pkt, self._buf = split
                else:
                    break

                # If we've read a full packet, return it
                try:
                    decoded = cobs.decode(pkt)
                    packet_type = uint16_t.unpack(decoded[0:2])[0]
                    packet_length = uint16_t.unpack(decoded[2:4])[0]
                    packet_data = decoded[4:(4+packet_length)]
                    # packet_crc32 = uint32_t.unpack(decoded[(4+packet_length):(4+packet_length+5)])[0]

                    yield (packet_type, packet_data)

                    count += 1
                except cobs.DecodeError:
                    # Try again
                    break

    def cmd(self, cmd):
        print("tx ->", cmd)
        self.send(cmd)
        for p_type, p_data in self.read_packets():
            if p_type == PacketType.LOG:
                ret = p_data.decode("ascii").strip()
                break
        print("rx <-", ret)
        return ret

    def transmit_test(self, rate, chunk_size, timeout=5.0):
        self.cmd(f"t{rate},{chunk_size}")

        total_size = 0
        deltas = []
        start = timeit.default_timer()
        while True:
            if self.packet:
                last_timestamp = None
                for p_type, p_data in self.read_packets():
                    timestamp = uint32_t.unpack(p_data[0:4])[0]
                    if last_timestamp:
                        deltas.append(timestamp - last_timestamp)
                    total_size += len(p_data)
                    if (timeit.default_timer() - start) > timeout:
                        break
                    last_timestamp = timestamp
            else:
                ret = self.read(chunk_size)
                if ret:
                    total_size += len(ret)
            if (timeit.default_timer() - start) > timeout:
                break
        end = timeit.default_timer()

        self.cmd("x")

        elapsed_seconds = end - start
        speed_bytes_per_second = total_size / elapsed_seconds
        speed_Mbps = speed_bytes_per_second * 8 / 1e6
        print(
            f"Received {total_size} bytes in {elapsed_seconds} seconds: {speed_bytes_per_second:.2f} B/s ({speed_Mbps:.2f} Mbps)"
        )
        print(f"Average delta {sum(deltas)/len(deltas)} uS")


def main():
    args = parser.parse_args()

    with serial.Serial(port=args.port, baudrate=args.baud, timeout=0.001) as port:
        d = SpeedTestDevice(port, packet=args.packet)

        if args.test == "transmit":
            d.transmit_test(rate=args.rate, chunk_size=args.chunk_size)

        else:
            parser.error("Invalid test type")


if __name__ == "__main__":
    main()
