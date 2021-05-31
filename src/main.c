#include <fileioc.h>
#include <fontlibc.h>
#include "gfx/gfx.h"
#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tice.h>

extern unsigned char tilemapM[];

#define TILE_WIDTH          16
#define TILE_HEIGHT         16

#define TILEMAP_WIDTH       120
#define TILEMAP_HEIGHT      60

#define TILEMAP_DRAW_WIDTH  20
#define TILEMAP_DRAW_HEIGHT 15

#define Y_OFFSET            0
#define X_OFFSET            0

//define color values:
#define BLACK				1
#define GRAY				3
#define GREEN				4
#define RED					5
#define TRANSPARENT	0
#define WHITE				2
#define YELLOW			6

//define current version:
const char version[] = "1.0";

//variables:
const int testModeAvailable = 0;
int testMode;

int selection;
int selecting;

int stage;
int score;
int lifes;

int firstXBlock;
int firstYBlock;
int scrollX;
int scrollY;
int maxScrollX;

int endLeft;
int endRight;
int endDown;

//player vars:
int playerX;
const int leftTweakX = 4;
int playerY;
int playerDirection;
int playerAnimation;
int playerAnimationCount;
int playerGrounded;
int playerJump;

int playerAnimationSpeed;
int playerWalkSpeed;
int playerJumpHeight;
int playerJumpSpeed;
int playerFallSpeed;

int dashBootsActive;

//temp variables:
int cacheX;
int cacheY;

int a;
int i;

//shop variables:
const int shopTime = 600;
const int shopOutTime = 200;

int shop[3];
int shopTimer;
int shopActive;

const int timeStopDuration = 150;
const int invincibilityDuration = 250;

//item variables:
int timeStopActive;
int invincibilityActive;

int sword;
const int basicSwordHitbox = 6;

//other variables:
int universal2Animation;
int universal2AnimationCount;
int universal2AnimationSpeed;

int enemy1Speed;
int enemy2Speed;
int enemy3Speed;

int enemy1SpeedBackup;
int enemy2SpeedBackup;
int enemy3SpeedBackup;

int damageMultiplier;

//enemy data vars:
int enemySpawnTime;
int enemyTimer;
int enemy[30];
int enemyX[30];
int enemyY[30];

int enemyPositioner;

const int enemyBaseScore = 75;
const int enemyXHitbox = 28;
const int enemyYHitbox = 28;
const int enemyYTolerance = 10;

int currentExplosion;
int explosionX[8];
int explosionY[8];
int explosionAnimation[8];

//appvar configuration:
ti_var_t myAppVar;
const char *appvarName = "PXSAV";
char nameBuffer[10];

typedef struct {
    char name[15];
    uint8_t active;
    uint8_t testMode;
    uint32_t stage;
    uint32_t score;
    uint32_t lifes;
    uint32_t firstXBlock;
    uint32_t firstYBlock;
    uint32_t scrollX;
    uint32_t scrollY;
    uint32_t maxScrollX;
    uint32_t endLeft;
    uint32_t endRight;
    uint32_t endDown;
    uint32_t playerX;
    uint32_t playerY;
    uint8_t playerDirection;
    uint8_t playerAnimation;
    uint8_t playerAnimationCount;
    uint8_t playerGrounded;
    uint32_t playerJump;
    uint32_t timeStopActive;
    uint32_t invincibilityActive;
    uint8_t dashBootsActive;
    uint8_t sword;
    uint32_t shopTimer;
    uint8_t shopActive;
    
    uint8_t playerAnimationSpeed;
    uint8_t playerWalkSpeed;
    uint8_t playerJumpHeight;
    uint8_t playerJumpSpeed;
    uint8_t playerFallSpeed;
    
    uint8_t universal2AnimationSpeed;
    uint8_t enemy1Speed;
    uint8_t enemy2Speed;
    uint8_t enemy3Speed;
    uint8_t damageMultiplier;
    
    uint8_t enemySpawnTime;
    int enemy[30];
    int enemyX[30];
    int enemyY[30];
} data_t;

data_t data;

sk_key_t key;

//graphic variables:
gfx_tilemap_t tilemap;
gfx_tilemap_t hudTilemap;

gfx_sprite_t *playerWalking1Flipped;
gfx_sprite_t *playerWalking2Flipped;
gfx_sprite_t *playerWalking3Flipped;
gfx_sprite_t *playerWalking4Flipped;

gfx_sprite_t *sword1Flipped;
gfx_sprite_t *sword2Flipped;

gfx_sprite_t *enemy1_1Flipped;
gfx_sprite_t *enemy1_2Flipped;
gfx_sprite_t *enemy2_1Flipped;
gfx_sprite_t *enemy2_2Flipped;

gfx_sprite_t *enemy3_1Flipped;
gfx_sprite_t *enemy3_2Flipped;
gfx_sprite_t *enemy4_1Flipped;
gfx_sprite_t *enemy4_2Flipped;

gfx_sprite_t *enemy5_1Flipped;
gfx_sprite_t *enemy5_2Flipped;

gfx_sprite_t *enemy6_1Flipped;
gfx_sprite_t *enemy6_2Flipped;

int getXBlock(int xBlock)  //get x-block from tilemap
{
	return TILE_WIDTH * TILEMAP_DRAW_WIDTH * xBlock;
}

int getYBlock(int yBlock)  //get y-block from tilemap
{
	return TILE_HEIGHT * TILEMAP_DRAW_HEIGHT * yBlock;
}

void movePlayer()  //move the player
{
  if(playerDirection == 0)  //right
  {
    if(playerX + playerWalkSpeed > endRight)
    {
      playerX = endRight;
      playerAnimation = 1;
      playerAnimationCount = 1;
    }
    else
    {
      playerX += playerWalkSpeed;
    }
  }
  else  //left
  {
    if(playerX - playerWalkSpeed < endLeft)
    {
      playerX = endLeft;
      playerAnimation = 1;
      playerAnimationCount = 1;
    }
    else
    {
      playerX -= playerWalkSpeed;
    }
  }
  
  if(playerJump > 0)  //handle player jumping
  {
    playerGrounded = 0;
    playerY -= playerJumpSpeed;
    playerJump--;
  }
  else if(playerGrounded == 0)
  {
    playerY += playerFallSpeed;
    if(playerY >= endDown)
    {
      playerY = endDown;
      playerGrounded = 1;
    }
  }
  
  //handle scrolling:
  cacheX = playerX - scrollX;
  
  if(cacheX <= 130 && scrollX > 0)
  {
    scrollX -= playerWalkSpeed;
    if(scrollX < 0)
    {
      scrollX = 0;
    }
  }
  else if(cacheX >= 190 && scrollX < maxScrollX)
  {
    scrollX += playerWalkSpeed;
    if(scrollX > maxScrollX)
    {
      scrollX = maxScrollX;
    }
  }
}

