#include <reg52.h>
#include <intrins.h>

#define GPIO_LED            P2  //define LED matrix port

#define SEVEN_SEGMENT_SEG_PIN    P0
#define SEVEN_SEGMENT_COMMON_PIN P1

bit bTimer1IsRunning = 0;
bit bMelodyIsPlaying = 0;

unsigned char matrixLED = 1;
unsigned char iSongCount = 0;
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

//keyboard
sbit K1=P1^4;			  //Bit OP, K1 connected to P1, pin4 
sbit K2=P1^5;
sbit K3=P1^6;
sbit K4=P1^7;
sbit K5=P3^5;			  //P3

//Assembly code in delay.src
void Delay10MC(unsigned char);
//Assembly code in timerdelay.src
void TimerDelay10MC(unsigned char);
void Melodyplay(const int Pitch, unsigned int interval);

void EndNote(void);
void InitTimer1(void);

//buzzer connect to P21
sbit SPK_PIN_PORT = P2^1; //same as LED at P2^1

#define pitch_P		0
#define pitch_C5	64580
#define pitch_Db5	64634
#define pitch_D5	64685
#define pitch_Eb5	64732
#define pitch_E5	64778
#define pitch_F5	64820
#define pitch_Gb5	64860
#define pitch_G5	64898
#define pitch_Ab5	64934
#define pitch_A5	64968
#define pitch_Bb5	65000
#define pitch_B5	65030
#define pitch_C6	65058
#define pitch_Db6	65085
#define pitch_D6	65110
#define pitch_Eb6	65134
#define pitch_E6	65157
#define pitch_F6	65178
#define pitch_Gb6	65198
#define pitch_G6	65217
#define pitch_Ab6	65235
#define pitch_A6	65252
#define pitch_Bb6	65268
#define pitch_B6	65283

#define MelodyPin	SPK_PIN_PORT

unsigned int qtrN, qtrN_us, wholeN, wholeN_us, halfN, halfN_us, eighthN, eighthN_us,
			 sixteenN, sixteenN_us, thirtyTwoN, thirtyTwoN_us, sixtyFourN, sixtyFourN_us,
			 sixN, sixN_us, twelveN, twelveN_us, twentyFourN, twentyFourN_us;

//5	TIMER/COUNTER 2 (8052)	002Bh
void Timer2_ISR(void) interrupt 5 {
	MelodyPin = ~MelodyPin; //toggle out pin
	TF2 = 0; //reset the interrupt/overflow flag
}

unsigned int iTimer1RunMS = 0;

//https://what-when-how.com/8051-microcontroller/programming-timers-0-and-1-in-8051-c/
//http://www.keil.com/support/man/docs/c51/c51_le_interruptfuncs.htm
//3	TIMER/COUNTER 1	001Bh
void Timer1_ISR(void) interrupt 3 {

    if (iTimer1RunMS > 0)
	{
		iTimer1RunMS--;

		TH1 = 0xFC;      // reload it with the same value as inittimer0
		TL1 = 0x18;      //TH&TL is 16 bit
		TF1 = 0;     // Clear the interrupt flag to start again
	}
	else
	{
		//stop timer1
		TR1 = 0;
		TF1 = 0; //reset the interrupt/overflow flag
		ET1 = 0; // Disable Timer1 interrupts

		bTimer1IsRunning = 0;

	}
}

short iTickCount = 0;
unsigned char segmentLEDRoll = 0x01;

//1	TIMER/COUNTER 0	000Bh
void Timer0_ISR (void) interrupt 1   // It is called after every 250usec
{
	TH0 = 0xEA;      // reload it with the same value as inittimer0
	TL0 = 0x60;      //TH&TL is 16 bit

	if (++iTickCount > 25) {
		iTickCount = 0;
		if (matrixLED < 128)
			matrixLED = matrixLED << 1;
		else
			matrixLED = 1;

		if (segmentLEDRoll >= 0x20) { //0010 0000
			segmentLEDRoll = 1;
		}
		else {
		    segmentLEDRoll = segmentLEDRoll << 1;
		}
	}

	TF0 = 0;     // Clear the interrupt flag to start again
}

void MelodyTempo(int Tempo){
	qtrN = ((60000/Tempo));
	wholeN = qtrN*4;
	halfN = (qtrN*2);
	eighthN = (qtrN/2);
	sixteenN = (qtrN/4);
	thirtyTwoN = (qtrN/8);
	sixtyFourN = (qtrN/16);
	sixN = (wholeN/6);
	twelveN = (wholeN/12);
	twentyFourN = (wholeN/24);
}

