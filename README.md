# Tools for speed-testing Arduino serial port speeds

This was a quick sketch for testing max USB Serial transfer speed of Teensy 3.x
and 4.x boards. It could be easily expanded to support testing receive speeds.

# Observed speeds (Arduino 1.8.9, Teensyduino 1.47)

| Rate   | Chunk Size | Teensy 3.5                      | Teensy 4.0                        |
| ------ | ---------- | ------------------------------- | --------------------------------- |
| 100000 | 64         | 1088413.98 b/s (8.70859 Mb/s)   | Not Tested                        |
| 100000 | 256        | 1088494.65 b/s (8.7079572 Mb/s) | 5025606.97 b/s (40.2048558 Mb/s)  |
| 100000 | 512        | Not Tested                      | 11040667.09 b/s (88.3253367 Mb/s) |
