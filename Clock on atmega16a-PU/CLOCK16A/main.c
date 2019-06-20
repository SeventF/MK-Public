/*
 * simple clock on atmega 16a
 *
 * Created: 20.06.2019 17:28:57
 * Author : SeveNT
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include "LCD1602.h"

uint8_t ClockSym[8]= {0x0,0xE,0x15,0x17,0x11,0xE,0x0};// character of clock
//uint8_t  Dynamic[8]= {0x1,0x12,0x18,0x1b,0x18,0x12,0x1};//alarm clock character
bool CheckDots = true;
unsigned char Sec,Min,Hour = 0;
char TimeBuffer[2];

void TIMER_INI();
void PORTS_INI();

int main(void)
{
	DDRC = 0xFF;
	
	PORTS_INI();
	LCD_INI();
	TIMER_INI();
	SetPos(1, 0);
	SendStr("00:00:00");
	SetCgramAddress(0x0F);
	for(uint8_t i=0;i<8;i++){
		SendByte(ClockSym[i],1);
	}	
	SetDdramAddress(0x08);
	SetPos(0,0);
	SendByte(0x02,1);
	/*---------------alarm clock character output block------------
	SetCgramAddress(0x08);
	for(uint8_t i=0;i<8;i++){
		SendByte(Dynamic[i],1);
	}	
	SetDdramAddress(0x00);
	SetPos(9,0);
	SendByte(0x01,1);		
	*///-------------alarm clock character output block----------
		
	while (ASSR&((1<<TCN2UB)|(1<<OCR2UB)|(1<<TCR2UB))); //We wait for readiness of Timer2
	TIMSK |=(1<<TOIE2);// Include interruption on overflow
	sei(); //Allow interruptions
	
    /* Replace with your application code */
 while (1)
 {
	 if(!(PINA&(1<<PORTA1))){
		 _delay_ms(400); //Remove noise of the button
		 Hour++;
		 Sec=0;
		 if(Hour<=9) {SetPos(2,0);SendStr(itoa(Hour,TimeBuffer,10));}
		  else       {SetPos(1,0);SendStr(itoa(Hour,TimeBuffer,10));}
	 }
	 if(!(PINA&(1<<PORTA2))){
		 _delay_ms(400); //Remove noise of the button-----------------+
		 Min++;
		 Sec=0;
		 if(Min<=9)  {SetPos(5,0);SendStr(itoa(Min,TimeBuffer,10));}
		  else       {SetPos(4,0);SendStr(itoa(Min,TimeBuffer,10));}
	 }

 }
 }
//----------------------------
void TIMER_INI(){
	TIMSK &=~(1<<OCIE2)|(1<<TOIE2);//Prohibit interruptions of Timer2
	ASSR |=(1<<AS2);// Include an asynchronous mode of external quartz
	TCNT2 = 0x00; //We reset the counter
	TCCR2 |=(1<<CS20)|(1<<CS22);//Installation of a predivider (32768/128=256tiks per second)
}
//----------------------------
ISR(TIMER2_OVF_vect){
	Sec++;
	
	if(Sec<=9){SetPos(8,0);SendStr(itoa(Sec,TimeBuffer,10));}
	 else	  {SetPos(7,0);SendStr(itoa(Sec,TimeBuffer,10));}
	if(Sec==60){
		Sec=0; SetPos(7,0);SendStr(itoa(Sec,TimeBuffer,10));
		Min++;
		if(Min<=9){SetPos(5,0);SendStr(itoa(Min,TimeBuffer,10));}
		 else     {SetPos(4,0);SendStr(itoa(Min,TimeBuffer,10));}
	}
	if(Min==60){
		Min=0; SetPos(4,0);SendStr(itoa(Min,TimeBuffer,10));
		Hour++;
		if(Hour<=9){SetPos(2,0);SendStr(itoa(Hour,TimeBuffer,10));}
		 else      {SetPos(1,0);SendStr(itoa(Hour,TimeBuffer,10));}
	}
	if(Hour==24) {Sec=0;Min=0;Hour=0;SetPos(1,0);SendStr("00:00:00");}
	if(CheckDots){SetPos(3,0);SendStr(" ");SetPos(6,0);SendStr(" ");CheckDots=false;}
	 else        {SetPos(3,0);SendStr(":");SetPos(6,0);SendStr(":");CheckDots=true;}
}
//----------------------------
void PORTS_INI(){
		DDRD  = 0xFF;
		PORTD = 0x00;
		DDRA  = 0x00;
		PORTA|=(1<<PORTA1)|(1<<PORTA2);			
}
//----------------------------