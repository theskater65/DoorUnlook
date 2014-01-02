/*
 * LoginDoor.c
 *
 * Created: 21/12/2013 6:12:44 p. m.
 *  Author: andy
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#define BIT_TOGGLE(addr,bit) ((addr) ^= (bit))
#define SET_HIGH(addr,bit) ((addr) |= (1<<bit))
#define SET_LOW(addr,bit) ((addr) &= ~(1<<bit))
#define INI 0
#define IDLE 1
#define KEY 2
#define UNLOCK 3

void ShiftR(int index)
{
	switch (index)
	{
		case 0:
			// PB0 PD7 PD6 PD5
			//  0   1   1   1
			SET_HIGH(PORTD,PD5);
			SET_LOW(PORTB,PB0);
			break;
		case 1:
			// PB0 PD7 PD6 PD5
			//  1   0   1   1
			SET_HIGH(PORTB,PB0);
			SET_LOW(PORTD,PD7);
			break;
		case 2:
			// PB0 PD7 PD6 PD5
			//  1   1   0   1
			SET_HIGH(PORTD,PD7);
			SET_LOW(PORTD,PD6);
			break;
		case 3:
			// PB0 PD7 PD6 PD5
			//  1   1   1   0
			SET_HIGH(PORTD,PD6);
			SET_LOW(PORTD,PD5);
			break;
	}
		
}

int OnKeyPressed()
{
	if (((PINB7 == PINB6) == PIND4) == PIND3)
		return 0;
	else
		return 1;
}

char GetKey(int *row,int *col)
{
	
	if (col[0]==0 && row[0]==0)
		return '1';
	if (col[0]==0 && row[1]==0)
		return '2';
	if (col[0]==0 && row[2]==0)
		return '3';
	if (col[0]==0 && row[3]==0)
		return 'A';
	if (col[1]==0 && row[0]==0)
		return '4';
	if (col[1]==0 && row[1]==0)
		return '5';
	if (col[1]==0 && row[2]==0)
		return '6';
	if (col[1]==0 && row[3]==0)
		return 'B';
	if (col[2]==0 && row[0]==0)
		return '7';
	if (col[2]==0 && row[1]==0)
		return '8';
	if (col[2]==0 && row[2]==0)
		return '9';
	if (col[2]==0 && row[3]==0)
		return 'C';
	if (col[3]==0 && row[0]==0)
		return '*';
	if (col[3]==0 && row[1]==0)
		return '0';
	if (col[3]==0 && row[2]==0)
		return '#';
	if (col[3]==0 && row[3]==0)
		return 'D';
	else
	{
		return 'N';
	}
}

int main(void)
{
	/*door & led output*/
	DDRB = (1<<PB1);
	
	/* 
	filas:		inputs  high->	PD3 PD4 PB6 PB7
	columnas:	outputs high->	PD5 PD6 PD7 PB0		
	*/
	DDRD = (1<<PD5)|(1<<PD6)|(1<<PD7);
	DDRB = (1<<PB0);
	
	
	int rows[4]; // PB7 PB6 PD4 PD3
	int cols[4]; // PB0 PD7 PD6 PD5
	char password[4]="0712";	char currKey[4];
	int shifted=0;	int contKey=0;
	
	int currState;
	int nextState=INI;
    while(1)
    {	
		currState = nextState;
		switch (currState)
		{
			case INI:
				// COLUMNAS OUTPUTS
				SET_HIGH(PORTB,PB0); //1
				SET_HIGH(PORTD,PD7); //2
				SET_HIGH(PORTD,PD6); //3
				SET_HIGH(PORTD,PD5); //4
				// FILAS INPUTS PULL-UP
				SET_HIGH(PORTB,PB7); //5
				SET_HIGH(PORTB,PB6); //6
				SET_HIGH(PORTD,PD4); //7
				SET_HIGH(PORTD,PD3); //8
				
				//currKey=NULL;
				shifted=0;
				contKey=0;
				_delay_ms(1000);
				nextState = IDLE;
				break;
					
			case IDLE:
				ShiftR(shifted);
				shifted++;
				if (shifted==4)
					shifted=0;
				//_delay_ms(20);
				nextState=KEY;
				break;
			
			case KEY:
				if (OnKeyPressed()==1)
				{
					_delay_ms(20);
					rows[0]=PINB7; rows[1]=PINB6; rows[2]=PIND4; rows[3]=PIND3;
					cols[0]=PINB0; cols[1]=PIND7; cols[2]=PIND6; cols[3]=PIND5;
					
					currKey[contKey] = GetKey(&rows[0],&cols[0]);
					contKey++;
					if (contKey==4)
					{
						contKey=0;
						nextState=UNLOCK;
					}
				}
				else
					nextState=IDLE;
				break;
				
			case UNLOCK:
				if (strcmp(currKey,password)==0)
				{
					// door unlock
					SET_HIGH(PORTB,PB1);
					_delay_ms(2000);
					BIT_TOGGLE(PORTB,PB1);
				}
				else
				{
					// led protoboard para ver que fallo
					SET_HIGH(PORTB,PB1);
					_delay_ms(500);
					BIT_TOGGLE(PORTB,PB1);
				}
				nextState=INI;
				break;
		}
				
	}
}
