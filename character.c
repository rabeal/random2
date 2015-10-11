/* Characters */
#include "character.h"
#include "collision.h"
#include "enemy.h"
#include "level.h"
#include <stdlib.h>
extern Map maps[MAX_MAPS];
extern int g_currentLevel;
extern int g_enemySpawned;
extern ScreenData  S_Data;


void InitCharacter(character *sprite)
{
	sprite->x = 496;
	sprite->y = 588;
	sprite->csprite = LoadSprite("images/cyan.png",36, 48);
	sprite->flag = 0;
	sprite->frame = 0;
	sprite->collision.h = 48;
	sprite->collision.w = 36;
	sprite->collision.x = 496;
	sprite->collision.y = 588;
}

void InitFighter(fighter *sprite)
{
	sprite->x = 800;
	sprite->y = 350;
	sprite->fsprite = LoadSprite("images/fighter.png",48,60);
	sprite->hp = 105;
	sprite->maxhp = 105;
	sprite->time = 0;
	sprite->exp = 0;
	sprite->attack = 40;
	sprite->defense = 10;
	sprite->nextlvl = 50;
	sprite->lvl = 1;

}
void InitMage(mage *sprite)
{
	sprite->x = 800;
	sprite->y = 475;
	sprite->msprite = LoadSprite("images/mage.png",48,60);
	sprite->hp = 65;
	sprite->maxhp = 65;
	sprite->time = 0;
	sprite->exp = 0;
	sprite->attack = 60;
	sprite->defense = 2;
	sprite->nextlvl = 50;
	sprite->lvl = 1;

}

void DrawPCs(fighter* f1, mage* m1, SDL_Surface* screen)
{
	DrawSprite(f1->fsprite,screen,f1->x,f1->y,0);
	DrawSprite(m1->msprite,screen,m1->x,m1->y,0);

}

void DrawCharacter(character *c1, SDL_Surface* screen, int currentLevel)
{
	if (g_currentLevel == 0)
		DrawSprite(c1->csprite,screen,c1->x,c1->y,c1->frame);
	else if (g_currentLevel == 1)
		DrawSprite(c1->csprite,screen,(c1->x),(c1->y),c1->frame);
	else if (g_currentLevel == 2)
		DrawSprite(c1->csprite,screen,(c1->x),(c1->y),c1->frame);
	c1->collision.x = c1->x;
	c1->collision.y = c1->y;
	
}

void TeleportCharacter(character *ch, int new_level)
{
	g_currentLevel = new_level;
	ch->x = maps[new_level].start[0] * 64;
	ch->y = maps[new_level].start[1] * 48;
	g_enemySpawned = 0;
}

void CharacterMove(character *sprite, Uint8* keys)
{
	int tilex, tiley;
	if(keys[SDLK_a]|| keys[SDLK_LEFT])
	{
		sprite->x = sprite->x - 6;
		if(sprite->x < 0)
			sprite->x += 6;
		tilex = sprite->x/64;
		tiley = sprite->y/48;
		if (maps[g_currentLevel].tiles[tilex][tiley].pass != '1')
		{
			sprite->x=sprite->x + 6;
		}
		else if(maps[g_currentLevel].tiles[tilex][tiley].teleportnum != -1)
			TeleportCharacter(sprite, maps[g_currentLevel].tiles[tilex][tiley].teleportnum);
	}
	else if(keys[SDLK_s]|| keys[SDLK_DOWN])
	{
		sprite->y = sprite->y + 6;
		tilex = sprite->x/64;
		tiley = (sprite->y+48)/48;

		if (maps[g_currentLevel].tiles[tilex][tiley].pass != '1')
		{
			sprite->y -= 6;
		}
		else if(maps[g_currentLevel].tiles[tilex][tiley].teleportnum != -1)
			TeleportCharacter(sprite, maps[g_currentLevel].tiles[tilex][tiley].teleportnum);

		if(sprite->y+48 > S_Data.yres)
			sprite->y -= 6;
	}
	else if(keys[SDLK_d] || keys[SDLK_RIGHT])
	{
		sprite->x = sprite->x + 6;
		if(sprite->x+32 > S_Data.xres)
			sprite->x -= 6;
		tilex = (sprite->x+32)/64;
		tiley = sprite->y/48;
		if (maps[g_currentLevel].tiles[tilex][tiley].pass != '1')
		{
			sprite->x=sprite->x - 6;
		}
		else if(maps[g_currentLevel].tiles[tilex][tiley].teleportnum != -1)
			TeleportCharacter(sprite, maps[g_currentLevel].tiles[tilex][tiley].teleportnum);
	}
	else if(keys[SDLK_w]|| keys[SDLK_UP])
	{
		sprite->y = sprite->y - 6;
		tilex = sprite->x/64;
		tiley = sprite->y/48;
		if(sprite->y < 0)
			sprite->y += 6;
		if (maps[g_currentLevel].tiles[tilex][tiley].pass != '1')
		{
			sprite->y += 6;
		}
		else if(maps[g_currentLevel].tiles[tilex][tiley].teleportnum != -1)
			TeleportCharacter(sprite, maps[g_currentLevel].tiles[tilex][tiley].teleportnum);

	}
	else if (keys[SDLK_l])
	{
		printf("location: %d, %d \n",
			sprite->x/64,sprite->y/48);

	}
}

void FreeCharacter(character* c1)
{
	FreeSprite(c1->csprite);
	memset(c1,0,sizeof(character));
}

void FreeFighter(fighter* f1)
{
	FreeSprite(f1->fsprite);
	memset(f1,0,sizeof(fighter));
}

void FreeMage(mage* m1)
{
	FreeSprite(m1->msprite);
	memset(m1,0,sizeof(mage));
}