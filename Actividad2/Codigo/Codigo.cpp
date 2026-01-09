#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2
#define UART_FISICA UART_NUM_2 // Usaremos los pines 17 y 16

void tarea_sensor(void *pvParameters) {
    int contador = 0;
    while (1) {
        printf("[TAREA SENSOR] Valor: %d\n", contador++);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void tarea_puente_uart(void *pvParameters) {
    uint8_t teclado_data[128];
    uint8_t rx_data[128];

    while (1) {
        // 1. Leemos del teclado (UART0)
        int len_teclado = uart_read_bytes(UART_NUM_0, teclado_data, 127, pdMS_TO_TICKS(20));
        if (len_teclado > 0) {
            // 2. RETRANSMITIMOS por el pin 17 (TX2) para que viaje por el cable
            uart_write_bytes(UART_FISICA, (const char*)teclado_data, len_teclado);
        }

        // 3. LEEMOS del pin 16 (RX2). Solo si el cable está conectado, esto funcionará.
        int len_rx = uart_read_bytes(UART_FISICA, rx_data, 127, pdMS_TO_TICKS(20));
        if (len_rx > 0) {
            rx_data[len_rx] = '\0';
            if (strstr((char*)rx_data, "on")) {
                gpio_set_level(LED_PIN, 1);
                printf(">>> PIN 16 (RX2) RECIBIÓ: ENCENDER LED\n");
            } else if (strstr((char*)rx_data, "off")) {
                gpio_set_level(LED_PIN, 0);
                printf(">>> PIN 16 (RX2) RECIBIÓ: APAGAR LED\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Configurar UART0 (Consola)
    uart_config_t uart_cfg0 = {.baud_rate = 115200,
     .data_bits = UART_DATA_8_BITS,
     .parity = UART_PARITY_DISABLE,
     .stop_bits = UART_STOP_BITS_1, 
     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, 
     .source_clk = UART_SCLK_DEFAULT};
    uart_param_config(UART_NUM_0, &uart_cfg0);
    uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);

    // Configurar UART2 (Pines 17 y 16)
    uart_config_t uart_cfg2 = {.baud_rate = 115200, 
    .data_bits = UART_DATA_8_BITS, 
    .parity = UART_PARITY_DISABLE, 
    .stop_bits = UART_STOP_BITS_1, 
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, 
    .source_clk = UART_SCLK_DEFAULT};
    uart_param_config(UART_FISICA, &uart_cfg2);
    uart_set_pin(UART_FISICA, 17, 16, -1, -1);
    uart_driver_install(UART_FISICA, 1024, 0, 0, NULL, 0);

    xTaskCreate(tarea_sensor, "Sensor", 2048, NULL, 1, NULL);
    xTaskCreate(tarea_puente_uart, "Puente_UART", 2048, NULL, 2, NULL);
}

void setup() { app_main(); }
void loop() {}