void drawHud()  //draw the HUD
{
  if(shopActive == 0 || universal2Animation == 1)
  {
    gfx_Tilemap(&hudTilemap, getXBlock(1), getYBlock(1));  //get HUD interface from tilemap
  }
  else
  {
    gfx_Tilemap(&hudTilemap, getXBlock(4), getYBlock(1));  //get HUD interface with shop message from tilemap
  }
  
  if(lifes > 100)
  {
    gfx_SetColor(GREEN);
    gfx_FillRectangle(67, 213, (lifes - 100) * 0.9, 11);
    gfx_SetColor(RED);
    gfx_FillRectangle(67, 224, 90, 11);
  }
  else
  {
    gfx_SetColor(RED);
    gfx_FillRectangle(67, 224, lifes * 0.9, 11);
  }
  
  gfx_SetTextFGColor(WHITE);
  gfx_SetTextBGColor(BLACK);
  if(shopActive == 0 || universal2Animation == 1)
  {
    gfx_SetTextScale(2, 2);
    gfx_SetTextXY(180, 220);
    gfx_PrintInt(score, 6);
  }
  
  if(testMode == 1)  //display test mode message if active
  {
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("TEST MODE", 10, 10);
  }
}

void drawPlayer()  //draw the player and handle the animations
{
  cacheX = playerX - scrollX;
  cacheY = playerY - scrollY;
  
  if(playerDirection == 0)  //right
  {
    switch(playerAnimation)
    {
      case 1:
        gfx_TransparentSprite(playerWalking1, cacheX, cacheY);
        break;
      
      case 2:
        gfx_TransparentSprite(playerWalking2, cacheX, cacheY);
        break;
      
      case 3:
        gfx_TransparentSprite(playerWalking3, cacheX, cacheY);
        break;
      
      case 4:
        gfx_TransparentSprite(playerWalking4, cacheX, cacheY);
        break;
    }
    
    switch(sword)
    {
      case 1:
        gfx_TransparentSprite(sword1, cacheX + 10, cacheY + 12);
        break;
      case 2:
        gfx_TransparentSprite(sword2, cacheX + 11, cacheY + 10);
        break;
    }
  }
  else  //left
  {
    switch(playerAnimation)
    {
      case 1:
          gfx_TransparentSprite(playerWalking1Flipped, cacheX, cacheY);
          break;
        
        case 2:
          gfx_TransparentSprite(playerWalking2Flipped, cacheX, cacheY);
          break;
        
        case 3:
          gfx_TransparentSprite(playerWalking3Flipped, cacheX, cacheY);
          break;
        
        case 4:
          gfx_TransparentSprite(playerWalking4Flipped, cacheX, cacheY);
          break;
    }
    
    switch(sword)
    {
      case 1:
        gfx_TransparentSprite(sword1Flipped, cacheX - 16, cacheY + 12);
        break;
      case 2:
        gfx_TransparentSprite(sword2Flipped, cacheX - 27, cacheY + 10);
        break;
    }
  }
  
  //animation management:
  if(playerAnimationCount < playerAnimationSpeed)
  {
    playerAnimationCount += 1;
  }
  else
  {
    playerAnimationCount = 1;
    
    if(playerAnimation < 4)
    {
      playerAnimation++;
    }
    else
    {
      playerAnimation = 1;
    }
  }
}

void countIn()  //3 2 1 GO!
{
  gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock));
  gfx_SetColor(BLACK);
  gfx_FillRectangle(0, 100, 320, 40);
  drawHud();
  gfx_SetTextScale(2, 2);
  gfx_PrintStringXY("3", 156, 112);
  gfx_SwapDraw();
  delay(500);
  gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock));
  gfx_SetColor(BLACK);
  gfx_FillRectangle(0, 100, 320, 40);
  drawHud();
  gfx_PrintStringXY("2", 156, 112);
  gfx_SwapDraw();
  delay(500);
  gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock));
  gfx_SetColor(BLACK);
  gfx_FillRectangle(0, 100, 320, 40);
  drawHud();
  gfx_PrintStringXY("1", 156, 112);
  gfx_SwapDraw();
  delay(500);
  gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock));
  gfx_SetColor(BLACK);
  gfx_FillRectangle(0, 100, 320, 40);
  drawHud();
  gfx_PrintStringXY("GO!", 144, 112);
  gfx_SwapDraw();
  delay(500);
}

void nextStage()  //animation and handling for changing to the next stage
{
  while(scrollY < 240)
  {
    gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock) + scrollY);
    drawHud();
    gfx_SwapDraw();
    scrollY += 8;
  }
  delay(500);
  
  stage++;
  
  lifes += 50;
  if(lifes > 200)
  {
    lifes = 200;
  }
  
  scrollX = 0;
  
  playerX = 160;
  playerY = 153;
  playerDirection = 0;
  playerAnimation = 1;
  playerAnimationCount = 1;
  playerGrounded = 1;
  playerJump = 0;
  
  for(i = 0; i <= 29; i++)
  {
    enemy[i] = 0;
    enemyX[i] = 0;
    enemyY[i] = 0;
  }
  
  for(i = 0; i <= 7; i++)
  {
    explosionAnimation[i] = 0;
  }
  
  if(stage == 5 || stage == 9)
  {
    playerWalkSpeed += 2;
    playerAnimationSpeed--;
    playerJumpHeight--;
    playerJumpSpeed++;
    playerFallSpeed++;
    enemy1Speed++;
    enemy2Speed += 2;
    enemy3Speed += 3;
    universal2AnimationSpeed--;
    enemySpawnTime -= 2;
  }
  
  if(stage > 4 && stage < 9)
  {
    i = stage - 4;
  }
  else if(stage > 8)
  {
    i = stage - 8;
  }
  else
  {
    i = stage;
  }
  
  switch(i)  //set new stage vars
  {
    case 1:
      firstXBlock = 2;
      firstYBlock = 0;
      maxScrollX = 320;
      
      endLeft = 96;
      endRight = 527;
      endDown = 153;
      break;
    case 2:
      firstXBlock = 0;
      firstYBlock = 2;
      maxScrollX = 320;
      
      endLeft = 64;
      endRight = 513;
      endDown = 153;
      break;
    case 3:
      firstXBlock = 2;
      firstYBlock = 2;
      maxScrollX = 320;
      
      endLeft = 48;
      endRight = 527;
      endDown = 153;
      break;
    case 4:
      firstXBlock = 4;
      firstYBlock = 2;
      maxScrollX = 320;
      
      endLeft = 32;
      endRight = 591;
      endDown = 153;
      break;
  }
  
  while(scrollY > 0)
  {
    gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock) + scrollY);
    drawHud();
    gfx_SwapDraw();
    scrollY -= 8;
  }
  
  countIn();
}

void stageChecker()  //check if enough score for next stage
{
  if(stage >= 4 && stage < 8)
  {
    a = score - 40000;
    i = stage - 4;
  }
  else if(stage >= 8)
  {
    a = score - 80000;
    i = stage - 8;
  }
  else
  {
    a = score;
    i = stage;
  }
  
  if(a >= 0 && a < 10000 && i < 1)
  {
    nextStage();
  }
  else if(a >= 10000 && a < 20000 && i < 2)
  {
    nextStage();
  }
  else if(a >= 20000 && a < 30000 && i < 3)
  {
    nextStage();
  }
  else if(a >= 30000 && a < 40000 && i < 4)
  {
    nextStage();
  }
}

