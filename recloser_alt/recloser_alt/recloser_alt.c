/*
 * recloser_alt.c
 *
 * Created: 9/13/2019 8:57:37 PM
 *  Author: Hagar Usama
 */ 

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// port configuration migrating from PIC to AVR ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

B7:B2  (PIC) ----> B5:B1 (AVR)
D7:D5  (PIC) ----> C5:C3 (AVR)
D2:D0  (PIC) ----> C2:C0 (AVR)

*/


#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 8000000UL						/* Define CPU clock Frequency e.g. here its 8MHz */

#define P 0x00
#define R 0x08
#define S 0x04
#define T 0x02
#define U 0x18
#define O 0x11
#define V 0x22
#define T 0x88
#define Z 0x55
/**************** UART Functions *********************/
void USART_Init(uint16_t BAUDRATE);
char USART_RxChar();
void USART_TxChar(char data);
void USART_SendString(char *str);
/*****************************************************/
////////////////////// My Functions //////////////////
void enable_SIM();
void enable_Bluetooth();
void disable_Bluetooth();
uint8_t capture();
uint8_t compare_caps(uint8_t cap1 , uint8_t cap2);
char select_action(uint8_t choice);
void send_by_Bluetooth( char msg);
char recieve_by_Bluetooth();
void take_action(uint8_t cap1 , uint8_t cap2);

int main(void)
{
	DDRB = 0xff;		/* make PORT as input port */
	DDRC = 0x00;		/* make PORT as output port */
	USART_Init(9600);	/* initialize USART with 9600 baud rate */
	
	/**********************************************/
	
	PORTC |= 1<<PC0;
	uint8_t cap1 = capture();
	_delay_ms(1000);
	uint8_t cap2 = capture();
	
	take_action(cap1 , cap2);

	/**********************************************/
			
return 0;

}



void USART_Init(uint16_t BAUDRATE)				/* USART initialize function */
{ 
	uint16_t BAUD_PRESCALE  = (((F_CPU / (BAUDRATE * 16UL))) - 1); /* 51 */
	UCSRB |= (1 << RXEN) | (1 << TXEN);				/* Enable USART transmitter and receiver */
	UCSRC |= (1 << URSEL)| (1 << UCSZ0) | (1 << UCSZ1);	/* Write USCRC for 8 bit data and 1 stop bit */ 
	UBRRL = BAUD_PRESCALE;							/* Load UBRRL with lower 8 bit of prescale value */
	UBRRH = (BAUD_PRESCALE >> 8);					/* Load UBRRH with upper 8 bit of prescale value */
}

char USART_RxChar()									/* Data receiving function */
{
	while (!(UCSRA & (1 << RXC)));					/* Wait until new data receive */
	return(UDR);									/* Get and return received data */ 
}

void USART_TxChar(char data)						/* Data transmitting function */
{
	UDR = data;										/* Write data to be transmitting in UDR */
	while (!(UCSRA & (1<<UDRE)));					/* Wait until data transmit and buffer get empty */
}

void USART_SendString(char *str)					/* Send string of USART data function */ 
{
	int i=0;																	
	while (str[i]!=0)
	{
		USART_TxChar(str[i]);						/* Send each char of string till the NULL */
		i++;
	}
}


void enable_SIM(){
	for(uint8_t i=0 ; i<3; i++){
		PORTC |=1<<PC1;
		_delay_ms(1000);
		PORTC &= ~(1<<PC1);
		_delay_ms(1000);	
	}
	
}

void enable_Bluetooth(){
	PORTC |=1<<PC2;
	_delay_ms(100);
}


void disable_Bluetooth(){
	PORTC &= ~(1<<PC2);
	_delay_ms(100);
}


uint8_t capture(){ return PINB;}
uint8_t compare_caps(uint8_t cap1 , uint8_t cap2){
	if(cap2 == 0x00){
		/* random disrupt */
	}else if( cap2 != cap1){
		/* random disrupt but different */
		
	}else if( cap2 == cap1){
			
		return cap2;
	}
	
}

char select_action(uint8_t choice){
	
	//20 possibilities
	switch(choice){
		
		case R:
		
		PORTC |=1<<PC5; /* PC5 instead of PD7 */
		PORTC &= ~(1<<PC4);
		PORTC &= ~(1<<PC3);
		return 'R';		
		case S:
		/* code */
		return 'S';
		
		case T:
		/* code */
		return 'T';
		
		case U:
		/* code */
		return 'U';
		
		
		default:
		return 'P';
		
	}
	
	
}

void send_by_Bluetooth( char msg){
	USART_TxChar(msg);	
}

char recieve_by_Bluetooth(){
	USART_RxChar();	
}


void take_action(uint8_t cap1 , uint8_t cap2){
	char msg1 , msg2;
	msg1= select_action( compare_caps(cap1,cap2) );
	
	enable_SIM();
	enable_Bluetooth();
	
	while(msg2 !=msg1){
		send_by_Bluetooth(msg1);
		msg2 = recieve_by_Bluetooth();		
	}
	
	//useless
	disable_Bluetooth();
	//useless
	enable_SIM();
	//useless line
	enable_Bluetooth();
	
	uint8_t cap3 = capture();
	if(cap3 == P ) send_by_Bluetooth('P');
	else if( cap3 != cap2 ){
		PORTC |= 1<<PC2;
		send_by_Bluetooth(msg2);
	}	
	
}
