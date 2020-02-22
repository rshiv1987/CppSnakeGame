#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width)),
      random_h(0, static_cast<int>(grid_height)) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();
    
    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food, spinach, draw_spinach);

    frame_end = SDL_GetTicks();
    
    // Set draw_spinach as false after 5 seconds of score being a multiple of 10
    if (frame_end - spinach_start_time >= 5000){
      draw_spinach = false;
    }

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y) && !(SpinachCell(x,y) && draw_spinach)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

bool Game::FoodCell(int x, int y){
  if ( x == food.x && y == food.y){
    return true;
  }
  return false;
}

bool Game::SpinachCell(int x, int y){
  if ( x == spinach.x && y == spinach.y && draw_spinach){
    return true;
  }
  return false;
}

void Game::PlaceSpinach() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item and is not a food cell before
    // placing spinach.
    if (!snake.SnakeCell(x, y) && !FoodCell(x,y)) {
      spinach.x = x;
      spinach.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y){
    score++;
    PlaceFood();
    // Place a new Spinach when score is a multiple of 10 and start timer
    if (score % 10 == 0 && score > 0 && !draw_spinach){
      PlaceSpinach();
      draw_spinach = true;
      spinach_start_time = SDL_GetTicks();
    }
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
  // Check if there is spinach over here
  else if (spinach.x == new_x && spinach.y == new_y && draw_spinach){
    //Increment score, change location of spinach and set draw_spinach to false
    score++;
    draw_spinach = false;
    // Shrink the snake's body and decrement speed
    snake.ShrinkBody();
    snake.speed -= 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }