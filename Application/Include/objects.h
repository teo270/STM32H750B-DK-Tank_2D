// objects.h
#ifndef OBJECTS_H
#define OBJECTS_H

#include <stdint.h>
#include "game_input.h"

typedef struct {
    float x, y;        // CENTER tanka
    float prev_x, prev_y; // prejšnja pozicija
    float heading;     // smer (rad)
    float speed;       // hitrost (px/frame)
    const uint16_t* sprite;
    int shot_down;          // Ali je tank uničem, če je sestreljen == 1;
    int ovira			// se lahko doda tanku, ki bo predstavljal oviro za igralca

} Tank;

#define MAX_BULLETS 10
#define BULLET_SIZE 10
#define BULLET_HALF (BULLET_SIZE/2)

typedef struct
{
    float x, y;
    float prev_x, prev_y;
    float heading;
    float speed;
    const uint16_t* sprite;
    int owner_id;     // ID tanka, ki je izstrelil
    int active;       // 0 = neaktiven, 1 = leti

} Bullet;






// Init
void OBJ_Tank_Init( Tank* t, float x, float y, float heading, float speed, const uint16_t* sprite);
void OBJ_Bullet_Init(Bullet* b, float x, float y, float heading, float speed, const uint16_t* sprite, int owner_id);


// Gibanje
void OBJ_Tank_MoveForward(Tank* t);
void OBJ_Tank_MoveBackward(Tank* t);
void OBJ_Tank_UpdateDirection(Tank* t, int tank_id, Tank* tanks, int num_tanks);
void OBJ_Tank_UpdateDirection_Single(Tank* t, PlayerInput* p, Tank* tanks, int num_tanks);
void OBJ_Bullet_Update(Bullet* b, Tank* tanks, int num_tanks);


// Omejitve
int OBJ_Tanks_CheckCollision(Tank* t1, Tank* t2);
int OBJ_Tank_Screen_Crash(Tank* t);


void FireBullet(Tank* t, int owner_id, Bullet* bullets);



void Game_Input_EndFrame(void);
int Check_Wining_Condition(const Tank* tanks);

#endif
