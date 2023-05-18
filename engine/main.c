/***************************************************************************
 *   Copyright (C) 2023 by e2203130   *
 *   e2203130@edu.vamk.fi   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*********************************************************************

 1.  NAME 
    Game of Life

 2.  DESCRIPTION
    This program is a simple implementation of Conway's Game of Life.
    The program uses ncurses library to display the board on the terminal.
    The initial pattern is read from the file initial_pattern.txt.
    The program can be compiled using the makefile provided.

 3.  VERSIONS
       Original: 01.04.2023
       Version history:
         01.04.2023 - Initial version

**********************************************************************/

/*-------------------------------------------------------------------*
*    HEADER FILES                                                    *
*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses/ncurses.h>

/*-------------------------------------------------------------------*
*    GLOBAL VARIABLES AND CONSTANTS                                  *
*--------------------------------------------------------------------*/
/* Control flags */
#define DEBUG 1

/* Global constants */

#define TICK_LIMIT 10000
#define TICK_DELAY 50
#define BOARD_MAX_Y 10000
#define BOARD_MAX_X 10000
#define ERROR_MESSAGE_LENGTH 200

/* Global variables */

/* Global structures */

typedef struct 
{
    bool cell_status; // 1 for cell_status, 0 for DEAD
} Cells;

typedef struct 
{
    int y;
    int x;
} Resolution;


/*-------------------------------------------------------------------*
*    FUNCTION PROTOTYPES                                             *
*--------------------------------------------------------------------*/
int neighbors_state(Cells **board, int y, int x, Resolution resolution);
void handle_error(char error_message[ERROR_MESSAGE_LENGTH]);
void print_board(Cells **board, Resolution resolution); 
void update_board(Cells **board, Cells **new_board, Resolution resolution);
void free_board(Cells **board, Resolution resolution);
void generate_or_kill(Cells **new_board, Cells **board, int neighbor_count, int y, int x);
Cells **initialize_board(Resolution resolution);
Cells **create_new_board(Resolution resolution);
Resolution calculate_resolution(void);

/*********************************************************************
 *    MAIN PROGRAM                                                      *
 **********************************************************************/


int main(void)
{
    int ticks = 0;
    initscr();
    curs_set(0); 
    Resolution resolution = calculate_resolution();

    Cells **board = initialize_board(resolution);
    Cells **new_board = create_new_board(resolution);

    while (ticks < TICK_LIMIT)
    {
        update_board(board, new_board, resolution);
        #if DEBUG
        printw("\n\n|ITERATION : %d| |ROUNDS LEFT : %d| |TICK DELAY : %d|", ticks, TICK_LIMIT-ticks, TICK_DELAY);
        printw("\n|RESOLUTION : (%d, %d)|" ,resolution.y, resolution.x);
        wrefresh(stdscr);
        #endif
        napms(TICK_DELAY);  

        ticks++;
    }

    free_board(board, resolution);
    free_board(new_board, resolution);
    endwin();
    return 0;

} /* end of main */

/*********************************************************************
*    FUNCTIONS                                                       *
**********************************************************************/

/*********************************************************************
 NAME: initialize_board
 DESCRIPTION: This function initializes the board with the initial pattern from the file initial_pattern.txt.
	Input: resolution -- the resolution of the board
	Output: board -- the initialized board
  Used global variables: BOARD_MAX_X, BOARD_MAX_Y
 REMARKS when using this function: 
*********************************************************************/


