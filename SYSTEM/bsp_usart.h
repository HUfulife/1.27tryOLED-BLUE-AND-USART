#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f10x.h" // 必须引入，否则识别不了USARTTypeDef这些类型
#include <stdio.h>     // 为了使用printf

/* 函数声明：
   1. 告诉编译器，这几个函数在别的地方实现了。
   2. 放在这里，只要其他文件 #include "bsp_usart.h"，就能直接调用这些函数。
*/
// 接收缓冲区
// 告诉所有引用本文件的代码：别处已经有这几个变量了，你们直接用
extern char RxBuffer1[100];
extern char RxBuffer2[100];
extern uint8_t RxFlag1;
extern uint8_t RxFlag2;

void usart_Init(uint32_t bound);
void USART1_SendByte(uint8_t ch);
static void NVIC_Configuration(void);
void USART1_RxEvent(void);
void USART2_RxEvent(void);

#endif
