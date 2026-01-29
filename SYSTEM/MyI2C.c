#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern void Delay_ms(uint32_t xms);

// ================= 软件 I2C 驱动 (OLED) =================
// SCL -> PA6, SDA -> PA7
void MyI2C_W_SCL(uint8_t BitValue) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, (BitAction)BitValue);
    Delay_ms(0); // 极短延时
}

void MyI2C_W_SDA(uint8_t BitValue) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)BitValue);
    Delay_ms(0);
}
void MyI2C_Init(void) 
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7); // 默认拉高
}
