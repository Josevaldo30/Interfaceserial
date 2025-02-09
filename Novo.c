#include <stdio.h>// Biblioteca padrão para entrada e saída de dados (printf, scanf, etc.)
#include "pico/stdlib.h"// Biblioteca padrão do Raspberry Pi Pico para GPIO, UART, e outras funcionalidades básicas
#include "hardware/i2c.h"// Biblioteca para comunicação via protocolo I2C
#include "hardware/uart.h"// Biblioteca para comunicação serial UART
#include <stdlib.h>// Biblioteca padrão do C para alocação de memória, conversões e outras funções utilitárias
#include "hardware/pio.h"// Biblioteca para utilizar os periféricos de entrada/saída programáveis (PIO)
#include "hardware/clocks.h"// Biblioteca para controle dos clocks do microcontrolador
#include "hardware/adc.h"// Biblioteca para manipulação do conversor analógico-digital (ADC)
#include "pico/bootrom.h"// Biblioteca para manipular funções da memória ROM, como reinicialização do sistema
#include "inc/ssd1306.h"// Biblioteca para controlar displays OLED SSD1306 via I2C
#include "inc/font.h"// Biblioteca contendo fontes para exibição no display SSD1306
#include "novaMatrix.pio.h"// Código gerado pelo PIO para controle de matriz de LEDs
#include "hardware/timer.h"// Biblioteca para manipulação de temporizadores do hardware

// Definições para comunicação I2C
#define I2C_PORT i2c1// Define a porta I2C a ser utilizada (I2C1)
#define I2C_SDA 14// Pino GPIO 14 configurado como SDA (dados do I2C)
#define I2C_SCL 15// Pino GPIO 15 configurado como SCL (clock do I2C)
#define endereco 0x3C// Endereço I2C do display OLED SSD1306
#define L (0.4)  // define Intensidade de brilho dos LEDs (valores entre 0 e 1)
#define PIN_A 5// Pino GPIO 5 usado para um botão de entrada
#define PIN_B 6// Pino GPIO 6 usado para outro botão de entrada
#define LED_VERDE 11// Pino GPIO 11 para o LED verde
#define LED_AZUL 12// Pino GPIO 12 para o LED azul
#define LED_VERMELHO 13// Pino GPIO 13 para o LED vermelho

#define MATRIZ 7 // define Pino de dados conectado à matriz de LEDs
#define NUMERO_LEDS 25 // define Número total de LEDs na matriz (5x5)

// Variáveis voláteis para debounce dos botões 
static volatile uint32_t last = 0;// Armazena o tempo da última ativação do botão A
static volatile uint32_t last_time2 = 0;// Armazena o tempo da última ativação do botão B

// Definições dos pinos para UART
#define UART_ID uart1// Utiliza a interface UART1
#define BAUD_RATE 115200// Define a taxa de transmissão para 115200 bps

#define UART_TX_PIN 4// Pino GPIO 4 configurado como TX (transmissão)
#define UART_RX_PIN 5 // Pino GPIO 5 configurado como RX (recepção)

ssd1306_t ssd;  // Estrutura usada para armazenar o estado do display

// Matrizes que representam os números de 0 a 9 em uma matriz 5x5 de LEDs
// Cada posição da matriz contém um valor de intensidade de brilho (0.0 = apagado, L = ligado)

