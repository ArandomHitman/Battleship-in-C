/*
AUTHOR: Liam Nayyar
PURPOSE: A simplified version of the traditional boardgame 'Battleship'. 
The game is played on a 10x10 grid where each player has 4 ships to place on the grid. 
The ships and their lengths are as follows: Carrier (5), Battleship (4), Submarine (3), Patrol Boat (2). 
The ships can be placed either horizontally (H) or vertically (V) on the grid. The game is played in turns. 
The player and the enemy take turns attacking each other's grid. The game ends when all of the ships of one player are sunk. 
The player who sinks all of their enemy's ships wins the game. If the player would like to play again, they can do so after the game ends.
STATUS: COMPLETE (for now, currently under revisions on github as of 01/26/2025).
*/
/*-----------------------------------Included libraries-----------------------------------*/
#include <stdio.h>  // Standard input/output library
#include <stdlib.h> // Standard library
#include <time.h>   // Time library for random number generation
#include <string.h> // String library for memset
/*-----------------------------------Variable Declarations-----------------------------------*/
// Global Constant Variables
#define GRID_SIZE 10 // Variable to store the integer value of the grid size
#define MAX_NAME_LENGTH 32 // Variable to store the integer value of the maximum name length
#define EMPTY_CELL '.' // Variable to store the character value of the empty cell
#define HIT_CELL 'X' // Variable to store the character value of the hit cell
#define MISS_CELL 'O' // Variable to store the character value of the miss cell
#define SHIP_CELL 'S' // Dedicated constant for ship cells
#define DEBUG 0 // Set to 1 to enable the debug flags within my program 

// Function declarations (declared in advance since the calling loops cannot be in order)
int isValidAgreement(const char* playerAgreement);
void playerTurn();
int isValidName(const char* playerName);
void enemyTurn();
int isValidCoordinate(int xCoordinate, int yCoordinate);
int isPlacementValid(int xCoordinate, int yCoordinate, int shipLength, char shipOrientation, char grid[GRID_SIZE][GRID_SIZE]);
void placeShip(int xCoordinate, int yCoordinate, int shipLength, char shipOrientation, char grid[GRID_SIZE][GRID_SIZE]);
void printBoard(char grid[GRID_SIZE][GRID_SIZE], const char* boardTitle);
void HitandSunkShips(char grid[GRID_SIZE][GRID_SIZE], int* sunkShipCount);

// Global Variables
char playerGrid[GRID_SIZE][GRID_SIZE]; // Variable to store the player grid
char enemyGrid[GRID_SIZE][GRID_SIZE]; // Variable to store the enemy grid
char playerViewOfEnemyGrid[GRID_SIZE][GRID_SIZE]; // Variable to store the player's view of the enemy grid (blank so that the enemy's ships are hidden)
int playerHits = 0; // Variable to store the integer value of the player hits
int playerMisses = 0; // Variable to store the integer value of the player misses
int playerSunkShips = 0; // Variable to store the integer value of the player sunk ships
int enemyHits = 0; // Variable to store the integer value of the enemy hits
int enemyMisses = 0; // Variable to store the integer value of the enemy misses
int enemySunkShips = 0; // Variable to store the integer value of the enemy sunk ships
int lastHitXCoordinate = GRID_SIZE; // Track the last hit coordinates for the X axis, set to GRID_SIZE because it is out of valid range
int lastHitYCoordinate = GRID_SIZE; // Track the last hit coordinates for the Y axis, set to GRID_SIZE because it is out of valid range

/*-----------------------------------Clear Buffer Function-----------------------------------*/
/*
Purpose: Clear the input buffer to prevent buffer overflow.
Parameters: None
Return: None
Side Effects: None
*/
void (ClearBuffer)() {
    while (getchar() != '\n'); // Clear the input buffer of any remaining characters
}
/*-----------------------------------PrintMessage Function-----------------------------------*/
/*
Purpose: Print the welcome message to the player.
Parameters: None
Return: None
Side Effects: None
*/
void printMessage() {
printf("=================================================================================================================\n");
printf("                                           Welcome to Liam's Battleship Game!                                    \n");
printf("=================================================================================================================\n");
}

/*-----------------------------------Get Valid Input Function-----------------------------------*/
/*
Purpose: Get valid input from the user based on the provided validation function.
Parameters: const char* prompt - the prompt message to display.
            char* input - a pointer to a character array to store the input.
            int (*validationFunc)(const char*) - a pointer to the validation function.
Return: None
Side Effects: Modifies the input variable based on the user's input.
*/
void getValidInput(const char* prompt, char* input, int (*validationFunc)(const char*)) {
    do {
        printf("%s", prompt);
        if (scanf("%31[^\n]s", input) != 1) {
            printf("\nError reading input. Please try again.\n");
        }
        ClearBuffer();
    } while (!validationFunc(input));
}

