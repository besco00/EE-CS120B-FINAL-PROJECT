/*
 * EE_CS120B_Custome_lab.c
 *
 * Created: 7/21/2019 5:45:17 PM
 * Author : bhrayan
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "io.h"
#include "timer.h"
//#include "bit.h"
#include "scheduler.h"
#include <stdlib.h>





// Declaring inputs (buttons, switches, etc.)
#define F_CPU 1000000UL
#define UP (~PINA & 0x01) //the move up button
#define DOWN (~PINA & 0x02)// the down button
#define start (~PINA &0x04)// the start button
#define DEFAULT_PERIOD 100
#define  UP_P2 (~PINB & 0x01)
#define  DOWN_P2 (~PINB & 0x02)
#define SHOOT (~PINB & 0x04)


//extra variables
unsigned char gameState;
unsigned char sec_2= 10;
unsigned char counter =0;

unsigned char select_char;
unsigned char POSITION =7;
uint8_t highScore;
int current_score;
unsigned char CURRENT_PLAYER;
unsigned char speed= DEFAULT_PERIOD;
unsigned char CURRENT_PLAYER2_POS=16;
unsigned char CURRENT_PLAYER1_POS=1;

//Declaring special characters, level and character start postion
unsigned char player_1 [8]= { 0x0E, 0x0E, 0x04, 0x04, 0x1F, 0x04, 0x0A, 0x0A };
unsigned char player_2 [8]=  {0x00,0x00,0x00,0x0A,0x04,0x04,0x0A,0x00};
unsigned char player_3 [8]= {0x00,0x00,0x04,0x0A,0x1F,0x0A,0x00,0x00};
unsigned char player_4 [8]= {0x00,0x00,0x04,0x0A,0x04,0x04,0x0A,0x00};
//unsigned char character_location=0;


//LCD_WriteData(0x08);


//menu function
void update_menu(){
	 LCD_init();
	 LCD_ClearScreen();
	 LCD_DisplayString(18, "highscore: ");
	 LCD_Cursor(29);
	 highScore = eeprom_read_byte((uint8_t*)22);
	 if(highScore > 20){
		 highScore = 0;
	 }
	 eeprom_write_word(22, highScore);
	 LCD_WriteData(highScore + '0');
	 LCD_Cursor(31);
	
	 
	 LCD_createChar(0, player_1);
	 LCD_createChar(1, player_2);
	 LCD_createChar(2, player_3);
	 LCD_createChar(3, player_4);
	
	 LCD_Cursor(7);
	 LCD_WriteData(0x00);
	 LCD_WriteData(0x01);
	 LCD_WriteData(0x02);
	 LCD_WriteData(0x03);
	 LCD_Cursor(POSITION);

}



//state machine for the start of the game
enum menu_states{ START, TITLE_SCREEN, INSTRUCTIONS, WAIT, SELECT, WAIT2, INC,WAIT3, START_GAME }menu_states;
void start_menu(){
	switch (menu_states){
		case START:
			menu_states=TITLE_SCREEN;
			
			break;
		case TITLE_SCREEN:
			if(counter>=sec_2){
			menu_states= INSTRUCTIONS;	
			LCD_ClearScreen();
			
			counter=0;
			}
			else{
				menu_states= TITLE_SCREEN;
			}

			break;
		case INSTRUCTIONS:
			if(start && (counter<100)){
				menu_states=WAIT;
				
				//LCD_ClearScreen();
			}
			else if(!(counter<100) && !start){
				menu_states=INSTRUCTIONS;
			}
			
			else{
				menu_states=INSTRUCTIONS;
			}
			break;
		case WAIT:
			if(start){
				
				menu_states= WAIT;
				
				
			}
			else if(!start){
				menu_states=SELECT;
			}
			else{
				menu_states=WAIT;
			}
			break;
		case SELECT:
		    if(UP&&!start){
				menu_states= WAIT2;
			}
			else if(start&&!UP){
				menu_states=WAIT3;
			}
			else{
				menu_states=SELECT;
			}
			break;
		case WAIT2:
			 if(!UP){
				menu_states=INC;
			}
			else{
				menu_states=WAIT2;
			}
		break;
		case INC:
			menu_states=SELECT;
			break;
		case WAIT3:
			if(!start){
				menu_states=START_GAME;
			}
			else if(start){
				menu_states=WAIT3;
			}
			break;
		case START_GAME:
			if(gameState=0x01){
				menu_states=START_GAME;
			}
			else{
			menu_states=SELECT;
			}
			break;
	}
			
	switch(menu_states){
		case START:
		menu_states= TITLE_SCREEN;
		
			break;
		case TITLE_SCREEN:
		
			LCD_DisplayString_NO_CLEAR(1,"WELCOME TO");
	
			LCD_DisplayString_NO_CLEAR(18,"DODGE QUICK");
			counter++;
			break;
		case INSTRUCTIONS:
			
				LCD_DisplayString_NO_CLEAR(1,"PRESS START TO");
				LCD_DisplayString_NO_CLEAR(18, "BEGIN");
				counter++;

			break;
			case WAIT:
			//LCD_ClearScreen();
				break;
			case SELECT:
			break;
			case WAIT2:
			break;
			case INC:
			if(POSITION <9){
				POSITION++;	
			}
			else{
				POSITION=7;
			}
			update_menu();
			break;
		case WAIT3:
			break;
			
		case START_GAME:
			gameState=0x01;
			switch (POSITION){
				case 7:
				CURRENT_PLAYER=0x00;
				break;
			case 8:
			CURRENT_PLAYER=0x01;
			break;
			case  9:
			CURRENT_PLAYER=0x02;
			break;
				
			}
			render();
			break;
	}
	}
			
			
//PLAYER 1 POSITION STATE MACHINE
enum p1_pos{INIT, WAIT_1,BUTTON_UP,WAIT_2, BUTTON_DOWN,}p1_pos;
void P1_POS(){
	//transitions
	switch(p1_pos){
		
	
	case INIT:
		if(UP && !DOWN){
			p1_pos=WAIT_1;
		}
		else if(!UP && DOWN){
			p1_pos= WAIT_2;
		}
		else{
			p1_pos= INIT;
		}
		break;
	case WAIT_1:
		if(UP){
			p1_pos=WAIT_1;
		}
		else if(!UP){
		p1_pos=BUTTON_UP;
			
		}
		break;
		
	case BUTTON_UP:
		p1_pos= INIT;
		break;
	case WAIT_2:
		if(DOWN){
			p1_pos=WAIT_2;
		}
		else if(!DOWN){
			p1_pos=BUTTON_DOWN;
			
		}
		
		break;
	case BUTTON_DOWN:
		p1_pos= INIT;
		break;
		
	}
	switch(p1_pos){
		case INIT:
		break;
		case WAIT_1:
		break;
		case BUTTON_DOWN:
			CURRENT_PLAYER1_POS=17;
			
			break;
		case WAIT_2:
		break;
		case BUTTON_UP:
			CURRENT_PLAYER1_POS =1;
		
			break;
	}
	}
//player 2 logic
enum P2_LOGIC{INIT_P2, WAIT_P1, BUTTON_UP2, WAIT_P2, BUTTON_DOWN2}P2_LOGIC;
	void P2_logic(){
		//transitions
		switch(P2_LOGIC){
			case INIT_P2:
				if(UP_P2 && !DOWN_P2){
					P2_LOGIC=WAIT_P1;
				}
				else if(!UP_P2 && DOWN_P2){
					P2_LOGIC= WAIT_P2;
				}
				else{
					P2_LOGIC= INIT_P2;
				}
				break;
			case WAIT_P1:
				if(UP_P2){
					P2_LOGIC=WAIT_P1;
				}
				else if(!UP_P2){
					P2_LOGIC=BUTTON_UP2;
					
				}
				else{
					P2_LOGIC=WAIT_P1;
				}
				break;
			case BUTTON_UP2:
				P2_LOGIC= INIT_P2;
				break;
			case WAIT_P2:
				if(DOWN_P2){
					P2_LOGIC=WAIT_P2;
				}
				else if(!DOWN_P2){
					P2_LOGIC=BUTTON_DOWN2;
					
				}
				else{
					P2_LOGIC=WAIT_P2;
				}
				break;
			case BUTTON_DOWN2:
			 P2_LOGIC= INIT_P2;
			 break;
			 
		}
		switch(P2_LOGIC){
			case INIT_P2:
			break;
			case WAIT_P1:
			break;
			case BUTTON_DOWN2:
			CURRENT_PLAYER2_POS=32;
			
			break;
			case WAIT_P2:
			break;
			case BUTTON_UP2:
			CURRENT_PLAYER2_POS =16;
			
			break;
		}
		}

enum render_state{RENDER}render_state;
void render(){
	switch(render_state){
		case RENDER:
		render_state=RENDER;
		break;
	}
	switch(render_state){
		case RENDER:
		if(UP || DOWN || UP_P2 || DOWN_P2){
		LCD_init();
		//LCD_ClearScreen();
		LCD_Cursor(CURRENT_PLAYER1_POS);
		LCD_WriteData(CURRENT_PLAYER);
		//delay_ms(30);
		
		LCD_Cursor(CURRENT_PLAYER2_POS);
		LCD_WriteData(0x03);
		}
		break;
	}
		
	}
	

	
 

// EEPROM DECLARATIONS
void EEPROM_Write(unsigned char address, unsigned char data) {
	eeprom_write_byte(address, data);
}

unsigned char EEPROM_Read(unsigned char address) {
	return eeprom_read_byte(address);
}

void check_High_Score() {
	if (current_score > highScore) {
		eeprom_write_byte((uint8_t*)22, current_score);
		highScore = eeprom_read_byte((uint8_t*)22);
		
	}
}

int main(void)
{
 DDRA = 0x00; PORTA= 0xFF;
 DDRB= 0xFF; PORTB= 0x00;
 DDRC= 0xFF; PORTC= 0xFF;
 DDRD= 0xC0; PORTD= 0x7F;  
 LCD_init();
 //static task 
 static task task1;
static task task2;
static task task3;
 
 task * tasks[]={ & task1, & task2, & task3};
const unsigned short numTasks = sizeof(tasks)/sizeof(task *);

//task 1 initialization
task1.state =0;
task1.period=1;
task1.elapsedTime=0;
task1.TickFct= & P1_POS;

task2.state=0;
task2.period=1;
task2.elapsedTime=0;
task2.TickFct = & render;

task3.state=0;
task3.period=1;
task3.elapsedTime=0;
task3.TickFct = & P2_logic;

 TimerSet(speed);
  TimerOn();
unsigned short i;
 menu_states= START;
    while (1) 
    {
		if(gameState == 0x00){
			
			start_menu();
		
		
		}
		if(gameState==0x01){
			
	
		for (i = 0; i < numTasks; i++) {
			if (tasks[i]-> elapsedTime == tasks[i]-> period) {
				tasks[i]-> state = tasks[i] -> TickFct(tasks[i]-> state);
				tasks[i]-> elapsedTime = 0;
			}
			tasks[i]-> elapsedTime += 1;
		}
		}
		
	
		 while (!TimerFlag);
		 TimerFlag = 0;
	 }
		
		
	
return 0;
    }


