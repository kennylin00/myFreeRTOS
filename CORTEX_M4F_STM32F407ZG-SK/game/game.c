#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOVE 1
#define BANG 2
#define POWERBALL_GREEN 3
#define POWERBALL_RED 4


//Player1
int16_t player1X = 10;
int16_t player1Y = 10;
uint16_t player1W = 60;
uint16_t player1H = 10;
uint8_t player1IsReversed = 1;

//Player2
int16_t player2X = LCD_PIXEL_WIDTH - 20;
int16_t player2Y = LCD_PIXEL_HEIGHT - 20;
uint16_t player2W = 60;
uint16_t player2H = 10;
uint8_t player2IsReversed = 0;

//Ball
uint16_t ballSize = 5;
int16_t ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
int16_t ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;
int16_t volatile ballVX = 5;
int16_t volatile ballVY = 5;
uint8_t ballIsRun = 0;

//Mode
uint8_t demoMode = 0;

//PowerBall
int PowerBallX = 0;
int PowerBallY = 0;
static volatile int CrushFlag = MOVE;
int PowerBallType = POWERBALL_RED;
extern int RefreshSpeed;

void
BallReset()
{
	ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
	ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;

		ballVX = 5;
		ballVY = 5;
	ballIsRun = 1;
}

void
GAME_EventHandler1()
{
	if( STM_EVAL_PBGetState( BUTTON_USER ) ){

		player1IsReversed = 0;

		while( STM_EVAL_PBGetState( BUTTON_USER ) );

		player1IsReversed = 1;
	}
}

void
GAME_EventHandler2()
{
	if( IOE_TP_GetState()->TouchDetected ){

		player2IsReversed = 1;

		while( IOE_TP_GetState()->TouchDetected );

		player2IsReversed = 0;
	}
}

void
GAME_EventHandler3()
{
	if( ballIsRun == 0 ){
		BallReset();
	}
}

void
GAME_PowerBall()
{
	int i,j=0;
	int Type = (rand()%2)+1;
	if(Type == 1)
		PowerBallType = POWERBALL_GREEN;
	else
		PowerBallType = POWERBALL_RED;

	//PowerBallX = (rand()% 230)+ 50;
	//PowerBallY = (rand()% 200)+ 0;
	PowerBallX = 110;//(rand()% 230)+ 50; //110
	PowerBallY = 160;//(rand()% 200)+ 50; //160


	while(j<5){
		if(CrushFlag == BANG) {
			CrushFlag = MOVE;
			break;
		}
		if(PowerBallType == POWERBALL_GREEN)
			LCD_SetTextColor( LCD_COLOR_GREEN );
		else
			LCD_SetTextColor( LCD_COLOR_RED );

		LCD_DrawFullRect( PowerBallX, PowerBallY, ballSize, ballSize );


		for(i=0;i<1000000;i++) {
		 __asm volatile("nop");	
		 __asm volatile("nop");	
		 __asm volatile("nop");	
		 __asm volatile("nop");	
		 __asm volatile("nop");	
		}

		LCD_SetTextColor( LCD_COLOR_BLACK );
		LCD_DrawFullRect( PowerBallX, PowerBallY, ballSize, ballSize );
		j++;

	}
	

}

int DetectCrush(int Src, int Targ)
{
	if(((Targ - Src) >=-4) && ((Targ -Src) <=4))
		return 1;
	else
		return 0;
}

