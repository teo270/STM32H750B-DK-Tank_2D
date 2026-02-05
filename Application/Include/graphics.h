#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "images.h"
#include <stdint.h>
#include "objects.h"



#define SRC_SIZE 50
#define DST_SIZE 72


void LCD_DrawSprite565_DMA2D( uint32_t Instance, int x, int y, int w, int h, const uint16_t *sprite);
void DrawSprite_DMA2D(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *sprite);

void pad_image_50_to_72(const uint16_t* src, uint16_t* dst);
uint16_t* rotate_image(const uint16_t* src, float angle_deg);
uint16_t* rotate_image_bilinear(const uint16_t* src, float angle_deg);

void GFX_DrawTank(const Tank* t);

void GFX_DrawBullet(const Bullet* b);
void GFX_EraseBullet(const Bullet* b);
void GFX_Erase_Previous_Bullet_Location(const Bullet* b);



					// izvorna slika // trenutni kot na krogu (rad)	// center kroga	   // radius   // korak kota na frame
void move_tank_circle( const uint16_t* tank_sprite, float* theta,  float cx, float cy, float R, float dtheta);


void tank_draw(Tank* t);



























void DrawSpriteRotated1(int cx, int cy, int w, int h, float angle_deg, const uint16_t *img);



#endif // GRAPHICS_H_
