
//graphic.c

#include "graphics.h"
#include "objects.h"

#include "stm32h750b_discovery_lcd.h"
#include "stm32h750b_discovery_ts.h"
#include "stm32h750b_discovery_bus.h"
#include "stm32h750b_discovery_sdram.h"

#include "images.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32_lcd.h"


extern DMA2D_HandleTypeDef hlcd_dma2d;
extern LTDC_HandleTypeDef  hlcd_ltdc;
extern BSP_LCD_Ctx_t       Lcd_Ctx[LCD_INSTANCES_NBR];

#include "core_cm7.h"   // za SCB_CleanDCache...



#define SCREEN_W 480
#define SCREEN_H 272
#define TANK_SIZE 50
#define TANK_HALF (TANK_SIZE / 2)

// Globalni bufferji
static uint16_t padded_72[DST_SIZE * DST_SIZE];
static uint16_t rotated_72[DST_SIZE * DST_SIZE];



void LCD_DrawSprite565_DMA2D(
    uint32_t Instance,
    int x,
    int y,
    int w,
    int h,
    const uint16_t *sprite
)
{
    uint32_t fb_base;
    uint32_t dst;
    uint32_t size_bytes;

    fb_base = hlcd_ltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress;
    dst = fb_base + ((y * Lcd_Ctx[Instance].XSize + x) * 2);

    size_bytes = w * h * 2;

    /* --- CACHE --- */
    SCB_CleanDCache_by_Addr((uint32_t*)sprite, size_bytes);

    /* --- DMA2D reset --- */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();

    hlcd_dma2d.Instance = DMA2D;
    hlcd_dma2d.Init.Mode = DMA2D_M2M;
    hlcd_dma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
    hlcd_dma2d.Init.OutputOffset = Lcd_Ctx[Instance].XSize - w;

    HAL_DMA2D_Init(&hlcd_dma2d);

    /* Source */
    DMA2D->FGPFCCR = DMA2D_INPUT_RGB565;
    DMA2D->FGMAR   = (uint32_t)sprite;

    /* Destination */
    DMA2D->OMAR = dst;

    /* Size */
    DMA2D->NLR = ((uint32_t)w << 16) | (uint32_t)h;

    /* Start */
    HAL_DMA2D_Start(&hlcd_dma2d,
                    (uint32_t)sprite,
                    dst,
                    w,
                    h);

    HAL_DMA2D_PollForTransfer(&hlcd_dma2d, HAL_MAX_DELAY);

    /* Invalidate framebuffer cache */
    SCB_InvalidateDCache_by_Addr((uint32_t*)dst, size_bytes);
}

void DrawSprite_DMA2D(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *sprite)
{
    uint32_t dest_addr;

    /* Ciljni naslov v framebufferju (RGB565 = 2 bajta) */
    dest_addr =
        hlcd_ltdc.LayerCfg[0].FBStartAdress +
        2 * (y * Lcd_Ctx[0].XSize + x);			// dolžina zaslona:  Lcd_Ctx[0].XSize  ničla predstavlja instanco 0

    /* Ustavi DMA2D, če je slučajno aktiven */
    hlcd_dma2d.Instance->CR = 0;

    /* Mode: Memory to Memory */
    hlcd_dma2d.Instance->CR = DMA2D_M2M;

    /* Barvni formati */
    hlcd_dma2d.Instance->FGPFCCR = DMA2D_INPUT_RGB565;
    hlcd_dma2d.Instance->OPFCCR  = DMA2D_OUTPUT_RGB565;

    /* Line offset:
       framebuffer je širši od sprite-a */
    hlcd_dma2d.Instance->OOR =
    		Lcd_Ctx[0].XSize - w;

    /* Naslovi */
    hlcd_dma2d.Instance->FGMAR = (uint32_t)sprite;
    hlcd_dma2d.Instance->OMAR  = dest_addr;

    /* Velikost: */
    hlcd_dma2d.Instance->NLR =
        (w << 16) | h;

    /* Start */
    hlcd_dma2d.Instance->CR |= DMA2D_CR_START;

    /* Počakaj, da zaključi */
    while (hlcd_dma2d.Instance->CR & DMA2D_CR_START);
}











void pad_image_50_to_72(const uint16_t* src, uint16_t* dst)
{
    // 1️ Počistimo celoten output (črni pixli)
    for (int i = 0; i < DST_SIZE * DST_SIZE; i++) {
        dst[i] = 0x0000;
    }

    // 2️ Padding offset
    const int pad = (DST_SIZE - SRC_SIZE) / 2; // (72-50)/2 = 11

    // 3️ Kopiramo 50×50 v sredino 72×72
    for (int y = 0; y < SRC_SIZE; y++) {
        int dst_y = y + pad;
        int dst_row = dst_y * DST_SIZE;
        int src_row = y * SRC_SIZE;

        for (int x = 0; x < SRC_SIZE; x++) {
            dst[dst_row + (x + pad)] = src[src_row + x];
        }
    }
}




