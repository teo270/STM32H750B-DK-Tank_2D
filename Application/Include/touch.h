#ifndef TOUCH_H
#define TOUCH_H


#include <stdint.h>

void Touch_Init(void);
void Touch_Process(uint16_t x_size, uint16_t y_size);
void Display_String(void);

/* samo deklaracija */
extern uint32_t touch_x_size; //Ta spremenljivka obstaja nekje drugje — ne tukaj.
extern uint32_t touch_y_size;	//pove samo compilerju:  “Verjemi mi, linker jo bo našel drugje.”



#endif
