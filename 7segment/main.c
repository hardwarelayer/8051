#include<reg52.h>

//4x7 segment LED control
//Common cathode(?) module
//common pin LED1-4 connect to P1: P10-P13

//segment led will connect to P0 (0-7)
//and we input 1 to turn one segment on
unsigned char code Disp_Tab[] = {
	      //.gfe dcba
	0x3f, //0011 1111 - 0
	0x06, //0000 0110 - 1
	0x5b, //0101 1011 - 2
	0x4f, //0100 1111 - 3
	0x66, //0110 0110 - 4
	0x6d, //0110 1101 - 5
	0x7d, //0111 1101 - 6
	0x07, //0000 0111 - 7
	0x7f, //0111 1111 - 8
	0x6f, //0110 1111 - 9
	0x40  //0100 0000 - dau - o giua
	};

void delay()
{
	unsigned int i=50000;
	while(i--);
}


void main()
{
	unsigned char i = 0;
	P0 = 0x3f;		//Power on digital tube display 0
	P1 = 0xff;		//1111 1111 turn on all common cathode pin (connected to collector pin of NPN transistor to drag current from it to GND)

	for(i = 0; i < 8; i++)	delay();

	while(1)
	{
		for(i = 0; i < 8; i++)
		{
			P0 =  Disp_Tab[i];
			delay();
		}
	}
}