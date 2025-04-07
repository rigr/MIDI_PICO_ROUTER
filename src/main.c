#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pio_usb.h"
#include "tusb.h"
#include "hardware/uart.h"
#include <stdbool.h>

#define UART_BAUD_RATE 31250  // MIDI-Baudrate

// Puffer für MIDI-Daten
static uint8_t midi_buffer[64];

// DIN-MIDI-Pins (Beispiel, anpassen!)
#define UART0_TX_PIN 0  // DIN-MIDI 1 TX
#define UART0_RX_PIN 1  // DIN-MIDI 1 RX
#define UART1_TX_PIN 4  // DIN-MIDI 2 TX
#define UART1_RX_PIN 5  // DIN-MIDI 2 RX
#define UART2_TX_PIN 8  // DIN-MIDI 3 TX (Software-UART)
#define UART2_RX_PIN 9  // DIN-MIDI 3 RX

// Core 1: USB-Tasks
void core1_entry() {
    while (1) {
        pio_usb_host_task(); // USB-Host
        tuh_task();          // TinyUSB Host
        tud_task();          // TinyUSB Device
    }
}

// MIDI an alle Ausgänge senden
void forward_midi_to_all(uint8_t src_type, uint8_t src_id, uint8_t *data, uint32_t len) {
    // USB-Host-Ausgänge
    for (uint8_t dev_addr = 1; dev_addr <= CFG_TUSB_HOST_DEVICE_MAX; dev_addr++) {
        if ((src_type != 0 || dev_addr != src_id) && tuh_midi_mounted(dev_addr)) {
            tuh_midi_packet_write(dev_addr, data);
        }
    }
    // DIN-MIDI-Ausgänge
    uart_puts(uart0, (const char*)data);
    uart_puts(uart1, (const char*)data);
    uart_puts(uart2, (const char*)data); // Software-UART später
    // USB-Device (zum Computer)
    if (tud_midi_mounted()) {
        tud_midi_packet_write(data);
    }
}

int main() {
    stdio_init_all();

    // UARTs für DIN-MIDI initialisieren
    uart_init(uart0, UART_BAUD_RATE);
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);
    uart_init(uart1, UART_BAUD_RATE);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
    // UART2 als Platzhalter (Software-UART TBD)
    uart_init(uart2, UART_BAUD_RATE); // Vereinfacht, später anpassen
    gpio_set_function(UART2_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART2_RX_PIN, GPIO_FUNC_UART);

    // TinyUSB initialisieren
    tuh_init(0);
    tud_init(0);

    // PIO-USB für zwei USB-Host-Ports
    pio_usb_configuration_t pio_cfg0 = PIO_USB_DEFAULT_CONFIG;
    pio_cfg0.pio_idx = 0; // Erster USB-Host auf PIO0
    pio_usb_host_init(&pio_cfg0);

    pio_usb_configuration_t pio_cfg1 = PIO_USB_DEFAULT_CONFIG;
    pio_cfg1.pio_idx = 1; // Zweiter USB-Host auf PIO1
    pio_usb_host_init(&pio_cfg1);

    multicore_launch_core1(core1_entry);

    while (1) {
        // USB-Host-Eingänge
        uint8_t device_count = pio_usb_host_get_device_count();
        for (uint8_t dev_addr = 1; dev_addr <= device_count; dev_addr++) {
            if (tuh_midi_mounted(dev_addr)) {
                uint32_t len = tuh_midi_packet_read(dev_addr, midi_buffer);
                if (len > 0) {
                    forward_midi_to_all(0, dev_addr, midi_buffer, len);
                }
            }
        }
        // DIN-MIDI-Eingänge
        if (uart_is_readable(uart0)) {
            uart_read_blocking(uart0, midi_buffer, 3);
            forward_midi_to_all(1, 0, midi_buffer, 3);
        }
        if (uart_is_readable(uart1)) {
            uart_read_blocking(uart1, midi_buffer, 3);
            forward_midi_to_all(1, 1, midi_buffer, 3);
        }
        if (uart_is_readable(uart2)) {
            uart_read_blocking(uart2, midi_buffer, 3);
            forward_midi_to_all(1, 2, midi_buffer, 3);
        }
        // USB-Device-Eingang (vom Computer)
        if (tud_midi_available()) {
            uint32_t len = tud_midi_packet_read(midi_buffer);
            if (len > 0) {
                forward_midi_to_all(2, 0, midi_buffer, len);
            }
        }
        sleep_ms(1);
    }

    return 0;
}
