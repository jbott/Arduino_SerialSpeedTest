# Tools for speed-testing Arduino serial port speeds

This was a quick sketch for testing max USB Serial transfer speed of Teensy 3.x
and 4.x boards. It could be easily expanded to support testing receive speeds.

# Observed speeds, Raw (Arduino 1.8.9, Teensyduino 1.47)

| Rate   | Chunk Size | Teensy 3.5                      | Teensy 4.0                        |
| ------ | ---------- | ------------------------------- | --------------------------------- |
| 100000 | 64         | 1088413.98 B/s (8.70859 Mb/s)   | Not Tested                        |
| 100000 | 256        | 1088494.65 B/s (8.7079572 Mb/s) | 5025606.97 B/s (40.2048558 Mb/s)  |
| 100000 | 512        | Not Tested                      | 11040667.09 B/s (88.3253367 Mb/s) |

# Observed speeds, Packets (Arduino 1.8.9, Teensyduino 1.47)

| Rate   | Chunk Size | Teensy 3.5                      | Teensy 4.0                        |
| ------ | ---------- | ------------------------------- | --------------------------------- |
| 1000   | 256        | 239287.99 B/s (1.91 Mb/s)       | 254244.43 B/s (2.03 Mb/s)         |
| 10000  | 256        | 1076053.96 B/s (8.61 Mb/s)      | 2401448.02 B/s (19.21 Mb/s)       |
| 100000 | 256        | 1075604.71 B/s (8.60 Mb/s)      | 10537925.68 B/s (84.30 Mb/s)      |
| 1000   | 512        | 450726.90 B/s (3.61 Mb/s)       | 506438.10 B/s (4.05 Mb/s)         |
| 10000  | 512        | 1079668.77 B/s (8.64 Mb/s)      | 4612729.06 B/s (36.90 Mb/s)       |
| 100000 | 512        | 1079994.40 B/s (8.64 Mb/s)      | 10828915.44 B/s (86.63 Mb/s)      |
| 1000   | 1024       | 634207.56 B/s (5.07 Mb/s)       | 1004813.73 B/s (8.04 Mb/s)        |
| 10000  | 1024       | 1082226.65 B/s (8.66 Mb/s)      | 8535930.45 B/s (68.29 Mb/s)       |
| 100000 | 1024       | 1081920.37 B/s (8.66 Mb/s)      | 11040584.69 B/s (88.32 Mb/s)      |