Cells **initialize_board(Resolution resolution) 
{ 
    int y, x;
    char line[BOARD_MAX_X], error_message[ERROR_MESSAGE_LENGTH];
    FILE *file_pointer;
    Cells **board;

    sprintf(error_message, "FATAL ERROR -- MEMORY ALLOCATION FAILED AT FUNCTION initialize_board");

    file_pointer = fopen("engine/initial_pattern.txt", "r");
    
    board = (Cells **)malloc(resolution.y * sizeof(Cells *));
    if (board == NULL || file_pointer == NULL)
    {
        handle_error(error_message);
    }
    

    for (y = 0; y < resolution.y; y++) 
    {
        board[y] = (Cells *)malloc(resolution.x * sizeof(Cells));
        if (board[y] == NULL)
        {
            handle_error(error_message);
        }
    }

    for (y = 0; y < resolution.y; y++)
    {
        fgets(line, BOARD_MAX_X, file_pointer);
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

    return board;
}

/*********************************************************************
 NAME: create_new_board
 DESCRIPTION: This function creates a new board.
	Input: resolution -- the resolution of the board
	Output: new_board -- the new board
  Used global variables:  
 REMARKS when using this function: New board always has the next generation of the previous board.
*********************************************************************/

Cells **create_new_board(Resolution resolution)
{
    int y;
    char error_message[ERROR_MESSAGE_LENGTH];
    Cells **new_board;

    sprintf(error_message, "FATAL ERROR -- MEMORY ALLOCATION FAILED AT FUNCTION create_new_board");

    new_board = (Cells **)malloc(resolution.y * sizeof(Cells *));
    if (new_board == NULL)
    {
        handle_error(error_message);
    }
    
    for (y = 0; y < resolution.y; y++) 
    {
        new_board[y] = (Cells *)malloc(resolution.x * sizeof(Cells));

        if (new_board[y] == NULL)
        {
            handle_error(error_message);
        }
    }

    return new_board;
}

/*********************************************************************
 NAME: update_board 
 DESCRIPTION: This function updates the board with the next generation.
	Input: board -- the board to be updated
           new_board -- the new board
           resolution -- the resolution of the board
	Output: 
  Used global variables: 
 REMARKS when using this function: 
*********************************************************************/
void update_board(Cells **board, Cells **new_board, Resolution resolution)
{
    int y, x, neighbor_count;

    for (y = 0; y < resolution.y; y++)
    {
        memcpy(new_board[y], board[y], resolution.x * sizeof(Cells)); // This creates a deep copy of 2D struct **Cells by copying the pointer that point to sub structures.
        for (x = 0; x < resolution.x; x++)
        {
            neighbor_count = neighbors_state(board, y, x, resolution);
            generate_or_kill(new_board, board, neighbor_count, y, x); 
        }
    }

    print_board(board, resolution);

    for (y = 0; y < resolution.y; y++)
    {
        memcpy(board[y], new_board[y], resolution.x * sizeof(Cells)); // This creates a deep copy of 2D struct **Cells by copying the pointer that point to sub structures.
    }
    
}

/*********************************************************************
 NAME: free_board
 DESCRIPTION: This function frees the memory allocated for the board.
	Input: board -- the board to be freed
           resolution -- the resolution of the board
	Output: 
  Used global variables: 
 REMARKS when using this function: 
*********************************************************************/
void free_board(Cells **board, Resolution resolution) // This functions loops over all the arrays that are in the pointer array y and frees them.
{
    int y;
    for (y = 0; y < resolution.y; y++)
    {
        free(board[y]); 
    }
    free(board);
}


/*********************************************************************
 NAME: generate_or_kill
 DESCRIPTION: This function generates or kills a cell based on the number of neighbors.
	Input: new_board -- the new board
           board -- the board to be updated
           neighbor_count -- the number of neighbors
           y -- the y coordinate of the cell
           x -- the x coordinate of the cell
	Output: 
  Used global variables: 
 REMARKS when using this function: 
*********************************************************************/
void generate_or_kill(Cells **new_board, Cells **board, int neighbor_count, int y, int x)
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

/*********************************************************************
 NAME: print_board
 DESCRIPTION: This function prints the board.
	Input: board -- the board to be printed
           resolution -- the resolution of the board
	Output: 
  Used global variables: 
 REMARKS when using this function: 
*********************************************************************/
void print_board(Cells **board, Resolution resolution) 
{
    int y, x;

    erase();
    for(y = 0; y < resolution.y; y++) 
    {
        for(x = 0; x < resolution.x; x++) 
        {
            if (board[y][x].cell_status == 1)
            {
                printw("O");
            }

            else if (board[y][x].cell_status == 0)
            {
                printw(".");
            }
            
        }
        printw("\n");
    }
    wrefresh(stdscr);
}

/*********************************************************************
 NAME: calculate_resolution
 DESCRIPTION: This function calculates the resolution of the board and stores it in a struct.
	Input: 
	Output: resolution -- the resolution of the board
  Used global variables: 
 REMARKS when using this function: 
*********************************************************************/
Resolution calculate_resolution(void)
{
    FILE *file_pointer;
    Resolution resolution = {0,0};
    char line[BOARD_MAX_X], error_message[ERROR_MESSAGE_LENGTH];;
    int last_iteration_x_resolution;

    file_pointer = fopen("engine/initial_pattern.txt", "r");
    if (file_pointer == NULL)
    {
        exit(EXIT_FAILURE);
    }

    while ((fgets(line, BOARD_MAX_X, file_pointer)) != NULL && line[0] != '\n')
    {
        resolution.x = strlen(line);
        resolution.y++;

        if (resolution.y > 1)
        {
            if (last_iteration_x_resolution != resolution.x)
            {
                sprintf(error_message, "FATAL ERROR -- FILE FORMAT ERROR. CHECK LINE (%d) FOR ERRORS. (CHECK FOR SPACES!)", resolution.y);
                handle_error(error_message);
            }
        }

        last_iteration_x_resolution = resolution.x;
    }


    fclose(file_pointer);
    resolution.x--; // Remove newline character from the total x resolution
    return resolution;
}

/*********************************************************************
 NAME: neighbors_state
 DESCRIPTION: This function counts the number of living neighbors.
	Input: board -- the board to be checked
           y -- the y coordinate of the cell
           x -- the x coordinate of the cell
           resolution -- the resolution of the board
	Output: living_neigbours -- the number of living neighbors
  Used global variables: 
 REMARKS when using this function: 
*********************************************************************/
int neighbors_state(Cells **board, int y, int x, Resolution resolution)
{
    int living_neigbours = 0, i, j, neighbor_y, neighbor_x;
    
    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            neighbor_y = y + i;
            neighbor_x = x + j;
            if((neighbor_y != y || neighbor_x != x) && (neighbor_y >= 0 && neighbor_x >= 0) && (neighbor_y < resolution.y && neighbor_x < resolution.x) && board[neighbor_y][neighbor_x].cell_status)
            {
                living_neigbours++;
            }
        }
    }
    
    return living_neigbours;
}

/*********************************************************************
 NAME: handle_error
 DESCRIPTION: This function handles errors.
	Input: error_message -- the error message to be printed
	Output: 
  Used global variables: 
 REMARKS when using this function: 
*********************************************************************/
void handle_error(char error_message[ERROR_MESSAGE_LENGTH])
{
    erase();
    printw("%s\n", error_message);
    refresh();
    napms(5000);
    exit(EXIT_FAILURE);
}