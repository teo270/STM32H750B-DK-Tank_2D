
//object.c
#include "objects.h"
#include "graphics.h"
#include <math.h>

#include "game_input.h"
#include "Game.h"

#define SCREEN_W 480
#define SCREEN_H 272
#define TANK_REAL_SIZE 50
#define TANK_REAL_HALF (TANK_REAL_SIZE/2)


#define TANK_DRAW_SIZE 72

#define TANK_DRAW_HALF (TANK_DRAW_SIZE / 2)
#define TURN_SPEED (1.8f * M_PI / 180.0f) // 1.8° na frame





void OBJ_Tank_Init( Tank* t, float x, float y, float heading, float speed, const uint16_t* sprite)
{
    t->x = x;
    t->y = y;
    t->heading = heading;
    t->speed = speed;
    t->sprite = sprite;
    t->shot_down = 0;
    t->ovira = 0;
}


void OBJ_Bullet_Init( Bullet* b, float x, float y, float heading, float speed, const uint16_t* sprite, int owner_id)
{
    b->x = x;
    b->y = y;
    b->prev_x = x;
    b->prev_y = y;

    b->heading = heading;
    b->speed = speed;
    b->sprite = sprite;

    b->owner_id = owner_id;
    b->active = 1;
}





void OBJ_Tank_MoveForward(Tank* t)
{
	t->x -= sinf(t->heading) * t->speed;
	t->y += cosf(t->heading) * t->speed;

}

void OBJ_Tank_MoveBackward(Tank* t)
{
    t->x += sinf(t->heading) * t->speed;   // opazuj: brez minusa
    t->y -= cosf(t->heading) * t->speed;
}






int OBJ_Tanks_CheckCollision(Tank* t1, Tank* t2)
{
    if (t1 == t2) return 0; // tank sam s sabo


    if (fabsf(t1->x - t2->x) < TANK_REAL_SIZE &&
        fabsf(t1->y - t2->y) < TANK_REAL_SIZE &&
		!(t2-> shot_down)						)   //Preveri, ali je tank 2 že uničen
    {
        return 1; // trk
    }
    return 0;
}


int OBJ_Tank_Screen_Crash(Tank* t)
{
	int exit = 0;  // spremenljivka za dvig zastavice, če je zaznan dotik
	// ZGORNJI ROB
	    if ((t->y) < TANK_REAL_HALF){
	    	//t->y = TANK_REAL_HALF;
	    	exit = 1;

	    }

    // SPODNJI ROB
	    if ((t->y) > SCREEN_H - TANK_DRAW_HALF){
	    	//t->y = SCREEN_H - TANK_DRAW_HALF;
	    	exit =1;
	    }

   // LEVI ROB
	    if (t->x < TANK_REAL_HALF){
	    	//t->x = TANK_REAL_HALF;
	    	exit = 1;
	    }

  // DESNI ROB
	    if (t->x > SCREEN_W - TANK_REAL_HALF){
	    	//t->x = SCREEN_W - TANK_REAL_HALF;
	    	exit = 1;
	    }


	 return exit;
}

int OBJ_Bullet_HitTank(Bullet* b, Tank* t, int tank_id)
{
	if (!b->active) return 0;
    if (b->owner_id == tank_id) return 0; // ne zadane svojega

    if (fabsf(b->x - t->x) < TANK_REAL_HALF &&
        fabsf(b->y - t->y) < TANK_REAL_HALF)
    {
        b->active = 0;      // izstrelek izgine
        t->shot_down = 1;   //tank je zadet / uničen
        return 1;
    }
    return 0;
}


void FireBullet(Tank* t, int owner_id, Bullet* bullets)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            OBJ_Bullet_Init(
                &bullets[i],
                t->x,
                t->y,
                t->heading,
                MISSLE_SPEED,
                Square,
                owner_id
            );
            break;
        }
    }
}






