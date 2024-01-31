#include <Arduino.h>
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

static const uint8_t max_size = 255;
static char* str_ptr = NULL;
static volatile bool string_is_received = 0;

void readSerialTask(void* parameter) {
  char received_str;
  char str_buffer[max_size];
  uint8_t index = 0;
  while(1) {
    if(Serial.available() > 0) {
      received_str = Serial.read();

      if(index < max_size - 1) {
        str_buffer[index] = received_str;
      }
      if(received_str == '\n') {
        str_buffer[index - 1] = '\0';

        if(string_is_received == 0) {
          str_ptr = (char*)pvPortMalloc(index*sizeof(char));
          memcpy(str_ptr, str_buffer, index);
          string_is_received = 1;
        }

        memset(str_buffer, 0, max_size);
        index = 0;
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void printToSerialTask(void* parameter) {
  while(1) {
    if(string_is_received == 1) {
      Serial.println(str_ptr);
      vPortFree(str_ptr);
      str_ptr = NULL;
      string_is_received = 0;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("Enter a string");
  xTaskCreatePinnedToCore(readSerialTask, "ReadingSerialMonitor", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(printToSerialTask, "PrintingToSerialMonitor", 1024, NULL, 1, NULL, app_cpu);
  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}
