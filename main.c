#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "SDL2/SDL.h"

#define WIN_W 1024
#define WIN_H 1024
#define BLOCK_SIZE 64

#define SNAKE_SIZE BLOCK_SIZE 
#define SNAKE_SPEED BLOCK_SIZE  

#define GAME_RECT_PADDING BLOCK_SIZE 
#define GAME_RECT_SIZE (((WIN_W + WIN_H) / 2) - (GAME_RECT_PADDING * 2))
#define GAME_RECT_COLS GAME_RECT_SIZE / BLOCK_SIZE
#define GAME_RECT_ROWS GAME_RECT_COLS 

struct Snake {
  char* name;
  SDL_Rect rect;
  struct Snake *prev;
  struct Snake *next;
};

struct Apple {
  SDL_Rect rect; 
};

struct Snake snake = { "Head", {(WIN_W / 2) - SNAKE_SIZE, (WIN_H / 2) - SNAKE_SIZE, SNAKE_SIZE, SNAKE_SIZE}, NULL, NULL};
struct Snake snake_tail = { "Tail", {(WIN_W / 2) - SNAKE_SIZE, (WIN_H / 2) - (SNAKE_SIZE * 2), SNAKE_SIZE, SNAKE_SIZE}, NULL, NULL};
struct Apple apple = {0, 0, SNAKE_SIZE, SNAKE_SIZE};

int dx = 0, dy = 0, speed = 0;
size_t quit = 0;
bool start; 

void debug_pos() {
  printf("head_x: %d, head_y: %d, apple_x: %d, apple_y: %d\n", snake.rect.x, snake.rect.y, apple.rect.x, apple.rect.y);
}

void apple_draw(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
  SDL_RenderFillRect(renderer, &apple.rect);
}

void apple_move() {
  // CHECK IF THE POSITION IS OCCUPED BY THE SNAKE
  apple.rect.x = SNAKE_SIZE * ((rand() % GAME_RECT_ROWS)+1);
  apple.rect.y = SNAKE_SIZE * ((rand() % GAME_RECT_COLS)+1);
}

// Returns the last block of the snake struct
struct Snake* snake_get_tail() {
  struct Snake *curr = &snake;
  while(curr->next != NULL) {
    curr = curr->next;
  }
  return curr;
}

void snake_draw(SDL_Renderer *renderer) {
  struct Snake *curr = &snake;
  SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);

  while(curr->next != NULL) {
    SDL_RenderFillRect(renderer, &curr->rect);
//printf("%s has been drawed!\n", curr->name);
    curr = curr->next;
  }
  SDL_RenderFillRect(renderer, &curr->rect);
  //printf("%s has been drawed!\n", curr->name);
}

void snake_add_block() {
  struct Snake* snake_tail = snake_get_tail();

  struct Snake* new_tail = (struct Snake*)malloc(sizeof(struct Snake));
  if(new_tail == NULL) {
    fprintf(stderr, "Cannot allocate new_tail");
    exit(1);
  }

  snake_tail->name = "Block";

  new_tail->name = "Tail";

  if(dx == 0) {
    new_tail->rect.x = snake_tail->rect.x;
    new_tail->rect.y = snake_tail->rect.y + SNAKE_SIZE;
  }
  if(dy == 0) {
    new_tail->rect.x = snake_tail->rect.x + SNAKE_SIZE;
    new_tail->rect.y = snake_tail->rect.y;
  }

  new_tail->rect.h = SNAKE_SIZE;
  new_tail->rect.w = SNAKE_SIZE;
  new_tail->prev = snake_tail;
  new_tail->next = NULL;

  snake_tail->next = new_tail;
}

void snake_check_coll() {
  // snake and apple collision
  bool coll_x = snake.rect.x == apple.rect.x;
  bool coll_y = snake.rect.y == apple.rect.y;

  if(coll_x && coll_y) {
    snake_add_block();
    apple_move();
  }

  // sanke collision with himself
  struct Snake* snake3block = snake.next->next;

  if(snake3block != NULL) {

    while(snake3block->next != NULL) {
      bool coll_x = snake.rect.x == snake3block->rect.x;
      bool coll_y = snake.rect.y == snake3block->rect.y;

      if(coll_x && coll_y)
        quit = 1;

      snake3block = snake3block->next;
    }
  }

}

