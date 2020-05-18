/*
 * Ultrasonic sensor HC-05 interfacing with AVR ATmega16
 * http://www.electronicwings.com
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define  Trigger_pin	PIND4	/* Trigger pin */

int TimerOverflow = 0;
int rise;
long count;
double distance;

void pulso()
{
 PORTD |= (1 << Trigger_pin);
 _delay_us(10);
 PORTD &= (~(1 << Trigger_pin));	
}

ISR(TIMER2_OVF_vect)
{
	TimerOverflow++;		/* Increment Timer Overflow count */
}

ISR(TIMER1_CAPT_vect)
{
	if (rise==1)
	{
		TCCR1B = 0x01;		/* Capture on falling edge, No prescaler */
		TimerOverflow = 0;	/* Clear Timer overflow count */
		rise=0;
		TCNT2 = 0;			/* Clear Timer counter */
	}
	else
	{
		TCCR1B = 0x41;		/* Capture on rising edge, No prescaler*/
	 	count = ICR1 + (65535 * TimerOverflow);		/* Take count */
	 	/* 8MHz Timer freq, sound speed =343 m/s */
	 	distance = (double)count / 466.47;
		rise=1;
		TCNT2 = 0;	
	}
	

}

int main(void)
{

	
	DDRD = 0x11;		/* Make trigger pin as output */
	//PORTD = 0xFF;		/* Turn on Pull-up */
	
	
	TIMSK2 = 0x01;		/* Enable Timer2 overflow interrupts */
	//TCCR1A = 0;		/* Set all bit to zero Normal operation */
	TCCR2B = 1;
	TCCR1B = 0x41;		/* Capture on rising edge, No prescaler*/
	TIMSK1|=(1<<ICIE1);
	sei();				/* Enable global interrupt */
	rise=1;

	while(1)
	{
		pulso();
		TCNT2 = 0;
		//TCNT1 = 0;	/* Clear Timer counter */		
		
		if (distance>=30.0)
		{
			PORTD=0x01;
		}
		
		else
		{
			PORTD=0x00;
		}
	}
}