void OBJ_Tank_UpdateDirection(Tank* t, int tank_id, Tank* tanks, int num_tanks)
{

	int crash_screen_detected;

	// Shrani prejšnjo pozicijo
	    t->prev_x = t->x;
	    t->prev_y = t->y;


	/*
    if (tank_id == 1)
    {
        if (t1_left)  t->heading -= TURN_SPEED;
        if (t1_right) t->heading += TURN_SPEED;

        if (t1_up)   OBJ_Tank_MoveForward(t);
        if (t1_down) OBJ_Tank_MoveBackward(t);
    }
    else if (tank_id == 2)
    {
        if (t2_left)  t->heading -= TURN_SPEED;
        if (t2_right) t->heading += TURN_SPEED;

        if (t2_up)   OBJ_Tank_MoveForward(t);
        if (t2_down) OBJ_Tank_MoveBackward(t);
    }

    */

	    PlayerInput* p = (tank_id == 1) ? &player1 : &player2;


	    if (p->up)    OBJ_Tank_MoveForward(t);
	    if (p->down)  OBJ_Tank_MoveBackward(t);
	    if (p->left)  t->heading -= TURN_SPEED;
	    if (p->right) t->heading += TURN_SPEED;




    /*
    else if (tank_id == 3)
        {
         if (t3_left)  t->heading -= TURN_SPEED;
         if (t3_right) t->heading += TURN_SPEED;

         if (t3_up)   OBJ_Tank_MoveForward(t);
         if (t3_down) OBJ_Tank_MoveBackward(t);
        }

    */



    // Normalizacija kota
    if (t->heading > 2*M_PI) t->heading -= 2*M_PI;
    if (t->heading < 0)      t->heading += 2*M_PI;

    // Crash zgornji/spodnji rob
    crash_screen_detected = OBJ_Tank_Screen_Crash(t);

     if(crash_screen_detected != 1){

    	 // Trk s drugimi tanki
        for(int i = 0; i < num_tanks; i++)
        {
            Tank* other = &tanks[i];
            if (t == other) continue;

            if (OBJ_Tanks_CheckCollision(t, other))
            {
                // Vrnemo na prejšnjo pozicijo
                t->x = t->prev_x;
                t->y = t->prev_y;
            }
        }
     }
     else if(crash_screen_detected == 1){
    	 // Vrnemo na prejšnjo pozicijo
    	 	 t->x = t->prev_x;
    	     t->y = t->prev_y;
     }




}




void OBJ_Bullet_Update(Bullet* b, Tank* tanks, int num_tanks)
{
    if (!b->active) return;

    b->prev_x = b->x;
    b->prev_y = b->y;

    b->x -= sinf(b->heading) * b->speed;
    b->y += cosf(b->heading) * b->speed;

    // Zunaj zaslona → deaktiviraj
    if (b->x < 0 || b->x > SCREEN_W ||
        b->y < 0 || b->y > SCREEN_H)
    {
        b->active = 0;

    }


    // 2️ Preveri trk z vsemi tanki
        for (int i = 0; i < num_tanks; i++)
        {
            Tank* t = &tanks[i];

            // Ne zadane svojega tanka
            if (b->owner_id == i + 1)
                continue;

            // Če je tank že uničen in ni ovira
            if (t->shot_down && !t->ovira)  //
                continue;

            float dx = b->x - t->x;
            float dy = b->y - t->y;


            // Trk: bullet znotraj tanka
            if (fabsf(dx) < TANK_REAL_HALF &&
                fabsf(dy) < TANK_REAL_HALF)
            {
            	b->active = 0;   // metek se ugasne

            	if (!t->ovira)
                {
                    //  zadetek igralca
            		t->shot_down = 1;
                }
            // zadetek ovire → brez efekta

            return;
            }

        }
}

/*
void Game_Input_EndFrame(void)
{
    t1_up = t1_down = t1_left = t1_right = 0;
    t2_up = t2_down = t2_left = t2_right = 0;
    t3_up = t3_down = t3_left = t3_right = 0;
}
*/
int Check_Wining_Condition(const Tank* tanks)
{
    if (tanks[0].shot_down || tanks[1].shot_down)
    {
        return 1;   // konec igre
    }

    return 0;       // igra teče naprej
}


void OBJ_Tank_UpdateDirection_Single(Tank* t, PlayerInput* p, Tank* tanks, int num_tanks)
{
    int crash_screen_detected;

    // Shrani prejšnjo pozicijo
    t->prev_x = t->x;
    t->prev_y = t->y;

    // 1️ Posodobi heading pred premikom
        if (p->left)  t->heading -= TURN_SPEED;
        if (p->right) t->heading += TURN_SPEED;

        // Normalizacija kota
        if (t->heading > 2*M_PI) t->heading -= 2*M_PI;
        if (t->heading < 0)      t->heading += 2*M_PI;

        // 2️ Premik po **že posodobljenem heading-u**
        if (p->up)    OBJ_Tank_MoveForward(t);
        if (p->down)  OBJ_Tank_MoveBackward(t);




    // 3️Crash zgornji/spodnji rob
    crash_screen_detected = OBJ_Tank_Screen_Crash(t);

    if(crash_screen_detected != 1)
    {
        // 4️⃣ Trk s drugimi tanki
        for(int i = 0; i < num_tanks; i++)
        {
            Tank* other = &tanks[i];
            if (t == other) continue;

            if (OBJ_Tanks_CheckCollision(t, other))
            {
                // Vrnemo na prejšnjo pozicijo
                t->x = t->prev_x;
                t->y = t->prev_y;
            }
        }
    }
    else
    {
        // Vrnemo na prejšnjo pozicijo ob crashu ob zaslon
        t->x = t->prev_x;
        t->y = t->prev_y;
    }
}


