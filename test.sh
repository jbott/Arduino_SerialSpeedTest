#!/bin/bash
set -e -x

SERIAL_PORT=/dev/ttyACM0

./.venv/bin/python speed_test.py \
    transmit \
    ${SERIAL_PORT} \
    --packet \
    --rate 1000 \
    --chunk-size 1024

./.venv/bin/python speed_test.py \
    transmit \
    ${SERIAL_PORT} \
    --packet \
    --rate 10000 \
    --chunk-size 1024

./.venv/bin/python speed_test.py \
    transmit \
    ${SERIAL_PORT} \
    --packet \
    --rate 100000 \
    --chunk-size 1024

./.venv/bin/python speed_test.py \
    transmit \
    ${SERIAL_PORT} \
    --packet \
    --rate 1000000 \
    --chunk-size 1024
