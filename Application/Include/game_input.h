#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

//extern volatile uint8_t t1_up, t1_down, t1_left, t1_right;
//extern volatile uint8_t t2_up, t2_down, t2_left, t2_right;
//extern volatile uint8_t t3_up, t3_down, t3_left, t3_right;



//iztrelek
//extern volatile uint8_t t1_fire;
//extern volatile uint8_t t2_fire;
// globalni prejemni bajt
//extern uint8_t uart3_rx_char;





typedef struct {
    uint8_t up:1;
    uint8_t down:1;
    uint8_t left:1;
    uint8_t right:1;
    uint8_t fire:1;
} PlayerInput;

extern volatile PlayerInput player1;
extern volatile PlayerInput player2;

// Funkcija za posodabljanje stanja igralcev iz 16-bitnega paketa (10 bitov uporabljenih)
void Game_Input_ProcessWord(uint16_t bits);

void Game_Input_ProcessChar(uint8_t c);

#endif