/*-----------------------------------Get Player Name-----------------------------------*/
/*
Purpose: Prompt the player to enter their name.
Parameters: char* playerName - a pointer to a character array to store the player's name.
Return: None
Side Effects: char* playerName - modifies the playerName variable to store the player's name.
*/
void getPlayerName(char* playerName) {
    memset(playerName, 0, MAX_NAME_LENGTH); // Initialize memory to prevent garbage values
    getValidInput("\nWelcome player, please enter your name: ", playerName, isValidName);
    printf("\nWelcome, %s! It's a pleasure meeting you!\n", playerName);
}

/*-----------------------------------Check if Name is Valid-----------------------------------*/
/*
Purpose: Check if the name entered by the player is valid. The name must be between 1 and 32 characters long and can only contain letters and numbers.
Parameters: const char* playerName - a pointer to a character array containing the player's name.
Return: 1 if the name is valid, 0 if the name is invalid.
Side Effects: const char* playerName - validates and returns the state of the variable (see above)
*/
int isValidName(const char* playerName) {
    int nameLength = strlen(playerName);
    if (nameLength == 0 || nameLength > MAX_NAME_LENGTH) {
        return 0;
    }
    for (int index = 0; index < nameLength; index++) {
        if (!(playerName[index] >= 'A' && playerName[index] <= 'Z') &&
            !(playerName[index] >= 'a' && playerName[index] <= 'z') &&
            !(playerName[index] >= '0' && playerName[index] <= '9') &&
            playerName[index] != ' ') {
            return 0;
        }
    }
    return 1;
}

/*-----------------------------------Get Agreement-----------------------------------*/
/*
Purpose: Get the agreement from the player to start the game. The player must agree to the rules of the game before starting.
Parameters: char* playerAgreement - a pointer to a character to store the player's agreement.
Return: None
Side Effects: char* playerAgreement - validates and modifies the state of the variable based on the players input (Y/N)
*/
void getAgreement(char* playerAgreement) {
    printf("\nNow that we have introduced ourselves, let's go over the rules of the game.\n");
    printf("1. The game is played on a 10x10 grid. Each player has 4 ships to place on the grid.\n");
    printf("2. The ships and their lengths are as follows: Carrier (5), Battleship (4), Submarine (3), Patrol Boat (2).\n");
    printf("3. You have the opportunity to place your ships wherever you'd like on the grid, but you can either do so Horizontally or Vertically\n");
    printf("4. The game is played in turns. The player and the enemy take turns attacking each other's grid.\n");
    printf("5. The game ends when all of the ships of one player are sunk.\n");
    printf("6. The player who sinks all of the enemy's ships wins the game.\n");
    printf("7. If you would like to play again, you can do so after the game ends.\n");
    printf("8. Whenever you are ready, please enter 'Y' to agree to the rules and start the game or 'N'.\n");
    getValidInput("Do you agree to the game rules? (Y/N): ", playerAgreement, isValidAgreement);
}

/*-----------------------------------Rule Agreement Validation Function-----------------------------------*/
/*
Purpose: Check if the agreement entered by the player is valid. The agreement must be either 'Y' or 'N'.
Parameters: const char* playerAgreement - a pointer to a character containing the player's agreement.
Return: 1 if the agreement is valid, 0 if the agreement is invalid.
Side Effects: const char* playerAgreement - returns the users choice (Y/y or N/n)
*/
int isValidAgreement(const char* playerAgreement) { 
    return (*playerAgreement == 'Y' || *playerAgreement == 'y' || *playerAgreement == 'N' || *playerAgreement == 'n');
}

/*-----------------------------------Initialization Function-----------------------------------*/
/*
Purpose: Initialize the game. The function will set all the grids to empty cells and reset the game statistics.
Parameters: None
Return: None
Side Effects: 
Modifies the global variables playerGrid, enemyGrid, playerViewOfEnemyGrid, playerHits, playerMisses, playerSunkShips, 
enemyHits, enemyMisses, enemySunkShips, lastHitXCoordinate, and lastHitYCoordinate via initialization.
*/
void initializeGame() {
    memset(playerGrid, EMPTY_CELL, sizeof(playerGrid)); // Initialize player grid
    memset(enemyGrid, EMPTY_CELL, sizeof(enemyGrid)); // Initialize enemy grid
    memset(playerViewOfEnemyGrid, EMPTY_CELL, sizeof(playerViewOfEnemyGrid)); // Initialize player's view of enemy grid
    playerHits = playerMisses = playerSunkShips = 0;
    enemyHits = enemyMisses = enemySunkShips = 0;
    int lastHitXCoordinate,lastHitYCoordinate = GRID_SIZE; // Set to GRID_SIZE because it is out of valid range
}