void PlayNote(const unsigned char PitchH, const unsigned char PitchL) {
	RCAP2H = PitchH; //Timer/Counter 2 Reload/Capture High Byte
	RCAP2L = PitchL;
	TH2 = PitchH;
	TL2 = PitchL;
	//Once timer flag (TF) is set, the programmer must clear it before it can be set again.
	TF2 = 0; //reset/clear the interrupt/overflow flag
	TR2 = 1; //timer 2 enabled (start)
	ET2 = 1; //Enable timer 2
	EA = 1; //global interrupt enable 
}

void EndNote(void){
	//EA = 0; //disable all interrupt
	TR2 = 0; //disable timer 2
	TF2 = 0; //reset the overflow flag
	MelodyPin = 1;
}

void delay_ms(unsigned int ms){
	for(;ms>0;ms--)
		Delay10MC(100);
}

void Melodyplay(const int Pitch, unsigned int interval) {

	if(Pitch!=0)
		PlayNote(Pitch >> 8, Pitch);

	//iTimer1RunMS = interval;
	//InitTimer1();
	delay_ms(interval);
	EndNote();
}

/*
osc = 11.0592Mhz
Delay = 50ms
1000uS = 1mS
maximum of timer = 65mS (2^16=65536)
*/
void InitTimer0(void)
{
	TMOD = 0x01;    // Set timer0 in mode 1 (16 bit timer)
	
	TH0 = 0xEA;      // 16 bit timer, D8F0 mean 65536-5536=60000 (10000=1milisecond in 12MHz)
	TL0 = 0x60;

	TR0 = 1;         // Start Timer 0	
	ET0 = 1;         // Enable Timer0 interrupts
	EA  = 1;         // Global interrupt enable

}

//https://what-when-how.com/8051-microcontroller/programming-timers-0-and-1-in-8051-c/
void InitTimer1(void)
{
	if (bTimer1IsRunning == 0) {
		bTimer1IsRunning = 1;
	}
	else {
		return;
	}

	TMOD &= 0x0F;    // Clear 4bit high for timer1, keep 4 bit low for timer 0 (or timer 0 will be reset with unknown data)
	TMOD |= 0x10;    // Set timer1 in mode 1 (16 bit timer)
	
	TH1 = 0xFC;      // 16 bit timer, D8F0 mean 60000-1000=FC18
	TL1 = 0x18;

	TR1 = 1;         // Start Timer 1
	ET1 = 1;         // Enable Timer1 interrupts
	EA = 1;         // Global interrupt enable

}

void LooneyToons(){ //d=4,o=5,b=140
	MelodyTempo(140);
	Melodyplay(pitch_C6, qtrN); //c6
	Melodyplay(pitch_F6, eighthN); //8f6
	Melodyplay(pitch_E6, eighthN); //8e6
	Melodyplay(pitch_D6, eighthN); //8d6
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_A5, qtrN); //a.
	Melodyplay(pitch_A5, eighthN); //a.
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_F6, eighthN); //8f6
	Melodyplay(pitch_E6, eighthN); //8e6
	Melodyplay(pitch_D6, eighthN); //8d6
	Melodyplay(pitch_Eb6, eighthN); //8d#6
	Melodyplay(pitch_E6, qtrN); //e.6
	Melodyplay(pitch_E6, eighthN); //e.6
	Melodyplay(pitch_E6, eighthN); //8e6
	Melodyplay(pitch_E6, eighthN); //8e6
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_D6, eighthN); //8d6
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_E6, eighthN); //8e6
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_D6, eighthN); //8d6
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_A5, eighthN); //8a
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_G5, eighthN); //8g
	Melodyplay(pitch_Bb5, eighthN); //8a#
	Melodyplay(pitch_A5, eighthN); //8a
	Melodyplay(pitch_F5, eighthN); //8f
	Melodyplay(pitch_P, wholeN); //1P
}

