//static 关键字表示这个函数是“私有”的。
//   只能在这个 .c 文件里被调用，外部 main 函数看不到它。不应该被别的模块调用，防止命名污染
//   因为 NVIC 配置通常是初始化的一部分，不需要单独暴露给外部。
#include "bsp_usart.h"

char RxBuffer1[100]; // 主机接收缓冲
char RxBuffer2[100]; // 从机接收缓冲
uint8_t RxFlag1 = 0;
uint8_t RxFlag2 = 0;



static void NVIC_Configuration(void)				//为什么中断服务函数写在stm32f10x_it.c
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 设置中断优先级分组 (整个工程只设置一次，通常放在main开头，这里为了封装放在这)	//USART_ITConfig：外设层
																																												//NVIC_Init：内核层
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // 2. 配置 USART1 中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          // 指定中断源：串口1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // 开启中断
    
    NVIC_Init(&NVIC_InitStructure);
	
}

/* 这是真正的初始化函数，所有逻辑都在这里 
*/
void usart_Init(uint32_t bound)
{
	//	声明结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // ==========================================
    // 第一步：开启时钟 (最容易忘记的一步！)
    // ==========================================
	
	
	
    // 开启 GPIOA 的时钟 (因为串口1在 PA9, PA10)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // 开启 USART1 外设的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	

    // ==========================================
    // 第二步：配置 串口TX,RX的GPIO 引脚
    // ==========================================
    
	
	
    // 配置 PA9 (TX 发送脚)
    // 解析：为什么是复用推挽(AF_PP)? 
    // 因为这个引脚的控制权要交给串口外设，而不是普通的GPIO高低电平控制。
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化GPIOA.9 ，&是取地址的意思，
		
		
		
    // 配置 PA10 (RX 接收脚)
    // 解析：为什么是浮空输入(IN_FLOATING)?
    // 标准库推荐配置。也可以用上拉输入，但浮空更通用。
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ==========================================
    // 第三步：配置 USART 参数
    // ==========================================
    USART_InitStructure.USART_BaudRate = 9600;                  // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;   // 8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;        // 1位停止
    USART_InitStructure.USART_Parity = USART_Parity_No;           // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 同时允许发送和接收

    // 将配置写入寄存器
		USART_Init(USART1, &USART_InitStructure); 										//USART_Init (大写 U)：这才是 ST 官方标准库 的函数。
		USART_Init(USART2, &USART_InitStructure); 																												//它才需要 USART_TypeDef* 和 USART_InitTypeDef* 这两个参数。

    // ==========================================
    // 第四步：配置中断 (如果不需要接收数据，这段可以不要)
    // ==========================================
    NVIC_Configuration(); // 调用上面的私有函数配置优先级
    
    // 开启“接收缓冲区非空”中断 (RXNE)
    // 意思就是：一旦收到数据，就产生中断通知 CPU
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // ==========================================
    // 第五步：开启串口 (总开关)
    // ==========================================
    USART_Cmd(USART1, ENABLE);
		
		
}


/* 发送一个字节的封装函数
*/
void USART1_SendByte(uint8_t ch)
{
    USART_SendData(USART1, ch);
    // 必须等待，否则发太快会丢数据
    // TXE = Transmit Data Register Empty (发送寄存器空)
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}


/*	串口中断1的回调函数 接收中断的通知	*/
void USART1_RxEvent(void)
{
	//    uint8_t ch;//这里的变量不会影响到printf发送的数据的，但是为了规范，用了局部变量idx，
	//两个中断的idx一样不影响，局部概念。
		static uint8_t idx = 0;
		char data = USART_ReceiveData(USART1);
		if (data == '\n' || idx >= 99) { // 遇到换行或满
            RxBuffer1[idx] = '\0';
            idx = 0;
            RxFlag1 = 1;
        } else if (data != '\r') { // 忽略 \r
            RxBuffer1[idx++] = data;
        }

    /* 真正读 DR 的地方在模块里 */
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    // 你要做什么：
    // - 放缓冲
    // - DMA
    // - 标志位
}

//uint8_t usart1_rx_buf[USART1_RX_BUF_SIZE];
//uint16_t usart1_rx_len = 0;
//void USART1_IdleEvent(void)
//{
//if (usart1_rx_len > 0)
//{
////Bluetooth_FrameReady(usart1_rx_buf, usart1_rx_len);
//usart1_rx_len = 0;
//}
//}

void USART2_RxEvent(void)
{
	
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
			{
				static uint8_t idx = 0;
        char data = USART_ReceiveData(USART2);
        if (data == '\n' || idx >= 99) {
            RxBuffer2[idx] = '\0';
            idx = 0;
            RxFlag2 = 1;
        } 
				else if (data != '\r') 
						{	
            RxBuffer2[idx++] = data;
						}
						USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
/*
   重定向 printf 函数     实现“不接 TX/RX 也能看 printf”
   C语言标准库的 printf 底层会调用 fputc，
   我们在这里重写 fputc，把数据塞给串口。
*/
int fputc(int ch, FILE *f)
{
    // 等待上一个字符发送完成
    while((USART1->SR & USART_FLAG_TXE) == 0);
    // 把当前字符丢进串口1的发送寄存器
    USART1->DR = (uint8_t)ch;
    return ch;
}

//int fputc(int ch, FILE *f)
//{
//    // 直接把字符丢进 ITM 调试通道 0
//    // 数据会通过 SWDIO 线传回 Keil，而不是走 PA9 引脚
//    if (CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) {
//        while (ITM->PORT[0].u32 == 0);
//        ITM->PORT[0].u8 = (uint8_t)ch;
//    }
//    
//    /* 如果你以后接了 TX/RX 线，想让串口和 Keil 窗口同时显示，可以保留下面这两行 */
//    // while((USART1->SR & 0X40) == 0);
//    // USART1->DR = (u8) ch;

//    return ch;
//]
