#define F_CPU 8000000


#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
#include <util/delay.h>
#include "lcd_4bit.h"

#define SET_BIT(x,b) (x |= (1<<b))
#define TOG_BIT(x,b) (x=x^ (1<<b))
#define CLR_BIT(x,b) (x&= ~(1<<b))
#define GET_BIT(x,b) ((x & (1<<b))>>b)



xSemaphoreHandle sema;

char flag=0;
char flag1=0;
char PatternFlag=0;
typedef struct subjects{
	char arr1[16];
	char arr2[16];
	char op;
	
}calc;

calc one;

void vApplicationIdleHook( void );


/*tasks codes prototypes */
void TaskStartCode(void*pvParamter);
void TaskPatternCode(void*pvParamter);
void TaskCalculatorCode(void*pvParamter);
void TaskResetCode(void*pvParamter);



/*functions*/
char KEYPAD_Scan(void);
void DispAnswer(char i,char j);
void LedPattern(void);



int main(void)
{
	
	/*initialize LCD*/
	lcd_init();
	DDRA=0xff;

	DDRD=0X0F;
	PORTD|=0xF0;

	DDRC=0X00;
	PORTC|=0xFF;

	vSemaphoreCreateBinary(sema);
	xSemaphoreTake(sema,0xffff);
	

	/*Create tasks*/
	xTaskCreate(TaskStartCode,NULL,120,NULL,2,NULL);
	xTaskCreate(TaskCalculatorCode,NULL,200,NULL,3,NULL);
	xTaskCreate(TaskPatternCode,NULL,100,NULL,1,NULL);
	xTaskCreate(TaskResetCode,NULL,110,NULL,4,NULL);
	
	/*start Scheduler */
	vTaskStartScheduler();
	
	return 0;
	
}

void vApplicationIdleHook( void ){

	PORTA ^= 0xFF;
	_delay_ms(500);

}
/*Task1 Code */
void TaskStartCode(void*pvParamter)
{

	for(char s=0;s<=2;s++){

		for( char i=10;i>0;i--)
		{
			
			lcd_disp_string_xy("welcome" ,0 ,i-1);
			_delay_ms(50);
			if(i!=1){
				lcd_clrScreen();
			}
		}


		for(char j=0;j<=9;j++)
		{
			
			lcd_disp_string_xy("welcome" ,0 ,j);
			_delay_ms(50);
			lcd_clrScreen();
			
		}
		
	}



	xSemaphoreGive(sema);

	vTaskDelete(NULL);


}


/*Task 2 Code*/

void TaskPatternCode(void*pvParamter)
{

	for(char m=0;m<=13;m++){
		if(PatternFlag==0){
			lcd_disp_string_xy("press any key" ,0 ,1);
			lcd_disp_string_xy("to continue " ,1 ,2);
			_delay_ms(500);
			lcd_clrScreen();
			_delay_ms(250);
			}else{
			vTaskDelete(NULL);
		}
	}
	LedPattern();


}

/*task 3 */
void TaskCalculatorCode(void*pvParamter)
{
	xSemaphoreTake(sema,0xffff);//to prevent welcome interruption 

	char key;

	//calc two;


	static char i=0,j=0,count=0;

	while(1){
		key=KEYPAD_Scan();

		while(key=='k')
		{
			vTaskDelay(5);
			key=KEYPAD_Scan();
		}
		if(count==0){
			PatternFlag=1;
			count++;
		}
		if(key=='o')
		{
			lcd_clrScreen();
			lcd_displayChar('0');
			flag=0;
			i=0;
			j=0;
			continue;
		}
		if(flag!=1)
		{

			if(flag1==1)
			{
				if(j==0)
				{
					lcd_clrScreen();
				}
				lcd_displayChar(key+48);
				one.arr2[j]=key;
				flag1=0;
				j++;
				
			}
			else
			{
				if(i==0)
				{
					lcd_clrScreen();
				}
				lcd_displayChar(key+48);
				one.arr1[i]=key;
				i++;
			}
		}
		else
		{
			flag=0;
			
			if(key=='e')
			{


				DispAnswer(i,j);
				i=0;
				j-0;
				continue;
			}
			flag1=1;
			one.op=key;
			
		}

	}


}



