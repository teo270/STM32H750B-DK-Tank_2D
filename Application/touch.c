
//touch.c

#include "touch.h"
#include "stm32h7xx_hal.h"
#include "stm32h750b_discovery_ts.h"
#include "stm32h750b_discovery_lcd.h"
#include "lcd.h"
#include "stm32_lcd.h"

/* definicija globalnih spremenljivk */ //tukaj se RAM dejansko rezervira
uint32_t touch_x_size;
uint32_t touch_y_size;



static TS_Init_t hTSs;  // static  vidna samo znotraj touch.c

void Touch_Init(void)
{
	 //Touch screen parameters
    BSP_LCD_GetXSize(0, &touch_x_size);
    BSP_LCD_GetYSize(0, &touch_y_size);

    hTSs.Width = touch_x_size;
    hTSs.Height = touch_y_size;
    hTSs.Orientation = TS_SWAP_XY;
    hTSs.Accuracy = 5;

    /* Touchscreen initialization */
    BSP_TS_Init(0, &hTSs);



}




void Display_String(void)
{
	// Set text color
	   	  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

	   	  //Set back color
	   	  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);

	   	  // set font size
	   	  UTIL_LCD_SetFont(&Font12);

	   	  //UTIL_LCD_DisplayStringAt(uint32_t Xpos, uint32_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode);

	   	  UTIL_LCD_DisplayStringAt(touch_x_size/2-70, touch_y_size/2, (uint8_t *)"Press The Screen To Start The Game", LEFT_MODE);


}




// Press location
static TS_State_t TS_State;

static uint8_t touch = 0;

// Number of presses
static uint8_t num_press = 0;

void Touch_Process(uint16_t x_size, uint16_t y_size)
{
    BSP_TS_GetState(0, &TS_State);

    // dotik zaznan
    if (TS_State.TouchDetected > 0 && touch == 0)
    {
        touch = 1;
        UTIL_LCD_Clear(UTIL_LCD_COLOR_BLUE);

        // preveri, če je gumb pritisnjen
        if (TS_State.TouchX > x_size/2 - 40 &&
            TS_State.TouchX < x_size/2 + 40 &&
            TS_State.TouchY > y_size/2 - 20 &&
            TS_State.TouchY < y_size/2 + 20)
        {
            UTIL_LCD_FillRect(x_size/2 - 50, y_size/2 - 20, 100, 40, UTIL_LCD_COLOR_BLACK);

            if (num_press == 0)
            {
                num_press++;
                UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
                UTIL_LCD_DisplayStringAt(x_size/2 - 20, y_size/2 - 5,
                                          (uint8_t *)"THANKS", LEFT_MODE);
            }
            else
            {
                UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
                UTIL_LCD_DisplayStringAt(x_size/2 - 33, y_size/2 - 5,
                                          (uint8_t *)"Only Once!!", LEFT_MODE);
            }
        }
    }

    // ni več dotika
    if (TS_State.TouchDetected == 0)
    {
        touch = 0;
    }
}
