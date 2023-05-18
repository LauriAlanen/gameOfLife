/***************************************************************************
 *   Copyright (C) $Year$ by $Author$   *
 *   $Email$   *
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


2.  DESCRIPTION


3.  VERSIONS
    Original:


    Version history:

**********************************************************************/

/*-------------------------------------------------------------------*
*    HEADER FILES                                                    *
*--------------------------------------------------------------------*/

#include "BOARD_functions.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/*-------------------------------------------------------------------*
*    GLOBAL VARIABLES AND CONSTANTS                                  *
*--------------------------------------------------------------------*/

/* Global variables */

/* Global structures */

typedef struct
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rectangle;
} Button;

typedef struct
{
    SDL_Surface *back_surface;
    SDL_Texture *back_texture;
    SDL_Rect back_rectangle;
    SDL_Surface *front_surface;
    SDL_Texture *front_texture;
    SDL_Rect front_rectangle;
} Slider;

typedef struct
{
    Button start_button;
    Button stop_button;
    Button regenerate_button;
    Button quit_button;
    Button randomize_button;
    Slider speed_slider;
    Slider spawn_slider;
    Uint16 speed;
    Uint16 death_percentage;
} Controls;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *front_buffer;
    SDL_Event event;
} Window;

/*-------------------------------------------------------------------*
*    FUNCTION PROTOTYPES                                             *
*--------------------------------------------------------------------*/
int poll_events(Window main_window, Controls *game_controls);
void get_monitor_resolution(Resolution *monitor_resolution);
void init_window(Window *main_window, Resolution board_resolution, Resolution monitor_resolution, Controls *game_controls, Button button[BUTTON_COUNT], Slider slider[SLIDER_COUNT]);
void init_buttons(SDL_Renderer *renderer, Controls *game_controls, Resolution monitor_resolution, Button button[BUTTON_COUNT]);
void init_sliders(SDL_Renderer *renderer, Controls *game_controls, Resolution monitor_resolution, Slider slider[SLIDER_COUNT]);
void print_board(Cells **board, Resolution board_resolution, Resolution monitor_resolution, SDL_Renderer *renderer, SDL_Surface *surface, SDL_Texture *front_buffer);
void render_controls(SDL_Renderer *renderer, Controls *game_controls);
void destroy_window(Window main_window, Controls *game_controls);
/*********************************************************************
 *    MAIN PROGRAM                                                      *
 **********************************************************************/

int main(void)
{
    int mode = 0, last_mode = 0, surface_width, surface_heigth;
    bool randomize = 0;
    Uint32 start_time, current_time = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_OPENGL_SHADERS, "1");

    Resolution board_resolution; 
    Resolution monitor_resolution;
    BOARD_calculate_resolution(&board_resolution, randomize);
    get_monitor_resolution(&monitor_resolution);

    Window main_window;
    Controls game_controls;
    Button button[BUTTON_COUNT];
    Slider slider[SLIDER_COUNT];
    init_window(&main_window, board_resolution, monitor_resolution, &game_controls, button, slider);
    game_controls.death_percentage = INITIAL_DEAD_PERCENTAGE;
    game_controls.speed = INITIAL_SPEED;

    Cells **board = BOARD_initialize(board_resolution, randomize, game_controls.death_percentage);
    Cells **new_board = BOARD_alloc_copy_board(board_resolution);

    start_time = SDL_GetTicks();
    print_board(board, board_resolution, monitor_resolution, main_window.renderer,
                main_window.surface, main_window.front_buffer);

    while (mode != -1)
    {
        mode = poll_events(main_window, &game_controls);
        current_time = SDL_GetTicks();

        if (current_time-start_time > game_controls.speed && mode == 1)
        {
            BOARD_update_board(board, new_board, board_resolution);
            print_board(board, board_resolution, monitor_resolution, main_window.renderer, 
                        main_window.surface, main_window.front_buffer);
            start_time = current_time;
        }

        else if (mode == 2 && last_mode != 2)
        {
            BOARD_free_board(board, board_resolution);
            board = BOARD_initialize(board_resolution, randomize, game_controls.death_percentage);
            print_board(board, board_resolution, monitor_resolution, main_window.renderer, 
                        main_window.surface, main_window.front_buffer);
        }
        
        else if (mode == 3 && last_mode != 3)
        {
            randomize ^= true;

            BOARD_free_board(board, board_resolution);
            BOARD_calculate_resolution(&board_resolution, randomize);
            new_board = BOARD_alloc_copy_board(board_resolution);

            surface_width = board_resolution.x * (monitor_resolution.x / board_resolution.x);
            surface_heigth = board_resolution.y * (monitor_resolution.y / board_resolution.y);
            
            SDL_Surface *new_surface = SDL_CreateRGBSurface(0, surface_width, surface_heigth + BUTTON_MAX_HEIGTH, 32, 0, 0, 0, 0);
            main_window.surface = new_surface;

            board = BOARD_initialize(board_resolution, randomize, game_controls.death_percentage);
            print_board(board, board_resolution, monitor_resolution, main_window.renderer, 
                        main_window.surface, main_window.front_buffer);
        }
        
        render_controls(main_window.renderer, &game_controls);
        SDL_RenderPresent(main_window.renderer);
        last_mode = mode;
    }

    destroy_window(main_window, &game_controls);
    BOARD_free_board(board, board_resolution);
    BOARD_free_board(new_board, board_resolution);

    return 0;
} /* end of main */

