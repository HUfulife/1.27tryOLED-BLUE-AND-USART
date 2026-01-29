#include "stm32f10x.h"
#include "BLUE.h"
	// ================= 蓝牙配置相关函数 =================

// 阻塞式发送字符串
void UART_SendString(USART_TypeDef* USARTx, char* str) {
    while (*str) {
        USART_SendData(USARTx, *str++);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}

// 步骤1: 初始化GPIO和串口用于AT配置 (波特率38400)
void HC05_Config_Init(void) 
	{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // UART1 (Master) PA9/PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // UART2 (Slave) PA2/PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 或 IPU
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // KEY (EN) 引脚: PA8, PA4 推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 串口配置 (AT模式必须 38400)
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 38400; 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

// 步骤2: 执行自动配置
void HC05_AutoSetup(void) {
    // 拉高 EN 进入 AT 模式
    GPIO_SetBits(HC05_MASTER_KEY_PORT, HC05_MASTER_KEY_PIN);
    GPIO_SetBits(HC05_SLAVE_KEY_PORT, HC05_SLAVE_KEY_PIN);
    Delay_ms(50); // 等待模块反应

    // === 配置从机 (USART2) ===
    UART_SendString(USART2, "AT+ORGL\r\n");      Delay_ms(200);
    UART_SendString(USART2, "AT+ROLE=0\r\n");    Delay_ms(100); // 0=Slave
    UART_SendString(USART2, "AT+PSWD=\"1234\"\r\n"); Delay_ms(100);
    UART_SendString(USART2, "AT+UART=9600,0,0\r\n"); Delay_ms(100);
    // UART_SendString(USART2, "AT+RESET\r\n");  // 稍后统一复位

    // === 配置主机 (USART1) ===
    UART_SendString(USART1, "AT+ORGL\r\n");      Delay_ms(200);
    UART_SendString(USART1, "AT+ROLE=1\r\n");    Delay_ms(100); // 1=Master
    UART_SendString(USART1, "AT+CMODE=1\r\n");   Delay_ms(100); // 任意地址连接
    UART_SendString(USART1, "AT+PSWD=\"1234\"\r\n"); Delay_ms(100);
    UART_SendString(USART1, "AT+UART=9600,0,0\r\n"); Delay_ms(100);
    
    // 拉低 EN 退出 AT 模式
    GPIO_ResetBits(HC05_MASTER_KEY_PORT, HC05_MASTER_KEY_PIN);
    GPIO_ResetBits(HC05_SLAVE_KEY_PORT, HC05_SLAVE_KEY_PIN);
    Delay_ms(100);

    // 发送复位指令让配置生效 (此时虽然KEY拉低了，但还在38400连接中，发送RESET最稳妥)
    UART_SendString(USART1, "AT+RESET\r\n");
    UART_SendString(USART2, "AT+RESET\r\n");
    
    Delay_ms(1000); // 等待复位重连
}

// 步骤3: 重新初始化串口为数据模式 (9600) 并开启中断
void Serial_DataMode_Init(void) {
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600; // 切换回9600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Init(USART2, &USART_InitStructure);

    // 开启中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // NVIC配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;

    // USART1
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // USART2
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
}
