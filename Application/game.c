
//game.c

#include "Game.h"
#include "game_input.h"  // za t1_up, t1_down ...
#include "stm32h7xx_hal.h"
#include "stm32h750b_discovery.h"    // za BSP_LED_On/Off
#include "lcd.h"
#include "stm32_lcd.h"

#include "objects.h"
#include "graphics.h"

/*
void ConnectionTest(void)
{
    // Primer: LED1 bo gor, če je tipka W pritisnjena (t1_up)
	// Tipka W -> LED1
	    if (t1_up)
	        BSP_LED_On(LED1);
	    else
	        BSP_LED_Off(LED1);

	    // Tipka S -> LED2
	    if (t1_down)
	        BSP_LED_On(LED2);
	    else
	        BSP_LED_Off(LED2);

	    // Tipka I -> LED3
	    if (t2_up)
	    	BSP_LED_Off(LED3);   //malo obrnjena logika, ignor
	    else
	    	BSP_LED_On(LED3);
    // Če želiš, lahko dodaš tudi druge tipke in LED-e
    // npr. t1_down -> LED2
}

*/






#define MAX_TANKS 3

void Test_Game(void)
{

	volatile PlayerInput inputs[2];

	LCD_DrawSprite565_DMA2D(0,0,0, 160, 90, welcome_screen);

	HAL_Delay(2000);
	UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);


	Tank tanks[MAX_TANKS];

	Bullet bullets[MAX_BULLETS];
	int num_tanks = MAX_TANKS;


	OBJ_Tank_Init(&tanks[0], 100, 100, 0, MOVE_SPEED, Tank_Red);


	OBJ_Tank_Init(&tanks[1], 400, 100, 0, MOVE_SPEED, Tank_Blue);


	OBJ_Tank_Init(&tanks[2], 240, 110, 0, MOVE_SPEED, Tank_Orange);
	tanks[2].ovira =1;  // Ročno nastavim ovira

	//OBJ_Tank_Init(&tanks[3], 240, 50, 0, MOVE_SPEED, Tank_Orange);
	//tanks[3].ovira =1;  //




	// inicializacija izstrelkov
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		bullets[i].active = 0;
	}
	while (1)
	{

	        // * 1️ UPDATE LOGIKA
		//Game_Input_UpdateFromBuffer();  // update stanja tipk
		// 1. Posodobi vse inpute
		//Game_Input_ProcessWord(bits);

		// Kopiraj volatile PlayerInput iz ISR
		inputs[0] = player1;
		inputs[1] = player2;

	        // Tanki
		// Update vseh tankov
		OBJ_Tank_UpdateDirection_Single(&tanks[0], &inputs[0], tanks, num_tanks);
		OBJ_Tank_UpdateDirection_Single(&tanks[1], &inputs[1], tanks, num_tanks);

	        // Streljanje
	        /*
	        if (t1_fire);
	        {
	            FireBullet(&tanks[0], 1, bullets);
	            t1_fire = 0;
	        }
	        if (t2_fire)
	        {
	        	FireBullet(&tanks[1], 1, bullets);
	        	t2_fire = 0;
	        }

	*/


	        // po premikanju tankov
	        if (player1.fire) {
	            FireBullet(&tanks[0], 1, bullets);
	            player1.fire = 0;
	        }
	        if (player2.fire) {
	            FireBullet(&tanks[1], 2, bullets);
	            player2.fire = 0;
	        }


	        // * 2️ BULLETI: ERASE → UPDATE → DRAW

	        for (int i = 0; i < MAX_BULLETS; i++)
	        {
	            if (bullets[i].active)
	            {
	                //  1. izbriši PREJŠNJO pozicijo
	                GFX_EraseBullet(&bullets[i]);

	                //  2. posodobi (premik + hit)
	                OBJ_Bullet_Update(&bullets[i], tanks, num_tanks);

	                //  3. nariši novo, če je še aktiven
	                GFX_DrawBullet(&bullets[i]);
	            }
	        }


	         // 3RIŠI TANKE


	        for (int i = 0; i < num_tanks; i++)
	        {
	            GFX_DrawTank(&tanks[i]);
	        }

	        if (Check_Wining_Condition(tanks)) break;   // zapusti while zanko

	}

	UTIL_LCD_Clear(UTIL_LCD_COLOR_BLUE);
	HAL_Delay(2000);


}

// UPORABNIK LAHKO DODA GAME STATE MACHINE ZA LEPŠO BERLJIVOST/IZVAJANJE




