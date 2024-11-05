// Program controlling 3 seperate LEDs using the HC-05 Bluetooth Module and using UART for serial communication.

#include <string.h>;
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"


UART_HandleTypeDef huart1;
GPIO_InitTypeDef GPIO_InitStruct;

void SystemClock_Config(void);
void UART1_Init(void);
void LED_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();  // Configure system clock to 84 MHz
    UART1_Init();          // Initialize UART1
    LED_Init();            // Initialize LEDs

    char rxBuffer[10];     // Buffer to hold incoming data
    int index = 0;

    while (1) {
        uint8_t rxData;

        // Receive data byte-by-byte
        if (HAL_UART_Receive(&huart1, &rxData, 1, HAL_MAX_DELAY) == HAL_OK) {
            // Store received character in the buffer
            rxBuffer[index++] = rxData;
            rxBuffer[index] = '\0';  // Null-terminate the string

            // Check for "O1" command to turn LED1 on
            if (strcmp((char *)rxBuffer, "O1") == 0) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);  // Turn LED1 on
                index = 0;  // Reset buffer index
            }
            // Check for "X1" command to turn LED1 off
            else if (strcmp((char *)rxBuffer, "X1") == 0) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);  // Turn LED1 off
                index = 0;  // Reset buffer index
            }
            // Check for "O2" command to turn LED2 on
            else if (strcmp((char *)rxBuffer, "O2") == 0) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);  // Turn LED2 on
                index = 0;  // Reset buffer index
            }
            // Check for "X2" command to turn LED2 off
            else if (strcmp((char *)rxBuffer, "X2") == 0) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);  // Turn LED2 off
                index = 0;  // Reset buffer index
            }
            // Check for "O3" command to turn LED3 on
            else if (strcmp((char *)rxBuffer, "O3") == 0) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);  // Turn LED3 on
                index = 0;  // Reset buffer index
            }
            // Check for "X3" command to turn LED3 off
            else if (strcmp((char *)rxBuffer, "X3") == 0) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);  // Turn LED3 off
                index = 0;  // Reset buffer index
            }

            // Reset index if buffer is full or newline character is received
            if (index >= 9 || rxData == '\n') {
                index = 0;
            }
        }
    }
}

// UART1 Initialization Function
void UART1_Init(void) {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;          // Set baud rate to 9600
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&huart1);
}

// LED Initialization Function
void LED_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Initialize LED1 (PA1) and LED2 (PA4)
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize LED3 (PB0)
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// System Clock Configuration Function for 84 MHz
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main PLL to reach 84 MHz system clock
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;  // Optional, based on USB or other peripherals

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        // Initialization Error
        while (1);
    }

    // Configure the clock dividers for AHB, APB1, and APB2
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // APB1 at 42 MHz (max allowed)
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // APB2 at 84 MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        // Initialization Error
        while (1);
    }
}
