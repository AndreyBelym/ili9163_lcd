#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "LCD_Config.h"
#include "delay.h"



//�����Գ���ʹ�õ���ģ��SPI�ӿ�����
//�����ɸ��Ľӿ�IO���ã�ʹ����������4 IO������ɱ���Һ��������ʾ
/******************************************************************************
�ӿڶ�����Lcd_Driver.h�ڶ��壬����ݽ����޸Ĳ��޸���ӦIO��ʼ��LCD_GPIO_Init()
#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_9  //PB9--->>TFT --BL
#define LCD_RS         	GPIO_Pin_10	//PB11--->>TFT --RS/DC
#define LCD_CS        	GPIO_Pin_11  //PB11--->>TFT --CS/CE
#define LCD_RST     		GPIO_Pin_12	//PB10--->>TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13--->>TFT --SCL/SCK
#define LCD_SDA        	GPIO_Pin_15	//PB15 MOSI--->>TFT --SDA/DIN
*******************************************************************************/


//Һ��IO��ʼ������
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
* ��    �ƣ�void ili9320_WriteIndex(u16 idx)
* ��    �ܣ�д ili9320 �������Ĵ�����ַ
* ��ڲ�����idx   �Ĵ�����ַ
* ���ڲ�������
* ˵    ��������ǰ����ѡ�п��������ڲ�����
* ���÷�����ili9320_WriteIndex(0x0000);
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
   u8 i=0;
   //SPI д����ʱ��ʼ
   LCD_CS_CLR;
   LCD_RS_CLR;
   //ˢ��100��  32��   320ms/��
   for(i=8;i>0;i--)
   {
      if(Index&0x80)	
	  LCD_SDA_SET; //�������
      else 
	  LCD_SDA_CLR; 

      LCD_SCL_CLR;      
      LCD_SCL_SET;
      Index<<=1; 
   }
   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�void ili9320_WriteData(u16 dat)
* ��    �ܣ�д ili9320 �Ĵ�������
* ��ڲ�����dat     �Ĵ�������
* ���ڲ�������
* ˵    �����������ָ����ַд�����ݣ�����ǰ����д�Ĵ�����ַ���ڲ�����
* ���÷�����ili9320_WriteData(0x1030)
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
   u8 i=0;
   LCD_CS_CLR;
   LCD_RS_SET;
    //ˢ��100��  32��   320ms/��
   for(i=8;i>0;i--)
   {
      if(Data&0x80)	
	  LCD_SDA_SET; //�������
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
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�,Y_IncMode��ʾ������y������x
����ֵ����
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
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(u16 x,u16 y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�������
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);

}    

/*****************************************
 �������ܣ���TFTĳһ�����ɫ                          
 ���ڲ�����color  ����ɫֵ                                 
******************************************/
unsigned int Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//���������ֽ�
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}
/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
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

