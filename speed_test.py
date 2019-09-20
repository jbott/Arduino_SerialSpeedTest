#!/usr/bin/env python
"""
Speed test an attached Arduino's USB speed.
"""
import argparse
import timeit

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

class SpeedTestDevice:
    def __init__(self, serial):
        self.serial = serial

    def cmd(self, cmd):
        print("tx ->", cmd)
        self.serial.write(cmd.encode("ascii") + b"\n")
        print("rx <-", self.serial.readline().decode("ascii").strip())

    def transmit_test(self, rate, chunk_size, timeout=5.0):
        self.cmd(f"t{rate},{chunk_size}")

        total_size = 0
        start = timeit.default_timer()
        while True:
            ret = self.serial.read(chunk_size)
            total_size += len(ret)
            if (timeit.default_timer() - start) > timeout:
                break
        end = timeit.default_timer()

        self.cmd("x")

        elapsed_seconds = end - start
        speed_bytes_per_second = total_size / elapsed_seconds
        speed_kbps = speed_bytes_per_second / 1024.0
        print(f"Received {total_size} bytes in {elapsed_seconds} seconds: {speed_bytes_per_second:.2f} b/s ({speed_kbps:.2f} kb/s)")


def main():
    args = parser.parse_args()

    with serial.Serial(port=args.port, baudrate=args.baud, timeout=0.1) as port:
        d = SpeedTestDevice(port)

        if args.test == "transmit":
            d.transmit_test(rate=100000, chunk_size=1024)

        else:
            parser.error("Invalid test type")


if __name__ == "__main__":
    main()