// Representação do número 0
double numero0[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

// Representação do número 1
double numero1[25] = {0.0, L, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};

// Representação do número 2
double numero2[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

// Representação do número 3
double numero3[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

// Representação do número 4
double numero4[25] = {0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, L, 0.0};

// Representação do número 5
double numero5[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

// Representação do número 6
double numero6[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

// Representação do número 7
double numero7[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};

// Representação do número 8
double numero8[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

// Representação do número 9
double numero9[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                       0.0, L, L, L, 0.0};

   // Matriz limpa (tudo apagado)                    
double limpa[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0};

// Função que converte valores de cor (RGB) para um formato de 32 bits adequado para a matriz LED
uint32_t matrix_rgb(float r, float g, float b)
{
    unsigned char R, G, B;
    R = r * 255;// Converte o valor normalizado (0 a 1) para a escala de 0 a 255
    G = g * 255;// Converte o valor normalizado (0 a 1) para a escala de 0 a 255
    B = b * 255;// Converte o valor normalizado (0 a 1) para a escala de 0 a 255

    return (G << 24) | (R << 16) | (B << 8);// Combina os valores RGB em um único número de 32 bits no formato esperado pela matriz
}

void initialize_gpio();// Função para inicializar os pinos GPIO
void acendeRGB(bool r, bool g, bool b);// Função para acender os LEDs RGB
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);// Função para exibir um padrão de desenho em uma matriz de LEDs RGB

// Variáveis globais para armazenar o estado dos LEDs
bool ledON = false; // Estado do LED verde
bool ledONb = false; // Estado do LED azul

// Função de interrupção acionada pelo pressionamento dos botões nos pinos PIN_A e PIN_B
void botao_interrupcao(uint gpio, uint32_t events)
{
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());// Obtém o tempo atual em microssegundos desde a inicialização

    if (gpio == PIN_A && tempo_atual - last > 200000)// Verifica se o botão conectado ao PIN_A foi pressionado e se já passou tempo suficiente desde a última interrupção (200ms)
    {
        last = tempo_atual;// Atualiza o tempo da última interrupção
        ledON = !ledON; // Alterna o estado do LED verde
        acendeRGB(0, ledON, 0); // Liga ou desliga o LED verde
        printf("Led verde %s\n", ledON ? "ligado" : "desligado");// Exibe mensagem no terminal

        // Atualiza o display OLED com o novo estado do LED
        ssd1306_fill(&ssd, 0);// Limpa a tela
        ssd1306_draw_string(&ssd, ledON ? "Verde ON!" : "Verde OFF!", 4, 10);// Escreve a mensagem
        ssd1306_send_data(&ssd);// Envia os dados para exibição
    }

    // Verifica se o botão conectado ao PIN_B foi pressionado e se já passou tempo suficiente desde a última interrupção (200ms)
    else if (gpio == PIN_B && tempo_atual - last_time2 > 200000)
    {
        last_time2 = tempo_atual;// Atualiza o tempo da última interrupção
        ledONb = !ledONb;// Alterna o estado do LED azul
        acendeRGB(0, 0, ledONb); // Liga ou desliga o LED verde
        printf("Led azul %s\n", ledONb ? "ligado" : "desligado");// Exibe mensagem no terminal
        ssd1306_fill(&ssd, 0);// Limpa a tela
        ssd1306_draw_string(&ssd, ledONb ? "Azul ON!" : "Azul OFF!", 4, 10);// Escreve a mensagem
        ssd1306_send_data(&ssd);// Envia os dados para exibição
    }
}

int main()
{
    PIO pio = pio0;// Definição da interface PIO (Programável I/O) a ser utilizada
    bool ok;// Variável para armazenar se a configuração do clock foi bem-sucedida
    uint16_t i = 0;// Variável de controle para iterações
    uint32_t valor_led;// Variável que armazenará os valores de cores para os LEDs
    double r = 0.0, b = 0.0, g = 0.0;// Variáveis para definir a intensidade das cores RGB (vermelho, azul, verde)
    ok = set_sys_clock_khz(128000, false);// Configura o clock do sistema para 128 MHz e armazena o status da operação
    printf("iniciando a transmissão PIO");// Exibe mensagem indicando o início da transmissão via PIO

    // Se a configuração do clock foi bem-sucedida, imprime a frequência configurada
    if (ok)
        printf("clock set to %id\n", clock_get_hz(clk_sys));      
    stdio_init_all();// Inicializa os dispositivos de entrada e saída padrão (USB, UART, etc.)   
    initialize_gpio(); // Inicializa os pinos GPIO
    uint sm = pio_claim_unused_sm(pio, true); // Obtém um State Machine (SM) disponível no PIO e o reserva para uso
    uint offset = pio_add_program(pio, &novaMatrix_program);// Adiciona o programa PIO "novaMatrix_program" à memória do PIO e armazena seu offset
    novaMatrix_program_init(pio, sm, offset, MATRIZ); // Inicializa a matriz de LEDs utilizando o programa PIO

    
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa a comunicação I2C no barramento especificado (I2C1) com velocidade de 400 kHz

    // Configura os pinos SDA e SCL para a função I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Habilita resistores de pull-up nos pinos SDA e SCL, pois o protocolo I2C requer isso
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
     // Inicializa o display OLED SSD1306 com as configurações necessárias
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd);                                         
    ssd1306_send_data(&ssd);

    // Preenche o display com pixels apagados (limpa a tela)                                      
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = true;// Variável booleana usada para alternar cores
    
    uart_init(UART_ID, BAUD_RATE);// Inicializa a comunicação UART com a taxa de transmissão especificada (115200 bps)
    
    // Configura os pinos TX e RX para a função UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Configura interrupções para os botões PIN_A e PIN_B
    // As interrupções são ativadas na borda de descida do sinal (pressionamento do botão)
    // Quando acionado, chama a função "botao_interrupcao"
    gpio_set_irq_enabled_with_callback(PIN_A, GPIO_IRQ_EDGE_FALL, true, &botao_interrupcao);
    gpio_set_irq_enabled_with_callback(PIN_B, GPIO_IRQ_EDGE_FALL, true, &botao_interrupcao);

    char letra; // Declara uma variável para armazenar o caractere digitado
    while (true)// Loop infinito
    {
        scanf("%c", &letra);// Lê um caractere do teclado e armazena na variável 'letra'
        printf("Letra digitada: %c", letra);  // Exibe no terminal a letra digitada
        ssd1306_fill(&ssd, 0);// Limpa a tela do display OLED
        ssd1306_draw_char(&ssd, letra, 60, 40);// Desenha o caractere digitado no display OLED na posição (60, 40)
        ssd1306_send_data(&ssd);// Envia os dados para o display OLED para atualizar a exibição

         // Verifica qual número foi digitado e aciona a função correspondente para exibição na matriz de LEDs
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
        default:// Caso um caractere não numérico seja digitado, a matriz de LEDs será limpa
            padrao1(limpa, valor_led, pio, sm, r, g, b);
            break;
        }
        sleep_ms(100); // Aguarda 100ms antes de continuar a próxima iteração do loop
    }
}

// Função para exibir um padrão na matriz de LEDs
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUMERO_LEDS; i++)// Percorre os 25 LEDs da matriz
    {
        valor_led = matrix_rgb(desenho[24 - i], g = 0.0, b = 0.0); // Define a cor do LED com base na matriz de desenho
        
        // Envia o valor do LED para o PIO, que controla a exibição na matriz
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}
void initialize_gpio()// Função para inicializar os pinos GPIO

{
    // Configura o pino do botão A como entrada com pull-upgpio_init(PIN_A);
    gpio_set_dir(PIN_A, GPIO_IN);
    gpio_pull_up(PIN_A);

    // Configura o pino do botão B como entrada com pull-up
    gpio_init(PIN_B);
    gpio_set_dir(PIN_B, GPIO_IN);
    gpio_pull_up(PIN_B);

    // Configura o LED azul como saída
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);

    // Configura o LED verde como saída
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    // Configura o LED vermelho como saída
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

void acendeRGB(bool r, bool g, bool b)// Função para acender ou apagar os LEDs RGB individualmente
{
    // Define o estado de cada LED RGB (ligado ou desligado)
    gpio_put(LED_VERMELHO, r);// LED vermelho
    gpio_put(LED_VERDE, g);// LED verde
    gpio_put(LED_AZUL, b);// LED azul
}