/*********************************************************************
*    FUNCTIONS                                                       *
**********************************************************************/

void print_board(Cells **board, Resolution board_resolution, Resolution monitor_resolution, SDL_Renderer *renderer, SDL_Surface *surface, SDL_Texture *front_buffer)
{
    int y, x, cell_width, cell_heigth;
    SDL_Rect cell;
    Uint32 cell_color;

    cell_width = monitor_resolution.x / board_resolution.x;
    cell_heigth = monitor_resolution.y / board_resolution.y;

    for (y = 0; y < board_resolution.y; y++)
    {
        for(x = 0; x < board_resolution.x; x++)
        {
            cell.x = x * cell_width;
            cell.y = y * cell_heigth;
            cell.w = cell_width;
            cell.h = cell_heigth;

            if (board[y][x].cell_status == 1) 
            {
                cell_color = SDL_MapRGB(surface->format, 255, 165 ,0);
                SDL_FillRect(surface, &cell, cell_color);
            } 

            else 
            {
                cell_color = SDL_MapRGB(surface->format, 0, 0, 0);
                SDL_FillRect(surface, &cell, cell_color);
            }
        }
    }

    front_buffer = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, front_buffer, NULL, NULL);
    SDL_DestroyTexture(front_buffer);
}


void get_monitor_resolution(Resolution *monitor_resolution)
{
    SDL_DisplayMode mode;

    if (SDL_GetCurrentDisplayMode(0, &mode) != 0)
    {
        monitor_resolution->x = 800;
        monitor_resolution->y = 600;
    }

    else
    {
        monitor_resolution->x = mode.w;
        monitor_resolution->y = mode.h;
    }
}

void init_sliders(SDL_Renderer *renderer, Controls *game_controls, Resolution monitor_resolution, Slider slider[SLIDER_COUNT])
{
    int i, x_position = 0, y_position = 0;
    char *bitmap_back[SLIDER_COUNT] = {"bitmaps/slider_backround.bmp", "bitmaps/slider_backround.bmp"};
    char *bitmap_front[SLIDER_COUNT] = {"bitmaps/speed_slider.bmp", "bitmaps/death_slider.bmp"};

    x_position = game_controls->randomize_button.rectangle.x + game_controls->randomize_button.rectangle.w;
    for (i = 0; i < SLIDER_COUNT; i++)
    {
        slider[i].back_surface = IMG_Load(bitmap_back[i]);
        slider[i].front_surface = IMG_Load(bitmap_front[i]);

        y_position = monitor_resolution.y - slider[i].back_surface->h;

        slider[i].back_texture = SDL_CreateTextureFromSurface(renderer, slider[i].back_surface);
        slider[i].front_texture = SDL_CreateTextureFromSurface(renderer, slider[i].front_surface);

        slider[i].back_rectangle.x = x_position;
        slider[i].back_rectangle.y = y_position;
        slider[i].front_rectangle.x = x_position + X_PADDING;
        slider[i].front_rectangle.y = y_position + Y_PADDING;

        slider[i].back_rectangle.w = slider[i].back_surface->w;
        slider[i].back_rectangle.h = BUTTON_MAX_HEIGTH;
        slider[i].front_rectangle.w = slider[i].front_surface->w;
        slider[i].front_rectangle.h = BUTTON_MAX_HEIGTH/2;

        x_position += slider[i].back_surface->w;
    }
    game_controls->speed_slider = slider[0];
    game_controls->spawn_slider = slider[1];
}

