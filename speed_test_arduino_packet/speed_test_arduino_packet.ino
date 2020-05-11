#include "frame_serial.h"

FrameSerial s;

enum PacketType
{
    PACKET_TYPE_NONE = 0,
    PACKET_TYPE_CMD,
    PACKET_TYPE_LOG,
    PACKET_TYPE_DATA,
};

struct packet_data_s
{
    uint32_t timestamp;
    uint32_t adc[32];
} data;

#define LED_PIN (13)
#define MAX_CHUNK_SIZE (1024)

enum test_state {
    TEST_STATE_NONE = 0,
    TEST_STATE_TRANSMIT,
};

enum test_state state = TEST_STATE_NONE;
int rate = 0;
int chunk_size = 0;
int last_micros = 0;

void setup()
{
    // Configure serial and the LED
    s.begin(115200); // Baud is ignored on Teensy
    s.setPacketHandler(&handlePacket);
    pinMode(LED_PIN, OUTPUT);
}

void set_led(bool val)
{
    digitalWrite(LED_PIN, val ? HIGH : LOW);
}

void loop()
{
    s.update();

    // Update the LED state
    set_led(state != TEST_STATE_NONE);

    switch (state)
    {
    case TEST_STATE_TRANSMIT:
        {
            if ((micros() - last_micros) > (1000000 / rate))
            {
                data.timestamp = micros();
                data.adc[0] = 0xFF00FF00;

                s.sendFrame(PACKET_TYPE_DATA, (const uint8_t*)&data, sizeof(data));

                last_micros = micros();
            }
        }
        break;

    default:
    case TEST_STATE_NONE:
        break;
    }
}

void handlePacket(const uint8_t *buf, size_t len)
{
    // s.sendFrame(PACKET_TYPE_LOG, buf, len);
    struct frame_s frame = { 0 };
    if (!s.decodePacket(buf, len, &frame)) {
        return;
    }

    if (frame.type != PACKET_TYPE_CMD) {
        return;
    }

    if (frame.size < 1)
    {
        return;
    }

    uint8_t tmp[256];
    char c = frame.data[0];
    switch (c)
    {
    case 't': // Transmit test
        {
            char tmp[64];
            uint8_t *comma = memchr(frame.data, ',', len);

            strlcpy(tmp, &frame.data[1], (comma - frame.data));
            int tx_rate = atoi(tmp);

            strlcpy(tmp, &comma[1], (len - (comma - frame.data)));
            int tx_chunk_size = atoi(tmp);

            rate = tx_rate;
            chunk_size = min(tx_chunk_size, MAX_CHUNK_SIZE);
            state = TEST_STATE_TRANSMIT;
            last_micros = micros();

            snprintf(tmp, sizeof(tmp), "$TRANSMIT rate=%d chunk_size=%d", rate, chunk_size);
            s.sendFrame(PACKET_TYPE_LOG, tmp, strlen(tmp));
        }
        break;

    case 'x': // Stop
        {
            state = TEST_STATE_NONE;

            snprintf(tmp, sizeof(tmp), "$NONE");
            s.sendFrame(PACKET_TYPE_LOG, tmp, strlen(tmp));
        }
        break;

    default:
    case '\0':
        break;
    }
}