void process_input(SDL_Event event, size_t *quit) {

  switch (event.type) {

    case SDL_QUIT : {
      *quit = 1;
      break;
    }

    case SDL_KEYDOWN : {
      start = true;

      printf("%s\n", SDL_GetKeyName(event.key.keysym.sym) ); 

      switch (event.key.keysym.sym) {

        case SDLK_ESCAPE: {
          *quit = 1;
          break;
        }

        case SDLK_UP : {
          dx = 0;
          dy = -SNAKE_SPEED;
          break;
        }

        case SDLK_DOWN: {
          dx = 0;
          dy = SNAKE_SPEED;
          break;
        }

        case SDLK_RIGHT: {
          dy = 0;
          dx = SNAKE_SPEED;
          break;
        }

        case SDLK_LEFT: {
          dy = 0;
          dx = -SNAKE_SPEED;
          break;
        }

        case SDLK_w : {
          dx = 0;
          dy = -SNAKE_SPEED;
          break;
        }

        case SDLK_s: {
          dx = 0;
          dy = SNAKE_SPEED;
          break;
        }

        case SDLK_d: {
          dy = 0;
          dx = SNAKE_SPEED;
          break;
        }

        case SDLK_a: {
          dy = 0;
          dx = -SNAKE_SPEED;
          break;
        }

        case SDLK_q: {
          snake_add_block();
          break;
        }

        case SDLK_e: {
          apple_move();
          break;
        }
      }

    } 

    default:
      break;

  }
}

int main(int argc, char *argv[])
{
  srand(time(NULL));
  start = false;

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL; 

  window = SDL_CreateWindow("Snake", 0, 0, WIN_W, WIN_H, 0);
  if(window == NULL) {
    fprintf(stderr, "Error SDL_Window is NULL!");
    exit(1);
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if(renderer == NULL) {
    fprintf(stderr, "Error SDL_Renderer is NULL!");
    exit(1);
  }

  int blend_mode = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  if(blend_mode != 0) {
    fprintf(stderr, "Error while setting the blend mode, check you grahics card driver!\n");
    exit(1);
  }

  Uint32 time = SDL_GetTicks();

  apple.rect.x = SNAKE_SIZE * ((rand() % GAME_RECT_ROWS)+1);
  apple.rect.y = SNAKE_SIZE * ((rand() % GAME_RECT_COLS)+1);

  snake.next = &snake_tail;
  snake_tail.prev = &snake;

  while (!quit) {

    SDL_Event event = {0};

    Uint32 delta_time = (SDL_GetTicks() - time);

    while (SDL_PollEvent(&event)) {

      process_input(event, &quit);

    }

    // Snake position updates every 100 ms
    if (delta_time >= 100 && start == true) {
      //struct Snake *curr = &snake;

      int prev_x = snake.rect.x;
      int prev_y = snake.rect.y;

      snake.rect.x += dx;
      snake.rect.y += dy;

      struct Snake *curr = snake.next;

      while(curr != NULL) {
        int temp_x = curr->rect.x;
        int temp_y = curr->rect.y;

        curr->rect.x = prev_x;
        curr->rect.y = prev_y;

        prev_x = temp_x;
        prev_y = temp_y;

        curr = curr->next;
      }

      printf("dx: %d, dy: %d\n", dx, dy);
      time = SDL_GetTicks();
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
    SDL_Rect game_rect = {GAME_RECT_PADDING, GAME_RECT_PADDING, GAME_RECT_SIZE, GAME_RECT_SIZE};
    SDL_RenderDrawRect(renderer, &game_rect);

    SDL_SetRenderDrawColor(renderer, 128, 255, 128, 128);
   
    for (size_t y=GAME_RECT_PADDING; y<=WIN_H - GAME_RECT_PADDING; y+=SNAKE_SIZE) {
      SDL_RenderDrawLine(renderer, GAME_RECT_PADDING, y, WIN_W - GAME_RECT_PADDING, y);
    }

    for (size_t x=GAME_RECT_PADDING; x<=WIN_H - GAME_RECT_PADDING; x+=SNAKE_SIZE) {
      SDL_RenderDrawLine(renderer, x, GAME_RECT_PADDING, x, WIN_W - GAME_RECT_PADDING);
    }

    snake_draw(renderer);
    apple_draw(renderer);
    snake_check_coll();

    SDL_RenderPresent(renderer);

    SDL_Delay(10);
  }

  SDL_Quit();

  return EXIT_SUCCESS;
}
