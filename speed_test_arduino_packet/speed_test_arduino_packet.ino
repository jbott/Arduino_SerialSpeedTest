#include <PacketSerial.h>
#include "frame_sender.h"

PacketSerial s;

enum PacketType
{
    PACKET_TYPE_NONE = 0,
    PACKET_TYPE_LOG,
    PACKET_TYPE_DATA,
};

LogPacket<PACKET_TYPE_LOG, 256> logPacket;

struct packet_data_s
{
    uint32_t adc[8];
};
FramedPacket<PACKET_TYPE_DATA, packet_data_s> dataPacket;

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
                dataPacket.m_data.data.adc[0] = 0xFF00FF00;
                sendPacket(&dataPacket);
                last_micros = micros();
            }
        }
        break;

    default:
    case TEST_STATE_NONE:
        break;
    }
}

template <class T>
void sendPacket(T *pkt)
{
    s.send(pkt->updateCRCAndGetBuffer(), pkt->getSize());
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
            chunk_size = min(tx_chunk_size, MAX_CHUNK_SIZE);
            state = TEST_STATE_TRANSMIT;
            last_micros = micros();

            logPacket.printf("$TRANSMIT rate=%d chunk_size=%d", rate, chunk_size);
            sendPacket(&logPacket);
        }
        break;

    case 'x': // Stop
        {
            state = TEST_STATE_NONE;

            logPacket.printf("$NONE");
            sendPacket(&logPacket);
        }
        break;

    default:
    case '\0':
        break;
    }
}
