#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/* Global constants */

#define ERROR_MESSAGE_LENGTH 200
#define INITIAL_DEAD_PERCENTAGE 90
#define INITIAL_SPEED 10
#define BOARD_MAX_Y 300 
#define BOARD_MAX_X 300
#define FILE_LINE_LENGTH 1000
#define ERROR_MESSAGE_LENGTH 200
#define BUTTON_MAX_HEIGTH 55
#define BUTTON_COUNT 5
#define SLIDER_COUNT 2
#define X_PADDING 4
#define Y_PADDING 14
    
typedef struct 
{
    bool cell_status; // 1 for cell_status, 0 for DEAD
} Cells;

typedef struct 
{
    int y;
    int x;
} Resolution;

Cells **BOARD_initialize(Resolution resolution, bool generate_random, int death_percentage);
Cells **BOARD_alloc_copy_board(Resolution resolution);
void BOARD_calculate_resolution(Resolution *board_resolution, bool generate_random);
void BOARD_update_board(Cells **board, Cells **new_board, Resolution resolution);
void BOARD_handle_error(char error_message[ERROR_MESSAGE_LENGTH]);
void BOARD_generate_or_kill(Cells **new_board, Cells **board, int neighbor_count, int y, int x);
void BOARD_free_board(Cells **board, Resolution resolution);
int BOARD_neighbors_state(Cells **board, int y, int x, Resolution resolution);
int constrain(int value, int min, int max);
int map(int value, int old_min, int old_max, int new_min, int new_max);