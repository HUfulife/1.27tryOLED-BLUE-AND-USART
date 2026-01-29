#include "SHT20.h"

void sht20_GPIO_init(void)
{
	//SCL PB12  SDA PB13
	GPIO_InitTypeDef sht20;  //定义结构体变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
	sht20.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	sht20.GPIO_Mode = GPIO_Mode_Out_PP; 	//输出模式
	sht20.GPIO_Speed = GPIO_Speed_50MHz;    //快速
	GPIO_Init(GPIOB,&sht20);           //选中PA组并sht20SCL参数初始配置
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);   //固定输出高电平
}
void sht20_init(void)
{

	sht20_GPIO_init();

	IIC_Startsht();//起始条件
	IIC_WriteBytesht(0x80);//器件地址地址+写0x80
	IIC_Wait_ACKsht();//等待应答
	IIC_WriteBytesht(0xfe);//写软复位命令0xfe
	IIC_Wait_ACKsht();//等待应答
	IIC_WriteBytesht(0xE6);//写用户寄存器0xe6
	IIC_Wait_ACKsht();//等待应答
	IIC_WriteBytesht(0x83);//设置分辨率0x83
	IIC_Stopsht();//停止条件
	
}
 float sht20_Readhumidity(void)
{
	float hum;
	u32 msb;
	u8 lsb;

	IIC_Startsht();//起始条件
	IIC_WriteBytesht(0x80);//器件地址地址+写0x80
	if(IIC_Wait_ACKsht()==0)//等待应答
	{
		IIC_WriteBytesht(0xf5);//写相对湿度指令0xf5
		if(IIC_Wait_ACKsht()==0)//等待应答
		{
			do
			{
				IIC_Startsht();//起始条件
				IIC_WriteBytesht(0x81);//器件地址地址+读0x81
			}
			
			while(IIC_Wait_ACKsht()==1);
			msb=IIC_ReadBytesht(1);//读取高位数据，定义一个变量接收MSB
			lsb=IIC_ReadBytesht(0);//读取低位数据，定义一个变量接收LSB
			IIC_Stopsht();//停止条件
			lsb &= 0xfc;//清除状态位
			msb=(msb<<8)|lsb;//获取14位有效数据
			hum=((float)msb/65536)*125-6; //公式换算
			return hum;
		}
	}
	return 0;
}

 float sht20_ReadTemp(void)
{
	float Temp;
	u32 msb;
	u8 lsb;

	IIC_Startsht();//起始条件
	IIC_WriteBytesht(0x80);//器件地址地址+写0x80
	if(IIC_Wait_ACKsht()==0)//等待应答
	{
		IIC_WriteBytesht(0xf3);//写相对温度指令0xf3
		if(IIC_Wait_ACKsht()==0)//等待应答
		{
			do
			{
				IIC_Startsht();//起始条件
				IIC_WriteBytesht(0x81);//器件地址地址+读0x81
			}
			while(IIC_Wait_ACKsht()==1);
			msb=IIC_ReadBytesht(1);//读取高位数据，定义一个变量接收MSB
			lsb=IIC_ReadBytesht(0);//读取低位数据，定义一个变量接收LSB
			IIC_Stopsht();//停止条件
			lsb &= 0xfc;//清除状态位
			msb=(msb<<8)|lsb;//获取14位有效数据
			Temp=((float)msb/65536)*175.72f-46.85f; //公式换算
			return Temp;
		}
	}
	return 0;
}

