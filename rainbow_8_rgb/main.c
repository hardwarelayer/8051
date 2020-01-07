/*
By TienTN, W78E052 chip with 8 RGB LED
Scrolling colors
*/
#include<reg51.h>

sbit P_00=P0^0;
sbit P_01=P0^1;
sbit P_02=P0^2;
sbit P_03=P0^3;
sbit P_04=P0^4;
sbit P_05=P0^5;
sbit P_06=P0^6;
sbit P_07=P0^7;

sbit P_10=P1^0;
sbit P_11=P1^1;
sbit P_12=P1^2;
sbit P_13=P1^3;
sbit P_14=P1^4;
sbit P_15=P1^5;
sbit P_16=P1^6;
sbit P_17=P1^7;

sbit P_20=P2^0;
sbit P_21=P2^1;
sbit P_22=P2^2;
sbit P_23=P2^3;
sbit P_24=P2^4;
sbit P_25=P2^5;
sbit P_26=P2^6;
sbit P_27=P2^7;

void Delay(unsigned int a);

unsigned char ledVals[] = {1, 2, 4, 3, 7, 5};

//diagram layout
//P2: LED 1, LED 2, LED 3(2 first pin)
//P3: LED 3 (1 last pin), LED 4, LED 5, LED 6(1 first pin)
//P1: LED 6 (2 last pin), LED7, LED 8

void loopLEDByMixColorVal(unsigned int pVal) {
  P3 = 0;
  P1 = 0;
  
  P2 = pVal;
  Delay(220);
  P2 = P2 << 3;
  Delay(220);
  P2 = P2 << 3;
  P3 = pVal & 1;  //lay bit cuoi dem qua P3
  Delay(220);

  P2 = 0;
  P1 = 0;
  
  P3 = pVal << 1; //skip pin 30 la chan thu 3 cua led cuoi P2
  Delay(220);
  P3 = P3 << 3;
  Delay(220);
  P3 = P3 << 3; //lay bit cuoi bo vao chan cuoi cung
  P1 = pVal >> 1; //lay 2 bit dau vo bao P10+11
  Delay(220);

  P3 = 0;
  P2 = 0;
  
  P1 = pVal << 2; //skip pin P10+11 la chan cuoi led cuoi P3
  Delay(220);
  P1 = P1 << 3;
  Delay(220);
  P1 = P1 << 3;
  Delay(220);
}

void allLEDBySingleColorVal(unsigned char pVal, unsigned int delayVal) {

  //reset
  P2 = 0;
  P3 = 0;
  P1 = 0;
  
  if (pVal == 1) {
    P2 = 73; // 0100 1001 <-- first pin (right to left)
    P3 = 146; // 1001 0010
    P1 = 36; // 0010 0100
  }
  else if (pVal == 2) {
    P2 = 146; // 1001 0010
    P3 = 36;  // 0010 0100
    P1 = 73; // 0100 1001
    
  }
  else if (pVal == 4) {
    P2 = 36; //0010 0100
    P3 = 73; //0100 1001
    P1 = 146;//1001 0010
  }

/*
  P1 |= (pVal << 2) | //pin P10+11 la chan cuoi led cuoi P3
       (pVal << 5) |
       (pVal << 8);
*/
  Delay(delayVal);
}

void blockLEDBySingleColorVal(unsigned char pVal, unsigned int delayVal) {

  unsigned char iIdx;

  for (iIdx = 0; iIdx < 3; iIdx++) {
    if (pVal == 1) {
      P2 = 73; // 0100 1001 <-- first pin (right to left)
      P3 = 146; // 1001 0010
      P1 = 36; // 0010 0100
    }
    else if (pVal == 2) {
      P2 = 146; // 1001 0010
      P3 = 36;  // 0010 0100
      P1 = 73; // 0100 1001
      
    }
    else if (pVal == 4) {
      P2 = 36; //0010 0100
      P3 = 73; //0100 1001
      P1 = 146;//1001 0010
    }
    
    if (iIdx == 0) {
      P3 = 0;
    }
    else if (iIdx == 1) {
      P1 = 0;
    }
    else if (iIdx == 2) {
      P2 = 0;
    }
    Delay(delayVal);
  }

/*
  P1 |= (pVal << 2) | //pin P10+11 la chan cuoi led cuoi P3
       (pVal << 5) |
       (pVal << 8);
*/
}

void loopLEDBySingleColorVal(unsigned int pVal) {
  P3 = 0;
  P1 = 0;
  
  P2 = pVal;
  Delay(220);
  P2 = P2 << 3;
  Delay(220);
  if (pVal < 4) {
    P2 = P2 << 3;
  }
  else {
    P2 = 0;
    P3 = 1; //pin 30 la chan thu 3 cuoi led cuoi P2
  }
  Delay(220);

  P2 = 0;
  P1 = 0;
  
  P3 = pVal << 1; //pin 30 la chan thu 3 cua led cuoi P2
  Delay(220);
  P3 = P3 << 3;
  Delay(220);
  if (pVal == 1) {
    P3 = P3 << 3;
  }
  else if (pVal >= 2) {
    P3 = 0;
    P1 = pVal >> 1; //skip to next port (vi 2 chan cuoi cua LED thu 6 nam ben port 1
  }
  Delay(220);

  P3 = 0;
  P2 = 0;
  
  P1 = pVal << 2; //pin P10+11 la chan cuoi led cuoi P3
  Delay(220);
  P1 = P1 << 3;
  Delay(220);
  P1 = P1 << 3;
  Delay(220);
}

//for common cathode LED
void main()
{

  unsigned int delayVal;
  while(1)
  {
    loopLEDBySingleColorVal(1);
    loopLEDBySingleColorVal(2);
    loopLEDBySingleColorVal(4);
    loopLEDByMixColorVal(3);
    loopLEDByMixColorVal(7);
    loopLEDByMixColorVal(5);
    
    for (delayVal = 4000; delayVal > 500; delayVal-=500) {
      allLEDBySingleColorVal(1, delayVal);
      allLEDBySingleColorVal(2, delayVal);
      allLEDBySingleColorVal(4, delayVal);
    }
    
    for (delayVal = 3000; delayVal > 0; delayVal-=1000) {
      blockLEDBySingleColorVal(1, delayVal);
      blockLEDBySingleColorVal(2, delayVal);
      blockLEDBySingleColorVal(4, delayVal);
    }

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