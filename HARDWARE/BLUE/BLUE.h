#ifndef __BLUE_H
#define __BLUE_H

// ================= 宏定义 =================
#define HC05_MASTER_KEY_PORT GPIOA
#define HC05_MASTER_KEY_PIN  GPIO_Pin_8  // 连接主机 EN
#define HC05_SLAVE_KEY_PORT  GPIOA
#define HC05_SLAVE_KEY_PIN   GPIO_Pin_4  // 连接从机 EN
extern void Delay_ms(uint32_t xms);

void UART_SendString(USART_TypeDef* USARTx, char* str); 	
void HC05_Config_Init(void);
void HC05_AutoSetup(void);
void Serial_DataMode_Init(void) ;



#endif
