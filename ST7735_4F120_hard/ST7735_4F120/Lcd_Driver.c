#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "LCD_Config.h"
#include "delay.h"



//本测试程序使用的是模拟SPI接口驱动
//可自由更改接口IO配置，使用任意最少4 IO即可完成本款液晶驱动显示
/******************************************************************************
接口定义在Lcd_Driver.h内定义，请根据接线修改并修改相应IO初始化LCD_GPIO_Init()
#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
#define LCD_LED        	GPIO_Pin_9  //PB9--->>TFT --BL
#define LCD_RS         	GPIO_Pin_10	//PB11--->>TFT --RS/DC
#define LCD_CS        	GPIO_Pin_11  //PB11--->>TFT --CS/CE
#define LCD_RST     		GPIO_Pin_12	//PB10--->>TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13--->>TFT --SCL/SCK
#define LCD_SDA        	GPIO_Pin_15	//PB15 MOSI--->>TFT --SDA/DIN
*******************************************************************************/


//液晶IO初始化配置
void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
      
}


/****************************************************************************
* 名    称：void ili9320_WriteIndex(u16 idx)
* 功    能：写 ili9320 控制器寄存器地址
* 入口参数：idx   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
* 调用方法：ili9320_WriteIndex(0x0000);
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
   u8 i=0;
   //SPI 写命令时序开始
   LCD_CS_CLR;
   LCD_RS_CLR;
   //刷屏100次  32秒   320ms/屏
   for(i=8;i>0;i--)
   {
      if(Index&0x80)	
	  LCD_SDA_SET; //输出数据
      else 
	  LCD_SDA_CLR; 

      LCD_SCL_CLR;      
      LCD_SCL_SET;
      Index<<=1; 
   }
   LCD_CS_SET;
}

/****************************************************************************
* 名    称：void ili9320_WriteData(u16 dat)
* 功    能：写 ili9320 寄存器数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，调用前需先写寄存器地址，内部函数
* 调用方法：ili9320_WriteData(0x1030)
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
   u8 i=0;
   LCD_CS_CLR;
   LCD_RS_SET;
    //刷屏100次  32秒   320ms/屏
   for(i=8;i>0;i--)
   {
      if(Data&0x80)	
	  LCD_SDA_SET; //输出数据
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
   }
   LCD_CS_SET; 
}

void Lcd_WriteReg(u8 Index,u8 Data)
{
	Lcd_WriteIndex(Index);
   	Lcd_WriteData(Data);
}

void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_ms(100);
	LCD_RST_SET;
	delay_ms(50);
}


void Lcd_Init(u8 dir)
{	
	int i=0;
	LCD_GPIO_Init();
	Lcd_Reset();

	Lcd_WriteIndex(0xB9);//SETEXTC
	Lcd_WriteData(0xFF);
	Lcd_WriteData(0x83);
	Lcd_WriteData(0x53);
	
	Lcd_WriteIndex(0xB0);//RADJ
	Lcd_WriteData(0x3C);
	Lcd_WriteData(0x01);
	
	
	Lcd_WriteIndex(0xB6);//VCOM
	Lcd_WriteData(0x94);
	Lcd_WriteData(0x6C);
	Lcd_WriteData(0x50);   
	
	Lcd_WriteIndex(0xB1);//PWR
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x1B);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x77);
	Lcd_WriteData(0x89);
	
	Lcd_WriteIndex(0xE0); //Gamma setting for tpo Panel
	Lcd_WriteData(0x50);
	Lcd_WriteData(0x77);
	Lcd_WriteData(0x40);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0xBF);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x0F);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x73);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x72);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0xB0);
	Lcd_WriteData(0x0F);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x0F);
		
	Lcd_WriteIndex(0x3A);   
	Lcd_WriteData(0x05);  //05 
	Lcd_WriteIndex(0x36);    
	Lcd_WriteData(0xC0); //83  //0B 
		
	Lcd_WriteIndex(0x11); // SLPOUT  
	delay_ms(150);	
	Lcd_WriteIndex(0x29);    // display on

	delay_ms(150);
	Lcd_WriteIndex(0x2D);  //Look up table
	for(i=0;i<32;i++)
	 {Lcd_WriteData(2*i);} //Red
	for(i=0;i<64;i++)
	 {Lcd_WriteData(1*i);} //Green
	for(i=0;i<32;i++)
	 {Lcd_WriteData(2*i);} //Blue 
	
	Lcd_WriteIndex(0x2c);  
	delay_ms(150);	 


}


/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点,Y_IncMode表示先自增y再自增x
返回值：无
*************************************************/
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{		
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end);

	Lcd_WriteIndex(0x2c);

}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 x,u16 y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);

}    

/*****************************************
 函数功能：读TFT某一点的颜色                          
 出口参数：color  点颜色值                                 
******************************************/
unsigned int Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}
/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteIndex(0x2C);
   for(i=0;i<128;i++)
    for(m=0;m<160;m++)
    {	
	  	Lcd_WriteData(Color>>8);
			Lcd_WriteData(Color);
    }   
}

