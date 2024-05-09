# Game of Life Simulation using C and SDL Graphics Library

Game of Life Simulation, a project crafted with C and the SDL graphics library. This simulation, inspired by mathematician John Conway's cellular automaton.

## Key Features

1. **Conway's Game of Life Rules:** Conway's Game of Life, where cells undergo life, death, and multiplication based on the states of their neighboring cells.

2. **User Interaction:** Take control of the simulation by adjusting the speed of its progression and manipulating the initial cell configurations. The amount of generated cells can also be altered.

3. **Pattern Loading:** Initial patterns can be set in the  `initial_pattern.txt` file. The grid size dynamically adjusts to accommodate the loaded pattern.

## Getting Started

1. **Clone the Repository:** Begin by cloning the repository to your local machine.
   
   ```bash
   git clone https://github.com/LauriAlanen/gameOfLife.git
   cd gameOfLife
   make
   ./main

2. Now a window like this should open
![image](https://github.com/LauriAlanen/gameOfLife/assets/80245457/69c6f40f-9e52-4291-b795-695d2b330f2e)
   - By pressing **start** you can start the simulation
   - **Stop** stops the current simulation
   - **Regenerate** refreshes the pattern randomly or from the `initial_pattern.txt` file
   - **Mode** switches between random or predefined pattern from `initial_pattern.txt`
   - By moving the **car** you can alter the speed which the simulation is ran
   - By moving the **pacman** you can alter how many cells are generated when using the random mode
