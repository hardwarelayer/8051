#include<reg51.h>
#include<intrins.h>	

#define GPIO_LED P2		  //define P2 port
sbit K1=P1^4;			  //Bit OP, K1 connected to P1, pin4 
sbit K2=P1^5;
sbit K3=P1^6;
sbit K4=P1^7;
sbit K5=P3^5;			  //P3
void Delay10ms( ); 

void main(void)
{

	
	while(1)
	{		 
		if(K1==0)		//Check if button K1 is pressed
		{
			Delay10ms();	//Wait and hope that noise will over
			if(K1==0)
			{
				GPIO_LED = 0xfe;	//1111 1110 the first LED is on(D1)
			}
		}
		
		if(K2==0)		//Check if button K2 is pressed
		{
			Delay10ms();	//Wait and hope that noise will over
			if(K2==0)
			{
				GPIO_LED = 0xfd;	//1111 1101 µÚ¶þ¸öµÆÁÁ
			}
		}
		
		if(K3==0)		//Check if button K3 is pressed
		{
			Delay10ms();	//Wait and hope that noise will over
			if(K3==0)
			{
				GPIO_LED = 0xfb;	//1111 1011
			}
		}
		
		if(K4==0)		//Check if button K4 is pressed
		{
			Delay10ms();
			if(K4==0)
			{
				GPIO_LED = 0xf7; //1111 0111	
			}
		}
		
		if(K5==0)		//K5
		{
			Delay10ms();
			if(K5==0)
			{
				GPIO_LED = 0xef;	 //1110 1111
			}
		}		

	}				
}
/*******************************************************************************
* Function name : Delay10ms
* Function      : Delay function, delay 10ms
* Input         : No
* Output        : No
*******************************************************************************/
void Delay10ms(void)   //Îó²î 0us
{ //correct if use ooccislator 11.0954MHz
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}