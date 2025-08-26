/*-----------------------------------Variable Declarations-----------------------------------*/


// Function declarations (declared in advance since the calling loops cannot be in order)
int isValidAgreement(const char* playerAgreement);
void playerTurn();
int isValidName(const char* playerName);
void enemyTurn();
int isValidCoordinate(int xCoordinate, int yCoordinate);
int isPlacementValid(int xCoordinate, int yCoordinate, int shipLength, char shipOrientation, char grid[GRID_SIZE][GRID_SIZE]);
void placeShip(int xCoordinate, int yCoordinate, int shipLength, char shipOrientation, char grid[GRID_SIZE][GRID_SIZE]);
void printBoard(char grid[GRID_SIZE][GRID_SIZE], const char* boardTitle);
void hitAndSunkShips(char grid[GRID_SIZE][GRID_SIZE], int* sunkShipCount);