/*task 4*/
void TaskResetCode(void*pvParamter)
{
	while(1)
	{
		if (GET_BIT(PINC,0)==0)
		{
			int y=xTaskGetTickCount();
			lcd_clrScreen();
			vTaskDelayUntil(&y,24000000);
			if (GET_BIT(PINC,0)==0)
			{
				lcd_clrScreen();
				LedPattern();
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
}



char KEYPAD_Scan(void)
{
	char key='k';
	
	

	CLR_BIT(PORTD,0);
	SET_BIT(PORTD,1);
	SET_BIT(PORTD,2);
	SET_BIT(PORTD,3);

	if (GET_BIT(PIND,4)== 0) {
		key = 7;
		while (GET_BIT(PIND,4) ==0);

		} else if (GET_BIT(PIND,5) == 0) {
		key = 8;
		while (GET_BIT(PIND,5) ==0);


		} else if (GET_BIT(PIND,6) == 0) {
		key = 9;
		while (GET_BIT(PIND,6) ==0);


		} else if (GET_BIT(PIND,7) == 0) {
		key = 'd';
		while (GET_BIT(PIND,7) ==0);

		flag=1;
	}

	

	CLR_BIT(PORTD,1);
	SET_BIT(PORTD,0);
	SET_BIT(PORTD,2);
	SET_BIT(PORTD,3);

	if (GET_BIT(PIND,4)== 0) {
		key = 4;
		while (GET_BIT(PIND,4) ==0);

		} else if (GET_BIT(PIND,5) == 0) {
		key = 5;
		while (GET_BIT(PIND,5) ==0);

		} else if (GET_BIT(PIND,6) == 0) {
		key = 6;
		while (GET_BIT(PIND,6) ==0);

		} else if (GET_BIT(PIND,7) == 0) {
		key = 't';
		flag=1;
		while (GET_BIT(PIND,7) ==0);
	}


	CLR_BIT(PORTD,2);
	SET_BIT(PORTD,0);
	SET_BIT(PORTD,1);
	SET_BIT(PORTD,3);

	if (GET_BIT(PIND,4)== 0) {
		key = 1;
		while (GET_BIT(PIND,4) ==0);

		} else if (GET_BIT(PIND,5) == 0) {
		key = 2;
		while (GET_BIT(PIND,5) ==0);

		} else if (GET_BIT(PIND,6) == 0) {
		key = 3;
		while (GET_BIT(PIND,6) ==0);


		} else if (GET_BIT(PIND,7) == 0) {
		key = 'm';
		flag=1;
		while (GET_BIT(PIND,7) ==0);
	}


	CLR_BIT(PORTD,3);
	SET_BIT(PORTD,0);
	SET_BIT(PORTD,1);
	SET_BIT(PORTD,2);
	if (GET_BIT(PIND,4)== 0) {
		key = 'o';
		while (GET_BIT(PIND,4) ==0);

		} else if (GET_BIT(PIND,5) == 0) {
		key = 0;
		while (GET_BIT(PIND,5) ==0);

		} else if (GET_BIT(PIND,6) == 0) {
		key = 'e';
		flag=1;
		while (GET_BIT(PIND,6) ==0);


		} else if (GET_BIT(PIND,7) == 0) {
		key = 'p';
		flag=1;
		while (GET_BIT(PIND,7) ==0);


		
	}
	

	
	return key;
}








void DispAnswer(char i,char j)
{
	int result1=0,result2=0;
	long long  int result=0;
	char rest=0;
	char arr[16]={0};
	
	for(char count=0;count<=i;count++)
	{
		result1=result1*10+(one.arr1[count]);
		

	}
	for(char count=0;count<=j;count++)
	{
		result2=result2*10+(one.arr2[count]);
		

	}

	switch(one.op)
	{
		case 'p':
		result=result1+result2;
		break;
		case 'm':
		result=result1-result2;
		break;
		case 't':
		result=result1*result2;
		break;
		case 'd':
		result=result1/result2;
		break;
	}
	

	char s=0;

	while (result != 0)
	{
		rest = result % 10;
		arr[s] = rest;
		result = result / 10;

		s++;
	}

	lcd_clrScreen();
	
	if(one.op=='t')
	{
		for(char c=s;c>2;c--)
		{
			
			lcd_displayChar(arr[c-1]+48);
			
		}

	}
	else if(one.op=='d')
	{
		for(char c=0;c<s;c++)
		{
			
			lcd_displayChar(arr[c]+48);
			
		}
	}
	else if(one.op=='m'||one.op=='p'){
		for(char c=s;c>1;c--)
		{
			
			lcd_displayChar(arr[c-1]+48);
			
		}
		
	}
}


void LedPattern(void)
{
	char duty;

	// initialize timer0 in PWM mode
	TCCR0 |= (1<<WGM00)|(1<<COM01)|(1<<WGM01)|(1<<CS00);
	
	DDRB |= (1<<PB3);

	for(duty=1; duty<=10; duty++)   

	{

		OCR0=duty*255/10;    

		_delay_ms(25);

	}

	TCCR0 &= (0<<WGM00)|(0<<COM01)|(0<<WGM01)|(0<<CS00);

	PORTB |=(1<<3);
	_delay_ms(250);

	
	TCCR0 |= (1<<WGM00)|(1<<COM01)|(1<<WGM01)|(1<<CS00);
	


	for(duty=10; duty>0; duty--)  

	{

		OCR0=duty*255/10;     

		_delay_ms(25);

	}
	TCCR0 &= (0<<WGM00)|(0<<COM01)|(0<<WGM01)|(0<<CS00);
	PORTB &=~(1<<3);
	_delay_ms(250);

}