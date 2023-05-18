#include "BOARD_functions.h"

void BOARD_handle_error(char error_message[ERROR_MESSAGE_LENGTH])
{
    printf("%s\n", error_message);
    exit(EXIT_FAILURE);
}

void BOARD_calculate_resolution(Resolution *board_resolution, bool generate_random)
{
    char line[FILE_LINE_LENGTH], error_message[ERROR_MESSAGE_LENGTH];;
    int x_resolution_largest = 0;
    board_resolution->x = 0;
    board_resolution->y = 0;

    if (!generate_random)
    {
        FILE *file_pointer;
        file_pointer = fopen("engine/initial_pattern.txt", "r");
        if (file_pointer == NULL)
        {
            sprintf(error_message, "FATAL ERROR -- FILE DOESN'T EXIST OR CANNOT BE ACCESSED.");
        }

        while ((fgets(line, FILE_LINE_LENGTH, file_pointer)) != NULL && line[0] != '\n')
        {
            board_resolution->x = strlen(line);
            board_resolution->y++;

            if (board_resolution->x > x_resolution_largest)
            {
                x_resolution_largest = board_resolution->x;
            }
        }
        board_resolution->x = x_resolution_largest;
        board_resolution->x++;
        fclose(file_pointer);
    }

    else
    {
        board_resolution->y = BOARD_MAX_Y;
        board_resolution->x = BOARD_MAX_X;
    }
}

Cells **BOARD_initialize(Resolution resolution, bool generate_random, int death_percentage)
{ 
    int y, x, random_number;
    char line[1000], error_message[ERROR_MESSAGE_LENGTH];
    FILE *file_pointer;
    Cells **board;
    srand(time(NULL));

    sprintf(error_message, "FATAL ERROR -- MEMORY ALLOCATION FAILED AT FUNCTION initialize_board");

    board = (Cells **)malloc(resolution.y * sizeof(Cells *));
    if (board == NULL)
    {
        BOARD_handle_error(error_message);
    }
    
    for (y = 0; y < resolution.y; y++) 
    {
        board[y] = (Cells *)malloc(resolution.x * sizeof(Cells));
        if (board[y] == NULL)
        {
            BOARD_handle_error(error_message);
        }
    }

    if (!generate_random)
    {
        file_pointer = fopen("engine/initial_pattern.txt", "r");
        if (!file_pointer)
        {
            sprintf(error_message, "FATAL ERROR -- FILE DOESN'T EXIST OR CANNOT BE ACCESSED.");
            BOARD_handle_error(error_message);
        }

        for (y = 0; y < resolution.y; y++)
        {
            fgets(line, resolution.x, file_pointer);
            for (x = 0; x < resolution.x; x++)
            {
                if (line[x] == 'O')
                {
                    board[y][x].cell_status = 1;
                }

                else
                {
                    board[y][x].cell_status = 0;
                }
            }    
        }
        fclose(file_pointer);
    }
    
    else
    {
        for (y = 0; y < BOARD_MAX_Y; y++)
        {
            for (x = 0; x < BOARD_MAX_X; x++)
            {
                random_number = rand() % 100;
                if (random_number > death_percentage)
                {
                    board[y][x].cell_status = 1;
                }
                
                else
                {
                    board[y][x].cell_status = 0;
                }
            }
        }
    }

    return board;
}


Cells **BOARD_alloc_copy_board(Resolution resolution)
{
    int y;
    char error_message[ERROR_MESSAGE_LENGTH];
    Cells **new_board;

    sprintf(error_message, "FATAL ERROR -- MEMORY ALLOCATION FAILED AT FUNCTION BOARD_alloc_copy_board");

    new_board = (Cells **)malloc(resolution.y * sizeof(Cells *));
    if (new_board == NULL)
    {
        BOARD_handle_error(error_message);
    }
    
    for (y = 0; y < resolution.y; y++) {
        new_board[y] = (Cells *)malloc(resolution.x * sizeof(Cells));

        if (new_board[y] == NULL)
        {
            BOARD_handle_error(error_message);
        }
    }

    return new_board;
}



void BOARD_generate_or_kill(Cells **new_board, Cells **board, int neighbor_count, int y, int x)
{
    if (!board[y][x].cell_status && neighbor_count == 3)
    {
        new_board[y][x].cell_status = 1;
    }

    else if(board[y][x].cell_status && (neighbor_count == 3 || neighbor_count == 2))
    {
        new_board[y][x].cell_status = 1;
    }

    else if(board[y][x].cell_status && neighbor_count > 3)
    {
        new_board[y][x].cell_status = 0;
    }

    else if(board[y][x].cell_status && neighbor_count < 2)
    {
        new_board[y][x].cell_status = 0;
    }
    
    else
    {
        new_board[y][x].cell_status = 0;
    }
}

void BOARD_free_board(Cells **board, Resolution resolution)
{
    int y;
    for (y = 0; y < resolution.y; y++)
    {
        free(board[y]); // Free memory allocated for each row
    }
    free(board); // Free memory allocated for the array of row pointers
}


int BOARD_neighbors_state(Cells **board, int y, int x, Resolution resolution)
{
    int living_neigbours = 0, i, j, neighbor_y, neighbor_x;
    
    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            neighbor_y = y + i;
            neighbor_x = x + j;
            if((neighbor_y != y || neighbor_x != x) && (neighbor_y >= 0 && neighbor_x >= 0)
            && (neighbor_y < resolution.y && neighbor_x < resolution.x) && board[neighbor_y][neighbor_x].cell_status)
            {
                living_neigbours++;
            }
        }
    }
    
    return living_neigbours;
}

void BOARD_update_board(Cells **board, Cells **new_board, Resolution resolution)
{
    int y, x, neighbor_count;

    for (y = 0; y < resolution.y; y++)
    {
        memcpy(new_board[y], board[y], resolution.x * sizeof(Cells)); // This creates a deep copy of 2D struct **Cells by copying the pointer that point to sub structures.
        for (x = 0; x < resolution.x; x++)
        {
            neighbor_count = BOARD_neighbors_state(board, y, x, resolution);
            BOARD_generate_or_kill(new_board, board, neighbor_count, y, x); 
        }
    }

    for (y = 0; y < resolution.y; y++)
    {
        memcpy(board[y], new_board[y], resolution.x * sizeof(Cells)); // This creates a deep copy of 2D struct **Cells by copying the pointer that point to sub structures.
    }
}

int constrain(int value, int min, int max)
{
    if (value < min)
    {
        return min;
    }

    else if (value > max)
    {
        return max;
    }

    else
    {
        return value;
    }
}

int map(int value, int old_min, int old_max, int new_min, int new_max)
{
    int mapped_value;
    mapped_value = (int)(value - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;
    return mapped_value;
}
