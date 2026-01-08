Ejercicio 1 - Sistema de Comandos UART2
Tarea de Sistemas Embebidos - ESP32
Framework: ESP-IDF
Plataforma: PlatformIO + Visual Studio Code

Descripción
Sistema avanzado de comandos seriales implementado sobre el puerto UART2 del ESP32 utilizando drivers nativos del ESP-IDF. Permite recibir comandos desde un terminal serial, procesarlos y ejecutar acciones sobre el microcontrolador.
Objetivos Cumplidos

Inicialización correcta de UART2 con baud rate configurable
Recepción de comandos de texto desde terminal serial
Interpretación y ejecución de comandos
Respuestas estructuradas por UART
Manejo de buffers y lectura no bloqueante
Código modular y profesional

Características Técnicas
Configuración UART2

Puerto: UART_NUM_2
TX Pin: GPIO 17
RX Pin: GPIO 16
Baud Rate: 115200 bps
Data Bits: 8
Parity: None
Stop Bits: 1
Buffer Size: 1024 bytes

Comandos Implementados
Comando Descripción status : Muestra el estado actual del sistema (LED, contador, uptime) , ledon Enciende el LED conectado al GPIO 2l, edoff Apaga el LEDinfoMuestra información técnica del sistema (UART, ESP-IDF, chip), reset Reinicia variables internas (no reinicia el MCU), help Muestra la lista de comandos disponibles.
