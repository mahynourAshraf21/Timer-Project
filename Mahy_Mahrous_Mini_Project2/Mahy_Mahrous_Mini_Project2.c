/*
 Group 80
 Mahynour Ashraf Saeed
 fb name Mahy Mahrous
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define SEVEN_SEGMENT_DIGITS 6
unsigned char se7enSegmentNums[SEVEN_SEGMENT_DIGITS]={0};
ISR(TIMER1_COMPA_vect){
	TCNT1=0; //set the initial value
	se7enSegmentNums[0]++; //increment the first digit in the seven segment
	if(se7enSegmentNums[0]==10){ //handling when the first digit reaches 10
		se7enSegmentNums[0]=0;
		se7enSegmentNums[1]++;

	}
	if(se7enSegmentNums[1]==6){ //handling when the seconds reaches 60
		se7enSegmentNums[1]=0;
		se7enSegmentNums[2]++; //increment the minutes

	}
	if(se7enSegmentNums[2]==10){ //handling when the third digit reaches 10
		se7enSegmentNums[2]=0;
		se7enSegmentNums[3]++;

	}
	if(se7enSegmentNums[3]==6){ //handling when the minutes reaches 60
		se7enSegmentNums[3]=0;
		se7enSegmentNums[4]++; //increment the hours

	}
	if(se7enSegmentNums[4]==10){ //handling when the fifth digit reaches 10
		    se7enSegmentNums[4]=0;
			se7enSegmentNums[5]++;

		}
		if(se7enSegmentNums[5]==10){ //when the hours reaches 100
			se7enSegmentNums[0]=0; //reset the timer
			se7enSegmentNums[1]=0;
			se7enSegmentNums[2]=0;
			se7enSegmentNums[3]=0;
			se7enSegmentNums[4]=0;
			se7enSegmentNums[5]=0;


		}
}
ISR(INT0_vect){ //interrupt for reseting
	for(int i=0 ; i<SEVEN_SEGMENT_DIGITS ; i++){
      se7enSegmentNums[i]=0; //reset all the digits
	}
	 //WGM12 enable the compare register
	// FOC1A and FOC1B disable PWM
	  TCCR1A = (1<<WGM12) | (1<<FOC1A) | (1<<FOC1B);
	 // CS10 and CS12 to enable prescaler with 1024 clock
	  TCCR1B = (1<< CS10) | (1<<CS12);
	_delay_ms(20); //waste time till return back to counting
}
ISR(INT1_vect){ //interrupt for pausing
	//close the clock
	TCCR1B &= ~(1<<CS12) & ~(1<<CS10);
}
ISR(INT2_vect){ //interrupt for resuming
	TCCR1A = (1<<WGM12) | (1<<FOC1A) | (1<<FOC1B); //return back to counting
	TCCR1B = (1<< CS10) | (1<<CS12);
}



void enableInterrupt0(void){
	DDRD &= ~(1<<PD2); //set the direction to input
	//enable external interrupt 0
	GICR |= (1<<INT0);
	//enable timer with falling edge
	MCUCR |= (1<<ISC11) | (1<<ISC10);

}
void enableInterrupt1(void){
	DDRD &= ~(1<<PD3); //set the direction to input
	//enable external interrupt 1
	GICR |= (1<<INT1);
	//enable interrupt with rising edge
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
}
void enableInterrupt2(void){
	DDRB &= ~(1<<PB2); //set the direction to input
	//enable external interrupt 2
	GICR |= (1<<INT2);
	MCUCSR &= ~(1<<ISC2);
	//enable interrupt with falling edge
	//MCUCSR &= ~(1<<ISC2);
}
void enableTimer(void){
	TIMSK |=(1<<OCIE1A); //enable timer with compare mode
	TCNT1=0; //set initial value
	// set the compare mode and disable PWM
	TCCR1A = (1<<WGM12) | (1<<FOC1A) | (1<<FOC1B);
	//set the prescaler
	TCCR1B = (1<< CS10) | (1<<CS12);
	OCR1A = 1000; //compare value according to the calculations
}
int main(void){
	SREG |= (1<<7); //enable i bit
	DDRC |= 0x0F; //enable the four pins that is connected to the decoder to output pins
	PORTC &= 0xF0; //set them to zero
	//configure the 6 pins that controller the seven segments to output pins
	DDRA |= (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5);
	//set them to zero at the beginning
	PORTA &= ~(1<<0) & ~(1<<1) & ~(1<<2) & ~(1<<3) & ~(1<<4) & ~(1<<5);
	// enable the internal pull up of the pins
	PORTD |= (1<<PD2);
	PORTB |= (1<<PB2);
	enableInterrupt0();
	enableInterrupt1();
	enableInterrupt2();
	enableTimer();
	while(1){
		for(int i=0 ; i<SEVEN_SEGMENT_DIGITS ; i++){
			//enable the pin connected to the seven segment to be displayed
			PORTA |= (1<<i);
			//displaying the number on the port
			PORTC = (PORTC & 0xF0) | (se7enSegmentNums[i] & 0x0F);
			_delay_ms(4);
			//disable them all
			PORTA &= ~(1<<0) & ~(1<<1) & ~(1<<2) & ~(1<<3) & ~(1<<4) & ~(1<<5);
		}


	}

}


