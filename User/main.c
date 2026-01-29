#include "main.h"
int main(void) {
    // 1. 初始化 I2C
    MyI2C_Init();
    
    // 2. 蓝牙配置阶段 (38400波特率, 拉高EN)
    HC05_Config_Init();
    HC05_AutoSetup(); // 自动配置主从
    
    // 3. 切换到数据通信阶段 (9600波特率, 开启中断)
    Serial_DataMode_Init();

    while (1) {
        // 检查主机是否收到消息
        if (RxFlag1==1) {
            // 在这里处理主机收到的数据 (RxBuffer1)
            // 比如转发给 OLED 显示
            RxFlag1 = 0;
        }

        // 检查从机是否收到消息
        if (RxFlag2==1) {
            // 在这里处理从机收到的数据 (RxBuffer2)
            RxFlag2 = 0;
        }
        
         //测试：发送心跳包
        UART_SendString(USART1, "Hello from Master\r\n");
         Delay_ms(1000);
    }
}
