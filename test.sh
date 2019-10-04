#!/bin/bash

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 1000 \
    --chunk-size 256

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 10000 \
    --chunk-size 256

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 100000 \
    --chunk-size 256

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 1000 \
    --chunk-size 512

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 10000 \
    --chunk-size 512

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 100000 \
    --chunk-size 512

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 1000 \
    --chunk-size 1024

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 10000 \
    --chunk-size 1024

./.venv/bin/python speed_test.py \
    transmit \
    /dev/tty.usb* \
    --packet \
    --rate 100000 \
    --chunk-size 1024
