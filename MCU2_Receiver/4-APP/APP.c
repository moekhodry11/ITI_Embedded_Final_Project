#include"APP.h"
u16 digitalValue = 0;
volatile u8  strFlag = 0;
volatile u8 Timer2_Falg =0;
u8 tempFlag = 0;
u8 received_msg[4];
void APP_Init(void)
{
	GI_enu_Enable();
	UART_enu_Init();
	ADC_enu_Init();
	DIO_enu_SetPinDirection(DIO_U8_PORTC,DIO_U8_PIN0,DIO_U8_OUTPUT);
	DIO_enu_SetPinDirection(DIO_U8_PORTC,DIO_U8_PIN1,DIO_U8_OUTPUT);
	DIO_enu_SetPinDirection(DIO_U8_PORTC,DIO_U8_PIN2,DIO_U8_OUTPUT);
	DIO_enu_SetPinDirection(DIO_U8_PORTC,DIO_U8_PIN3,DIO_U8_OUTPUT);
	DIO_enu_SetPinDirection(DIO_U8_PORTB,DIO_U8_PIN3,DIO_U8_OUTPUT);
	Timers_enu_Init(TIMER0,FAST_PWM_NON_INVERTING,CLK_8);
	Servo_enu_Init(1,0);
	DC_Motor_enu_Init(1,0);
	DC_Motor_enu_Rotate(MOTOR1,CW);
	LCD_enu_Init();
}
void Set_Machine_speed(void)
{
	u8 speed = 0;
	ADC_enu_GetDigitalValueSynchNonBlocking(ADC_U8_CHANNEL0,&digitalValue);
	speed =(digitalValue * 255UL)/1023;
	Timers_enu_TimerSetCompareValue(TIMER0,speed);
}
void Normal_state(void)
{
	u8 i = 0;
	u8 temp = 0;
	if(!tempFlag){
		LCD_enu_Clear();
		LCD_enu_SendString("Normal State");
		LCD_enu_SetCursor(1,0);
		LCD_enu_SendString("Temp: ");
		tempFlag = 1;
	}
	LCD_enu_SetCursor(1,6);
	while(received_msg[i] != '\r'){
		temp = (temp*10) + (received_msg[i] - '0');
		i++;
	}
	LCD_enu_SendNum(temp);
	//_delay_ms(100);
	if(temp < 20)
	{
		/* turn on green led */
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN2,DIO_U8_HIGH);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN0,DIO_U8_LOW);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN1,DIO_U8_LOW);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN3,DIO_U8_LOW);
	}
	else if((temp >= 20)&&(temp <= 40))
	{
		/* turn on yellow led */
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN1,DIO_U8_HIGH);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN0,DIO_U8_LOW);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN2,DIO_U8_LOW);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN3,DIO_U8_LOW);
	}
	else if ((temp > 40)&&(temp <= 50))
	{
		/* turn on Red led */
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN0,DIO_U8_HIGH);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN1,DIO_U8_LOW);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN2,DIO_U8_LOW);
		DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN3,DIO_U8_LOW);
	}
	else if(temp > 50)
	{
		LCD_enu_Clear();
		LCD_enu_SendString("Emergency State");
		_delay_ms(200);
		Emeregency_state();
	}

}
void Emeregency_state(void)
{
	tempFlag = 0;
	/* lighten red LED & run buzzer */
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN0,DIO_U8_HIGH);
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN3,DIO_U8_HIGH);
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN1,DIO_U8_LOW);
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN2,DIO_U8_LOW);
}
void Abnormal_state_5Sec(void)
{
	tempFlag = 0;
	LCD_enu_Clear();
	LCD_enu_SendString("Abnormal State");
	_delay_ms(200);
	/* Count 5 Seconds */
	Timers_enu_SetTimerValue(TIMER2,211);
	Timers_enu_SetCallBack(TIMER2,&Timer2_InterruptHandler,NORMAL_MODE);
	Timers_enu_Init(TIMER2,NORMAL_MODE,TIMER2_CLK_1024);
	/* lighten red LED */
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN0,DIO_U8_HIGH);
	/* run the buzzer */
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN3,DIO_U8_HIGH);
	/* open servo with 90 degrees */
	Servo_enu_SetAngle(MOTOR_1, 90);
	/*  stop the machine */
	DC_Motor_enu_Stop(MOTOR1);
	/* Wait 5 Seconds */
	while(!Timer2_Falg);
	Timer2_Falg = 0;
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN0,DIO_U8_LOW);
	DIO_enu_SetPinValue(DIO_U8_PORTC,DIO_U8_PIN3,DIO_U8_LOW);
	Servo_enu_SetAngle(MOTOR_1, 0);
}
void Stop_Machine(void){
    LCD_enu_Clear();
	LCD_enu_SendString("Stop Machine");
	DC_Motor_enu_Stop(MOTOR1);
}
void StringInterruptHandler(void)
{
	strFlag = 1;
}
void Timer2_InterruptHandler(void)
{
	static u16 counter = 0;
	counter++;
	if(306 == counter){
		Timer2_Falg = 1;
		counter = 0;
		Timers_enu_Stop(TIMER2);
	}
}
u8 strCmp(u8 * str1, u8 * str2){
	u8 i = 0 , cmpFlag = 0;
	while((str1[i] != '\0')&&(str2[i] != '\0')){
		if(str1[i]!=str2[i]){cmpFlag = 1;}
		i++;
	}
	return cmpFlag;
}
