#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ws2812.h"     // Biblioteca para LEDs WS2812
#include "ssd1306.h"    // Biblioteca para o display OLED
#include "font.h"       // Biblioteca de fontes (modificada para incluir minúsculas)

// Definição de GPIOs
#define LED_R_PIN 11
#define LED_G_PIN 12
#define LED_B_PIN 13
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define WS2812_PIN 7
#define SDA_PIN 14
#define SCL_PIN 15

// Variáveis de estado
volatile bool ledG_state = false;
volatile bool ledB_state = false;
volatile bool buttonA_pressed = false;
volatile bool buttonB_pressed = false;

// Configuração da UART
void UART_init(uint32_t baud) {
    uint16_t ubrr = F_CPU/16/baud - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Ativa RX, TX e interrupção RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Modo 8 bits
}

// Enviar dados via UART
void UART_send(char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Espera buffer estar livre
    UDR0 = data;
}

// Receber dados via UART (Interrupção)
ISR(USART_RX_vect) {
    char received = UDR0;
    ssd1306_clear();
    ssd1306_drawChar(10, 20, received, 1); // Exibe o caractere no display
    ssd1306_update();

    // Se for um número, mostrar na matriz WS2812
    if (received >= '0' && received <= '9') {
        uint8_t number = received - '0';
        ws2812_displayNumber(number); // Função para exibir número na matriz 5x5
    }
}

// Configuração das interrupções dos botões
void setupInterrupts() {
    PCMSK |= (1 << PCINT5) | (1 << PCINT6); // Habilita interrupções para os botões
    GIMSK |= (1 << PCIE); // Habilita interrupções de mudança de pino
    sei(); // Ativa interrupções globais
}

// Rotina de interrupção para os botões
ISR(PCINT0_vect) {
    if (!(PINB & (1 << BUTTON_A_PIN))) { // Botão A pressionado
        _delay_ms(50); // Debounce
        if (!(PINB & (1 << BUTTON_A_PIN))) {
            ledG_state = !ledG_state;
            PORTB = (ledG_state) ? (PORTB | (1 << LED_G_PIN)) : (PORTB & ~(1 << LED_G_PIN));

            // Atualizar display e Serial Monitor
            ssd1306_clear();
            ssd1306_drawString("LED Verde: ");
            ssd1306_drawString(ledG_state ? "ON" : "OFF");
            ssd1306_update();
            UART_send(ledG_state ? "LED Verde Ligado\n" : "LED Verde Desligado\n");
        }
    }
    if (!(PINB & (1 << BUTTON_B_PIN))) { // Botão B pressionado
        _delay_ms(50); // Debounce
        if (!(PINB & (1 << BUTTON_B_PIN))) {
            ledB_state = !ledB_state;
            PORTB = (ledB_state) ? (PORTB | (1 << LED_B_PIN)) : (PORTB & ~(1 << LED_B_PIN));

            // Atualizar display e Serial Monitor
            ssd1306_clear();
            ssd1306_drawString("LED Azul: ");
            ssd1306_drawString(ledB_state ? "ON" : "OFF");
            ssd1306_update();
            UART_send(ledB_state ? "LED Azul Ligado\n" : "LED Azul Desligado\n");
        }
    }
}

// Configuração do hardware
void setup() {
    // Configuração de pinos
    DDRB |= (1 << LED_R_PIN) | (1 << LED_G_PIN) | (1 << LED_B_PIN); // LEDs como saída
    DDRB &= ~((1 << BUTTON_A_PIN) | (1 << BUTTON_B_PIN)); // Botões como entrada
    PORTB |= (1 << BUTTON_A_PIN) | (1 << BUTTON_B_PIN); // Ativa pull-ups internos

    // Inicializações
    UART_init(9600);
    ssd1306_init();
    ws2812_init();
    setupInterrupts();
}

// Loop principal
int main(void) {
    setup();

    while (1) {
        // O código principal não precisa verificar os botões, pois já são tratados por interrupções
    }
}