void init_buttons(SDL_Renderer *renderer, Controls *game_controls, Resolution monitor_resolution, Button button[BUTTON_COUNT])
{
    int i, x_position = 0, y_position = 0;

    char *bitmap[BUTTON_COUNT] = {"bitmaps/button_start.bmp", "bitmaps/button_stop.bmp",
                                       "bitmaps/button_regenerate.bmp", "bitmaps/button_quit.bmp", "bitmaps/button_randomize_check.bmp"};
    for (i = 0; i < BUTTON_COUNT; i++)
    {
        button[i].surface = IMG_Load(bitmap[i]);
        y_position = monitor_resolution.y - button[i].surface->h;
        button[i].texture = SDL_CreateTextureFromSurface(renderer, button[i].surface);
        button[i].rectangle.x = x_position;
        button[i].rectangle.y = y_position;
        button[i].rectangle.w = button[i].surface->w;
        button[i].rectangle.h = button[i].surface->h;
        
        x_position += button[i].surface->w;
    }
    
    game_controls->start_button = button[0];
    game_controls->stop_button = button[1];
    game_controls->regenerate_button = button[2];
    game_controls->quit_button = button[3];
    game_controls->randomize_button = button[4];
}


void render_controls(SDL_Renderer *renderer, Controls *game_controls)
{
    SDL_RenderCopy(renderer, game_controls->start_button.texture, NULL, &game_controls->start_button.rectangle);
    SDL_RenderCopy(renderer, game_controls->stop_button.texture, NULL, &game_controls->stop_button.rectangle);
    SDL_RenderCopy(renderer, game_controls->regenerate_button.texture, NULL, &game_controls->regenerate_button.rectangle);
    SDL_RenderCopy(renderer, game_controls->quit_button.texture, NULL, &game_controls->quit_button.rectangle);
    SDL_RenderCopy(renderer, game_controls->randomize_button.texture, NULL, &game_controls->randomize_button.rectangle);
    SDL_RenderCopy(renderer, game_controls->speed_slider.back_texture, NULL, &game_controls->speed_slider.back_rectangle);
    SDL_RenderCopy(renderer, game_controls->speed_slider.front_texture, NULL, &game_controls->speed_slider.front_rectangle);
    SDL_RenderCopy(renderer, game_controls->spawn_slider.back_texture, NULL, &game_controls->spawn_slider.back_rectangle);
    SDL_RenderCopy(renderer, game_controls->spawn_slider.front_texture, NULL, &game_controls->spawn_slider.front_rectangle);
}

void destroy_window(Window main_window, Controls *game_controls)
{
    SDL_FreeSurface(game_controls->start_button.surface);
    SDL_FreeSurface(game_controls->stop_button.surface);
    SDL_FreeSurface(game_controls->regenerate_button.surface);
    SDL_FreeSurface(game_controls->quit_button.surface);
    SDL_FreeSurface(game_controls->speed_slider.back_surface);
    SDL_FreeSurface(game_controls->speed_slider.front_surface);
    SDL_FreeSurface(game_controls->spawn_slider.back_surface);
    SDL_FreeSurface(game_controls->spawn_slider.front_surface);

    SDL_DestroyTexture(game_controls->start_button.texture);
    SDL_DestroyTexture(game_controls->stop_button.texture);
    SDL_DestroyTexture(game_controls->regenerate_button.texture);
    SDL_DestroyTexture(game_controls->quit_button.texture);
    SDL_DestroyTexture(game_controls->speed_slider.back_texture);
    SDL_DestroyTexture(game_controls->speed_slider.front_texture);
    SDL_DestroyTexture(game_controls->spawn_slider.back_texture);
    SDL_DestroyTexture(game_controls->spawn_slider.front_texture);

    SDL_FreeSurface(main_window.surface);
    SDL_DestroyTexture(main_window.front_buffer);
    SDL_DestroyRenderer(main_window.renderer);
    SDL_DestroyWindow(main_window.window);

    SDL_Quit();
}


