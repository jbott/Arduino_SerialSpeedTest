#include <PacketSerial.h>

PacketSerial s;

#define LED_PIN (13)
#define MAX_CHUNK_SIZE (1024)

#define MIN(a, b) (a > b ? b : a)

enum test_state {
    TEST_STATE_NONE = 0,
    TEST_STATE_TRANSMIT,
};

enum test_state state = TEST_STATE_NONE;
int rate = 0;
int chunk_size = 0;
unsigned char buf[MAX_CHUNK_SIZE];
int last_micros = 0;

void setup()
{
    // Initialize the buffer to an empty value
    memset(buf, '0', sizeof(buf));

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
                s.send(buf, chunk_size);
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
    if (len < 1)
    {
        return;
    }

    char c = buf[0];
    switch (c)
    {
    case 't': // Transmit test
        {
            char tmp[64];
            uint8_t *comma = memchr(buf, ',', len);

            strlcpy(tmp, &buf[1], (comma - buf));
            int tx_rate = atoi(tmp);

            strlcpy(tmp, &comma[1], (len - (comma - buf)));
            int tx_chunk_size = atoi(tmp);

            rate = tx_rate;
            chunk_size = MIN(tx_chunk_size, MAX_CHUNK_SIZE);
            state = TEST_STATE_TRANSMIT;
            last_micros = micros();

            snprintf(tmp, sizeof(tmp), "$TRANSMIT rate=%d chunk_size=%d", rate, chunk_size);
            s.send(tmp, strnlen(tmp, sizeof(tmp)));
        }
        break;

    case 'x': // Stop
        {
            state = TEST_STATE_NONE;

            s.send("$NONE", 5);
        }
        break;

    default:
    case '\0':
        break;
    }
}
