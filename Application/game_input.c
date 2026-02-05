
//game_input.c

#include "game_input.h"
#include "uart.h"

/*
volatile uint8_t t1_up, t1_down, t1_left, t1_right;
volatile uint8_t t2_up, t2_down, t2_left, t2_right;

volatile uint8_t t3_up, t3_down, t3_left, t3_right;
*/

//STREL
volatile uint8_t t1_fire = 0;
volatile uint8_t t2_fire = 0;

// globalni prejemni bajt
//uint8_t uart3_rx_char;





/*
void Game_Input_ProcessChar(uint8_t c)
{
    switch (c)
    {
        // Tank 1 (WASD)
        case 'W': t1_up = 1; break;
        case 'w': t1_up = 0; break;
        case 'A': t1_left = 1; break;
        case 'a': t1_left = 0; break;
        case 'S': t1_down = 1; break;
        case 's': t1_down = 0; break;
        case 'D': t1_right = 1; break;
        case 'd': t1_right = 0; break;

        // Tank 2 (IJKL)
        case 'I': t2_up = 1; break;
        case 'i': t2_up = 0; break;
        case 'J': t2_left = 1; break;
        case 'j': t2_left = 0; break;
        case 'K': t2_down = 1; break;
        case 'k': t2_down = 0; break;
        case 'L': t2_right = 1; break;
        case 'l': t2_right = 0; break;

        //  STREL
        case 'R': t1_fire = 1;
        break;

        case 'P': t2_fire = 1;
        break;
    }
}

*/

/*
void Game_Input_UpdateFromBuffer(void)
{
    t1_up    = uart_buffer[0] - '0';
    t1_left  = uart_buffer[1] - '0';
    t1_down  = uart_buffer[2] - '0';
    t1_right = uart_buffer[3] - '0';

    t2_up    = uart_buffer[4] - '0';
    t2_left  = uart_buffer[5] - '0';
    t2_down  = uart_buffer[6] - '0';
    t2_right = uart_buffer[7] - '0';

    t1_fire  = uart_buffer[8] - '0';
    t2_fire  = uart_buffer[9] - '0';
}

*/


volatile PlayerInput player1;
volatile PlayerInput player2;

// Bitovno mapiranje: bit0=up, bit1=down, bit2=left, bit3=right, bit4=fire
void Game_Input_ProcessWord(uint16_t bits)
{
    // Tank 1
    player1.up    = (bits >> 0) & 1;
    player1.left  = (bits >> 1) & 1;
    player1.down  = (bits >> 2) & 1;
    player1.right = (bits >> 3) & 1;
    player1.fire  = (bits >> 4) & 1;

    // Tank 2
    player2.up    = (bits >> 5) & 1;
    player2.left  = (bits >> 6) & 1;
    player2.down  = (bits >> 7) & 1;
    player2.right = (bits >> 8) & 1;
    player2.fire  = (bits >> 9) & 1;
}




