#pragma once

#define LEVEL_ROWS 5
#define LEVEL_COLS 5

class LevelData {

public:
  int width = LEVEL_COLS;
  int height = LEVEL_ROWS;
  int tile_data[LEVEL_ROWS][LEVEL_COLS] = {0};

  void set_tile_data(int new_data[LEVEL_ROWS][LEVEL_COLS], int rows, int cols);
};
