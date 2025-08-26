// Global Constant Variables
#define GRID_SIZE 10 // Variable to store the integer value of the grid size
#define MAX_NAME_LENGTH 32 // Variable to store the integer value of the maximum name length
#define EMPTY_CELL '.' // Variable to store the character value of the empty cell
#define HIT_CELL 'X' // Variable to store the character value of the hit cell
#define MISS_CELL 'O' // Variable to store the character value of the miss cell
#define SHIP_CELL 'S' // Dedicated constant for ship cells
#define DEBUG 0 // Set to 1 to enable the debug flags within my program 


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