void
GAME_Update()
{

	int i;
	//Player1
	LCD_SetTextColor( LCD_COLOR_BLACK );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );

	if( demoMode == 0 ){

		if( player1IsReversed )
			player1X -= 5;
		else
			player1X += 5;

		if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;

		//Player2
		if( player2IsReversed )
			player2X -= 5;
		else
			player2X += 5;

		if( player2X <= 0 )
			player2X = 0;
		else if( player2X + player2W >= LCD_PIXEL_WIDTH )
			player2X = LCD_PIXEL_WIDTH - player2W;

		//Ball
		if( ballIsRun == 1 ){

			LCD_SetTextColor( LCD_COLOR_BLACK );
			LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );

			if(DetectCrush(ballX, PowerBallX) && (DetectCrush(ballY, PowerBallY))){
				RefreshSpeed=5;
				LEDLightOff(4);
				LEDLightOff(3);
				CrushFlag = BANG;
				LCD_SetTextColor( LCD_COLOR_BLACK );
				LCD_DrawFullRect( PowerBallX, PowerBallY, ballSize, ballSize );
		
				LCD_SetTextColor( LCD_COLOR_YELLOW );
				LCD_DrawCircle( PowerBallX +2, PowerBallY +2, 5 );
				
				if(PowerBallType == POWERBALL_GREEN){
					RefreshSpeed= 10;
					LEDLightOn(3);
					LEDLightOff(4);
				}
				else{
					RefreshSpeed= 5;
					LEDLightOn(4);
					LEDLightOff(3);
				}

				LCD_DrawCircle( PowerBallX+2, PowerBallY +2, 5 );
				for(i=0;i<500000;i++) {
					 __asm volatile("nop");
					 __asm volatile("nop");	
					 __asm volatile("nop");	
				}	
				LCD_SetTextColor( LCD_COLOR_BLACK );
				LCD_DrawCircle( PowerBallX+2, PowerBallY +2, 5 );
				
			}


			//Touch wall
			ballX += ballVX;
			if( ballX <= 0 ){
				ballX = 0;
				ballVX *= -1;
			}
			else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
				ballX = LCD_PIXEL_WIDTH - ballSize;
				ballVX *= -1;
			}

			//PONG!
			ballY += ballVY;
			if( ballY + ballSize >= player2Y ){
				if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
					if( ballX - ballSize <= player2Y + player2W/4 ){
						ballVY =-3;
						ballVX =-7;
					}
					else if( ballX >= player2Y + player2W - player2W/4 ){
						ballVY =-3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player2Y + player2W/2 ){
						ballVY =-7;
						ballVX =-3;
					}
					else if( ballX > player2Y + player2W/2 ){
						ballVY =-7;
						ballVX = 3;
					}
					else{
						ballVY =-9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}

			if( ballY <= player1Y + player1H ){

					if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
						if( ballX - ballSize <= player1Y + player1W/4 ){
							ballVY = 3;
							ballVX =-7;
						}
						else if( ballX >= player1Y + player1W - player1W/4 ){
							ballVY = 3;
							ballVX = 7;
						}
						else if( ballX + ballSize < player1Y + player1W/2 ){
							ballVY = 7;
							ballVX =-3;
						}
						else if( ballX > player1Y + player1W/2 ){
							ballVY = 7;
							ballVX = 3;
						}
						else{
							ballVY = 9;
							ballVX = 0;
						}
					}
					else
						BallReset();
				}
			}
		}
		else{	//if demoMode == 1

			//Player1 move
			if( ballVY < 0 ){
				if( player1X + player1W/2 < ballX + ballSize/2 ){
					player1X += 8;
					player2X += 2;
				}
				else{
					player1X -= 8;
					player2X -= 2;
				}
			}

			//Player2 move
			if( ballVY > 0 ){
				if( player2X + player2W/2 < ballX + ballSize/2 ){
					player1X += 2;
					player2X += 8;
				}
				else{
					player1X -= 2;
					player2X -= 8;
				}

			}

			if( player1X <= 0 )
				player1X = 0;
			else if( player1X + player1W >= LCD_PIXEL_WIDTH )
				player1X = LCD_PIXEL_WIDTH - player1W;

			if( player2X <= 0 )
				player2X = 0;
			else if( player2X + player2W >= LCD_PIXEL_WIDTH )
				player2X = LCD_PIXEL_WIDTH - player2W;


			//Ball
			if( ballIsRun == 1 ){

				LCD_SetTextColor( LCD_COLOR_BLACK );
				LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );

				//Touch wall
				ballX += ballVX;
				if( ballX <= 0 ){
					ballX = 0;
					ballVX *= -1;
				}
				else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
					ballX = LCD_PIXEL_WIDTH - ballSize;
					ballVX *= -1;
				}

				//PONG!
				ballY += ballVY;
				if( ballY + ballSize >= player2Y ){
					if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
					if( ballX - ballSize <= player2Y + player2W/4 ){
						ballVY =-3;
						ballVX =-7;
					}
					else if( ballX >= player2Y + player2W - player2W/4 ){
						ballVY =-3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player2Y + player2W/2 ){
						ballVY =-7;
						ballVX =-3;
					}
					else if( ballX > player2Y + player2W/2 ){
						ballVY =-7;
						ballVX = 3;
					}
					else{
						ballVY =-9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}

			if( ballY <= player1Y + player1H ){
				if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
					if( ballX - ballSize <= player1Y + player1W/4 ){
						ballVY = 3;
						ballVX =-7;
					}
					else if( ballX >= player1Y + player1W - player1W/4 ){
						ballVY = 3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player1Y + player1W/2 ){
						ballVY = 7;
						ballVX =-3;
					}
					else if( ballX > player1Y + player1W/2 ){
						ballVY = 7;
						ballVX = 3;
					}
					else{
						ballVY = 9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}
		}
	}
}

	void
GAME_Render()
{
	LCD_SetTextColor( LCD_COLOR_WHITE );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
	LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );
	LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
}
