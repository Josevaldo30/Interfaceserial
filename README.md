Projeto: Controle de LED e Display OLED com Raspberry Pi Pico

Descrição

Este projeto utiliza um Raspberry Pi Pico para controlar uma matriz de LEDs RGB e um display OLED via I2C. O sistema permite a interação através de botões para ligar e desligar LEDs, bem como exibir caracteres no display OLED. O código inclui comunicação UART, controle de GPIOs e manipulação de interrupções.

Componentes Utilizados

Raspberry Pi Pico

Matriz de LEDs RGB

Display OLED SSD1306 (via I2C)

Botões de entrada

LEDs RGB individuais

Bibliotecas Necessárias

O projeto depende das seguintes bibliotecas para funcionar corretamente:

pico/stdlib.h

hardware/i2c.h

hardware/uart.h

hardware/pio.h

hardware/clocks.h

hardware/adc.h

hardware/timer.h

ssd1306.h (para o controle do display OLED)

font.h (para manipulação de textos no display OLED)

novaMatrix.pio.h (para controle da matriz de LEDs)

Funcionalidades

Controle de LEDs RGB:

Ligar e desligar LEDs através de botões

Controlar cores através de variáveis de estado

Exibição no Display OLED:

Exibir caracteres digitados pelo usuário

Mostrar status dos LEDs (ligado/desligado)

Matriz de LEDs:

Exibir números de 0 a 9 com base na entrada do usuário

Apagar matriz quando um caractere inválido for digitado

Interrupções:

Detectar pressão dos botões e alternar estado dos LEDs

Configuração dos Pinos

I2C (Display OLED)

SDA: GPIO14

SCL: GPIO15

Endereço I2C: 0x3C

UART (Comunicação Serial)

TX: GPIO4

RX: GPIO5

Baud Rate: 115200

GPIOs

Botão A: GPIO5

Botão B: GPIO6

LED Verde: GPIO11

LED Azul: GPIO12

LED Vermelho: GPIO13

Matriz de LEDs: GPIO7

Como Usar

Conecte os componentes conforme o esquema de ligação.

Compile e carregue o código no Raspberry Pi Pico.

Interaja com os botões para alternar o estado dos LEDs.

Digite um número no terminal para exibi-lo na matriz de LEDs.

Observe o status dos LEDs e caracteres exibidos no display OLED.

Estrutura do Código

main(): Inicializa os periféricos e gerencia o loop principal.

initialize_gpio(): Configura os GPIOs e define seus modos de entrada/saída.

acendeRGB(bool r, bool g, bool b): Controla o estado dos LEDs RGB.

botao_interrupcao(uint gpio, uint32_t events): Gerencia as interrupções dos botões.

link para acessar o video postado no youtube: https://youtu.be/X_TyZ2lQr5Y

padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b): Atualiza a matriz de LEDs com base no padrão selecionado.