void Flintstones(){//d=4,o=5,b=200
	MelodyTempo(200);
	Melodyplay(pitch_Ab5, qtrN); //g#
	Melodyplay(pitch_Db5, qtrN); //c#
	Melodyplay(pitch_P, eighthN); //8p
	Melodyplay(pitch_Db6, qtrN); //c#6
	Melodyplay(pitch_Bb5, eighthN); //8a#
	Melodyplay(pitch_Ab5, qtrN); //g#
	Melodyplay(pitch_Db5, qtrN); //c#
	Melodyplay(pitch_P, eighthN); //8p
	Melodyplay(pitch_Ab5, qtrN); //g#
	Melodyplay(pitch_Gb5, eighthN); //8f#
	Melodyplay(pitch_F5, eighthN); //8f
	Melodyplay(pitch_F5, eighthN); //8f
	Melodyplay(pitch_Gb5, eighthN); //8f#
	Melodyplay(pitch_Ab5, eighthN); //8g#
	Melodyplay(pitch_Db5, qtrN); //c#
	Melodyplay(pitch_Eb5, qtrN); //d#
	Melodyplay(pitch_F5, halfN); //2f
	Melodyplay(pitch_P, halfN); //2p
	Melodyplay(pitch_Ab5, qtrN); //g#
	Melodyplay(pitch_Db5, qtrN); //c#
	Melodyplay(pitch_P, eighthN); //8p
	Melodyplay(pitch_Db6, qtrN); //c#6
	Melodyplay(pitch_Bb5, eighthN); //8a#
	Melodyplay(pitch_Ab5, qtrN); //g#
	Melodyplay(pitch_Db5, qtrN); //c#
	Melodyplay(pitch_P, eighthN); //8p
	Melodyplay(pitch_Ab5, qtrN); //g#
	Melodyplay(pitch_Gb5, eighthN); //8f#
	Melodyplay(pitch_F5, eighthN); //8f
	Melodyplay(pitch_F5, eighthN); //8f
	Melodyplay(pitch_Gb5, eighthN); //8f#
	Melodyplay(pitch_Ab5, eighthN); //8g#
	Melodyplay(pitch_Db5, qtrN); //c#
	Melodyplay(pitch_Eb5, qtrN); //d#
	Melodyplay(pitch_Db5, halfN); //2c#
}

void AdamsFamily(){ //d=4, o=6, b=50
	MelodyTempo(50);
	Melodyplay(pitch_P, thirtyTwoN); //32P
	Melodyplay(pitch_Db6, thirtyTwoN); //32c#
	Melodyplay(pitch_Gb6, sixteenN); //16f#
	Melodyplay(pitch_Bb6, thirtyTwoN); //32a#
	Melodyplay(pitch_Gb6, sixteenN); //16f#
	Melodyplay(pitch_Db6, thirtyTwoN); //32c#
	Melodyplay(pitch_C6, sixteenN); //16c
	Melodyplay(pitch_Ab6, eighthN); //8g#
	Melodyplay(pitch_Gb6, thirtyTwoN); //32f#
	Melodyplay(pitch_F6, sixteenN); //16f
	Melodyplay(pitch_Ab6, thirtyTwoN); //32g#
	Melodyplay(pitch_F6, sixteenN); //16f
	Melodyplay(pitch_Db6, thirtyTwoN); //32c#
	Melodyplay(pitch_Bb5, sixteenN); //16a#5
	Melodyplay(pitch_Gb6, eighthN); //8f#
	Melodyplay(pitch_Db6, thirtyTwoN); //32c#
	Melodyplay(pitch_Gb6, sixteenN); //16f#
	Melodyplay(pitch_Bb6, thirtyTwoN); //32a#
	Melodyplay(pitch_Gb6, sixteenN); //16f#
	Melodyplay(pitch_Db6, thirtyTwoN); //32c#
	Melodyplay(pitch_C6, sixteenN); //16c
	Melodyplay(pitch_Ab6, eighthN); //8g#
	Melodyplay(pitch_Gb6, thirtyTwoN); //32f#
	Melodyplay(pitch_F6, sixteenN); //16f
	Melodyplay(pitch_Db6, thirtyTwoN); //32c#
	Melodyplay(pitch_Eb6, sixteenN); //16d#
	Melodyplay(pitch_F6, thirtyTwoN); //32f
	Melodyplay(pitch_Gb6, qtrN); //f#
}

