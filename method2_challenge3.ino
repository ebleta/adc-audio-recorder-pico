#define SAMPLE_RATE (4000)
#define MICROSECOND (1)
#define MILLISECOND (1000 * MICROSECOND)
#define SECOND      (1000 * MILLISECOND)
#define PIN_ADC (26)

unsigned long wait_until;

void setup(void) {
    pinMode(PIN_ADC, INPUT);
    wait_until = micros();
    Serial.begin(2000000);
}

void loop(void) {
    if (micros() < wait_until) {
        return;
    }

    uint16_t sample = analogRead(PIN_ADC);
    Serial.println(sample);

    wait_until += SECOND / SAMPLE_RATE;
}
