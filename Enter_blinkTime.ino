// Use only one core
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;
static int led_delay = 500;
// LED blink task

void readSerial(void* parameter) {
  while(1){
    if(Serial.available() > 0){
      led_delay = Serial.parseInt();
    }
  }
}

void blinkLED(void* parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
  }
}


void setup() {
  Serial.begin(115200);

  pinMode(led_pin, OUTPUT);

  xTaskCreatePinnedToCore(blinkLED, "blink_LED", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(readSerial, "read_Serial", 1024, NULL, 1, NULL, app_cpu);

  vTaskDelete(NULL);
}

void loop() {

}
