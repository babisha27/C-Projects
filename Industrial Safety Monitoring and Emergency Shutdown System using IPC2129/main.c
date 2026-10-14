#include<LPC21XX.H>
#include"lcd.h"

#define SENSOR 1<<10
#define LED 1<<11

#define MOTOR_IN1 1<<12
#define MOTOR_IN2 1<<13
#define MOTOR_EN  1<<14

#define EINT0_PIN 1<<16

volatile unsigned int emergency=0;

void Motor_Init(void)
{
IODIR0 |= MOTOR_IN1| MOTOR_IN2 |MOTOR_EN;
IOCLR0 =  MOTOR_IN1 |MOTOR_IN2 |MOTOR_EN;
}

void Motor_ON(void)
{
IOSET0=MOTOR_EN;
IOSET0=MOTOR_IN1;
IOCLR0=MOTOR_IN2;
}

void Motor_OFF(void)
{
IOCLR0=MOTOR_EN;
IOCLR0=MOTOR_IN1 | MOTOR_IN2;
}

void LED_Init(void)
{
IODIR0 |=LED;
IOSET0 =LED;
}

void LED_ON(void)
{
IOCLR0=LED;
}

void LED_OFF(void)
{
IOSET0=LED;
}

void Sensor_Init(void)
{
 IODIR0 &=~(SENSOR);
 }

void EINT0_ISR(void)__irq
{
emergency=1;
Motor_OFF();
LED_ON();
EXTINT=0X01;
VICVectAddr=0;
}

void EINT0_Init(void)
{
PINSEL1 &=~(3<<0);
PINSEL1 |=1<<0;
EXTMODE |=0X01;
EXTPOLAR &=~(0X01);
VICIntSelect &=~(1<<14);
VICVectAddr0=(unsigned long)EINT0_ISR;
VICVectCntl0=(0X20)|14;
VICIntEnable |=1<<14;
}

int main()
{
LCD_INIT();
Motor_Init();
LED_Init();
Sensor_Init();
EINT0_Init();
__enable_irq();
LCD_Clear();
LCD_COMMAND(0X80);
LCD_STR("INDUSTRIAL");
LCD_COMMAND(0XC0);
LCD_STR("SAFETY SYSTEM");
delay_ms(2000);
LCD_Clear();

while(1)
{
if(emergency==1)
{
Motor_OFF();
LED_ON();
LCD_Clear();
LCD_COMMAND(0X80);
LCD_STR("EMERGENCY");
LCD_COMMAND(0XC0);
LCD_STR("MOTOR STOPPED");
while(1)
{
Motor_OFF();
LED_ON();
}
}

if((IOPIN0 & SENSOR)==0)
{
Motor_OFF();
LED_ON();
LCD_Clear();
LCD_COMMAND(0X80);
LCD_STR("DANGER");
LCD_COMMAND(0XC0);
LCD_STR("MOTOR STOPPED");
delay_ms(500);
}

else
{
Motor_ON();
LED_OFF();
LCD_Clear();
LCD_COMMAND(0X80);
LCD_STR("NORMAL");
LCD_COMMAND(0XC0);
LCD_STR("MOTOR RUNNING");
delay_ms(500);
}
}
}















