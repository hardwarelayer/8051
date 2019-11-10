#include<reg51.h>        

unsigned char iLEDCounter = 1;
unsigned char iLoop = 0;
unsigned char iSevSegLoop = 1;
unsigned char iRollResult = 4;
unsigned char iSevSeg1 = 0;
unsigned char iSevSeg2 = 0;

sbit LED1=P0^0;
sbit LED2=P0^1;
sbit LED3=P0^2;
sbit LED4=P0^3;
sbit LED5=P0^4;
sbit LED6=P0^5;
sbit LED7=P0^6;
sbit LED8=P0^7;

sbit LED9=P2^7;
sbit LED10=P2^6;
sbit LED11=P2^5;
sbit LED12=P2^4;

sbit SEVSEG_COM1=P2^2;
sbit SEVSEG_COM2=P2^3;

sbit SEVSEG_A=P3^1;
sbit SEVSEG_B=P3^2;
sbit SEVSEG_C=P3^3;
sbit SEVSEG_D=P3^4;
sbit SEVSEG_E=P3^5;
sbit SEVSEG_F=P3^6;
sbit SEVSEG_G=P3^7;

//Assembly code in delay.src
void Delay10MC(unsigned char);

void delay_ms(unsigned short ms){
  for(;ms>0;ms--)
    Delay10MC(100);
}

void initialize()   // Initialize Timer 1 for serial communication
{
  TMOD=0x20;  //Timer1, mode 2, baud rate 9600 bps (8-bit reload mode for baudrate generation)
  TH1=0XFD;    //Baud rate 9600 bps
  SCON=0x50;    //Serial Mode 1, 8-Data Bit, REN Enabled   
  TR1=1;       //Start timer 1
}

unsigned char receive()    //Function to receive serial data
{
  unsigned char value;
  while(RI==0);     //wait till RI flag is set
  value=SBUF;   
  //P1=value;
  RI=0;          //Clear the RI flag
  return value;
}

void setSegmentValue(unsigned char iSeg, bit value) {
  switch (iSeg) {
  case 1:
    SEVSEG_A = value;
    break;
  case 2:
    SEVSEG_B = value;
    break;
  case 3:
    SEVSEG_C = value;
    break;
  case 4:
    SEVSEG_D = value;
    break;
  case 5:
    SEVSEG_E = value;
    break;
  case 6:
    SEVSEG_F = value;
    break;
  case 7:
    SEVSEG_G = value;
    break;
  }
}

void setSevLEDNumber(unsigned char iNum) {
  switch (iNum) {
  case 0:
    setSegmentValue(1, 0);
    setSegmentValue(2, 0);
    setSegmentValue(3, 0);
    setSegmentValue(4, 0);
    setSegmentValue(5, 0);
    setSegmentValue(6, 0);
    setSegmentValue(7, 1); //off
    break;
  case 1:
    setSegmentValue(1, 1);
    setSegmentValue(2, 0);
    setSegmentValue(3, 0);
    setSegmentValue(4, 1);
    setSegmentValue(5, 1);
    setSegmentValue(6, 1);
    setSegmentValue(7, 1); //off
    break;
  case 2:
    setSegmentValue(1, 0);
    setSegmentValue(2, 0);
    setSegmentValue(3, 1);
    setSegmentValue(4, 0);
    setSegmentValue(5, 0);
    setSegmentValue(6, 1);
    setSegmentValue(7, 0);
    break;
  case 3:
    setSegmentValue(1, 0);
    setSegmentValue(2, 0);
    setSegmentValue(3, 0);
    setSegmentValue(4, 0);
    setSegmentValue(5, 1);
    setSegmentValue(6, 1);
    setSegmentValue(7, 0);
    break;
  case 4:
    setSegmentValue(1, 1);
    setSegmentValue(2, 0);
    setSegmentValue(3, 0);
    setSegmentValue(4, 1);
    setSegmentValue(5, 1);
    setSegmentValue(6, 0);
    setSegmentValue(7, 0);
    break;
  case 5:
    setSegmentValue(1, 0);
    setSegmentValue(2, 1);
    setSegmentValue(3, 0);
    setSegmentValue(4, 0);
    setSegmentValue(5, 1);
    setSegmentValue(6, 0);
    setSegmentValue(7, 0);
    break;
  case 6:
    setSegmentValue(1, 0);
    setSegmentValue(2, 1);
    setSegmentValue(3, 0);
    setSegmentValue(4, 0);
    setSegmentValue(5, 0);
    setSegmentValue(6, 0);
    setSegmentValue(7, 0);
    break;
  case 7:
    setSegmentValue(1, 0);
    setSegmentValue(2, 0);
    setSegmentValue(3, 0);
    setSegmentValue(4, 1);
    setSegmentValue(5, 1);
    setSegmentValue(6, 1);
    setSegmentValue(7, 1); //off
    break;
  case 8:
    setSegmentValue(1, 0);
    setSegmentValue(2, 0);
    setSegmentValue(3, 0);
    setSegmentValue(4, 0);
    setSegmentValue(5, 0);
    setSegmentValue(6, 0);
    setSegmentValue(7, 0);
    break;
  case 9:
    setSegmentValue(1, 0);
    setSegmentValue(2, 0);
    setSegmentValue(3, 0);
    setSegmentValue(4, 0);
    setSegmentValue(5, 1);
    setSegmentValue(6, 0);
    setSegmentValue(7, 0);
    break;
  }
}

