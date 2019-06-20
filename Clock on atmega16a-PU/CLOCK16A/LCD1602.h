#include <stdlib.h>
#include <util\delay.h>

#define RS   PORTD0
#define E	 PORTD1
#define DB4  PORTD4
#define DB5  PORTD5
#define DB6  PORTD6
#define DB7  PORTD7

void LCD_INI();
void SendHalfByte(unsigned char c);
void SendByte(unsigned char c, unsigned char mode);
void SendChar(unsigned char c);
void SetPos(unsigned char x,unsigned char y);
void SendStr(char str[]);
void SetDdramAddress(uint8_t address);
void SetCgramAddress(uint8_t address);

//--------------------------------
void LCD_INI(){
	_delay_ms(40);
	SendHalfByte(0b00000011);
	_delay_ms(5);
	SendHalfByte(0b00000011);
	_delay_us(100);
	SendHalfByte(0b00000011);
	_delay_ms(1);
	SendHalfByte(0b00000010);
	_delay_ms(1);
	SendByte(0b00101000, 0); // to switch on 4-bit (DL=0) the mode and in 2 lines (N=1)
	_delay_ms(1);
	SendByte(0b00001100, 0); // to switch on the image on the display (D=1), cursors are switched-off (C=0,B=0)
	_delay_ms(1);
	SendByte(0b0000110, 0); // the cursor (invisible) moves to the left
	_delay_ms(1);
}
//--------------------------------
void SendHalfByte(unsigned char c){
	c<<=4;
	PORTD |=(1<<E);	 //to switch on line E
	_delay_us(50);
	PORTD &=~((1<<DB4)|(1<<DB5)|(1<<DB6)|(1<<DB7));
	PORTD |= c;
	PORTD &=~(1<<E); //to switch off line E
	_delay_us(50);
}
//--------------------------------
void SendByte(unsigned char c, unsigned char mode){
	unsigned char hc;
	if(mode==0) PORTD &=~(1<<RS); //RS == 0 (command)
	else PORTD |=(1<<RS);		  //RS == 1 (data)
	hc = c>>4;
	SendHalfByte(hc); //send younger half-byte
	SendHalfByte(c); //send the senior half-byte
}
//--------------------------------
void SendChar(unsigned char c){
	SendByte(c, 1); // byte with data
}
//--------------------------------
void SetPos(unsigned char x,unsigned char y){
	char adress;
	adress=(0x40*y+x)|0b10000000;
	SendByte(adress, 0);
}
//---------------------------------
void SendStr(char str[]){
	wchar_t n;
	for(n=0;str[n]!='\0';n++)
	SendChar(str[n]);
}
//--------------------------------
void SetDdramAddress(uint8_t address){
	SendByte(address|0x80,0);
}
//--------------------------------
void SetCgramAddress(uint8_t address){
	SendByte(address|0x40,0);
}