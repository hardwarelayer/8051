#include<reg51.h>

sbit LED1=P2^0;
sbit LED2=P2^2;

void Delay(unsigned int a);

bit bLedFlg = 0;

void initialize()     // Initialize Timer 1 for serial communication
    {
     TMOD=0x20;    //Timer1, mode 2, baud rate 9600 bps (8-bit reload mode for baudrate generation)
     TH1=0XFD;      //Baud rate 9600 bps
     SCON=0x50;      //Serial Mode 1, 8-Data Bit, REN Enabled   
     TR1=1;             //Start timer 1
    }

void receive()        //Function to receive serial data
   {
    unsigned char value;
    while(RI==0);       //wait till RI flag is set
    value=SBUF;     
    //P1=value;
    RI=0;                    //Clear the RI flag
	if (bLedFlg == 1)
		bLedFlg = 0;
	else
		bLedFlg = 1;
   }

void main()
{
	while(1)
	{
		LED2 = 1;

	   	initialize();
		receive();

		if (bLedFlg == 1) {
			LED1=0;
		}
		else {
			LED1=1;
		}
		//Delay(2500);
		LED2 = 0;
		Delay(2500);
	}
}

void Delay(unsigned int a)
{
	unsigned char b;
	for(;a>0;a--)
	{
		for(b=110;b>0;b--);
	}
}