void setPinValue(unsigned char iPin, bit value) {
  switch (iPin) {
  case 1:
    LED1 = value;
    break;
  case 2:
    LED2 = value;
    break;
  case 3:
    LED3 = value;
    break;
  case 4:
    LED4 = value;
    break;
  case 5:
    LED5 = value;
    break;
  case 6:
    LED6 = value;
    break;
  case 7:
    LED7 = value;
    break;
  case 8:
    LED8 = value;
    break;
  case 9:
    LED9 = value;
    break;
  case 10:
    LED10 = value;
    break;
  case 11:
    LED11 = value;
    break;
  case 12:  
    LED12 = value;
    break;
  default:
    break;
  }
}

void main()            
{

  while(1)
  {
    //turn off all
    for (iLEDCounter = 1; iLEDCounter <= 12; iLEDCounter++)
      setPinValue(iLEDCounter, 1);

    //on 2 7SegLED
    SEVSEG_COM1 = 1;
    SEVSEG_COM2 = 1;
    
    for (iLoop = 0; iLoop < 3; iLoop++) {
      for (iLEDCounter = 1; iLEDCounter <= 12; iLEDCounter++) {
        //turn off previous LED
        if (iLEDCounter > 1) {
          setPinValue(iLEDCounter-1, 1);
        }
        else {
          setPinValue(12, 1);
        }
        
        //on current
        setPinValue(iLEDCounter, 0);
        
        if (iSevSegLoop > 6) {
          iSevSegLoop = 1;
        }
        else {
          iSevSegLoop++;
        }
        //turn off previous SEG
        if (iSevSegLoop > 1) {
          setSegmentValue(iSevSegLoop-1, 1);
        }
        else {
          setSegmentValue(6, 1);
        }
        //on current
        setSegmentValue(iSevSegLoop, 0);
        //off G always
        setSegmentValue(7, 1);
        delay_ms(100);
      }
    }

    //end of roll
    //display value
    for (iLEDCounter = 1; iLEDCounter <= iRollResult; iLEDCounter++) {
      //turn off previous LED
      if (iLEDCounter > 1) {
        setPinValue(iLEDCounter-1, 1);
      }
      else {
        setPinValue(12, 1);
      }
      
      //on current
      setPinValue(iLEDCounter, 0);
      
      if (iSevSegLoop > 6) {
        iSevSegLoop = 1;
      }
      else {
        iSevSegLoop++;
      }
      //turn off previous SEG
      if (iSevSegLoop > 1) {
        setSegmentValue(iSevSegLoop-1, 1);
      }
      else {
        setSegmentValue(6, 1);
      }
      //on current
      setSegmentValue(iSevSegLoop, 0);
      //off G always
      setSegmentValue(7, 1);
      delay_ms(120);
    }
    //show on 7segment, too
    if (iRollResult < 10) {
      iSevSeg1 = 0;
    }
    else {
      iSevSeg1 = (iRollResult/10)%10;
      if (iSevSeg1 > 9) {
        iSevSeg1 = 1;
      }
    }
    iSevSeg2 = iRollResult%10;
    for (iLEDCounter = 0; iLEDCounter < 100; iLEDCounter++) {
      SEVSEG_COM2 = 0;
      if (iSevSeg1 > 0) {
        SEVSEG_COM1 = 1;
        setSevLEDNumber(iSevSeg1);
        delay_ms(10);
      }
      else {
        //don't use it
        SEVSEG_COM1 = 0;
      }

      SEVSEG_COM1 = 0;
      SEVSEG_COM2 = 1;
      setSevLEDNumber(iSevSeg2);
      delay_ms(10);
    }
    delay_ms(10000);
    //off all
    SEVSEG_COM1 = 0;
    SEVSEG_COM2 = 0;

    initialize();
    iRollResult = receive(); //block until interrupt found

  }
}