uint16_t* rotate_image(const uint16_t* src, float angle_deg)
{
    // Padding: 50×50 → 72×72
    pad_image_50_to_72(src, padded_72);

    //  Počistimo output buffer
    for (int i = 0; i < DST_SIZE * DST_SIZE; i++) {
        rotated_72[i] = 0x0000;
    }

    //  Priprava rotacije
    float rad = angle_deg * (float)M_PI / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    float cx = (DST_SIZE - 1) * 0.5f;
    float cy = (DST_SIZE - 1) * 0.5f;

    //  Inverse mapping
    for (int y = 0; y < DST_SIZE; y++) {
        for (int x = 0; x < DST_SIZE; x++) {

            float dx = x - cx;
            float dy = y - cy;

            float sx =  cos_a * dx + sin_a * dy + cx;
            float sy = -sin_a * dx + cos_a * dy + cy;

            int ix = (int)(sx + 0.5f);
            int iy = (int)(sy + 0.5f);

            if (ix >= 0 && ix < DST_SIZE &&
                iy >= 0 && iy < DST_SIZE) {
                rotated_72[y * DST_SIZE + x] =
                    padded_72[iy * DST_SIZE + ix];
            }
        }
    }

    //  Vrnemo stabilen buffer
    return rotated_72;
}





uint16_t* rotate_image_bilinear(const uint16_t* src, float angle_deg)
{
    // 1️ Padding 50x50 -> 72x72
    pad_image_50_to_72(src, padded_72);

    // 2️ Počistimo output
    for (int i = 0; i < DST_SIZE * DST_SIZE; i++)
        rotated_72[i] = 0x0000;

    // 3️ Priprava rotacije
    float rad = angle_deg * (float)M_PI / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    float cx = (DST_SIZE - 1) * 0.5f;
    float cy = (DST_SIZE - 1) * 0.5f;

    // 4️ Loop po ciljnem pixlu
    for (int y = 0; y < DST_SIZE; y++) {
        for (int x = 0; x < DST_SIZE; x++) {

            // Koordinate v source (inverse mapping)
            float dx = x - cx;
            float dy = y - cy;
            float sx =  cos_a * dx + sin_a * dy + cx;
            float sy = -sin_a * dx + cos_a * dy + cy;

            // Check boundaries
            if (sx < 0 || sx >= DST_SIZE - 1 || sy < 0 || sy >= DST_SIZE - 1)
                continue; // ostane črno

            //  Bilinear interpolation
            int ix = (int)sx;
            int iy = (int)sy;
            float fx = sx - ix;
            float fy = sy - iy;

            // Četrtine
            uint16_t Q11 = padded_72[iy * DST_SIZE + ix];
            uint16_t Q21 = padded_72[iy * DST_SIZE + ix + 1];
            uint16_t Q12 = padded_72[(iy+1) * DST_SIZE + ix];
            uint16_t Q22 = padded_72[(iy+1) * DST_SIZE + ix + 1];

            // Ločimo R,G,B
            uint8_t r11 = (Q11 >> 11) & 0x1F;
            uint8_t g11 = (Q11 >> 5) & 0x3F;
            uint8_t b11 = Q11 & 0x1F;

            uint8_t r21 = (Q21 >> 11) & 0x1F;
            uint8_t g21 = (Q21 >> 5) & 0x3F;
            uint8_t b21 = Q21 & 0x1F;

            uint8_t r12 = (Q12 >> 11) & 0x1F;
            uint8_t g12 = (Q12 >> 5) & 0x3F;
            uint8_t b12 = Q12 & 0x1F;

            uint8_t r22 = (Q22 >> 11) & 0x1F;
            uint8_t g22 = (Q22 >> 5) & 0x3F;
            uint8_t b22 = Q22 & 0x1F;

            // Interpolacija po x
            float r_top = r11 * (1 - fx) + r21 * fx;
            float r_bot = r12 * (1 - fx) + r22 * fx;
            float g_top = g11 * (1 - fx) + g21 * fx;
            float g_bot = g12 * (1 - fx) + g22 * fx;
            float b_top = b11 * (1 - fx) + b21 * fx;
            float b_bot = b12 * (1 - fx) + b22 * fx;

            // Interpolacija po y
            uint8_t r = (uint8_t)(r_top * (1 - fy) + r_bot * fy);
            uint8_t g = (uint8_t)(g_top * (1 - fy) + g_bot * fy);
            uint8_t b = (uint8_t)(b_top * (1 - fy) + b_bot * fy);

            // Nazaj v RGB565
            rotated_72[y * DST_SIZE + x] = (r << 11) | (g << 5) | b;
        }
    }

    return rotated_72;
}



void GFX_DrawTank(const Tank* t)
{

	if (t->shot_down && !(t->ovira)) return;  // tank je sestreljen, ne riši ga bil je igralčev

    int draw_x = (int)(t->x - TANK_HALF);
    int draw_y = (int)(t->y - TANK_HALF);

    uint16_t* rotated = rotate_image_bilinear(
        t->sprite,
        t->heading * 180.0f / M_PI
    );

    LCD_DrawSprite565_DMA2D(
        0,
        draw_x,
        draw_y,
        DST_SIZE,
        DST_SIZE,
        rotated
    );
}