void init_window(Window *main_window, Resolution board_resolution, Resolution monitor_resolution, Controls *game_controls, Button button[BUTTON_COUNT], Slider slider[SLIDER_COUNT])
{
    int surface_width, surface_heigth;

    surface_width = board_resolution.x * (monitor_resolution.x / board_resolution.x);
    surface_heigth = board_resolution.y * (monitor_resolution.y / board_resolution.y);

    main_window->window = SDL_CreateWindow("gameOfLife", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        monitor_resolution.x, monitor_resolution.y, SDL_WINDOW_FULLSCREEN_DESKTOP);
    main_window->renderer = SDL_CreateRenderer(main_window->window, -1, SDL_RENDERER_SOFTWARE);
    main_window->surface = SDL_CreateRGBSurface(0, surface_width, surface_heigth+BUTTON_MAX_HEIGTH, 32, 0, 0, 0, 0);
    main_window->front_buffer = SDL_CreateTexture(main_window->renderer, SDL_PIXELFORMAT_RGBA8888, 
                                                SDL_TEXTUREACCESS_TARGET, surface_width, surface_heigth+BUTTON_MAX_HEIGTH);
    SDL_SetRenderDrawBlendMode(main_window->renderer, SDL_BLENDMODE_MUL);

    init_buttons(main_window->renderer, game_controls, monitor_resolution, button);
    init_sliders(main_window->renderer, game_controls, monitor_resolution, slider);
}

int poll_events(Window main_window, Controls *game_controls)
{
    int mouse_y, mouse_x, x_pos = 0, speed = 100, death_percentage = 0;
    static int mode = 0, slider_clicked = 0;

    SDL_PollEvent(&main_window.event);
    SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_Point point = {mouse_x, mouse_y};
    switch (main_window.event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
            if (SDL_PointInRect(&point, &game_controls->start_button.rectangle))
            {
                printf("Start button clicked!\n");
                mode = 1;
            }

            else if (SDL_PointInRect(&point, &game_controls->stop_button.rectangle))
            {
                printf("Stop button clicked!\n");
                mode = 0;
            }

            else if (SDL_PointInRect(&point, &game_controls->regenerate_button.rectangle))
            {
                printf("Regenerate button clicked!\n");
                mode = 2;
            }   

            else if (SDL_PointInRect(&point, &game_controls->quit_button.rectangle))
            {
                printf("Quit button clicked!\n");
                mode = -1;
            }

            else if (SDL_PointInRect(&point, &game_controls->randomize_button.rectangle))
            {
                printf("Randomize button clicked!\n");
                mode = 3;
            }

            else if (SDL_PointInRect(&point, &game_controls->speed_slider.front_rectangle))
            {
                printf("Speed slider clicked!\n");
                slider_clicked = 1;
            }   

            else if (SDL_PointInRect(&point, &game_controls->spawn_slider.front_rectangle))
            {
                printf("Spawn slider clicked!\n");
                slider_clicked = 2;
            }   
            break;

        case SDL_MOUSEBUTTONUP:
            slider_clicked = 0;
            break;

        case SDL_MOUSEMOTION:
            if (slider_clicked && slider_clicked == 1)
            {
                x_pos = game_controls->speed_slider.back_rectangle.x;

                game_controls->speed_slider.front_rectangle.x = constrain(mouse_x, x_pos + X_PADDING,
                    x_pos + (game_controls->speed_slider.back_rectangle.w - game_controls->speed_slider.front_rectangle.w)-X_PADDING);

                speed = game_controls->speed_slider.front_rectangle.x - x_pos;
                game_controls->speed = map(speed, X_PADDING, game_controls->speed_slider.back_rectangle.w, 10, 1000);
            }

            else if (slider_clicked && slider_clicked == 2)
            {
                x_pos = game_controls->spawn_slider.back_rectangle.x;

                game_controls->spawn_slider.front_rectangle.x = constrain(mouse_x, x_pos + X_PADDING,
                    x_pos + (game_controls->spawn_slider.back_rectangle.w - game_controls->spawn_slider.front_rectangle.w) - X_PADDING);
                death_percentage = game_controls->spawn_slider.front_rectangle.x - x_pos;
                
                game_controls->death_percentage = map(death_percentage, X_PADDING, game_controls->spawn_slider.back_rectangle.w, 70, 99);
            }
            break;
    }
    return mode;
}


