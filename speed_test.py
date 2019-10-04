#!/usr/bin/env python
"""
Speed test an attached Arduino's USB speed.
"""
import argparse
import time
import timeit

from cobs import cobs
import serial


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

    def send(self, buf):
        raw = buf.encode("ascii")
        if self.packet:
            pkt = cobs.encode(raw) + b"\0"

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
                    yield cobs.decode(pkt)
                    count += 1
                except cobs.DecodeError:
                    # Try again
                    break

    def cmd(self, cmd):
        print("tx ->", cmd)
        self.send(cmd)
        for p in self.read_packets():
            if chr(p[0]) == "$":
                ret = p.decode("ascii").strip()
                break
        # while True:
        #     rx = self.read()
        #     if rx and chr(rx[0]) == "$":
        #         ret = rx.decode("ascii").strip()
        #         break
        print("rx <-", ret)

    def transmit_test(self, rate, chunk_size, timeout=5.0):
        self.cmd(f"t{rate},{chunk_size}")

        total_size = 0
        start = timeit.default_timer()
        while True:
            if self.packet:
                for p in self.read_packets():
                    total_size += len(p)
                    if (timeit.default_timer() - start) > timeout:
                        break
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
