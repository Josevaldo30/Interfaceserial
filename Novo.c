#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include <stdlib.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "novaMatrix.pio.h"
#include "hardware/timer.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define L (0.4) // Luminosidade dos leds
#define PIN_A 5
#define PIN_B 6
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13
#define MATRIZ 7       // Pino de dados conectado à matriz
#define NUMERO_LEDS 25 // Número de LEDs na matriz
static volatile uint32_t last = 0;
static volatile uint32_t last_time2 = 0;
// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5
ssd1306_t ssd;
double numero0[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero1[25] = {0.0, L, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};

double numero2[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero3[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

double numero4[25] = {0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, L, 0.0};

double numero5[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

double numero6[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero7[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};

double numero8[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero9[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};
double limpa[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0};

uint32_t matrix_rgb(float r, float g, float b)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;

    return (G << 24) | (R << 16) | (B << 8);
}
void initialize_gpio();
void acendeRGB(bool r, bool g, bool b);
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
bool ledON = false;
bool ledONb = false;
void botao_interrupcao(uint gpio, uint32_t events)
{
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());

    if (gpio == PIN_A && tempo_atual - last > 200000)
    {
        last = tempo_atual;
        ledON = !ledON;
        acendeRGB(0, ledON, 0);
        printf("Led verde %s\n", ledON ? "ligado" : "desligado");
        ssd1306_fill(&ssd, 0);
        ssd1306_draw_string(&ssd, ledON ? "Verde ON!" : "Verde OFF!", 4, 10);
        ssd1306_send_data(&ssd);
    }
    else if (gpio == PIN_B && tempo_atual - last_time2 > 200000)
    {
        last_time2 = tempo_atual;
        ledONb = !ledONb;
        acendeRGB(0, 0, ledONb);
        printf("Led azul %s\n", ledONb ? "ligado" : "desligado");
        ssd1306_fill(&ssd, 0);
        ssd1306_draw_string(&ssd, ledONb ? "Azul ON!" : "Azul OFF!", 4, 10);
        ssd1306_send_data(&ssd);
    }
}

int main()
{
    PIO pio = pio0;
    bool ok;
    uint16_t i = 0;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;
    ok = set_sys_clock_khz(128000, false);
    printf("iniciando a transmissão PIO");
    if (ok)
        printf("clock set to %id\n", clock_get_hz(clk_sys));
    stdio_init_all();
    initialize_gpio();
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &novaMatrix_program);
    novaMatrix_program_init(pio, sm, offset, MATRIZ);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    bool cor = true;

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART

    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart
    gpio_set_irq_enabled_with_callback(PIN_A, GPIO_IRQ_EDGE_FALL, true, &botao_interrupcao);
    gpio_set_irq_enabled_with_callback(PIN_B, GPIO_IRQ_EDGE_FALL, true, &botao_interrupcao);

    char letra;
    while (true)
    {
        scanf("%c", &letra);
        printf("Letra digitada: %c", letra); 
        ssd1306_fill(&ssd, 0);
        ssd1306_draw_char(&ssd, letra, 60, 40);
        ssd1306_send_data(&ssd);
        switch (letra)
        {
        case '0':
            padrao1(numero0, valor_led, pio, sm, r, g, b);
            break;
        case '1':
            padrao1(numero1, valor_led, pio, sm, r, g, b);
            break;
        case '2':
            padrao1(numero2, valor_led, pio, sm, r, g, b);
            break;
        case '3':
            padrao1(numero3, valor_led, pio, sm, r, g, b);
            break;
        case '4':
            padrao1(numero4, valor_led, pio, sm, r, g, b);
            break;
        case '5':
            padrao1(numero5, valor_led, pio, sm, r, g, b);
            break;
        case '6':
            padrao1(numero6, valor_led, pio, sm, r, g, b);
            break;
        case '7':
            padrao1(numero7, valor_led, pio, sm, r, g, b);
            break;
        case '8':
            padrao1(numero8, valor_led, pio, sm, r, g, b);
            break;
        case '9':
            padrao1(numero9, valor_led, pio, sm, r, g, b);
            break;
        default:
            padrao1(limpa, valor_led, pio, sm, r, g, b);
            break;
        }
        sleep_ms(100);
    }
}
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUMERO_LEDS; i++)
    {
        valor_led = matrix_rgb(desenho[24 - i], g = 0.0, b = 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}
void initialize_gpio()
{
    gpio_init(PIN_A);
    gpio_set_dir(PIN_A, GPIO_IN);
    gpio_pull_up(PIN_A);

    gpio_init(PIN_B);
    gpio_set_dir(PIN_B, GPIO_IN);
    gpio_pull_up(PIN_B);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

void acendeRGB(bool r, bool g, bool b)
{
    gpio_put(LED_VERMELHO, r);
    gpio_put(LED_VERDE, g);
    gpio_put(LED_AZUL, b);
}