/*-----------------------------------Helper Functions-----------------------------------*/
/*
Purpose: Validate user input for coordinates.
Parameters: const char* prompt - the prompt message to display.
Return: The valid coordinate (0-based index).
Side Effects: const char* prompt - prints the prompt message to the user and modifies the state of the variable based on the users input.
*/
int getValidCoordinate(const char* prompt) {
    int coordinate = 0; // the coordinates for the player, initalized to 0
    do {
        printf("%s", prompt);
        if (scanf("%d", &coordinate) != 1 || coordinate < 1 || coordinate > 10) {
            printf("\nInvalid input. Please enter a number between 1 and 10: ");
            ClearBuffer();
        } else {
            break;
        }
    } while (coordinate < 1 || coordinate > 10); 
    return coordinate - 1; // Adjust for 0-based index
}

/*
Purpose: Handle the confirmation of ship placement by the player.
Parameters: const char* shipName - the name of the ship.
            int x - the x coordinate.
            int y - the y coordinate.
            char orientation - the orientation of the ship.
Return: 1 if the placement is confirmed (either Y/N), 0 if the placement is not confirmed (should not happen under normal circumstances).
Side Effects: const char* shipName, int x, int y, char orientation - prints the confirmation message and modifies the state of the variable based on the users input.
*/
int confirmPlacement(const char* shipName, int x, int y, char orientation) {
    char confirm;
    do {
        printf("\nDo you want to place the %s at (%d, %d) with orientation %c? (Y/N): ", shipName, x + 1, y + 1, orientation);
        if (scanf(" %c", &confirm) != 1) {
            printf("Error reading confirmation. Please try again.\n");
        }
        ClearBuffer();
    } while (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n');
    return (confirm == 'Y' || confirm == 'y');
}

/*
Purpose: Display the board of the game.
Parameters: char grid[GRID_SIZE][GRID_SIZE] - the grid to be printed.
            const char* title - the title of the board.
Return: None
Side Effects: char grid[GRID_SIZE][GRID_SIZE], const char* title - prints the board of the game and modifies the state of the variable based on the users input.
*/
void displayBoard(char grid[GRID_SIZE][GRID_SIZE], const char* title) {
    printBoard(grid, title);
}

/*-----------------------------------Place Player Ships-----------------------------------*/
/*
Purpose: Place the player's ships on the grid. The player will be prompted to enter the x and y coordinates to place the ships.
Parameters: None
Return: None
Side Effects: Modifies the global variable playerGrid.
*/
void placePlayerShips() {
    int shipLengths[] = {5, 4, 3, 2}; // Carrier, Battleship, Submarine, Patrol Boat
    char shipNames[][12] = {"Carrier", "Battleship", "Submarine", "Patrol Boat"};
    char shipOrientation = 0;
    int xCoordinate = 0;
    int yCoordinate = 0;

    for (int shipIndex = 0; shipIndex < 4; shipIndex++) { // For each ship, place it on the grid
        int validPlacement = 0;
        do /* I care about this? no, but I sure as hell feel like it needs to be done */ {
            printf("\nPlace your %s (length %d):\n", shipNames[shipIndex], shipLengths[shipIndex]);
            xCoordinate = getValidCoordinate("Enter starting X coordinate (1-10): ");
            yCoordinate = getValidCoordinate("Enter starting Y coordinate (1-10): ");

            printf("\nEnter orientation (H for horizontal, V for vertical): ");
            if (scanf(" %c", &shipOrientation) != 1 || (shipOrientation != 'H' && shipOrientation != 'V')) {
                printf("\nInvalid orientation '%c'. Please enter 'H' for horizontal or 'V' for vertical.\n", shipOrientation);
                ClearBuffer();
                continue;
            }
            ClearBuffer();

            if (isPlacementValid(xCoordinate, yCoordinate, shipLengths[shipIndex], shipOrientation, playerGrid)) {
                if (confirmPlacement(shipNames[shipIndex], xCoordinate, yCoordinate, shipOrientation)) {
                    placeShip(xCoordinate, yCoordinate, shipLengths[shipIndex], shipOrientation, playerGrid);
                    if (DEBUG) {
                        printf("\nPlaced %s at (%d, %d) with orientation %c\n", shipNames[shipIndex], xCoordinate + 1, yCoordinate + 1, shipOrientation);
                    }
                    validPlacement = 1;
                } else {
                    printf("\nRe-enter coordinates for %s...\n", shipNames[shipIndex]);
                }
            } else {
                printf("\nI'm sorry, but your placement of '%d, %d' is invalid. Please try again.\n", xCoordinate + 1, yCoordinate + 1);
            }
        } while (!validPlacement);
    }
}

/*-----------------------------------Place Enemy Ships-----------------------------------*/
/*
Purpose: Place the enemy ships on the grid. The enemy ships are placed randomly on the grid.
Parameters: None
Return: None
Side Effects: Modifies the global variable enemyGrid.
*/
void placeEnemyShips() {
    int shipLengths[] = {5, 4, 3, 2}; // Carrier, Battleship, Submarine, Patrol Boat
    char orientations[] = {'H', 'V'};
    int xCoordinate, yCoordinate, shipOrientation;
    char shipNames[][12] = {"Carrier", "Battleship", "Submarine", "Patrol Boat"};

    for (int shipIndex = 0; shipIndex < 4; shipIndex++) {
        int shipPlaced = 0;
        do {
            shipOrientation = rand() % 2; // Randomly select orientation
            xCoordinate = rand() % GRID_SIZE; // Randomly select x and y coordinates
            yCoordinate = rand() % GRID_SIZE;

            if (DEBUG) { // Debug flag to print the enemy orientation for testing purposes
                printf("\nEnemy orientation for %s: %c\n", shipNames[shipIndex], orientations[shipOrientation]);
            }

            if (isPlacementValid(xCoordinate, yCoordinate, shipLengths[shipIndex], orientations[shipOrientation], enemyGrid)) {
                placeShip(xCoordinate, yCoordinate, shipLengths[shipIndex], orientations[shipOrientation], enemyGrid);
                shipPlaced = 1; // Set placed to 1 to break the loop
                if (DEBUG) { // Debug flag to print the enemy placement for testing purposes
                    printf("\nPlaced enemy %s at (%d, %d) with orientation %c\n", shipNames[shipIndex], xCoordinate + 1, yCoordinate + 1, orientations[shipOrientation]);
                }
            }
        } while (!shipPlaced);
    }
    // Debug flag to print the enemy board for testing purposes
    if (DEBUG) { 
        printf("\nEnemy Board:\n");
        printBoard(enemyGrid, "Enemy Board");
    }
}

/*-----------------------------------Game Loop-----------------------------------*/
/* 
Purpose: Run the game loop. The game loop will continue until one of the players wins the game.
Parameters: None
Return: None
Side Effects: Calls playerTurn and enemyTurn functions, updates game state, and prints game status.
*/
void gameLoop() { 
    int gameWon = 0;
    while (gameWon == 0) {
        playerTurn();
        HitandSunkShips(enemyGrid, &enemySunkShips);
        if (enemySunkShips == 4) {
            printf("\nCongratulations! You won!\n");
            gameWon = 1;
            break;
        }
        enemyTurn();
        HitandSunkShips(playerGrid, &playerSunkShips);
        if (playerSunkShips == 4) {
            printf("\nThe enemy has won. Better luck next time!\n");
            gameWon = 1;
            break;
        }
    }
}

/*-----------------------------------Print Board Function-----------------------------------*/
/*
Purpose: Print the board of the game. The board is printed in a 10x10 grid format.
Parameters: char grid[GRID_SIZE][GRID_SIZE] - the grid to be printed.
            const char* boardTitle - the title of the board.
Return: None
Side Effects: None
*/
void printBoard(char grid[GRID_SIZE][GRID_SIZE], const char* boardTitle) {
    printf("\n%s\n", boardTitle); // Print the title of the board
    printf("    "); // Print the column numbers
    for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { // for each column
        printf("%2d ", colIndex + 1); // Print the column numbers
    }
    printf("\n"); // Print a newline
    printf("   ="); // Print the top border
    for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { // for each column
        printf("==="); // Print the top border
    }
    printf("=\n"); // Print a newline

    for (int rowIndex = 0; rowIndex < GRID_SIZE; rowIndex++) { // for each row
        printf("%2d |", rowIndex + 1); // Print the row number
        for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { // for each column
            printf("%2c ", grid[rowIndex][colIndex]); // Print the cell value
        }
        printf("|\n"); // Print a newline
    }
    printf("   ="); // Print the bottom border
    for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { // for each column
        printf("==="); // Print the bottom border
    }
    printf("=\n"); // Print a newline
    printf("X-axis is vertical, Y-axis is horizontal along the top.\n"); // Label the axes
}

/*-----------------------------------Print Enemy Board Function-----------------------------------*/
/*
Purpose: Print the enemy's board of the game. The board is printed in a 10x10 grid format.
Parameters: char grid[GRID_SIZE][GRID_SIZE] - the grid to be printed.
            const char* boardTitle - the title of the board.
Return: None
Side Effects: None
*/
void printEnemyBoard(char grid[GRID_SIZE][GRID_SIZE], const char* boardTitle) {
    printf("\n%s\n", boardTitle); // Print the title of the board
    printf("    ");
    for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { //   
        printf("%2d ", colIndex + 1); // Print the column numbers
    }
    printf("\n");
    printf("   =");
    for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { // for each column
        printf("==="); // Print the top border
    }
    printf("=\n");

    for (int rowIndex = 0; rowIndex < GRID_SIZE; rowIndex++) { // for each row
        printf("%2d |", rowIndex + 1); // Print the row number
        for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { // for each column
            if (grid[rowIndex][colIndex] == HIT_CELL || grid[rowIndex][colIndex] == MISS_CELL) { // if the cell is a hit or a miss,
                printf("%2c ", grid[rowIndex][colIndex]);                                        // print the cell value
            } else if (grid[rowIndex][colIndex] == 10) {                                         //else if the cell is 10,
                printf("%2c ", MISS_CELL);                                                       // print the miss cell
            } else {                                                                             // else print the empty cell
                printf("%2c ", EMPTY_CELL);
            }
        }
        printf("|\n"); // Print a newline
    }
    printf("   ="); // Print the bottom border
    for (int colIndex = 0; colIndex < GRID_SIZE; colIndex++) { // for each column,
        printf("===");                                         // print the bottom border
    }   
    printf("=\n"); // Print a newline
}

/*-----------------------------------Player Turn function-----------------------------------*/
/* 
Purpose: Allow the player to take their turn in the game. The player will be prompted to enter the x and y coordinates to attack.
Parameters: None
Return: None
Side Effects: Modifies the global variables playerHits, playerMisses, enemyGrid, and playerViewOfEnemyGrid.
*/
void playerTurn() {
    int xCoordinate = 0;
    int yCoordinate = 0;
    printf("\nYour turn.\n");
    xCoordinate = getValidCoordinate("Enter X coordinate to attack (1-10): ");
    yCoordinate = getValidCoordinate("Enter Y coordinate to attack (1-10): ");

    while (!isValidCoordinate(xCoordinate, yCoordinate)) {
        printf("Invalid coordinates (%d, %d). Enter X coordinate to attack (1-10): ", xCoordinate + 1, yCoordinate + 1);
        xCoordinate = getValidCoordinate("Enter X coordinate to attack (1-10): ");
        yCoordinate = getValidCoordinate("Enter Y coordinate to attack (1-10): ");
    }

    if (playerViewOfEnemyGrid[xCoordinate][yCoordinate] == HIT_CELL || playerViewOfEnemyGrid[xCoordinate][yCoordinate] == MISS_CELL) {
        printf("You have already guessed these coordinates. Skipping your turn.\n");
        return;
    }

    if (DEBUG) {
        printf("Player attacking cell: (%d, %d)\n", xCoordinate, yCoordinate);
    }

    if (enemyGrid[xCoordinate][yCoordinate] >= '2' && enemyGrid[xCoordinate][yCoordinate] <= '5') {
        int shipLength = enemyGrid[xCoordinate][yCoordinate] - '0';
        char *shipNames[] = {"Patrol Boat", "Submarine", "Battleship", "Carrier"};
        printf("HIT! You hit a %s!\n", shipNames[shipLength - 2]);
        playerHits++;
        enemyGrid[xCoordinate][yCoordinate] += 10; // Mark as hit
        playerViewOfEnemyGrid[xCoordinate][yCoordinate] = HIT_CELL; // Update player's view of enemy grid
        if (DEBUG) { // note: add 10 here
            printf("Marked enemy grid at (%d, %d) as HIT_CELL\n", xCoordinate, yCoordinate);
        }
        HitandSunkShips(enemyGrid, &enemySunkShips); // Pass the address of enemySunkShips to update it correctly
    } else {
        printf("MISS!\n");
        playerMisses++;
        enemyGrid[xCoordinate][yCoordinate] = MISS_CELL; // Mark as miss
        playerViewOfEnemyGrid[xCoordinate][yCoordinate] = MISS_CELL; // Update player's view of enemy grid
        if (DEBUG) {
            printf("Marked enemy grid at (%d, %d) as MISS_CELL\n", xCoordinate, yCoordinate);
        }
    }

    displayBoard(playerGrid, "Your Board");
    displayBoard(playerViewOfEnemyGrid, "Enemy Board");

    // Debug print to check the state after player's turn
    if (DEBUG) {
        printf("Player turn completed. playerHits: %d, playerMisses: %d\n", playerHits, playerMisses);
    }
}

/*-----------------------------------Enemy Turn function-----------------------------------*/
/*
Purpose: Allow the enemy to take its turn in the game. The enemy will randomly select coordinates to attack.
Parameters: None
Return: None
Side Effects: Modifies the global variables enemyHits, enemyMisses, playerGrid, lastHitXCoordinate, and lastHitYCoordinate.
*/
void enemyTurn() {
    int xCoordinate = 0;
    int yCoordinate = 0;
    int placementAttempts = 0;
    const int maxPlacementAttempts = 100; // Limit the number of attempts to avoid infinite loop

    if (lastHitXCoordinate != GRID_SIZE && lastHitYCoordinate != GRID_SIZE) {
        // Try to guess around the last hit in a systematic manner
        if (isValidCoordinate(lastHitXCoordinate + 1, lastHitYCoordinate) && playerGrid[lastHitXCoordinate + 1][lastHitYCoordinate] == SHIP_CELL) {
            xCoordinate = lastHitXCoordinate + 1;
            yCoordinate = lastHitYCoordinate;
        } else if (isValidCoordinate(lastHitXCoordinate - 1, lastHitYCoordinate) && playerGrid[lastHitXCoordinate - 1][lastHitYCoordinate] == SHIP_CELL) {
            xCoordinate = lastHitXCoordinate - 1;
            yCoordinate = lastHitYCoordinate;
        } else if (isValidCoordinate(lastHitXCoordinate, lastHitYCoordinate + 1) && playerGrid[lastHitXCoordinate][lastHitYCoordinate + 1] == SHIP_CELL) {
            xCoordinate = lastHitXCoordinate;
            yCoordinate = lastHitYCoordinate + 1;
        } else if (isValidCoordinate(lastHitXCoordinate, lastHitYCoordinate - 1) && playerGrid[lastHitXCoordinate][lastHitYCoordinate - 1] == SHIP_CELL) {
            xCoordinate = lastHitXCoordinate;
            yCoordinate = lastHitYCoordinate - 1;
        } else {
            // If all surrounding cells are already guessed, reset lastHitX and lastHitY
            lastHitXCoordinate = GRID_SIZE; // Reset last hit coordinates
            lastHitYCoordinate = GRID_SIZE; // Reset last hit coordinates
        } 
    }

    if (lastHitXCoordinate == GRID_SIZE && lastHitYCoordinate == GRID_SIZE) { // If the last hit coordinates are out of range, pick a random cell
        while (placementAttempts < maxPlacementAttempts) { // while the placement attempts are less than the max placement attempts, pick a random cell on the grid
            xCoordinate = rand() % GRID_SIZE; // picks x and y coordinates randomly
            yCoordinate = rand() % GRID_SIZE;
            if (playerGrid[xCoordinate][yCoordinate] != HIT_CELL && playerGrid[xCoordinate][yCoordinate] != MISS_CELL) { 
                break;
            }
            placementAttempts++; // increment the placement attempts
        }
    }

    if (placementAttempts >= maxPlacementAttempts) { // If the placement attempts are greater than or equal to the max placement attempts, skip the turn
        printf("Enemy failed to find a valid cell after %d attempts. Skipping turn.\n", maxPlacementAttempts);
        return; // this should not happen under normal circumstances
    }

    // debug flag to print the enemy guess
    if (DEBUG) {
        printf("Enemy guess: (%d, %d)\n", xCoordinate + 1, yCoordinate + 1);
    }

    if (playerGrid[xCoordinate][yCoordinate] >= '2' && playerGrid[xCoordinate][yCoordinate] <= '5') { // if the player grid at the x and y coordinates is greater than or equal to 2 and less than or equal to 5,
        int shipLength = playerGrid[xCoordinate][yCoordinate] - '0';                                  // get the ship length
        char *shipNames[] = {"Patrol Boat", "Submarine", "Battleship", "Carrier"};    // get the ship names
        printf("Enemy HIT! They hit your %s!\n", shipNames[shipLength - 2]); // print the hit message
        enemyHits++; // increment the enemy hits
        playerGrid[xCoordinate][yCoordinate] += 10; // mark the player grid at the x and y coordinates as HIT_CELL
        playerGrid[xCoordinate][yCoordinate] = HIT_CELL; // externally show the hit as 'X'
        lastHitXCoordinate = xCoordinate; // set the last hit x coordinate
        lastHitYCoordinate = yCoordinate; // set the last hit y coordinate
        // debug flag to print the marked player grid
        if (DEBUG) {
            printf("Marked player grid at (%d, %d) as HIT_CELL\n", xCoordinate, yCoordinate);
        }
        HitandSunkShips(playerGrid, &playerSunkShips);
    } else {
        printf("Enemy MISS!\n"); // print the miss message
        enemyMisses++; // increment the enemy misses
        playerGrid[xCoordinate][yCoordinate] = MISS_CELL; // mark the player grid at the x and y coordinates as MISS_CELL
        lastHitXCoordinate = GRID_SIZE; // Reset last hit coordinates
        lastHitYCoordinate = GRID_SIZE; // Reset last hit coordinates
        // debug flag to print the marked player grid
        if (DEBUG) {
            printf("Marked player grid at (%d, %d) as MISS_CELL\n", xCoordinate, yCoordinate);
        }
    }
    // debug flag to print the player and enemy boards
    if (DEBUG) {
        printf("Enemy turn state: enemyHits: %d, enemyMisses: %d\n", enemyHits, enemyMisses);
    }
    // debug flag to print the player and enemy boards
    printBoard(playerGrid, "Your Board");
    if (DEBUG) {
        printBoard(enemyGrid, "Enemy Board");
    }
    // debug flag to print the player and enemy boards
    if (DEBUG) {
        printf("Enemy turn completed. enemyHits: %d, enemyMisses: %d\n", enemyHits, enemyMisses);
    }
}

/*-----------------------------------Game Statistics-----------------------------------*/
/*
Purpose: Display the game statistics. The function will display the number of ships hit, missed, and sunk by the player and the enemy.
Parameters: None
Return: None
Side Effects: None
*/
void displayStatistics() {
    printf("\n=====================================\n");
    printf("           Game Statistics           \n");
    printf("=====================================\n");
    printf("Number of ships hit: %d\n", playerHits);
    printf("Number of ships missed: %d\n", playerMisses);
    printf("Number of ships sunk: %d\n", playerSunkShips);
    printf("Number of ships sunk by the enemy: %d\n", enemySunkShips);
    printf("=====================================\n");
}

/*-----------------------------------Play Again function-----------------------------------*/
/*
Purpose: Prompt the user if they would like to play again. The user will be prompted to enter 'Y' or 'N'.
Parameters: char* playAgainChoice - a pointer to a character to store the user's choice.
Return: None
Side Effects: char* playAgainChoice - modifies the state of the variable based on the users input.
*/
void playAgain(char* playAgainChoice) {
    getValidInput("Do you want to play again? (Y/N): ", playAgainChoice, isValidAgreement);
}

/*-----------------------------------Coordinate Validation Function-----------------------------------*/
/*
Purpose: Check if the given coordinates are valid within the grid.
Parameters: int xCoordinate - the x coordinate to check.
            int yCoordinate - the y coordinate to check.
Return: 1 if the coordinates are valid, 0 if the coordinates are invalid.
Side Effects: xCoordinate, yCoordinate - returns the state of the variables (see above)
*/
int isValidCoordinate(int xCoordinate, int yCoordinate) {
    return (xCoordinate >= 0 && xCoordinate < GRID_SIZE && yCoordinate >= 0 && yCoordinate < GRID_SIZE);
}

/*-----------------------------------Placement Validation Function-----------------------------------*/
/*
Purpose: Check if the placement of a ship is valid on the grid.
Parameters: int xCoordinate - the x coordinate to start the placement.
            int yCoordinate - the y coordinate to start the placement.
            int shipLength - the length of the ship.
            char shipOrientation - the orientation of the ship ('H' for horizontal, 'V' for vertical).
            char grid[GRID_SIZE][GRID_SIZE] - the grid to check the placement on.
Return: 1 if the placement is valid, 0 if the placement is invalid.
Side Effects: xCoordinate, yCoordinate, shipLength, shipOrientation, char grid[GRID_SIZE][GRID_SIZE] - returns the state of the variables (see above)
*/
int isPlacementValid(int xCoordinate, int yCoordinate, int shipLength, char shipOrientation, char grid[GRID_SIZE][GRID_SIZE]) {
    if (shipOrientation == 'H') { // if the ship orientation is horizontal, check if the ship fits within the grid and if the cells are empty
        if (yCoordinate + shipLength > GRID_SIZE) return 0;
        for (int index = 0; index < shipLength; index++) { // for each cell of the ship, if the cell is not empty, return 0 to indicate invalid placement
            if (grid[xCoordinate][yCoordinate + index] != EMPTY_CELL) return 0;
        }
    } else if (shipOrientation == 'V') { // else if the ship orientation is vertical, check if the ship fits within the grid and if the cells are empty
        if (xCoordinate + shipLength > GRID_SIZE) return 0;
        for (int index = 0; index < shipLength; index++) { // for each cell of the ship, if the cell is not empty, return 0 to indicate invalid placement
            if (grid[xCoordinate + index][yCoordinate] != EMPTY_CELL) {
                return 0;
            }
        }
    } else { // else return 0 to indicate invalid orientation
        printf("Invalid orientation. Please enter 'H' for horizontal or 'V' for vertical.\n");
        return 0;
    }
    return 1; // return 1 to indicate valid placement
}

/*-----------------------------------Place Ship function-----------------------------------*/
/*
Purpose: Place the ship on the grid. The ship will be placed based on the x and y coordinates, length, and orientation.
Parameters: int xCoordinate - the x coordinate to start the placement.
            int yCoordinate - the y coordinate to start the placement.
            int shipLength - the length of the ship.
            char shipOrientation - the orientation of the ship ('H' for horizontal, 'V' for vertical).
            char grid[GRID_SIZE][GRID_SIZE] - the grid to place the ship on.
Return: None
Side Effects: Modifies the grid parameters (grid[GRID_SIZE][GRID_SIZE], xCoordinate, yCoordinate, shipLength, shipOrientation).
*/
void placeShip(int xCoordinate, int yCoordinate, int shipLength, char shipOrientation, char grid[GRID_SIZE][GRID_SIZE]) {
    if (shipOrientation == 'H') { // if the ship orientation is horizontal, place the ship on the grid
        for (int index = 0; index < shipLength; index++) { // for each cell of the ship, place the ship on the grid
            grid[xCoordinate][yCoordinate + index] = '0' + shipLength; // Use the length as the cell value
        }
    } else if (shipOrientation == 'V') { // else if the ship orientation is vertical, place the ship on the grid
        for (int index = 0; index < shipLength; index++) { // for each cell of the ship, place the ship on the grid
            grid[xCoordinate + index][yCoordinate] = '0' + shipLength; // Use the length as the cell value
        }
    }
}

/*-----------------------------------Check Sunk Ships function-----------------------------------*/
/*
Purpose: To count ship hits and to check if the ships are sunk. If all cells of a ship are hit, the ship is sunk.
Parameters: char grid[GRID_SIZE][GRID_SIZE] - the grid to check for sunk ships.
            int sunkShipCount - a pointer to an integer to store the count of sunk ships.
Return: None
Side Effects: Modifies the grid parameter and the sunkShipCount parameter.
*/
void HitandSunkShips(char grid[GRID_SIZE][GRID_SIZE], int* sunkShipCount) { 
    int shipLengths[] = {5, 4, 3, 2}; // Carrier, Battleship, Submarine, Patrol Boat
    char* shipNames[] = {"Carrier", "Battleship", "Submarine", "Patrol Boat"}; // note: changed to array of strings
    int sunkCount = 0; // Initialize the sunk ship count to 0

    for (int shipIndex = 0; shipIndex < 4; shipIndex++) { // for each ship, check if the ship is sunk
        int length = shipLengths[shipIndex]; // Get the length of the ship
        int hitCount = 0; // Initialize the hit count to 0
        
        // Count how many cells of this ship have been hit 
        for (int row = 0; row < GRID_SIZE; row++) {     // for each row,
            for (int col = 0; col < GRID_SIZE; col++) { // for each column,
                if (grid[row][col] == ('0' + length + 10)) {  // if the cell is a hit,
                    hitCount++;                         // then increment the hit count
                }
            }
        }

        // If we've hit all cells of this ship's length, it's sunk
        if (hitCount == length) {
            sunkCount++;
            printf("You sunk a %s!\n", shipNames[shipIndex]);
        }
    }

    *sunkShipCount = sunkCount; // Update the sunk ship count
}

/*-----------------------------------Main Function-----------------------------------*/
/*
Purpose: The main function of the program. The main function will call all the other functions to run the game.
Parameters: None
Returns: 0 (indicates that the program ran successfully and ends).
Side Effects: None
*/
int main() { 
    // Variable declarations
/*--------------------------------------------*/
    // Char/character variables
    char playerName[MAX_NAME_LENGTH] = {0}; 
    char playerAgreement = 'N';
    char playAgainChoice = 'N';
    /*--------------------------------------------*/
    srand(time(NULL)); // Moved here to be called only once
    /*--------------------------------------------*/
    void printMessage(); // Calls the printMessage function to print the welcome message
    /*--------------------------------------------*/
    getPlayerName(playerName); // Calls the getPlayerName function to get the player's name
    /*--------------------------------------------*/
    getAgreement(&playerAgreement); // Calls the getAgreement function to get the user's agreement to the rules
    /*--------------------------------------------*/
    if (playerAgreement == 'Y' || playerAgreement == 'y') { // If the user puts 'Y' for agreement, the game will start
        do {
            initializeGame(); // Calls the initializeGame function to initialize the game
            placePlayerShips(); // Calls the placePlayerShips function to place the player's ships
            placeEnemyShips(); // Calls the placeEnemyShips function to place the enemy's ships
            printBoard(playerGrid, "Your Board"); // Calls the printBoard function to print the player's board
            gameLoop(); // Calls the gameLoop function to start the game
            displayStatistics(); // Calls the displayStatistics function to display the game statistics
            playAgain(&playAgainChoice); // Calls the PlayAgain function to prompt the user if they would like to play again
        } while (playAgainChoice == 'Y' || playAgainChoice == 'y');
    }
    /*--------------------------------------------*/
    // End of the game/closing statement
    printf("Thank you for playing!\n");
    return 0; // Returns 0 to indicate that the program ran successfully
}