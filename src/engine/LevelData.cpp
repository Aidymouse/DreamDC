#include <engine/LevelData.h>

void LevelData::set_tile_data(int new_data[LEVEL_ROWS][LEVEL_COLS], int rows,
                              int cols) {

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      tile_data[row][col] = new_data[row][col];
    }
  }
}
