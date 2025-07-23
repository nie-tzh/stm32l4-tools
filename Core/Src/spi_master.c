#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

#define DELAY_US (20)
#define TX_DEVICE_0    (0xA0)
#define TX_DEVICE_1    (0xA1)
#define TX_DEVICE_2    (0xA2)
#define TX_DEVICE_3    (0xA3)
#define SPI_DEVICE_SS1 (0xA4)
#define SPI_DEVICE_SS2 (0xA5)
#define TX_DEVICE_ID    TX_DEVICE_0
#define SPI_DEVICE_SS   SPI_DEVICE_SS1
#define SPI_RO          GPIO_PIN_4
#define SPI_BNE         GPIO_PIN_5
#define SPI_BNE_SET_TIMEOUT (20)

extern SPI_HandleTypeDef hspi1;

void delay_us(uint32_t us)
{
    uint8_t i = 0;

    while (us--) {
        i = 10;
        while(i--);
    }
}

static int wait_gpio_set_until_timeout(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t Timeout)
{
    uint32_t tickstart;
    /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    while (GPIO_PIN_SET != HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)) {
        if ((HAL_GetTick() - tickstart) > Timeout) {
            return -1;
        }
    }

    while (GPIO_PIN_SET != HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)) {
        if ((HAL_GetTick() - tickstart) > Timeout) {
            return -1;
        }
    }

    return 0;
}

int spi_passthrough_data_read_iam20685(uint32_t *times, uint32_t *errtimes)
{
    uint8_t buff[128];
    uint8_t wcmd[] = {TX_DEVICE_ID, SPI_DEVICE_SS, 0x2c, 0x00, 0x00, 0xf3};
    uint8_t rcmd[] = {0xa7, 0xa7, 0xa7, 0xa7, 0xa7};
    uint8_t cmd, rdata, default_value = 0, loopr = 0;
    // step1 Set RO. send A0/A1/A2/A3 A4/A5 ...
    HAL_GPIO_WritePin(GPIOB, SPI_RO, 1);
    HAL_SPI_Transmit(&hspi1, wcmd, sizeof(wcmd), 100);

    // step2 Wait for BNE = 1
    if (wait_gpio_set_until_timeout(GPIOB, SPI_BNE, SPI_BNE_SET_TIMEOUT) != 0) {
        printf("%s %d wait bne set timeout!\n", __func__, __LINE__);
    }

    // step2 Clear RO, Recv data
    HAL_GPIO_WritePin(GPIOB, SPI_RO, 0);
    HAL_SPI_TransmitReceive(&hspi1, rcmd, buff, sizeof(rcmd), 100);

    if ((buff[2] != 0xaa) || (buff[3] != 0x55)) {
        *errtimes += 1;
        printf("error, read:");
        for (uint8_t j = 0; j < sizeof(rcmd); j++) {
            printf("%02x ", buff[j]);
        }
        printf("\r\n");
        HAL_Delay(1000);
    }

    *times += 1;
    if ((*times % 10000) == 0) {
        printf("test %u times, errtimes:%u\r\n", *times, *errtimes);
    }
    
	return 0;
}


void shell_test_spi(void * arg)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint32_t times = 0, errtimes = 0;
	/* Configure GPIO pins : PB4(SPI_RO_O) */
    GPIO_InitStruct.Pin = SPI_RO;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure GPIO pin : PB5(SPI_BNE_I) */
    GPIO_InitStruct.Pin = SPI_BNE;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    for (uint32_t loop = 0; loop < 100000; loop++) {
        spi_passthrough_data_read_iam20685(&times, &errtimes);
    }
    printf("test_times:%u errtimes:%u!\n", times, errtimes);
}