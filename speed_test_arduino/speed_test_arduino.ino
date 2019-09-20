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
    Serial.begin(115200); // Baud is ignored on Teensy
    pinMode(LED_PIN, OUTPUT);
}

void set_led(bool val)
{
    digitalWrite(LED_PIN, val ? HIGH : LOW);
}

void loop()
{
    char c = '\0';
    if (Serial.available())
    {
        c = Serial.read();
    }

    switch (c)
    {
    case 't': // Transmit test
        {
            int tx_rate = Serial.parseInt();
            if (Serial.read() != ',')
            {
                break;
            }
            int tx_chunk_size = Serial.parseInt();

            rate = tx_rate;
            chunk_size = MIN(tx_chunk_size, MAX_CHUNK_SIZE);
            state = TEST_STATE_TRANSMIT;
            last_micros = micros();

            Serial.print("TRANSMIT rate=");
            Serial.print(rate);
            Serial.print(" chunk_size=");
            Serial.print(chunk_size);
            Serial.println();
        }
        break;

    case 'x': // Stop
        {
            state = TEST_STATE_NONE;

            Serial.println("NONE");
        }
        break;

    default:
    case '\0':
        break;
    }

    // Update the LED state
    set_led(state != TEST_STATE_NONE);

    switch (state)
    {
    case TEST_STATE_TRANSMIT:
        {
            if ((micros() - last_micros) > (1000000 / rate))
            {
                Serial.write(buf, chunk_size);
                last_micros = micros();
            }
        }
        break;

    default:
    case TEST_STATE_NONE:
        break;
    }
}
