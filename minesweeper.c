#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool hit_mine = false;

typedef struct{
  bool has_mine;
  bool already_selected;
  int adj_mine_count;
} minesweeper_cell;


void init_minesweeper_map(minesweeper_cell**, int, int);
bool check_valid_cell_selection(minesweeper_cell**, int, int, int, int);
void print_minesweeper_map(minesweeper_cell**, int, int);
bool is_game_out_of_move(minesweeper_cell**, int, int);
int calculate_adj_mine_count(minesweeper_cell**, int, int, int, int);
void traverse_adj_cell_mine_count(minesweeper_cell**, int, int, int, int);

// LOL, this shit is pre-compute before select the first cell, be careful
int main(void) {
  srand(time(NULL));
  int row, column;
  int selected_row, selected_column;
  printf("Enter row (>2) and column (>2) for the minesweeper map. Example. 3 3\n");
  scanf("%d %d", &row, &column);
  if (row <= 2 || column <= 2) {
    printf("invalid row or column. Exit.\n");
    return 1;
  }
  minesweeper_cell** map = malloc(sizeof(minesweeper_cell) * row);
  if (map == NULL) {
    printf("Can't assign memory to map. Exit..");
    return 1;
  }

  for (int i = 0; i < row; ++i) {
    map[i] = malloc(sizeof(minesweeper_cell) * column);
    if (map[i] == NULL) {
      printf("Can't assign memory to map. Exit..");
      return 1;
    }
    for (int j = 0; j < column; ++j) {
      map[i][j].has_mine = false;
      map[i][j].already_selected = false;
      map[i][j].adj_mine_count = 0;
    }
  }

  init_minesweeper_map(map, row, column);
  printf("Game Start!\n");

  while (!hit_mine && !is_game_out_of_move(map, row, column)) {
    print_minesweeper_map(map, row, column);
    bool user_selected = false;
    int break_count;
    while (!user_selected && break_count <= 10) {
      break_count = 0;
      printf("Enter the first cell you want to choose. E.g. 0 0\n");
      scanf("%d %d", &selected_row, &selected_column);
      user_selected = check_valid_cell_selection(map, row, column, selected_row, selected_column);
      if (!user_selected) {
        printf("invalid selection for %d %d, try again\n", selected_row, selected_column);
        ++break_count;
      } else {
        map[selected_row][selected_column].already_selected = true;
      }
    }
    if (break_count > 10) {
      printf("bro, too many wrong inputs\n");
      return 1;
    }

    if (map[selected_row][selected_column].has_mine) {
      printf("Hit mine!\n");
      hit_mine = true;
      break;
    } else {
      map[selected_row][selected_column].adj_mine_count = calculate_adj_mine_count(map, row, column, selected_row, selected_column);
      traverse_adj_cell_mine_count(map, row, column, selected_row, selected_column);
    }
  }

  print_minesweeper_map(map, row, column);
  if (!hit_mine) {
    printf("you won!\n");
  } else {
    printf("you lose!\n");
  }

  for (int i = 0; i < row; ++i) {
    free(map[i]);
  }
  free(map);
  return 0;
}

// hardcode init mines with r - 2, c - 2
void init_minesweeper_map(minesweeper_cell** map, int r, int c) {
  for (int i = 0; i < r - 2; ++i) {
    for (int j = 0; j < c - 2; ++j) {
        map[rand() % r][rand() % c].has_mine = true;
    }
  }
}

void print_minesweeper_map(minesweeper_cell** map, int r, int c) {
  printf("  ");
  for (int i = 0; i < r; ++i) {
    printf("%d ", i);
  }
  printf("\n");

  for (int i = 0; i < r; ++i) {
    printf("%d ", i);
    for (int j = 0; j < c; ++j) {
      if (!map[i][j].has_mine && map[i][j].already_selected) {
        printf("%d ", map[i][j].adj_mine_count);
      } else if (map[i][j].already_selected && map[i][j].has_mine) {
        printf("B ");
      } else {
        printf("X ");
      }
    }
    printf("\n");
  }
}

bool check_valid_cell_selection(minesweeper_cell** map, int r, int c, int selected_row, int selected_column) {
  return (selected_row >= 0) && (selected_row < r) && (selected_column >= 0) && (selected_column < c) && !map[selected_row][selected_column].already_selected;
}

bool is_game_out_of_move(minesweeper_cell** map, int r, int c) {
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      if (!map[i][j].has_mine && !map[i][j].already_selected) {
        return false;
      }
    }
  }

  return true;
}

int calculate_adj_mine_count(minesweeper_cell** map, int r, int c, int selected_row, int selected_column) {
  int sum = 0;

  // Handle non corners
  if (selected_row > 0) {
    sum += map[selected_row - 1][selected_column].has_mine? 1:0;    
  }
  
  if (selected_row < r - 1) {
    sum += map[selected_row + 1][selected_column].has_mine? 1:0;    
  }

  if (selected_column > 0) {
    sum += map[selected_row][selected_column - 1].has_mine? 1:0;    
  }
  
  if (selected_column < c - 1) {
    sum += map[selected_row][selected_column + 1].has_mine? 1:0;    
  }

  // Handle left top corner and right top corner
  if (selected_row > 0) {
    if (selected_column > 0) {
      sum += map[selected_row - 1][selected_column - 1].has_mine? 1: 0;
    } 
    if (selected_column < c - 1) {
      sum += map[selected_row - 1][selected_column + 1].has_mine? 1: 0;
    }
  }

  // Handle left bottom corner and right bottom corner
  if (selected_row < r - 1) {
    if (selected_column > 0) {
      sum += map[selected_row + 1][selected_column - 1].has_mine? 1: 0;
    } 
    if (selected_column < c - 1) {
      sum += map[selected_row + 1][selected_column + 1].has_mine? 1: 0;
    }
  }

  return sum;
}

void traverse_adj_cell_mine_count(minesweeper_cell** map, int r, int c, int selected_row, int selected_column) {
  // stop if adj mine count > 0
  map[selected_row][selected_column].adj_mine_count = calculate_adj_mine_count(map, r, c, selected_row, selected_column);
  map[selected_row][selected_column].already_selected = true;

  if (map[selected_row][selected_column].adj_mine_count == 0) {
    if (selected_row > 0 && !map[selected_row - 1][selected_column].already_selected) {
      traverse_adj_cell_mine_count(map, r, c, selected_row - 1, selected_column);
    }
    if (selected_row < r - 1 && !map[selected_row + 1][selected_column].already_selected) {
      traverse_adj_cell_mine_count(map, r, c, selected_row + 1, selected_column);
    }
    if (selected_column > 0 && !map[selected_row][selected_column - 1].already_selected) {
      traverse_adj_cell_mine_count(map, r, c, selected_row, selected_column - 1);
    }
    if (selected_column < c - 1 && !map[selected_row][selected_column + 1].already_selected) {
      traverse_adj_cell_mine_count(map, r, c, selected_row, selected_column + 1);
    }
  }
}
