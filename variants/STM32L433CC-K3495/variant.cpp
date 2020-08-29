/*
 * Copyright (c) 2016 Thomas Roell.  All rights reserved.
 * Copyright (c) 2019 Fredrik Hubinette.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimers.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimers in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Thomas Roell, nor the names of its contributors
 *     may be used to endorse or promote products derived from this Software
 *     without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 */

#include "Arduino.h"
#include "stm32l4_wiring_private.h"

#define PWM_INSTANCE_TIM1      0
#define PWM_INSTANCE_TIM2      1
#define PWM_INSTANCE_TIM15     2
#define PWM_INSTANCE_TIM16     3

/*
 * Pins descriptions
 */
extern const PinDescription g_APinDescription[NUM_TOTAL_PINS] =
{
  // 0, Not Connected
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA4),  GPIO_PIN_PA4,             (PIN_ATTR_ADC | PIN_ATTR_DAC | PIN_ATTR_EXTI),   PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_9    },
  // 1, Not Connected
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB10), GPIO_PIN_PB10_TIM2_CH3,   (PIN_ATTR_PWM | PIN_ATTR_EXTI),                  PWM_INSTANCE_TIM2,  PWM_CHANNEL_3,    ADC_INPUT_NONE },
  // 2, I2S RCLK
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB12), GPIO_PIN_PB12,            (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 3, I2S BCLK
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB13), GPIO_PIN_PB13,            (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 4, MEM CS
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB14), GPIO_PIN_PB14,            (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 5, Not Connected
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB15), GPIO_PIN_PB15_TIM15_CH2,  (PIN_ATTR_PWM | PIN_ATTR_EXTI),                  PWM_INSTANCE_TIM15, PWM_CHANNEL_2,    ADC_INPUT_NONE },
  // 6, Not Connected
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB0),  GPIO_PIN_PB0_TIM1_CH2N,   (PIN_ATTR_ADC | PIN_ATTR_PWM | PIN_ATTR_EXTI),   PWM_INSTANCE_TIM1,  PWM_CHANNEL_2,    ADC_INPUT_15   },
  // 7, I2C SDA
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB9),  GPIO_PIN_PB9,             (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 8, RX
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA15), GPIO_PIN_PA15_TIM2_CH1,   (PIN_ATTR_EXTI),                                 PWM_INSTANCE_TIM2,  PWM_CHANNEL_1,    ADC_INPUT_NONE },
  // 9, TX
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA2),  GPIO_PIN_PA2_TIM2_CH3,    (PIN_ATTR_PWM | PIN_ATTR_EXTI | PIN_ATTR_WKUP4), PWM_INSTANCE_TIM2,  PWM_CHANNEL_3,    ADC_INPUT_NONE },
  // 10, LED Channel 4 (Blue)
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB1),  GPIO_PIN_PB1_TIM1_CH3N,   (PIN_ATTR_ADC | PIN_ATTR_PWM | PIN_ATTR_EXTI),   PWM_INSTANCE_TIM1,  PWM_CHANNEL_3,    ADC_INPUT_16   },
  // 11, MEM Reset
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB11), GPIO_PIN_PB11_TIM2_CH4,   (PIN_ATTR_PWM | PIN_ATTR_EXTI),                  PWM_INSTANCE_TIM2,  PWM_CHANNEL_4,    ADC_INPUT_NONE },
  // 12, Motion Interrupt
  { GPIOC, GPIO_PIN_MASK(GPIO_PIN_PC13), GPIO_PIN_PC13,           (PIN_ATTR_EXTI | PIN_ATTR_WKUP2),                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 13, LED Channel 1 (Green)
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA1),  GPIO_PIN_PA1_TIM15_CH1N,  (PIN_ATTR_PWM | PIN_ATTR_EXTI),                  PWM_INSTANCE_TIM15, PWM_CHANNEL_1,    ADC_INPUT_NONE },
  // 14, Vtest (battery voltage measurement)
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA3),  GPIO_PIN_PA3_TIM2_CH4,    (PIN_ATTR_ADC | PIN_ATTR_PWM | PIN_ATTR_EXTI),   PWM_INSTANCE_TIM2,  PWM_CHANNEL_4,    ADC_INPUT_8    },
  // 15, Power Enable
  { GPIOH, GPIO_PIN_MASK(GPIO_PIN_PH0),  GPIO_PIN_PH0,            (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 16, Not Connected
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA0),  GPIO_PIN_PA0_TIM2_CH1,  (PIN_ATTR_ADC|PIN_ATTR_PWM|PIN_ATTR_EXTI|PIN_ATTR_WKUP1), PWM_INSTANCE_TIM2, PWM_CHANNEL_1, ADC_INPUT_5 },
  // 17, Not Connected
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB3),  GPIO_PIN_PB3_TIM2_CH2,    (PIN_ATTR_PWM | PIN_ATTR_EXTI),                  PWM_INSTANCE_TIM2,  PWM_CHANNEL_2,    ADC_INPUT_NONE },
  // 18, LED Channel 3 (White)
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA8),  GPIO_PIN_PA8_TIM1_CH1,    PIN_ATTR_PWM,                                    PWM_INSTANCE_TIM1,  PWM_CHANNEL_1,    ADC_INPUT_NONE },
  // 19, LED Channel 2 (Red)
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB8),  GPIO_PIN_PB8_TIM16_CH1,   (PIN_ATTR_PWM | PIN_ATTR_EXTI),                  PWM_INSTANCE_TIM16, PWM_CHANNEL_1, ADC_INPUT_NONE    },
  // 20, Not Connected
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB7),  GPIO_PIN_PB7,             (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 21, Button 1 (PWR)
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB6),  GPIO_PIN_PB6,             (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 22, Not Connected
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB4),  GPIO_PIN_PB4,             (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 23, Button 2 (AUX)
  { GPIOB, GPIO_PIN_MASK(GPIO_PIN_PB5),  GPIO_PIN_PB5,             (PIN_ATTR_EXTI),                                 PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 24, Amplifier Enable
  { GPIOH, GPIO_PIN_MASK(GPIO_PIN_PH1),  GPIO_PIN_PH1,             0,                                               PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 25, I2C SCL
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA9),  GPIO_PIN_PA9,            (PIN_ATTR_EXTI),                                  PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
  // 26, I2S Data
  { GPIOA, GPIO_PIN_MASK(GPIO_PIN_PA10), GPIO_PIN_PA10,           (PIN_ATTR_EXTI | PIN_ATTR_PWM),                   PWM_INSTANCE_NONE,  PWM_CHANNEL_NONE, ADC_INPUT_NONE },
};

extern const unsigned int g_PWMInstances[PWM_INSTANCE_COUNT] = {
    TIMER_INSTANCE_TIM1,
    TIMER_INSTANCE_TIM2,
    TIMER_INSTANCE_TIM15,
    TIMER_INSTANCE_TIM16,
};

// RX/TX, please use this one.
extern const stm32l4_uart_pins_t g_SerialPins = { GPIO_PIN_PA15_USART2_RX, GPIO_PIN_PA2_USART2_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
extern const unsigned int g_SerialInstance = UART_INSTANCE_USART2;
extern const unsigned int g_SerialMode = UART_MODE_RX_DMA;

// Serial1Pins is the same as Neopixels 2 & 3.
extern const stm32l4_uart_pins_t g_Serial1Pins = { GPIO_PIN_PB11_USART3_RX, GPIO_PIN_PB10_USART3_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
extern const unsigned int g_Serial1Instance = UART_INSTANCE_USART3;
extern const unsigned int g_Serial1Mode = UART_MODE_RX_DMA | UART_MODE_TX_DMA;

// Don't use, would interfere with LED Channel 2 and I2C CL
extern const stm32l4_uart_pins_t g_Serial2Pins = { GPIO_PIN_PA10_USART1_RX, GPIO_PIN_PA9_USART1_TX, GPIO_PIN_NONE, GPIO_PIN_NONE };
extern const unsigned int g_Serial2Instance = UART_INSTANCE_USART1;
extern const unsigned int g_Serial2Mode = UART_MODE_RX_DMA | UART_MODE_TX_DMA;

// SD card
extern const stm32l4_spi_pins_t g_SPIPins = { GPIO_PIN_PA7_SPI1_MOSI, GPIO_PIN_PA6_SPI1_MISO, GPIO_PIN_PA5_SPI1_SCK, GPIO_PIN_NONE };
extern const unsigned int g_SPIInstance = SPI_INSTANCE_SPI1;
extern const unsigned int g_SPIMode = SPI_MODE_RX_DMA | SPI_MODE_TX_DMA | SPI_MODE_RX_DMA_SECONDARY | SPI_MODE_TX_DMA_SECONDARY;

// Please do not use.
extern const stm32l4_spi_pins_t g_SPI1Pins = { GPIO_PIN_PB5_SPI3_MOSI, GPIO_PIN_PB4_SPI3_MISO, GPIO_PIN_PB3_SPI3_SCK, GPIO_PIN_NONE };
extern const unsigned int g_SPI1Instance = SPI_INSTANCE_SPI3;
extern const unsigned int g_SPI1Mode = SPI_MODE_RX_DMA | SPI_MODE_TX_DMA | SPI_MODE_RX_DMA_SECONDARY | SPI_MODE_TX_DMA_SECONDARY;

// Please do not use.
extern const stm32l4_spi_pins_t g_SPI2Pins = { GPIO_PIN_PB15_SPI2_MOSI, GPIO_PIN_PB14_SPI2_MISO, GPIO_PIN_PB13_SPI2_SCK, GPIO_PIN_NONE };
extern const unsigned int g_SPI2Instance = SPI_INSTANCE_SPI2;
extern const unsigned int g_SPI2Mode = 0;

// Main I2C
extern const stm32l4_i2c_pins_t g_WirePins = { GPIO_PIN_PA9_I2C1_SCL, GPIO_PIN_PB9_I2C1_SDA };
extern const unsigned int g_WireInstance = I2C_INSTANCE_I2C1;
extern const unsigned int g_WireMode = I2C_MODE_RX_DMA;

// Please do not use.
extern const stm32l4_i2c_pins_t g_Wire1Pins = { GPIO_PIN_PB13_I2C2_SCL, GPIO_PIN_PB14_I2C2_SDA };
extern const unsigned int g_Wire1Instance = I2C_INSTANCE_I2C2;
extern const unsigned int g_Wire1Mode = 0;

// I2S
extern const stm32l4_sai_pins_t g_SAIPins = { GPIO_PIN_PB13_SAI1_SCK_A, GPIO_PIN_PB12_SAI1_FS_A, GPIO_PIN_PA10_SAI1_SD_A, GPIO_PIN_PB14_SAI1_MCLK_A };
extern const unsigned int g_SAIInstance = SAI_INSTANCE_SAI1A;
extern const unsigned int g_SAIMode = SAI_MODE_DMA | SAI_MODE_DMA_SECONDARY;

// Please do not use.
extern const stm32l4_sai_pins_t g_SAI1Pins = { GPIO_PIN_PB3_SAI1_SCK_B, GPIO_PIN_PB6_SAI1_FS_B, GPIO_PIN_PB5_SAI1_SD_B, GPIO_PIN_PB4_SAI1_MCLK_B };
extern const unsigned int g_SAI1Instance = SAI_INSTANCE_SAI1B;
extern const unsigned int g_SAI1Mode = SAI_MODE_DMA | SAI_MODE_DMA_SECONDARY;

