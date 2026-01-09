#include <Arduino.h>
#define SEGUNDOS_A_MICROSEGUNDOS 1000000ULL 
#define TIEMPO_SUENO  5  

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT); 2

  Serial.println("--- ESP32 DESPIERTO ---");
  Serial.println("Ejecutando tarea: LED encendido por 3 segundos...");
  digitalWrite(2, HIGH);
  delay(3000); 
  digitalWrite(2, LOW);
  esp_sleep_enable_timer_wakeup(TIEMPO_SUENO * SEGUNDOS_A_MICROSEGUNDOS);

  Serial.println("Tarea finalizada. Entrando en Deep Sleep por 5 segundos...");
  Serial.flush(); 
  esp_deep_sleep_start();
}
void loop() {}