void GFX_DrawBullet(const Bullet* b)
{
    if (!b->active) return;

    int draw_x = (int)(b->x - BULLET_HALF);
    int draw_y = (int)(b->y - BULLET_HALF);

    LCD_DrawSprite565_DMA2D(
        0,
        draw_x,
        draw_y,
        BULLET_SIZE,
        BULLET_SIZE,
        b->sprite
    );
}



void GFX_EraseBullet(const Bullet* b)
{
    if (!b->active)
        return;

    UTIL_LCD_FillRect(
        (int)(b->x - BULLET_HALF),
        (int)(b->y - BULLET_HALF),
        BULLET_SIZE,
        BULLET_SIZE,
        UTIL_LCD_COLOR_BLACK
    );
}

void GFX_Erase_Previous_Bullet_Location(const Bullet* b)
{
	if(b->active){
		UTIL_LCD_FillRect(
				b->prev_x - BULLET_HALF,
				b->prev_y - BULLET_HALF,
				BULLET_SIZE,
				BULLET_SIZE,
				UTIL_LCD_COLOR_BLACK
		);
	}
}





















































void move_tank_circle(
	const uint16_t* tank_sprite, // izvorna slika
	float* theta,                // trenutni kot na krogu (rad)
    float cx, float cy,          // center kroga
    float R,                     // radius
    float dtheta                 // korak kota na frame
)
{

    *theta += dtheta;
    if (*theta >= 2.0f * M_PI) *theta -= 2.0f * M_PI;

    // 2️ Izračun lokacije centra tank-a
    float tank_x = cx + R * cosf(*theta);
    float tank_y = cy + R * sinf(*theta);

    // 3️ Izračun kota rotacije tank-a
    float angle_deg = (*theta) * 180.0f / M_PI + 90.0f; // +90° če sprite gleda "gor"

    // 4️ Priprava rotiranega sprite-a
    uint16_t* rotated = rotate_image_bilinear(tank_sprite, angle_deg);

    // 5️ Nariši sprite
    // x,y koordinata vrha sprite (zgoraj levo)
    int draw_x = (int)(tank_x - DST_SIZE/2);
    int draw_y = (int)(tank_y - DST_SIZE/2);
    LCD_DrawSprite565_DMA2D(0, draw_x, draw_y, DST_SIZE, DST_SIZE, rotated);
}


#define MAX_SPRITE_W 50
#define MAX_SPRITE_H 50

// Buffer mora biti dovolj velik za diagonalo sprite-a
#define ROTATED_BUFFER_SIZE 128


__attribute__((section(".sram1"))) static uint16_t rotated_buffer[ROTATED_BUFFER_SIZE  * ROTATED_BUFFER_SIZE];

/* funkcija za rotacijo in DMA2D izris */
/**
 * @brief Draw a rotated sprite with DMA2D
 *
 * @param cx       Center X na zaslonu
 * @param cy       Center Y na zaslonu
 * @param w        Width originalnega sprite-a
 * @param h        Height originalnega sprite-a
 * @param angle_deg Kot rotacije v stopinjah
 * @param img      Pointer na originalni sprite (RGB565)
 */
void DrawSpriteRotated1(int cx, int cy, int w, int h, float angle_deg, const uint16_t *img)
{
    float rad = angle_deg * (M_PI / 180.0f);
    float ca = cosf(rad);
    float sa = sinf(rad);

    int hw = w / 2;
    int hh = h / 2;

    int buff_size = (int)ceilf(sqrtf(w*w + h*h));
    if (buff_size > ROTATED_BUFFER_SIZE) buff_size = ROTATED_BUFFER_SIZE;
    int buff_hw = buff_size / 2;

    // Počisti buffer (npr. črno ozadje)
    for (int i = 0; i < buff_size * buff_size; i++)
        rotated_buffer[i] = 0x0000;

    for (int y = -hh; y < hh; y++)
    {
        for (int x = -hw; x < hw; x++)
        {
            int src_x = x + hw;
            int src_y = y + hh;
            if (src_x < 0 || src_x >= w || src_y < 0 || src_y >= h)
                continue;

            uint16_t px = img[src_y * w + src_x];

            // Rotacija
            int rx = (int)roundf(x * ca - y * sa);
            int ry = (int)roundf(x * sa + y * ca);

            int dst_x = rx + buff_hw;
            int dst_y = ry + buff_hw;
            if (dst_x >= 0 && dst_x < buff_size && dst_y >= 0 && dst_y < buff_size)
                rotated_buffer[dst_y * buff_size + dst_x] = px;
        }
    }

    //  DEBUG IZRIS – brez DMA2D
        int top_left_x = cx - buff_hw;
        int top_left_y = cy - buff_hw;

        for(int y=0; y<buff_size; y++)
        {
            for(int x=0; x<buff_size; x++)
            {
                uint16_t px = rotated_buffer[y * buff_size + x];
                if(px != 0x0000)
                {
                    BSP_LCD_WritePixel(0, top_left_x + x, top_left_y + y, px);
                }
            }
        }
}




