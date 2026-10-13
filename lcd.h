#include<LPC21XX.H>
#include"delay.h"
#define LCD_D 0xff<<0
#define RS 1<<8
#define E 1<<9
void LCD_INIT(void);
void LCD_COMMAND(unsigned char);
void LCD_DATA(unsigned char);
void LCD_STR(unsigned char *);
void LCD_Clear(void);

void LCD_INIT(void)
{
IODIR0|=LCD_D|RS|E;
LCD_COMMAND(0X01);
LCD_COMMAND(0X02);
LCD_COMMAND(0X0c);
LCD_COMMAND(0X38);

}

void LCD_STRING(unsigned char *s)
{
char pos;
while(*s)
{
LCD_DATA(*s++);
pos++;
if(pos==16)
{
LCD_COMMAND(0xc0);
}
}
}

void LCD_COMMAND(unsigned char cmd)
{
IOCLR0=LCD_D;
IOSET0=cmd;
IOCLR0=RS;
IOSET0=E;
delay_ms(2);
IOCLR0=E;
}

void LCD_DATA(unsigned char d)
{
IOCLR0=LCD_D;
IOSET0=d;
IOSET0=RS;
IOSET0=E;
delay_ms(2);
IOCLR0=E;
}

void LCD_STR(unsigned char *s)
{
while(*s)
LCD_DATA(*s++);
}

void LCD_Clear(void)
{
    LCD_COMMAND(0x01);
    delay_ms(2);
}

