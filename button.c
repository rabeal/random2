#include "button.h"

void InitAttackButton(attackButton *sprite)																	/** Init the attack button */
{

	sprite->cx = 300;
	sprite->cy = 300;
	sprite->sprite_c = LoadSprite("images/attackbutton.png",128,96);
	sprite->frame = 0;
	sprite->collision.h = 96;
	sprite->collision.w = 128;
	sprite->collision.x = 768;					//offsets
	sprite->collision.y = 638;					
}

void InitBlockButton(blockButton *sprite)																	/** Init the block button */
{

	sprite->cx = 300;
	sprite->cy = 300;
	sprite->sprite_c = LoadSprite("images/blockbutton.png",128,96);
	sprite->frame = 0;
	sprite->collision.h = 96;
	sprite->collision.w = 128;
	sprite->collision.x = 568;					//offsets
	sprite->collision.y = 638;					
}

void DrawButton_C(attackButton* ab1, blockButton* bb1, SDL_Surface* screen)					/** Draw the enemies to the combat screen */
{
	if (ab1->sprite_c != NULL)
	{
		DrawSprite(ab1->sprite_c,screen,ab1->collision.x,ab1->collision.y,ab1->frame);
	}
	if (bb1->sprite_c != NULL)
	{
		DrawSprite(bb1->sprite_c,screen,bb1->collision.x,bb1->collision.y,ab1->frame);
	}

}

void FreeButton(attackButton* ab1, blockButton* bb1)										/** Free the buttons from memory */
{
	FreeSprite(ab1->sprite_c);
	FreeSprite(bb1->sprite_c);
	memset(ab1,0,sizeof(attackButton));
	memset(bb1,0,sizeof(blockButton));
}