void PinkPanther(){//d=4,o=5,b=160
	MelodyTempo(160);
	Melodyplay(pitch_Eb5, eighthN); //8d#
	Melodyplay(pitch_E5, eighthN); //8e
	Melodyplay(pitch_P, halfN); //2p
	Melodyplay(pitch_Gb5, eighthN); //8f#
	Melodyplay(pitch_G5, eighthN); //8g
	Melodyplay(pitch_P, halfN); //2p
	Melodyplay(pitch_Eb5, eighthN); //8d#
	Melodyplay(pitch_E5, eighthN); //8e
	Melodyplay(pitch_P, sixteenN); //16p
	Melodyplay(pitch_Gb5, eighthN); //8f#
	Melodyplay(pitch_G5, eighthN); //8g
	Melodyplay(pitch_P, sixteenN); //16p
	Melodyplay(pitch_C6, eighthN); //8c6
	Melodyplay(pitch_B5, eighthN); //8b
	Melodyplay(pitch_P, sixteenN); //16p
	Melodyplay(pitch_Eb5, eighthN); //8d#
	Melodyplay(pitch_E5, eighthN); //8e
	Melodyplay(pitch_P, sixteenN); //16p
	Melodyplay(pitch_B5, eighthN); //8b
	Melodyplay(pitch_Bb5, halfN); //2a#
	Melodyplay(pitch_P, halfN); //2p
	Melodyplay(pitch_A5, sixteenN); //16a
	Melodyplay(pitch_G5, sixteenN); //16g
	Melodyplay(pitch_E5, sixteenN); //16e
	Melodyplay(pitch_D5, sixteenN); //16d
	Melodyplay(pitch_E5, halfN); //2e
}

void BeethovenPlay(){
	MelodyTempo(240);
	Melodyplay(pitch_E6, qtrN);
	Melodyplay(pitch_Eb6, qtrN);
	Melodyplay(pitch_E6, qtrN);
	Melodyplay(pitch_Eb6, qtrN);
	Melodyplay(pitch_E6, qtrN);
	Melodyplay(pitch_B5, qtrN);
	Melodyplay(pitch_D6, qtrN);

	Melodyplay(pitch_C6, qtrN);
	Melodyplay(pitch_A5, halfN);
	Melodyplay(pitch_C5, qtrN);
	Melodyplay(pitch_E6, qtrN);
	Melodyplay(pitch_A6, qtrN);
	Melodyplay(pitch_B6, halfN);
	
	Melodyplay(pitch_E5, qtrN);
	Melodyplay(pitch_A5, qtrN);
	Melodyplay(pitch_B5, qtrN);
	Melodyplay(pitch_C6, halfN);
}

void Saregama(){
	MelodyTempo(240);
	Melodyplay(pitch_Db5, qtrN);
	Melodyplay(pitch_Eb5, qtrN);
	Melodyplay(pitch_F5, qtrN);
	Melodyplay(pitch_Gb5, qtrN);
	Melodyplay(pitch_Ab5, qtrN);
	Melodyplay(pitch_Bb5, qtrN);
	Melodyplay(pitch_C6, qtrN);
	Melodyplay(pitch_Db6, qtrN);
	Melodyplay(pitch_P, wholeN);
	Melodyplay(pitch_Db6, qtrN);
	Melodyplay(pitch_C6, qtrN);
	Melodyplay(pitch_Bb5, qtrN);
	Melodyplay(pitch_Ab5, qtrN);
	Melodyplay(pitch_Gb5, qtrN);
	Melodyplay(pitch_F5, qtrN);
	Melodyplay(pitch_Eb5, qtrN);
	Melodyplay(pitch_Db5, qtrN);
}

/*
    AdamsFamily();
	delay_ms(1000);
	LooneyToons();
	delay_ms(1000);
	Flintstones();
	delay_ms(1000);
	PinkPanther();
	delay_ms(1000);
	BeethovenPlay();
	delay_ms(1000);
	Saregama();
	delay_ms(1000);
*/

void main(void)
{
    unsigned char i = 0;

    InitTimer0();

	SEVEN_SEGMENT_SEG_PIN = 0x3f;		//Power on digital tube display 0
	SEVEN_SEGMENT_COMMON_PIN = 0xff;		//1111 1111 turn on all common cathode pin (connected to collector pin of NPN transistor to drag current from it to GND)

	while(1)
	{		 
		if(K5==0)		//K5
		{
			delay_ms(10);
			if(K5==0)
			{
				SEVEN_SEGMENT_SEG_PIN =  Disp_Tab[i++];
				if (i > 9) i = 0;
				GPIO_LED = 0xef;	 //1110 1111

				bMelodyIsPlaying = 1;

				switch (iSongCount) {
				case 0:
				    AdamsFamily();
					break;
				case 1:
					LooneyToons();
					break;
				case 2:
					Flintstones();
					break;
				case 3:
					PinkPanther();
					break;
				case 4:
					BeethovenPlay();
					break;
				case 5:
					Saregama();
					break;
				}
				bMelodyIsPlaying = 0;
				if (iSongCount > 5)
					iSongCount = 0;
				else
					iSongCount++;
			}
		}		

		GPIO_LED = ~matrixLED;

		SEVEN_SEGMENT_SEG_PIN =  segmentLEDRoll;
	}				
}
