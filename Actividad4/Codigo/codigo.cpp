#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2
#define UART_FISICA UART_NUM_2
QueueHandle_t xCola;
bool estado_led = false; // Variable para que el reportero sepa el estado

// TAREA 1: RECIBE COMANDOS UART (Pines 17 -> 16)
void tarea_productora_pines(void *pvParameters) {
    uint8_t teclado[128], rx_fisico[128];
    char cmd_cola;
    while (1) {
        int len_t = uart_read_bytes(UART_NUM_0, teclado, 127, pdMS_TO_TICKS(20));
        if (len_t > 0) uart_write_bytes(UART_FISICA, (const char*)teclado, len_t);

        int len_r = uart_read_bytes(UART_FISICA, rx_fisico, 127, pdMS_TO_TICKS(20));
        if (len_r > 0) {
            rx_fisico[len_r] = '\0';
            if (strstr((char*)rx_fisico, "on")) {
                cmd_cola = '1';
                xQueueSend(xCola, &cmd_cola, 0);
            } else if (strstr((char*)rx_fisico, "off")) {
                cmd_cola = '0';
                xQueueSend(xCola, &cmd_cola, 0);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// TAREA 2: CONTROLA SALIDAS (LED)
void tarea_consumidora_led(void *pvParameters) {
    char recibido;
    while (1) {
        if (xQueueReceive(xCola, &recibido, portMAX_DELAY)) {
            estado_led = (recibido == '1');
            gpio_set_level(LED_PIN, estado_led ? 1 : 0);
            printf("[CONTROL] LED actualizado vía COLA\n");
        }
    }
}

// TAREA 3: REPORTA ESTADO PERIÓDICAMENTE (Cada 5 segundos)
void tarea_reportero(void *pvParameters) {
    while (1) {
        printf("\n[REPORTE SISTEMA] --- Hora: %lld ms ---\n", esp_timer_get_time() / 1000);
        printf("[REPORTE SISTEMA] Estado del LED: %s\n", estado_led ? "ENCENDIDO" : "APAGADO");
        vTaskDelay(pdMS_TO_TICKS(5000)); // Reporte cada 5 segundos
    }
}

void app_main(void) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    uart_config_t cfg = {.baud_rate = 115200, 
    .data_bits = UART_DATA_8_BITS, 
    .parity = UART_PARITY_DISABLE, 
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT};
    uart_param_config(UART_NUM_0, &cfg);
    uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);
    uart_param_config(UART_FISICA, &cfg);
    uart_set_pin(UART_FISICA, 17, 16, -1, -1);
    uart_driver_install(UART_FISICA, 1024, 0, 0, NULL, 0);

    xCola = xQueueCreate(5, sizeof(char));
    
    // Lanzamos las 3 tareas solicitadas
    xTaskCreate(tarea_productora_pines, "ReceptorUART", 2048, NULL, 3, NULL);
    xTaskCreate(tarea_consumidora_led, "ControlSalidas", 2048, NULL, 2, NULL);
    xTaskCreate(tarea_reportero, "Reportero", 2048, NULL, 1, NULL);
}

void setup() { app_main(); }
void loop() {}