void universalAnimationHandler()  //universal animation handler (for 2 frame animations)
{
  if(universal2AnimationCount >= universal2AnimationSpeed)
  {
    universal2AnimationCount = 1;
    
    if(universal2Animation == 2)
    {
      universal2Animation = 1;
    }
    else
    {
      universal2Animation++;
    }
  }
  else
  {
    universal2AnimationCount++;
  }
}

void shopHandler()  //handler for shop to appear
{
  shopTimer--;
  if(shopTimer == 0 && score < 120000)
  {
    shopActive = 1;
  }
  else if(shopTimer <= -shopOutTime)
  {
    shopActive = 0;
    shopTimer = shopTime;
  }
  
  if(shopActive == 1)
  {
    if(scrollX > 160)
    {
      if(universal2Animation == 1)
      {
        gfx_TransparentSprite(shopGuy1, 160 + (320 - scrollX), endDown);
      }
      else
      {
        gfx_TransparentSprite(shopGuy2, 160 + (320 - scrollX), endDown);
      }
    }
  }
}

bool explosionScreenCheck()
{
  if(explosionX[i] - scrollX > 33 && explosionX[i] - scrollX < 287)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void explosionHandler()
{
  for(i = 0; i <= 7; i++)
  {
    if(explosionAnimation[i] != 0 && explosionScreenCheck())
    {
      switch(explosionAnimation[i])
      {
        case 1:
          gfx_TransparentSprite(explosion1, explosionX[i] - scrollX, explosionY[i]);
          break;
        case 2:
          gfx_TransparentSprite(explosion2, explosionX[i] - scrollX, explosionY[i]);
          break;
        case 3:
          gfx_TransparentSprite(explosion3, explosionX[i] - scrollX, explosionY[i]);
          break;
        case 4:
          gfx_TransparentSprite(explosion4, explosionX[i] - scrollX, explosionY[i]);
          break;
        case 5:
          gfx_TransparentSprite(explosion3, explosionX[i] - scrollX, explosionY[i]);
          break;
        case 6:
          gfx_TransparentSprite(explosion2, explosionX[i] - scrollX, explosionY[i]);
          break;
        case 7:
          gfx_TransparentSprite(explosion1, explosionX[i] - scrollX, explosionY[i]);
          explosionAnimation[i] = -1;
          break;
      }
      explosionAnimation[i]++;
    }
  }
}

bool enemyScreenCheck()
{
  if(enemyX[i] - scrollX > 33 && enemyX[i] - scrollX < 287)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void enemyHandler()  //handle enemies (movement, drawing, hitboxes, ...)
{
  enemyPositioner += enemyTimer * damageMultiplier;
  if(enemyPositioner < endLeft + 4 || enemyPositioner >= endRight - 34)
  {
    enemyPositioner = endLeft + 4 + enemyTimer * damageMultiplier;
  }
  
  enemyTimer--;
  if(enemyTimer <= 0)  //spawn enemy
  {
    enemyTimer = enemySpawnTime;
    
    if(stage > 4 && stage < 9)
    {
      a = stage - 4;
    }
    else if(stage > 8)
    {
      a = stage - 8;
    }
    else
    {
      a = stage;
    }
    
    for(i = 0; i <= 29; i++)  //check for free slot
    {
      if(enemy[i] == 0)
      {
        enemyX[i] = enemyPositioner;
        
        if(a == 1)  //enemy selection depends on current stage
        {
          a = random();  //selection is random out of 2
          if(a < 0)
          {
            enemy[i] = 1;
            enemyY[i] = endDown + 24;
          }
          else
          {
            enemy[i] = 2;
            enemyY[i] = endDown + 20;
          }
        }
        else if(a == 2)
        {
          a = random();  //selection is random out of 2
          if(a < 0)
          {
            enemy[i] = 3;
            enemyY[i] = endDown + 22;
          }
          else
          {
            enemy[i] = 4;
            enemyY[i] = 42;
          }
        }
        else if(a == 3)
        {
          a = random();
          if(a < 0)
          {
            a = random();
            if(a < 0)
            {
              enemy[i] = 1;
              enemyY[i] = endDown + 24;
            }
            else
            {
              enemy[i] = 4;
              enemyY[i] = 84;
            }
          }
          else
          {
            enemy[i] = 5;
            enemyY[i] = 92;
          }
        }
        else if(a == 4)
        {
          a = random();
          if(a < 0)
          {
            a = random();
            if(a < 0)
            {
              enemy[i] = 2;
              enemyY[i] = endDown + 20;
            }
            else
            {
              enemy[i] = 3;
              enemyY[i] = endDown + 22;
            }
          }
          else
          {
            enemy[i] = 6;
            enemyY[i] = 84;
          }
        }
        
        break;
      }
    }
  }
  
  //time stop handling
  if(timeStopActive > 0)
  {
    enemy1SpeedBackup = enemy1Speed;
    enemy1Speed = 0;
    enemy2SpeedBackup = enemy2Speed;
    enemy2Speed = 0;
    enemy3SpeedBackup = enemy3Speed;
    enemy3Speed = 0;
  }
  
  for(i = 0; i <= 29; i++)  //check for active enemies
  {
    if(enemy[i] != 0)  //handle enemy
    {
      if(enemy[i] == 1)  //enemy 1 handling
      {
        if(enemyY[i] > endDown - 8)
        {
          enemyY[i]--;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy1_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        else
        {
          if(playerX < enemyX[i])
          {
            enemyX[i] -= enemy1Speed;
            if(enemyScreenCheck())
            {
              switch(universal2Animation)
              {
                case 1:
                  gfx_TransparentSprite(enemy1_1, enemyX[i] - scrollX, enemyY[i]);
                  break;
                case 2:
                  gfx_TransparentSprite(enemy1_2, enemyX[i] - scrollX, enemyY[i]);
                  break;
              }
            }
          }
          else
          {
            enemyX[i] += enemy1Speed;
            if(enemyScreenCheck())
            {
              switch(universal2Animation)
              {
                case 1:
                  gfx_TransparentSprite(enemy1_1Flipped, enemyX[i] - scrollX, enemyY[i]);
                  break;
                case 2:
                  gfx_TransparentSprite(enemy1_2Flipped, enemyX[i] - scrollX, enemyY[i]);
                  break;
              }
            }
          }
        }
      }
      else if(enemy[i] == 2)  //enemy 2 handling
      {
        if(enemyY[i] > endDown - 8)
        {
          enemyY[i]--;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy2_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        else
        {
          if(playerX < enemyX[i])
          {
            enemyX[i] -= enemy2Speed;
            if(enemyScreenCheck())
            {
              switch(universal2Animation)
              {
                case 1:
                  gfx_TransparentSprite(enemy2_1, enemyX[i] - scrollX, enemyY[i]);
                  break;
                case 2:
                  gfx_TransparentSprite(enemy2_2, enemyX[i] - scrollX, enemyY[i]);
                  break;
              }
            }
          }
          else
          {
            enemyX[i] += enemy2Speed;
            if(enemyScreenCheck())
            {
              switch(universal2Animation)
              {
                case 1:
                  gfx_TransparentSprite(enemy2_1Flipped, enemyX[i] - scrollX, enemyY[i]);
                  break;
                case 2:
                  gfx_TransparentSprite(enemy2_2Flipped, enemyX[i] - scrollX, enemyY[i]);
                  break;
              }
            }
          }
        }
      }
      else if(enemy[i] == 3)  //enemy 3 handling
      {
        if(enemyY[i] > endDown - 6)
        {
          enemyY[i] -= 3;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy3_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        else
        {
          a = random();  //selection is random out of 2
          if(a < 0)  //Randomly halt
          {
            //nothing here
          }
          else if(playerX < enemyX[i])
          {
            enemyX[i] -= enemy3Speed;
          }
          else
          {
            enemyX[i] += enemy3Speed;
          }
          
          if(playerX < enemyX[i] && enemyScreenCheck())
          {
            switch(universal2Animation)
            {
              case 1:
                gfx_TransparentSprite(enemy3_1, enemyX[i] - scrollX, enemyY[i]);
                break;
              case 2:
                gfx_TransparentSprite(enemy3_2, enemyX[i] - scrollX, enemyY[i]);
                break;
            }
          }
          else if(enemyScreenCheck())
          {
            switch(universal2Animation)
            {
              case 1:
                gfx_TransparentSprite(enemy3_1Flipped, enemyX[i] - scrollX, enemyY[i]);
                break;
              case 2:
                gfx_TransparentSprite(enemy3_2Flipped, enemyX[i] - scrollX, enemyY[i]);
                break;
            }
          }
        }
      }
      else if(enemy[i] == 4)  //enemy 4 handling
      {
        if(enemyY[i] < 128)
        {
          enemyY[i] += enemy2Speed;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy4_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        else if(enemyY[i] > 148)
        {
          enemyY[i] -= enemy2Speed;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy4_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        else
        {
          a = random();  //selection is random out of 2
          if(a < 0)  //randomly fly up or down
          {
            enemyY[i] += enemy1Speed;
          }
          else
          {
            enemyY[i] -= enemy1Speed;
          }
          
          if(playerX < enemyX[i])
          {
            enemyX[i] -= enemy1Speed;
            if(enemyScreenCheck())
            {
              switch(universal2Animation)
              {
                case 1:
                  gfx_TransparentSprite(enemy4_1, enemyX[i] - scrollX, enemyY[i]);
                  break;
                case 2:
                  gfx_TransparentSprite(enemy4_2, enemyX[i] - scrollX, enemyY[i]);
                  break;
              }
            }
          }
          else
          {
            enemyX[i] += enemy1Speed;
            if(enemyScreenCheck())
            {
              switch(universal2Animation)
              {
                case 1:
                  gfx_TransparentSprite(enemy4_1Flipped, enemyX[i] - scrollX, enemyY[i]);
                  break;
                case 2:
                  gfx_TransparentSprite(enemy4_2Flipped, enemyX[i] - scrollX, enemyY[i]);
                  break;
              }
            }
          }
        }
      }
      else if(enemy[i] == 5)  //enemy 5 handling
      {
        if(enemyY[i] < 92)
        {
          enemyY[i] += enemy3Speed;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy5_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        if(enemyY[i] > endDown - 6)
        {
          enemyY[i] -= enemy3Speed;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy5_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        else
        {
          if(enemyTimer < 5)
          {
            enemyY[i] -= enemy3Speed;
          }
          else
          {
            enemyY[i] += enemy2Speed;
          }
          
          if(playerX < enemyX[i] && enemyScreenCheck())
          {
            enemyX[i] -= enemy1Speed;
            switch(universal2Animation)
            {
              case 1:
                gfx_TransparentSprite(enemy5_1, enemyX[i] - scrollX, enemyY[i]);
                break;
              case 2:
                gfx_TransparentSprite(enemy5_2, enemyX[i] - scrollX, enemyY[i]);
                break;
            }
          }
          else if(enemyScreenCheck())
          {
            enemyX[i] += enemy1Speed;
            switch(universal2Animation)
            {
              case 1:
                gfx_TransparentSprite(enemy5_1Flipped, enemyX[i] - scrollX, enemyY[i]);
                break;
              case 2:
                gfx_TransparentSprite(enemy5_2Flipped, enemyX[i] - scrollX, enemyY[i]);
                break;
            }
          }
        }
      }
      else if(enemy[i] == 6)  //enemy 6 handling
      {
        if(enemyY[i] < 84)
        {
          enemyY[i] += enemy2Speed;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy6_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        if(enemyY[i] > endDown - 8)
        {
          enemyY[i] -= enemy2Speed;
          if(enemyScreenCheck())
          {
            gfx_TransparentSprite(enemy6_1, enemyX[i] - scrollX, enemyY[i]);
          }
        }
        else
        {
          a = random();
          if(enemyTimer < 5)
          {
            enemyY[i] -= enemy3Speed;
          }
          else
          {
            enemyY[i] += enemy2Speed;
          }
          
          if(playerX < enemyX[i] && enemyScreenCheck())
          {
            enemyX[i] -= enemy2Speed - 1;
            switch(universal2Animation)
            {
              case 1:
                gfx_TransparentSprite(enemy6_1, enemyX[i] - scrollX, enemyY[i]);
                break;
              case 2:
                gfx_TransparentSprite(enemy6_2, enemyX[i] - scrollX, enemyY[i]);
                break;
            }
          }
          else if(enemyScreenCheck())
          {
            enemyX[i] += enemy2Speed - 1;
            switch(universal2Animation)
            {
              case 1:
                gfx_TransparentSprite(enemy6_1Flipped, enemyX[i] - scrollX, enemyY[i]);
                break;
              case 2:
                gfx_TransparentSprite(enemy6_2Flipped, enemyX[i] - scrollX, enemyY[i]);
                break;
            }
          }
        }
      }
      
      //left teweak
      if(playerDirection == 1)
      {
        a = playerX - leftTweakX;
      }
      else
      {
        a = playerX;
      }
      
      //enemy collision detection:
      if(a >= enemyX[i] - enemyXHitbox && a <= enemyX[i] + enemyXHitbox && playerY >= enemyY[i] - enemyYHitbox && playerY <= enemyY[i] + enemyYHitbox)
      {
        //enemy or player damage?
        if(playerDirection == 0)  //right
        {
          if(a + (basicSwordHitbox / sword) < enemyX[i] && playerY - enemyYTolerance < enemyY[i] && playerY + enemyYTolerance > enemyY[i])  //hit with sword
          {
            score += enemyBaseScore * enemy[i] / damageMultiplier;
            enemy[i] = 0;
            explosionX[currentExplosion] = enemyX[i] + 2;
            explosionY[currentExplosion] = enemyY[i] + 2;
            explosionAnimation[currentExplosion] = 1;
            currentExplosion++;
            if(currentExplosion > 7)
            {
              currentExplosion = 0;
            }
          }
          else if(invincibilityActive == 0)  //player gets hit
          {
            lifes -= damageMultiplier;
          }
        }
        else  //left
        {
          if(a - (basicSwordHitbox / sword) > enemyX[i] && playerY - enemyYTolerance < enemyY[i] && playerY + enemyYTolerance > enemyY[i])  //hit with sword
          {
            score += enemyBaseScore * enemy[i];
            enemy[i] = 0;
            explosionX[currentExplosion] = enemyX[i] + 2;
            explosionY[currentExplosion] = enemyY[i] + 2;
            explosionAnimation[currentExplosion] = 1;
            currentExplosion++;
            if(currentExplosion > 7)
            {
              currentExplosion = 0;
            }
          }
          else if(invincibilityActive == 0)  //player gets hit
          {
            lifes -= damageMultiplier;
          }
        }
      }
    }
  }
  
  //time stop reversing
  if(timeStopActive > 0)
  {
    enemy1Speed = enemy1SpeedBackup;
    enemy2Speed = enemy2SpeedBackup;
    enemy3Speed = enemy3SpeedBackup;
  }
}

int main(void)  //main function
{
	//initialization
  playerWalking1Flipped = gfx_MallocSprite(17, 23);
	gfx_FlipSpriteY(playerWalking1, playerWalking1Flipped);
	playerWalking2Flipped = gfx_MallocSprite(17, 23);
	gfx_FlipSpriteY(playerWalking2, playerWalking2Flipped);
	playerWalking3Flipped = gfx_MallocSprite(17, 23);
	gfx_FlipSpriteY(playerWalking3, playerWalking3Flipped);
	playerWalking4Flipped = gfx_MallocSprite(17, 23);
	gfx_FlipSpriteY(playerWalking4, playerWalking4Flipped);
  
  sword1Flipped = gfx_MallocSprite(25, 7);
	gfx_FlipSpriteY(sword1, sword1Flipped);
  sword2Flipped = gfx_MallocSprite(34, 10);
	gfx_FlipSpriteY(sword2, sword2Flipped);
  
  enemy1_1Flipped = gfx_MallocSprite(31, 32);
	gfx_FlipSpriteY(enemy1_1, enemy1_1Flipped);
  enemy1_2Flipped = gfx_MallocSprite(31, 32);
	gfx_FlipSpriteY(enemy1_2, enemy1_2Flipped);
  enemy2_1Flipped = gfx_MallocSprite(27, 32);
	gfx_FlipSpriteY(enemy2_1, enemy2_1Flipped);
  enemy2_2Flipped = gfx_MallocSprite(27, 32);
	gfx_FlipSpriteY(enemy2_2, enemy2_2Flipped);
  
  enemy3_1Flipped = gfx_MallocSprite(30, 31);
	gfx_FlipSpriteY(enemy3_1, enemy3_1Flipped);
  enemy3_2Flipped = gfx_MallocSprite(30, 31);
	gfx_FlipSpriteY(enemy3_2, enemy3_2Flipped);
  enemy4_1Flipped = gfx_MallocSprite(30, 31);
	gfx_FlipSpriteY(enemy4_1, enemy4_1Flipped);
  enemy4_2Flipped = gfx_MallocSprite(30, 31);
	gfx_FlipSpriteY(enemy4_2, enemy4_2Flipped);
  
  enemy5_1Flipped = gfx_MallocSprite(26, 29);
	gfx_FlipSpriteY(enemy5_1, enemy5_1Flipped);
  enemy5_2Flipped = gfx_MallocSprite(26, 29);
	gfx_FlipSpriteY(enemy5_2, enemy5_2Flipped);
  
  enemy6_1Flipped = gfx_MallocSprite(32, 32);
	gfx_FlipSpriteY(enemy6_1, enemy6_1Flipped);
  enemy6_2Flipped = gfx_MallocSprite(32, 32);
	gfx_FlipSpriteY(enemy6_2, enemy6_2Flipped);
  
  //initialize tilemaps
	tilemap.map         = tilemapM;
  tilemap.tiles       = tileset_tiles;
  tilemap.type_width  = gfx_tile_16_pixel;
  tilemap.type_height = gfx_tile_16_pixel;
  tilemap.tile_height = TILE_HEIGHT;
  tilemap.tile_width  = TILE_WIDTH;
  tilemap.draw_height = TILEMAP_DRAW_HEIGHT;
  tilemap.draw_width  = TILEMAP_DRAW_WIDTH;
  tilemap.height      = TILEMAP_HEIGHT;
  tilemap.width       = TILEMAP_WIDTH;
  tilemap.y_loc       = Y_OFFSET;
  tilemap.x_loc       = X_OFFSET;
  
  hudTilemap.map         = tilemapM;
  hudTilemap.tiles       = tileset_tiles;
  hudTilemap.type_width  = gfx_tile_16_pixel;
  hudTilemap.type_height = gfx_tile_16_pixel;
  hudTilemap.tile_height = TILE_HEIGHT;
  hudTilemap.tile_width  = TILE_WIDTH;
  hudTilemap.draw_height = 2;
  hudTilemap.draw_width  = 20;
  hudTilemap.height      = TILEMAP_HEIGHT;
  hudTilemap.width       = TILEMAP_WIDTH;
  hudTilemap.y_loc       = 208;
  hudTilemap.x_loc       = X_OFFSET;
	
	gfx_Begin();
	
	gfx_SetPalette(global_palette, sizeof_global_palette, 0);
	gfx_SetTransparentColor(TRANSPARENT);
  gfx_SetColor(BLACK);
	
	gfx_SetDrawBuffer();
	
	gfx_SetMonospaceFont(8);
	gfx_SetTextFGColor(WHITE);
  gfx_SetTextBGColor(BLACK);
	
	gfx_Tilemap(&tilemap, getXBlock(0), getYBlock(0));
  gfx_TransparentSprite(techcrafter, 100, 60);
	gfx_PrintStringXY(version, 296, 232);
	gfx_SwapDraw();
	
	srand(rtc_Time());
	
	kb_Scan();
  key = kb_Data[7];
	if(key & kb_Up && key & kb_Down && key & kb_Left && key & kb_Right && testModeAvailable == 1)
	{
		testMode = 1;
	}
	
	delay(500);
  
  ti_CloseAll();
  myAppVar = ti_Open(appvarName, "r");
  if(!myAppVar)
  {
    delay(1500);
    goto license;
  }
  ti_Read(&data, sizeof(data_t), 1, myAppVar);
  if(data.active == 1)
  {
    goto load;
  }
  
  delay(1500);
  goto mainMenu;
  
  //---------------------------------------------------------------
	
	//license
	license:
  
  gfx_Tilemap(&tilemap, getXBlock(0), getYBlock(0));
  gfx_SetTextFGColor(WHITE);
  gfx_SetTextBGColor(BLACK);
  gfx_SetTextScale(2, 2);
  gfx_PrintStringXY("License terms", 0, 0);
  gfx_SetTextScale(1, 1);
  gfx_PrintStringXY("PROTO X - A fun arena fighting game.", 0, 24);
  gfx_PrintStringXY("Copyright (C) 2021  Techcrafter", 0, 32);
  gfx_PrintStringXY("This program is free software: you can", 0, 40);
  gfx_PrintStringXY("redistribute it and/or modify it under", 0, 48);
  gfx_PrintStringXY("the terms of the GNU General Public", 0, 56);
  gfx_PrintStringXY("License as published by the Free", 0, 64);
  gfx_PrintStringXY("Software Foundation, either version 3", 0, 72);
  gfx_PrintStringXY("the License, or (at your option) any", 0, 80);
  gfx_PrintStringXY("later version.", 0, 88);
  gfx_PrintStringXY("This program is distributed in the", 0, 96);
  gfx_PrintStringXY("hope that it will be useful, but", 0, 104);
  gfx_PrintStringXY("WITHOUT ANY WARRANTY; without even", 0, 112);
  gfx_PrintStringXY("the implied warranty of", 0, 120);
  gfx_PrintStringXY("MERCHANTABILITY or FITNESS FOR A", 0, 128);
  gfx_PrintStringXY("PARTICULAR PURPOSE.  See the GNU", 0, 136);
  gfx_PrintStringXY("General Public License for more", 0, 144);
  gfx_PrintStringXY("details.", 0, 152);
  gfx_PrintStringXY("You should have received a copy of the", 0, 160);
  gfx_PrintStringXY("GNU General Public License along with", 0, 168);
  gfx_PrintStringXY("this program.  If not, see", 0, 176);
  gfx_PrintStringXY("<https://www.gnu.org/licenses/>.", 0, 184);
  gfx_PrintStringXY("Press [2nd] to accept these terms.", 0, 205);
  gfx_PrintStringXY("Press [clear] to decline and quit.", 0, 215);
  gfx_SwapDraw();
  
  while(1)
  {
    key = os_GetCSC();
    if(key == sk_2nd)
    {
      myAppVar = ti_Open(appvarName, "w");
      strcpy(data.name, "PXSave");
      ti_Write(&data, sizeof(data_t), 1, myAppVar);
      goto mainMenu;
    }
    else if(key == sk_Clear)
    {
      goto quit;
    }
  }
  
  goto error;
  
	//---------------------------------------------------------------
	
	//mainMenu
	mainMenu:
  
  selection = 0;
  selecting = 1;
  
  while(selecting)
  {
    gfx_Tilemap(&tilemap, getXBlock(1), getYBlock(0));
    gfx_TransparentSprite(logo, 100, 56);
    
    if(testMode == 1)
		{
			gfx_SetTextFGColor(WHITE);
			gfx_SetTextBGColor(BLACK);
			gfx_SetTextScale(2, 2);
			gfx_PrintStringXY("TEST MODE", 10, 10);
		}
    
    key = os_GetCSC();
		if(key == sk_2nd)
		{
			selecting = 0;
		}
		else if(key == sk_Up && selection != 0)
		{
			selection--;
		}
		else if(key == sk_Down && selection != 1)
		{
			selection++;
		}
		
		if(selection == 0)
		{
			gfx_TransparentSprite(playButton1, 128, 138);
		}
		else
		{
			gfx_TransparentSprite(playButton0, 128, 138);
		}
		
		if(selection == 1)
		{
			gfx_TransparentSprite(quitButton1, 128, 180);
		}
		else
		{
			gfx_TransparentSprite(quitButton0, 128, 180);
		}
		
		gfx_SwapDraw();
  }
  
  switch(selection)
  {
    case 0:
      goto prepare;
      break;
    case 1:
      goto quit;
      break;
  }
  
  goto error;
  
  //---------------------------------------------------------------
	
	//prepare
	prepare:
  
  stage = 1;
  score = 0;
  lifes = 200;
  
  firstXBlock = 2;
  firstYBlock = 0;
  scrollX = 0;
  scrollY = 0;
  maxScrollX = 320;
  
  endLeft = 96;
  endRight = 527;
  endDown = 153;
  
  playerX = 160;
  playerY = 153;
  playerDirection = 0;
  playerAnimation = 1;
  playerAnimationCount = 1;
  playerGrounded = 1;
  playerJump = 0;
  
  dashBootsActive = 0;
  
  sword = 1;
  
  universal2Animation = 1;
  universal2AnimationCount = 1;
  
  playerAnimationSpeed = 3;
  playerWalkSpeed = 5;
  playerJumpHeight = 7;
  playerJumpSpeed = 8;
  playerFallSpeed = 7;
  
  universal2AnimationSpeed = 5;
  enemy1Speed = 2;
  enemy2Speed = 4;
  enemy3Speed = 6;
  damageMultiplier = 1;
  
  shopTimer = shopTime;
  shopActive = 0;
  
  invincibilityActive = 0;
  timeStopActive = 0;
  
  enemySpawnTime = 15;
  for(i = 0; i <= 29; i++)
  {
    enemy[i] = 0;
    enemyX[i] = 0;
    enemyY[i] = 0;
  }
  enemyPositioner = 128;
  
  currentExplosion = 0;
  for(i = 0; i <= 7; i++)
  {
    explosionX[i] = 0;
    explosionY[i] = 0;
    explosionAnimation[i] = 0;
  }
  
  goto play;
  
  //---------------------------------------------------------------
	
	//load
	load:
  
  gfx_Tilemap(&tilemap, getXBlock(1), getYBlock(0));
  gfx_TransparentSprite(logo, 100, 96);
	gfx_SwapDraw();
  
  testMode = data.testMode;
  stage = data.stage;
  score = data.score;
  lifes = data.lifes;
  firstXBlock = data.firstXBlock;
  firstYBlock = data.firstYBlock;
  scrollX = data.scrollX;
  scrollY = data.scrollY;
  maxScrollX = data.maxScrollX;
  endLeft = data.endLeft;
  endRight = data.endRight;
  endDown = data.endDown;
  playerX = data.playerX;
  playerY = data.playerY;
  playerDirection = data.playerDirection;
  playerAnimation = data.playerAnimation;
  playerAnimationCount = data.playerAnimationCount;
  playerGrounded = data.playerGrounded;
  playerJump = data.playerJump;
  timeStopActive = data.timeStopActive;
  invincibilityActive = data.invincibilityActive;
  dashBootsActive = data.dashBootsActive;
  sword = data.sword;
  shopTimer = data.shopTimer;
  shopActive = data.shopActive;
  
  playerAnimationSpeed = data.playerAnimationSpeed;
  playerWalkSpeed = data.playerWalkSpeed;
  playerJumpHeight = data.playerJumpHeight;
  playerJumpSpeed = data.playerJumpSpeed;
  playerFallSpeed = data.playerFallSpeed;
  universal2AnimationSpeed = data.universal2AnimationSpeed;
  enemy1Speed = data.enemy1Speed;
  enemy2Speed = data.enemy2Speed;
  enemy3Speed = data.enemy3Speed;
  damageMultiplier = data.damageMultiplier;
  
  enemySpawnTime = data.enemySpawnTime;
  for(i = 0; i <= 29; i++)
  {
    enemy[i] = data.enemy[i];
    enemyX[i] = data.enemyX[i];
    enemyY[i] = data.enemyY[i];
  }
  enemyPositioner = 128;
  
  delay(500);
  
  goto play;
  
  //---------------------------------------------------------------
	
	//play
	play:
  
  countIn();
  
  while(lifes > 0)
  {
    gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock));
    
    key = kb_Data[7];
		if(key & kb_Left)
		{
			playerDirection = 1;
		}
		else if(key & kb_Right)
		{
			playerDirection = 0;
		}
    
    if(kb_Data[1] == kb_2nd && playerGrounded == 1)
		{
      playerJump = playerJumpHeight;
    }
    
    if(kb_Data[1] != kb_2nd)
		{
			playerJump = 0;
		}
    
    if(kb_Data[2] == kb_Alpha && dashBootsActive == 1)
    {
      gfx_TransparentSprite(dashBoots, 143, 16);
      if(playerDirection == 0)
      {
        playerX += playerWalkSpeed / 2;
      }
      else
      {
        playerX -= playerWalkSpeed / 2;
      }
    }
    
    key = os_GetCSC();
		if(key == sk_Clear)
		{
			goto pause;
		}
    else if(key == sk_Mode && testMode == 1)
		{
			goto shop;
		}
    else if(key == sk_Del && testMode == 1)
		{
      score += 1000;
			lifes += 10;
      if(lifes > 200)
      {
        lifes = 200;
      }
		}
    
    universalAnimationHandler();
    
    shopHandler();
    if(shopActive == 1 && playerGrounded == 1 && scrollX > 160 && (playerX - scrollX) >= 150 + (320 - scrollX) && (playerX - scrollX) <= 170 + (320 - scrollX))
    {
      goto shop;
    }
    
    if(invincibilityActive > 0)
    {
      gfx_TransparentSprite(invincibilityPotion, 16, 16);
      invincibilityActive--;
    }
    if(timeStopActive > 0)
    {
      gfx_TransparentSprite(timeStop, 270, 16);
      timeStopActive--;
    }
    
    movePlayer();
    
    explosionHandler();
    enemyHandler();
    
    drawHud();
    drawPlayer();
    
    gfx_SwapDraw();
    
    stageChecker();
  }
  
  goto gameOver;
  
  //---------------------------------------------------------------
	
	//shop
	shop:
  
  shopTimer = shopTime;
  shopActive = 0;
  
  a = random();
  
  selection = 0;
  selecting = 1;
  
  while(selecting)
  {
    gfx_Tilemap(&tilemap, getXBlock(0), getYBlock(1));
    drawHud();
    
    gfx_SetTextScale(1, 1);
    
    gfx_SetTextFGColor(YELLOW);
    gfx_PrintStringXY("You can only buy one of those!", 44, 42);
    gfx_SetTextFGColor(WHITE);
    
    if(a < -5000000)
    {
      shop[0] = 1;
      gfx_TransparentSprite(watermellon, 56, 58);
      gfx_PrintStringXY("Watermellon (25hp)", 98, 62);
      gfx_PrintStringXY("2000 points", 98, 70);
      shop[1] = 4;
      gfx_TransparentSprite(bigHealPotion, 56, 90);
      gfx_PrintStringXY("Big heal potion (150hp)", 98, 94);
      gfx_PrintStringXY("9250 points", 98, 102);
      shop[2] = 7;
      gfx_TransparentSprite(dashBoots, 56, 122);
      gfx_PrintStringXY("Dash boots ([alpha])", 98, 126);
      gfx_PrintStringXY("20000 points", 98, 134);
    }
    else if(a < 0)
    {
      shop[0] = 2;
      gfx_TransparentSprite(meat, 56, 50);
      gfx_PrintStringXY("Meat (50hp)", 98, 62);
      gfx_PrintStringXY("4500 points", 98, 70);
      shop[1] = 4;
      gfx_TransparentSprite(bigHealPotion, 56, 88);
      gfx_PrintStringXY("Big heal potion (150hp)", 98, 94);
      gfx_PrintStringXY("9250 points", 98, 102);
      shop[2] = 6;
      gfx_TransparentSprite(invincibilityPotion, 56, 122);
      gfx_PrintStringXY("Invincibility potion", 98, 126);
      gfx_PrintStringXY("4250 points", 98, 134);
    }
    else if(a < 5000000)
    {
      shop[0] = 3;
      gfx_TransparentSprite(smallHealPotion, 56, 58);
      gfx_PrintStringXY("Small heal potion (75hp)", 98, 62);
      gfx_PrintStringXY("5000 points", 98, 70);
      shop[1] = 5;
      gfx_TransparentSprite(timeStop, 56, 90);
      gfx_PrintStringXY("Time stop", 98, 94);
      gfx_PrintStringXY("3000 points", 98, 102);
      shop[2] = 2;
      gfx_TransparentSprite(meat, 56, 114);
      gfx_PrintStringXY("Meat (50hp)", 98, 126);
      gfx_PrintStringXY("4500 points", 98, 134);
    }
    else
    {
      shop[0] = 1;
      gfx_TransparentSprite(watermellon, 56, 58);
      gfx_PrintStringXY("Watermellon (25hp)", 98, 62);
      gfx_PrintStringXY("2000 points", 98, 70);
      shop[1] = 3;
      gfx_TransparentSprite(smallHealPotion, 56, 90);
      gfx_PrintStringXY("Small heal potion (75hp)", 98, 94);
      gfx_PrintStringXY("5000 points", 98, 102);
      shop[2] = 8;
      gfx_TransparentSprite(swordUpgrade, 56, 122);
      gfx_PrintStringXY("Sword upgrade", 98, 126);
      gfx_PrintStringXY("16500 points", 98, 134);
    }
    
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("Exit", 98, 156);
    
    key = os_GetCSC();
		if(key == sk_2nd && selection == 3)
		{
			selecting = 0;
		}
		else if(key == sk_Up && selection != 0)
		{
			selection--;
		}
		else if(key == sk_Down && selection != 3)
		{
			selection++;
		}
    else if(key == sk_2nd)
    {
      switch(selection)
      {
        case 0:
          i = shop[0];
          break;
        case 1:
          i = shop[1];
          break;
        case 2:
          i = shop[2];
          break;
      }
      
      /*
      Shop list:
      1 - Watermellon 25hp (2000 points)
      2 - Meat 50hp (4500 points)
      3 - Small heal potion 75hp (5000 points)
      4 - Big heal potion 150 hp (9250 points)
      5 - Time stop (3000 points)
      6 - Invincibility potion (4250 points)
      7 - Dash boots (20000 points)
      8 - Sword upgrade (16500 points)
      */
      
      if(i == 0)
      {
        //nothing here
      }
      else if(i == 1 && score >= 2000 && lifes < 200)
      {
        score -= 2000;
        lifes += 25;
        if(lifes > 200)
        {
          lifes = 200;
        }
        selecting = 0;
      }
      else if(i == 2 && score >= 4500 && lifes < 200)
      {
        score -= 4500;
        lifes += 50;
        if(lifes > 200)
        {
          lifes = 200;
        }
        selecting = 0;
      }
      else if(i == 3 && score >= 5000 && lifes < 200)
      {
        score -= 5000;
        lifes += 75;
        if(lifes > 200)
        {
          lifes = 200;
        }
        selecting = 0;
      }
      else if(i == 4 && score >= 9250 && lifes < 200)
      {
        score -= 9250;
        lifes += 150;
        if(lifes > 200)
        {
          lifes = 200;
        }
        selecting = 0;
      }
      else if(i == 5 && score >= 3000)
      {
        score -= 3000;
        timeStopActive = timeStopDuration;
        selecting = 0;
      }
      else if(i == 6 && score >= 4250)
      {
        score -= 4250;
        invincibilityActive = invincibilityDuration;
        selecting = 0;
      }
      else if(i == 7 && score >= 20000 && dashBootsActive == 0)
      {
        score -= 20000;
        dashBootsActive = 1;
        selecting = 0;
      }
      else if(i == 8 && score >= 16500 && sword == 1)
      {
        score -= 16500;
        sword = 2;
        selecting = 0;
      }
    }
    
    switch(selection)
    {
      case 0:
        gfx_TransparentSprite(shopSelector, 42, 64);
        break;
      case 1:
        gfx_TransparentSprite(shopSelector, 42, 96);
        break;
      case 2:
        gfx_TransparentSprite(shopSelector, 42, 128);
        break;
      case 3:
        gfx_TransparentSprite(shopSelector, 42, 160);
        break;
    }
    
    gfx_SwapDraw();
  }
  
  goto play;
  
  //---------------------------------------------------------------
	
	//pause
	pause:
  
  gfx_Tilemap(&tilemap, getXBlock(firstXBlock) + scrollX, getYBlock(firstYBlock));
  drawHud();
	gfx_SetColor(GRAY);
	gfx_FillRectangle(0, 60, 320, 125);
	gfx_SetTextFGColor(WHITE);
	gfx_SetTextBGColor(GRAY);
	gfx_SetTextScale(2, 2);
	gfx_PrintStringXY("Pause menu", 80, 80);
	gfx_SetTextScale(1, 1);
	gfx_PrintStringXY("Press [clear] to continue...", 55, 115);
  gfx_PrintStringXY("Press [2nd] to save and quit...", 48, 135);
	gfx_PrintStringXY("Press [del] to give up...", 62, 155);
	gfx_SwapDraw();
	
	while(1)
	{
		key = os_GetCSC();
		if(key == sk_Clear)
		{
			goto play;
		}
    else if(key == sk_2nd)
    {
      goto save;
    }
		else if(key == sk_Del)
		{
			goto gameOver;
		}
	}
  
  goto play;
  
  //---------------------------------------------------------------
	
	//save
	save:
  
  gfx_Tilemap(&tilemap, getXBlock(0), getYBlock(0));
  gfx_SwapDraw();
  
  myAppVar = ti_Open(appvarName, "w");
  strcpy(data.name, "PXSave");
  
  data.active = 1;
  data.testMode = testMode;
  data.stage = stage;
  data.score = score;
  data.lifes = lifes;
  data.firstXBlock = firstXBlock;
  data.firstYBlock = firstYBlock;
  data.scrollX = scrollX;
  data.scrollY = scrollY;
  data.maxScrollX = maxScrollX;
  data.endLeft = endLeft;
  data.endRight = endRight;
  data.endDown = endDown;
  data.playerX = playerX;
  data.playerY = playerY;
  data.playerDirection = playerDirection;
  data.playerAnimation = playerAnimation;
  data.playerAnimationCount = playerAnimationCount;
  data.playerGrounded = playerGrounded;
  data.playerJump = playerJump;
  data.timeStopActive = timeStopActive;
  data.invincibilityActive = invincibilityActive;
  data.dashBootsActive = dashBootsActive;
  data.sword = sword;
  data.shopTimer = shopTimer;
  data.shopActive = shopActive;
  
  data.playerAnimationSpeed = playerAnimationSpeed;
  data.playerWalkSpeed = playerWalkSpeed;
  data.playerJumpHeight = playerJumpHeight;
  data.playerJumpSpeed = playerJumpSpeed;
  data.playerFallSpeed = playerFallSpeed;
  data.universal2AnimationSpeed = universal2AnimationSpeed;
  data.enemy1Speed = enemy1Speed;
  data.enemy2Speed = enemy2Speed;
  data.enemy3Speed = enemy3Speed;
  data.damageMultiplier = damageMultiplier;
  
  data.enemySpawnTime = enemySpawnTime;
  
  for(i = 0; i <= 29; i++)
  {
    data.enemy[i] = enemy[i];
    data.enemyX[i] = enemyX[i];
    data.enemyY[i] = enemyY[i];
  }
  
  ti_Write(&data, sizeof(data_t), 1, myAppVar);
  
  goto quit;
  
  //---------------------------------------------------------------
	
	//gameOver
	gameOver:
  
  myAppVar = ti_Open(appvarName, "w");
  strcpy(data.name, "PXSave");
  data.active = 0;
  ti_Write(&data, sizeof(data_t), 1, myAppVar);
  
  gfx_Tilemap(&tilemap, getXBlock(4), getYBlock(0));
  
  gfx_TransparentSprite(playerWalking1, 104, 132);
  
  gfx_SetTextScale(2, 2);
  gfx_SetTextFGColor(WHITE);
  gfx_SetTextBGColor(BLACK);
  gfx_SetTextXY(130, 144);
  gfx_PrintInt(score, 6);
  
  gfx_SetTextScale(1, 1);
  gfx_PrintStringXY("Press [2nd] to go to the main menu...", 16, 220);
  
  if(testMode == 1)
  {
    gfx_SetTextFGColor(WHITE);
    gfx_SetTextBGColor(GRAY);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("TEST MODE", 10, 10);
  }
  
  gfx_SwapDraw();
  
  while(os_GetCSC() != sk_2nd);
  
  goto mainMenu;
	
	//---------------------------------------------------------------
	
	//error
	error:
  
  myAppVar = ti_Open(appvarName, "w");
  strcpy(data.name, "PXSave");
  data.active = 0;
  ti_Write(&data, sizeof(data_t), 1, myAppVar);
	
	gfx_FillScreen(RED);
	gfx_SetTextFGColor(WHITE);
	gfx_SetTextBGColor(RED);
	gfx_SetTextScale(1, 1);
	gfx_PrintStringXY("An error has occurred!", 0, 0);
	gfx_PrintStringXY("Press [2nd] to go to the main menu...", 0, 10);
	gfx_SwapDraw();
	while(os_GetCSC() != sk_2nd);
	goto mainMenu;
	
	//---------------------------------------------------------------
	
	//quit
	quit:
	
	gfx_End();
	return 0;
}