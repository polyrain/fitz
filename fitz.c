#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TILE_HEIGHT 5
#define TILE_WIDTH 5
#define ROTATION_COUNT 4
#define INVALID_ARGS 1
#define INVALID_TILEFILE 2
#define INVALID_TILE_CONTENTS 3
#define INVALID_PLAYER 4
#define INVALID_BOARD_PARAM 5
#define PLAYER_TYPES "h12"
#define INVALID_SAVE_FILE 6
#define INVALID_SAVE_CONTENT 7
#define END_OF_INPUT 10
#define ROTATION_STEP 90 
#define TILE_FILE 't'
#define SAVE_FILE 's'
#define MAX_INPUT 70
#define TILE_CENTRE 2
#define MAX_ANGLE 270
#define MAX_WIDTH 999
#define MAX_HEIGHT 999

/*
 * Struct Datatype used to hold a 2D array containing the chars which make
 * up a singular tile. 
 */
typedef struct Tile { 
    char tileData[TILE_HEIGHT][TILE_WIDTH]; 
} Tile;

/*
 * Struct Datatype used throughout program to indicate current program status
 * using a singular int as the exit status
 */
typedef struct DataReadFlags { 
    int returnVal;
} DataReadFlag;

/*
 * Struct Datatype used to store the current fitz game information for saving;
 * This includes:
 *      -  Number of next tile to be played from tilefile (>= 0) 
 *      -  Next player to have their turn (0, 1)
 *      -  Number of rows in the current fitz game
 *      -  Number of columns in the current fitz game
 *      -  2D array copy of the current state of fitz game board
 *
 */
typedef struct GameState { 
    int currentTile;  
    int currentPlayer;
    int gridHeight;
    int gridWidth;
    char** grid;
} GameState;

/*
 * Struct Datatype used to store player information for gameplay.
 * This includes:
 *      -  player type; either 'h', '1', or '2' 
 *      -  player icon to be displayed on board
 *      -  player number; either 1 or 2
 *      -  last row played by this player
 *      -  last column played by this player
 */
typedef struct Player {
    char type;
    char icon;
    int playerNum;
    int lastRow;
    int lastCol;
} Player;


Tile** load_tiles(FILE** tileFile, DataReadFlag* tileFileFlag, char* tileName,
        int* numTiles);

void print_rotations(Tile** tiles, int numTiles);

Tile rotate_tile(Tile* tileStart, int numRotations);

void print_tile_rotations(int numTiles, Tile tiles[numTiles][ROTATION_COUNT]);

void create_new_grid(int height, int width, char*** grid);

void print_grid(int height, int width, char** grid);

int check_load_errors(DataReadFlag statusObj);

int check_point(int c, DataReadFlag* loadFlag);


Player* create_player(char* type, Player* player, DataReadFlag* playerFlag, 
        int playerNum); 

void check_parameters(char* heightRaw, char* widthRaw, int* height, 
        int* width, DataReadFlag* boardFlag);

FILE* open_file(char* tileName, DataReadFlag* loadFlag, char fileType);

void check_tile_contents(DataReadFlag* loadFlag, int pos, int col, int row);

void load_game(char* saveFileName, char*** grid, int* gameData, 
        DataReadFlag* saveFlag, FILE** saveFile, int* numTiles, int* height,
        int* width);

int check_tile_end(FILE** tileFile);


int check_row_end(FILE** tileFile);


char* get_params(FILE** saveFile, DataReadFlag* saveFlag); 

void check_save_params(long paramVals[4], int* numTiles, 
        DataReadFlag* saveFlag);

void set_invalid_save(DataReadFlag* saveFlag);


int check_grid_point(int c);

void load_grid(char*** grid, int height, int width, DataReadFlag* saveFlag,
        FILE** saveFile);

void main_game_loop(char*** grid, Tile** tiles, int* gameData, int* height,
        int* width, int* numTiles, Player* playerOne, Player* playerTwo,
        DataReadFlag* fitzFlag);

void make_move(Tile* tile, Player* player, char*** grid, int height, 
        int width, DataReadFlag* gameFlag, int* lastRow, int* lastCol, 
        GameState* currentGameData);

void print_tile(Tile* tile);


int check_user_input(int* row, int* col, DataReadFlag* readFlag, 
        int* rotateAngle, GameState* currentGameState);

int read_stdin(char** userInput, DataReadFlag* readFlag, 
        GameState* currentGameState); 

int validate_inputs(int* row, int* col, int* rotateAngle, char** userInput);

void clear_stdin(void);

int clean_inputs(char** userInput);

int attempt_place(int row, int col, Tile tile, char*** grid, int height,
        int width, Player* player);

int check_game_over(char*** grid, int height, int width, Tile* tile);

void free_grid(char*** grid, int height);

int auto_play_one(Player* player, int rStart, int cStart, int height, 
        int width, Tile* tile, char*** grid, int* row, int* col);

void game_over(Player* player);

void allocate_start_coords(Player* player, int height, int width);

int auto_play_two(Player* player, int height, int width, char*** grid, 
        Tile* tile);

void auto_two_move(Player* player, int height, int width, 
        int* currentRow, int* currentCol);

void print_auto_move(int currentRow, int currentCol, int currentAngle, 
        Player* player);

void collect_game_data(GameState* currentGameData, int tileIndex, 
        int playerNum, int height, int width, char*** grid);

void attempt_save(GameState* currentGameState, char** userInput); 

void update_last_play(int* lastRow, int* lastCol, int row, int col);

int main(int argc, char** argv) {

    FILE* tileFile = NULL; 
    Tile** tiles;
    char** grid;
    int* gameData = (int*) calloc(2, sizeof(int)); //Holder for next tile/move
    int width = 0, height = 0, numTiles = 0;
    Player* playerOne = NULL, *playerTwo = NULL; 
    DataReadFlag fitzFlag = {0};

    if (argc >= 2 && argc < 7 && argc != 3 && argc != 4) { //Valid # args
        tiles = load_tiles(&tileFile, &fitzFlag, argv[1], &numTiles);
    }

    if (argc == 5 || argc == 6) { //Arg values that require players
        playerOne = create_player(argv[2], playerOne, &fitzFlag, 1);
        playerTwo = create_player(argv[3], playerTwo, &fitzFlag, 2);
    }

    switch (argc) { 
        case 2: 
            print_rotations(tiles, numTiles);
            return 0;
        case 5:
            load_game(argv[4], &grid, gameData, &fitzFlag, &tileFile, 
                    &numTiles, &height, &width);
            break;
        case 6:
            check_parameters(argv[4], argv[5], &height, &width, &fitzFlag);
            create_new_grid(height, width, &grid);
            break;

        default:
            fitzFlag.returnVal = INVALID_ARGS;
            check_load_errors(fitzFlag); //Will exit the program
    }

    main_game_loop(&grid, tiles, gameData, &height, &width, &numTiles,
            playerOne, playerTwo, &fitzFlag);
    return 0;
}

/*
 * Takes a pointer to a player struct, the dimensions of the current game 
 * board, and assigns initial "last play" values dependent on player type as 
 * per the specification for automatic players. Returns nothing, no error 
 * conditions.
 */
void allocate_start_coords(Player* player, int height, int width) {
    if (player->type == '2') {
        if (player->playerNum == 1) { //First player starts in
            player->lastRow = -2; //Top corner
            player->lastCol = -2;
        } else { //Second player (type 2) starts in bottom right
            player->lastRow = height + 2;
            player->lastCol = width + 2;
        }
    }
}

/* 
 * Main game loop for fitz; takes a game board, a set of tiles, current game 
 * data, the dimensions of the game board (height and width), the number of
 * tiles inside the set of tiles, both players, and a status flag struct. 
 * Begins playing a game of fitz using this data.
 */
void main_game_loop(char*** grid, Tile** tiles, int* gameData, int* height,
        int* width, int* numTiles, Player* playerOne, Player* playerTwo,
        DataReadFlag* fitzFlag) {
    
    int lastRow = -2; //Default global positions for player type 1;
    int lastCol = -2;
    allocate_start_coords(playerOne, *height, *width);
    allocate_start_coords(playerTwo, *height, *width);
    GameState currentGameData;   

    while (1) {
        collect_game_data(&currentGameData, gameData[0], gameData[1], *height, 
                *width, grid);

        if (gameData[1] == 0) { //Player one
            make_move(tiles[gameData[0]], playerOne, grid, *height, *width,
                    fitzFlag, &lastRow, &lastCol, &currentGameData);

        } else {
            make_move(tiles[gameData[0]], playerTwo, grid, *height, *width,
                    fitzFlag, &lastRow, &lastCol, &currentGameData);
        }
    
        gameData[1] = !gameData[1]; //Change our player 

        if (gameData[0] == (*numTiles - 1)) { //-1 for indexing, resets index
            gameData[0] = 0;
        } else {
            gameData[0]++;
        }

        free_grid(&(currentGameData.grid), *height); //Free old save state
    }
}

/*
 * Collection function to create game save state.
 * Takes a GameState struct, current index of tile to be played,
 * the current players number, the height and width of the game board,
 * as well as the gameboard itself and stores 
 * this data inside the GameState struct for future use.
 */
void collect_game_data(GameState* currentGameData, int tileIndex, 
        int playerNum, int height, int width, char*** grid) {
    currentGameData->currentTile = tileIndex;
    currentGameData->currentPlayer = playerNum;
    currentGameData->gridHeight = height;
    currentGameData->gridWidth = width;
    create_new_grid(height, width, &(currentGameData->grid));
    
    //Populate created grid with current gameboard data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            currentGameData->grid[i][j] = (*grid)[i][j];
        }
    }
}

/*
 * Game over function. Takes a player and prints a gameover message declaring
 * the opposite player the winner, then exits fitz.
 */
void game_over(Player* player) {
    printf("Player %c wins\n", (player->playerNum == 1) ? '#' : '*');
    exit(0);
}

/*
 * Move function which takes the current tile to be played, 
 * the player making the move, the current game board,
 * the height and width of the board, a status flag struct,
 * the last played legal move (row and column values), and
 * a GameState struct for saving. 
 *
 * Checks for any valid moves
 * on board before prompting players for input; switching
 * behaviour based on the current players type as per the
 * specificaiton.
 */
void make_move(Tile* tile, Player* player, char*** grid, int height, 
        int width, DataReadFlag* gameFlag, int* lastRow, int* lastCol, 
        GameState* currentGameData) {
        
    int col = 0, row = 0, readMove = 1, valid = 0; //Placement vals
    int rotateAngle = 0; 

    print_grid(height, width, *grid);

    if (!check_game_over(grid, height, width, tile)) { 
        game_over(player);
    }

    switch (player->type) {
        case 'h':
            print_tile(tile);
            while (readMove) {
                printf("Player %c] ", player->icon);
                valid = check_user_input(&row, &col, gameFlag, &rotateAngle, 
                        currentGameData);
                if (valid) { //If valid user input for move, try to play
                    Tile playTile = rotate_tile(tile, 
                            (rotateAngle / ROTATION_STEP));
                    if (attempt_place(row, col, playTile, grid, height, width,
                            player)) {
                        readMove = 0; //End loop 
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            }
            break;

        case '1': 
            auto_play_one(player, *lastRow, *lastCol, height, width, tile, 
                    grid, &row, &col);
            break;
        case '2': 
            auto_play_two(player, height, width, grid, tile);
            row = player->lastRow; //Update these so type 1's can make move
            col = player->lastCol; //Grabs the updated play from player
            break;
    }
    
    update_last_play(lastRow, lastCol, row, col);
}

/*
 * Function used to update the last play (row and column) of the current
 * fitz game. Takes pointers to the values of the current game and
 * updates them with the taken row and col of the current play.
 */
void update_last_play(int* lastRow, int* lastCol, int row, int col) {
    *lastRow = row; 
    *lastCol = col;
}

/*
 * Automatic player algorithm type 2. Takes the player of type 2, the 
 * height and width of the game board, the game board itself,
 * and the current tile to be played, and begins searching for
 * a valid move as per the algorithm in spec. 
 * Returns 1 upon finding a valid move and making it; 0 otherwise.
 */
int auto_play_two(Player* player, int height, int width, char*** grid, 
        Tile* tile) {
    int currentRow = player->lastRow;
    int currentCol = player->lastCol;
    int searching = 1, tileDone = 0, currentAngle = 0;

    while (searching) {
        if (attempt_place(currentRow, currentCol, rotate_tile(tile, 
                currentAngle / ROTATION_STEP), grid, height, width, player)) {
            player->lastRow = currentRow;
            player->lastCol = currentCol; //Update with the last valid pos
            print_auto_move(currentRow, currentCol, currentAngle, player);
            return 1; 
        } else {
            currentAngle += ROTATION_STEP;
            if (currentAngle <= MAX_ANGLE) {
                continue;
            } else {
                currentAngle = 0;
                tileDone = 1; 
            }
        }
        
        if (tileDone) { //Move to next position in gameboard
            tileDone = 0;
            auto_two_move(player, height, width, &currentRow, &currentCol);
                
            if (currentRow == player->lastRow && 
                    currentCol == player->lastCol) {
                break; //Reached starting pos again
            }
        }
    }
    return 0;
}

/*
 * Movement function for automatic player algorithm type 2. 
 * Takes the current player, the height and width of the 
 * gameboard, as well as the row and column used immediately
 * previous by the player algorithm. 
 *
 * Increments the next position for the player to try 
 * based on whether they are player one or two as per the
 * spec, moving player one from left->right, top->bottom
 * and vice versa for both for player two.
 */
void auto_two_move(Player* player, int height, int width, 
        int* currentRow, int* currentCol) {
    if (player->playerNum == 1) {
        *currentCol = *currentCol + 1; //Increments 
        if (*currentCol > width + 2) {
            *currentCol = -2;
            *currentRow = *currentRow + 1;
        }
        if (*currentRow > height + 2) {
            *currentRow = -2;
        }
    } else {
        *currentCol = *currentCol - 1; //Decrements
        if (*currentCol < -2) {
            *currentCol = width + 2;
            *currentRow = *currentRow - 1;
        }

        if (*currentRow < -2) {
            *currentRow = height + 2;
        }
    }
}

/*
 * Print function for printing successful moves by automatic players.
 * Takes the successful row, column, and angle of play as well as
 * the player who made the successful move and outputs it to stdout.
 */
void print_auto_move(int currentRow, int currentCol, int currentAngle, 
        Player* player) {
    printf("Player %c => %d %d rotated %d\n", player->icon, currentRow, 
            currentCol, currentAngle);
}

/*
 * Automatic player algorithm one. Takes the player of type 1,
 * the starting row and columns for the algorithm to begin 
 * searching with, the height and width of the board, the
 * tile to be played, the game board itself, and pointers to 
 * the current games last legal move position.
 *
 * Begins searching for a valid play as per the algorithm in 
 * specification. 
 *
 * Returns 1 on successful play, 0 otherwise.
 */
int auto_play_one(Player* player, int rStart, int cStart, int height,
        int width, Tile* tile, char*** grid, int* row, int* col) {
    int currentRow = rStart; //
    int currentCol = cStart;
    int currentAngle = 0;
    int searching = 1;

    while (searching) {
        
        //Tries to place the tile on the grid with current index/theta
        if (attempt_place(currentRow, currentCol, rotate_tile(tile, 
                currentAngle / ROTATION_STEP), grid, height, width, player)) { 
            //update_last_play(row, col, currentRow, currentCol); 
            print_auto_move(currentRow, currentCol, currentAngle, player);
            return 1; //Placed!
        } else {
            currentCol++;

            if (currentCol > (width + 2)) {
                currentCol = -2;
                currentRow++;
            }

            if (currentRow > (height + 2)) {
                currentRow = -2;
            }
        }

        if (currentRow == rStart && currentCol == cStart) {
            currentAngle += ROTATION_STEP;
        }
        
        if (currentAngle == MAX_ANGLE) {
            break; 
        }
    }
    return 0; //No matches
}

/*
 * Memory function to ensure game over checks don't cause memory issues.
 * Takes a copy of the gameboard, and the height of the board, and frees
 * the memory associated with these addresses.
 */
void free_grid(char*** grid, int height) {
    for (int i = 0; i < height; i++) {
        free((*grid)[i]);
    }

    free(*grid);
}

/*
 * Game over check function. Takes the current fitz gameboard, the
 * gameboard height and width, and the current tile to be played.
 * Clones the board and checks every rotation of the tile on every
 * point of the board until a match is found.
 * Returns 1 if a valid move exists on the current board, 0
 * otherwise.
 */
int check_game_over(char*** grid, int height, int width, Tile* tile) {
    char** gridClone;
    create_new_grid(height, width, &gridClone);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            gridClone[i][j] = (*grid)[i][j];
        }
    } //Clone the current grid contents

    Player dummyPlayer;
    dummyPlayer.icon = 'X'; //Sentinel used so there's something to place

    for (int i = -2; i < height + 2; i++) { //-2 +2 to account for the out
        for (int j = -2; j < width + 2; j++) { //of bound space

            if (attempt_place(i, j, *tile, &gridClone,
                    height, width, &dummyPlayer)) {
                free_grid(&gridClone, height); //Stops memory issues
                return 1;
            }

            for (int k = 1; k < ROTATION_COUNT; k++) { //Try with 3 rotations
                if (attempt_place(i, j, rotate_tile(tile, k), &gridClone,
                        height, width, &dummyPlayer)) {
                    free_grid(&gridClone, height);
                    return 1;
                }
            }
        }
    }

    free_grid(&gridClone, height);
    return 0; //Went through entire grid, no plays found.
}


/*
 * Placement function. Takes the row and column of the attempted move,
 * the tile to be played, the current gameboard, the height and width 
 * of the gameboard, and the player making a move.
 * Tries to place the tile on the designated board coordinates.
 * If successful, returns 1. Else, returns 0.
 */
int attempt_place(int row, int col, Tile tile, char*** grid, int height,
        int width, Player* player) {
    
    int rowOffset = row - TILE_CENTRE; //Create transposed coordinates
    int colOffset = col - TILE_CENTRE; //Based from the centre of the tile
    char icon = player->icon;
    if (row < -2 || col < -2 || row > height + 2 || col > width + 2) {
        return 0; //Invalid, placement will cause entire tile to be off board
    }

    for (int i = 0; i < TILE_HEIGHT; i++) { 
        for (int j = 0; j < TILE_WIDTH; j++) { 
            if (((i + rowOffset) < 0 || (i + rowOffset) >= height) ||
                    ((j + colOffset < 0) || (j + colOffset >= width))) {
                //This means one of the offsetted cords is out of bounds
                if ((tile.tileData[i][j]) == '!') {
                    return 0;
                }
            } else { //Translated coords are within the board

                if (((*grid)[i + rowOffset][j + colOffset] != '.') && 
                        tile.tileData[i][j] != ',') {
                    return 0; //If there is data here already and we have a !
                } 
            }
        }
    }
    //If we reach here without exiting then the tile is good!
    for (int i = 0; i < TILE_HEIGHT; i++) {
        for (int j = 0; j < TILE_WIDTH; j++) {
            if (tile.tileData[i][j] != ',') {
                (*grid)[i + rowOffset][j + colOffset] = icon;
            }
        }
    }

    return 1;
}

/*
 * Input check function for h players. Takes pointers to the attempted 
 * row and col, a status flag struct, a pointer to the attempted 
 * rotation angle, and a GameState struct for saving.
 * Tries to read from stdin into a string. If successful, tries to validate 
 * the data retrieved as per the specification. If successful, returns 1. 
 * Else, returns 0 if any of these checks fail.
 */
int check_user_input(int* row, int* col, DataReadFlag* readFlag, 
        int* rotateAngle, GameState* currentGameData) {
    char* userInput;
    int valid = read_stdin(&userInput, readFlag, currentGameData);

    if (!valid) {
        return 0; //Read failed somehow but it wasnt exit worthy
    }

    valid = validate_inputs(row, col, rotateAngle, &userInput);

    if (!valid) {
        return 0; //Numbers weren't valid
    }

    return 1;
}

/*
 * Helper function. Clears stdin in the event of too much data inputted
 * to fitz to ensure further plays are not affected by the overflow.
 */
void clear_stdin(void) {
    char voidArr[MAX_INPUT]; //Used to dump anything in stdin
    char* stringHolder;
    
    do {
        stringHolder = fgets(voidArr, sizeof(voidArr), stdin);
        //Throw all the data still in stdin into the abyss, until we hit
        //new line or there is nothing left.
    } while (stringHolder != NULL && (strchr(voidArr, '\n') == NULL));
}

/*
 * Input function. Takes a pointer to the string to be read, a status flag
 * struct, and a GameState struct for saving.
 * Allocates memory for the string as per spec, and tries to read
 * into this string. If data overflows the buffer provided,
 * clears stdin to ensure an empty stdin.
 *
 * Once data collected, if of valid size, attempts to clean
 * it. If data is of valid size and clean as per the specification,
 * return 1. In all other cases, return 0. fitz exits if EOF detected
 * and no other data present with status 10. If save present in collected
 * string, attempts to save.
 */
int read_stdin(char** userInput, DataReadFlag* readFlag, 
        GameState* currentGameState) {
    //+2 to memory to account for \0 and overflow data
    *userInput = (char*) malloc(sizeof(char) * (MAX_INPUT + 2)); 

    if (fgets(*userInput, (MAX_INPUT + 2), stdin) != NULL) { 
        char* newlinePos = strchr(*userInput, '\n');
        if (newlinePos != NULL) {
            *newlinePos = '\0';
        } else { //No newline in string; EOF input or overflowed data
            if (strlen(*userInput) == 71) { //Data read in was too big
                if (!feof(stdin)) { //If this is true, means theres still data
                    clear_stdin(); //in stdin
                    return 0;
                } else { //Else there is nothing left in the pipe, 
                    return 0; //Yet we still overflowed
                }
            } //If the length isn't 71 is must be lower; therefore, valid 
            printf("\n"); //If it's EOF include a newline
        } 
    } else {
        readFlag->returnVal = END_OF_INPUT; //EOF with no data
        check_load_errors(*readFlag);
    }

    if (!(strncmp(*userInput, "save", 4))) { //Check if we have a save attempt
        attempt_save(currentGameState, userInput);
        return 0; //Used to reprompt later for valid move
    }

    if (clean_inputs(userInput)) { //Check if valid data
        return 1;
    } else {
        return 0;
    }
}

/*
 * Saving function. Takes a GameState struct with current game data, and
 * a user defined string from stdin. 
 * Attempts to save to the filepath specified in user defined string.
 * If file cannot be reached, return, else write to file with
 * current game data as per specificed format.
 */
void attempt_save(GameState* currentGameState, char** userInput) {
    char* saveFilePath = (char*) malloc(sizeof(char) * MAX_INPUT);
    strncpy(saveFilePath, (*userInput + 4), MAX_INPUT); //+4 to ignore "save"
    FILE* writeLocation = fopen(saveFilePath, "w");
    if (writeLocation == NULL) {
        fprintf(stderr, "Unable to save game\n");
        return; //Can't save
    } else {
        fprintf(writeLocation, "%d %d %d %d\n", currentGameState->currentTile,
                currentGameState->currentPlayer, currentGameState->gridHeight,
                currentGameState->gridWidth);

        for (int i = 0; i < currentGameState->gridHeight; i++) {
            for (int j = 0; j < currentGameState->gridWidth; j++) {
                fprintf(writeLocation, "%c", currentGameState->grid[i][j]);
            }
            fprintf(writeLocation, "\n");
        }
    }

    fclose(writeLocation);
    free(saveFilePath);
}

/*
 * Cleaning function. Takes user generated string from stdin
 * and checks it is valid as per specification requirements.
 * If data is valid, return 1, else return 0 in all other cases.
 */
int clean_inputs(char** userInput) {
    char c, prevData = ' ';
    int numSpaces = 0, index = 0;

    do {
        c = (*userInput)[index++]; 
    
        if (c == '\0') { //Hit end of string
            break;
        }

        if (c == ' ') {
            if (prevData == ' ') { //If this returns 1, two spaces
                return 0; //Also catches leading space
            } else {
                if (++numSpaces > 2) { //This signifies the end of an int
                    return 0; //Too many numbers given 
                }
            } 
        } else if (c == '+' || c == '-') { //Only let +/- appear
            if (prevData != ' ') { //Ensure no sign appears after digits
                return 0;
            } else {
                continue;
            }
        } else if (!isdigit(c)) { 
            return 0; //Not a number
            
        } 
        prevData = (char) c;
    } while (c != '\0');

    if (numSpaces != 2) { //Was there more than 2 spaces? 2 spaces == 3 ints
        return 0; //As no trailing spaces
    }
    return 1;
}

/*
 * Validation function. Takes pointers to the current row and column to be
 * played, a pointer to the current angle to be played, as well as a
 * cleaned user defined string from stdin.
 * Attempts to convert the string contents to values. Checks values
 * meet specification criteria regarding validity. If valid, reutrns 1,
 * else returns 0. 
 * Note: board size checks handled by attempted place
 * */
int validate_inputs(int* row, int* col, int* rotateAngle, char** userInput) {
    char* currentInt = *userInput;
    int index = 0, angle = 0;
    int inputs[3]; //String precleaned to have 3 ints 

    while (*currentInt != '\0') {
        if (isdigit(*currentInt) || ((currentInt[0] == '+' || 
                currentInt[0] == '-') && isdigit(currentInt[1]))) { //neg/pos
            inputs[index++] = strtol(currentInt, &currentInt, 10);
        } else {
            currentInt++;
        }
    }

    angle = inputs[2]; //used for clarity

    if (angle != 0 && angle != 90 && angle != 180 && angle != 270) {
        return 0;
    } else {
        *row = inputs[0]; //These will be checked for validity at placement
        *col = inputs[1];
        *rotateAngle = inputs[2];
    }

    return 1;
}

/*
 * Printing function. Takes a tile to be printed, and prints it to 
 * stdout.
 */
void print_tile(Tile* tile) {
    for (int i = 0; i < TILE_WIDTH; i++) {
        for (int j = 0; j < TILE_HEIGHT; j++) {
            printf("%c", tile->tileData[i][j]);
        }
        printf("\n");
    }
}

/*
 * Loading function. Takes a file pointer, a status flag struct,
 * a filepath, and a pointer to the number of tiles fitz has.
 *
 * Attempts to read from given file to construct the tiles 
 * fitz will use for the current game. If any invalid data is 
 * encountered, as per the specification, fitz will exit with
 * relevant exit status. Otherwise, upon successful reading
 * and processing, return an array of filled Tile structs for use
 * in fitz.
 */
Tile** load_tiles(FILE** tileFile, DataReadFlag* loadFlag, char* tileName, 
        int* numTiles) { 

    int tileCount = 1; //Assume one tile in file; if not, will error later
    Tile** tiles = (Tile**) malloc(sizeof(Tile*) * tileCount);
    int pos = 0, col = 0, row = 0, point = 0;
    char tempTile[TILE_HEIGHT][TILE_WIDTH]; //tiles are a fixed size

    *tileFile = open_file(tileName, loadFlag, TILE_FILE);

    while (point != EOF) {

        point = fgetc(*tileFile); //grab each char in file
        if(!check_point(point, loadFlag)) { //Check it's valid (,!\n)
            break;
        }

        tempTile[row][col++] = (char) point; //Put char in, incr column

        if (col == TILE_WIDTH && row != (TILE_HEIGHT - 1)) { //Hit end of row
            if(!check_row_end(tileFile)) { //Grabs next char
                break; //Mandate each row ends with a newline
            } else {
                row++; 
                col = 0;
            } 
        }
        
        if (row == (TILE_HEIGHT - 1) && col == TILE_WIDTH) {//Hit last row
            if (check_tile_end(tileFile)) {
                tiles[pos] = (Tile*) malloc(sizeof(Tile)); //Make new tile
                memcpy(tiles[pos++]->tileData, tempTile, sizeof(tempTile));
                memset(tempTile, 0, sizeof(char) * TILE_HEIGHT * TILE_WIDTH); 
                row = col = 0; //Put new tile in arr, and clear
            } else {
                break;
            }
        }

        if (pos == tileCount) { //Memory buffer
            tileCount *= 2; //Double # of tiles
            tiles = realloc(tiles, sizeof(Tile*) * tileCount);
        }
    }

    check_tile_contents(loadFlag, pos, col, row);
    *numTiles = pos; //# of inner tiles malloc'd
    fclose(*tileFile);
    *tileFile = NULL; //Dont let this dangle in case we need to load save
    return tiles;
}

/*
 * Printing function. Takes an array of filled Tile structs, and the 
 * number of tiles read in by fitz. 
 * For every tile in the array, print it alongside its three
 * rotated forms to stdout.
 */
void print_rotations(Tile** tiles, int numTiles) {
    Tile rotatedTiles[numTiles][ROTATION_COUNT]; //4 rotations of 90 possible
    
    for (int i = 0; i < numTiles; i++) {
        rotatedTiles[i][0] = *tiles[i]; //First tile needs no rotation
        for (int j = 1; j < ROTATION_COUNT; j++) {
            rotatedTiles[i][j] = rotate_tile(tiles[i], j); 
        }
    }

    print_tile_rotations(numTiles, rotatedTiles);
}

/*
 * Rotation function. Takes a tile to be rotated and a specified number
 * of rotations. Rotates the tile by this number of rotations
 * in increments of 90 degrees, and returns the rotated tile.
 */
Tile rotate_tile(Tile* tileStart, int numRotations) { 
    char tileCopy[TILE_HEIGHT][TILE_WIDTH];
    char tile[TILE_HEIGHT][TILE_WIDTH];
    if (numRotations == 0) {
        return *tileStart;
    }
    memcpy(tile, tileStart->tileData, sizeof(tile)); //Load tileData into mem
    for (int i = 0; i < numRotations; i++) {
        for (int j = 0; j < TILE_HEIGHT; j++) {
            for (int k = 0; k < TILE_WIDTH; k++) {
                tileCopy[j][k] = tile[k][j]; //Swap rows/cols
            }
        }
    
        for (int j = 0; j < TILE_HEIGHT; j++) {
            for (int k = 0; k < TILE_WIDTH / 2; k++) { //Mirror grid
                char tmp = tileCopy[j][k]; //Minus 1 for 0 based index
                tileCopy[j][k] = tileCopy[j][(TILE_WIDTH - 1) - k]; 
                tileCopy[j][(TILE_WIDTH - 1) - k] = tmp; 
            }
        }
        memcpy(tile, tileCopy, sizeof(tileCopy)); //Allows further rotations
    }
    Tile rotatedTile;
    memcpy(rotatedTile.tileData, tileCopy, sizeof(tileCopy));
    return rotatedTile;
} 

/*
 * Printing functions. Takes a Variable Length Array of Tiles 
 * and their rotations, where the number of tiles may vary between 
 * games. Prints each rotation of the tile to stdout sequentially,
 * separating different tiles with a newline.
 */
void print_tile_rotations(int tileNum, Tile tiles[tileNum][ROTATION_COUNT]) {
    for (int k = 0; k < tileNum; k++) {
        for (int i = 0; i < TILE_HEIGHT; i++) {

            int rotationCount = 0; //Used to print out the fixed 4 rotations

            for (int j = 0; j < TILE_WIDTH; j++) {
                printf("%c", tiles[k][rotationCount].tileData[i][j]);
            }

            rotationCount++;
            printf(" ");

            for (int j = 0; j < TILE_WIDTH; j++) {
                printf("%c", tiles[k][rotationCount].tileData[i][j]);
            }

            rotationCount++;
            printf(" ");

            for (int j = 0; j < TILE_WIDTH; j++) {
                printf("%c", tiles[k][rotationCount].tileData[i][j]);
            }   
            rotationCount++;
            printf(" ");

            for (int j = 0; j < TILE_WIDTH; j++) {
                printf("%c", tiles[k][rotationCount].tileData[i][j]);
           
            }
            printf("\n");
        }
        if (k != (tileNum - 1)) {
            printf("\n");
        }
    }
}

/*
 * Creation function. Takes the specified height and width of a game board,
 * and a pointer to an uninitalized board. Creates a board defined by these
 * dimensions and fills it with '.' chars.
 */
void create_new_grid(int height, int width, char*** grid) { 
    *grid = (char**) malloc(sizeof(char*) * height); //Bring us back to char**

    for (int i = 0; i < height; i++) { //Create grid in memory
        (*grid)[i] = (char*) malloc(sizeof(char) * width);
    }
    
    for (int i = 0; i < height; i++) { //Fill it with dots
        for (int j = 0; j < width; j++) {
            (*grid)[i][j] = '.';
        }
    }
}

/*
 * Printing function. Takes a boards height and width,
 * as well as a copy of the current gameboard, and
 * prints it's contents to stdout.
 */
void print_grid(int height, int width, char** grid) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
}

/*
 * Exit function. Takes a DataReadFlag (status flag) struct,
 * and exits fitz based upon the value contained within.
 * Prints the relevant exit message to stderr before
 * exiting the program.
 * Returns 0 if no valid error status exists for
 * value inside the status flag.
 */
int check_load_errors(DataReadFlag statusObj) {
    fflush(stdout); //Keep it clean before trying to print
    fflush(stderr);
    switch (statusObj.returnVal) {
        case 1:
            fprintf(stderr, "%s %s", "Usage: fitz tilefile [p1type p2type", 
                    "[height width | filename]]\n");
            break;
        case 2:
            fprintf(stderr, "%s", "Can't access tile file\n");
            break;
        case 3:
            fprintf(stderr, "%s\n", "Invalid tile file contents");
            break;
        case 4:
            fprintf(stderr, "%s", "Invalid player type\n");
            break;

        case 5: 
            fprintf(stderr, "%s", "Invalid dimensions\n");
            break;

        case 6:
            fprintf(stderr, "%s", "Can't access save file\n");
            break;

        case 7:
            fprintf(stderr, "Invalid save file contents\n");
            break;

        case 10:
            fprintf(stderr, "End of input\n");
            break;
        
        default:
            return 0;
            break;
    }

    exit(statusObj.returnVal);
}

/*
 * Checking function. Takes a character (returned from fgetc())
 * from a potential tile, and a status flag struct. Checks it's
 * validity as per the specification for tile files. Returns 1
 * if char is valid, else 0.
 */
int check_point(int c, DataReadFlag* loadFlag) {

    if ((char) c == '\n') {
        return 1;
    } else if (c == EOF) {
        return 0;
    } else if ((c != ',') && (c != '!')) { // Invalid char 
        loadFlag->returnVal = INVALID_TILE_CONTENTS;
        check_load_errors(*loadFlag);
        return 0;
    } else { 
        return 1;
    }
}

/*
 * File IO function. Takes a filepath, a status flag object, and a
 * file type. Tries to open the file for reading. If file cannot be 
 * opened, checks what type of file was parsed to raise the correct 
 * exit status. Else, returns the file pointer for the opened file.
 */
FILE* open_file(char* fileName, DataReadFlag* loadFlag, char fileType) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        if (fileType == TILE_FILE) {
            loadFlag->returnVal = INVALID_TILEFILE;
        } else if (fileType == SAVE_FILE) {
            loadFlag->returnVal = INVALID_SAVE_FILE;
        } 

        check_load_errors(*loadFlag);
    }
    return file;
} 
    
/*
 * Validation function. Takes a status flag struct, and counters from a
 * tile loading function call. If the column and row counters are not 0,
 * indicates invalid amount of data in tilefile. If pos is 0, indicates
 * empty file. Exits program if any of these invalid cases are true.
 */
void check_tile_contents(DataReadFlag* loadFlag, int pos, int col, int row) {

    if (row != 0 || col != 0 || pos == 0) { //Contents not exact or no content
        loadFlag->returnVal = INVALID_TILE_CONTENTS; 
        check_load_errors(*loadFlag);
    }
}

/*
 * Validation function. Takes strings from the commandline and
 * a status flag struct. Attempts to convert the strings to valid
 * board dimensions as per the specification. If not possible,
 * exits fitz.
 */
void check_parameters(char* heightRaw, char* widthRaw, int* height, 
        int* width, DataReadFlag* boardFlag) {
    *height = atoi(heightRaw);
    *width = atoi(widthRaw);
    if (*height < 1 || *width < 1 || *height > MAX_HEIGHT || 
            *width > MAX_WIDTH) { 
        boardFlag->returnVal = INVALID_BOARD_PARAM; 
        check_load_errors(*boardFlag);
    }
}

/*
 * Player creation function. Takes a string from commandline,
 * a Player struct, a status flag struct, and a player number.
 * Attempts to create a player with this data as per the specification.
 * Exits fitz in the event of invalid player type or number.
 * Returns created player struct if successful.
 */
Player* create_player(char* type, Player* player, DataReadFlag* playerFlag, 
        int playerNum) {

    if ((strlen(type) > 1)) {
        playerFlag->returnVal = INVALID_PLAYER;
    } else if (!(strpbrk(type, PLAYER_TYPES))) {
        playerFlag->returnVal = INVALID_PLAYER;
    } else if (playerNum != 1 && playerNum != 2) {
        playerFlag->returnVal = INVALID_PLAYER;
    } else {    
        player = (Player*) malloc(sizeof(Player)); //Return these?
        player->type = type[0]; //Get the only char out of the string
        if (type[0] == '1') {
            player->lastCol = 0;
            player->lastRow = 0;
        }
        player->playerNum = playerNum; //Can move this to main

        switch (playerNum) {
            case 1:
                player->icon = '*';
                break;
            case 2:
                player->icon = '#';
                break;
        }
    }
    check_load_errors(*playerFlag);
    return player;
}

/*
 * Loading function. Takes a filepath, a gameboard, an integer
 * array for game data, a status flag struct, a file pointer, the
 * number of tiles being used in this game of fitz, the height and the
 * width of the current gameboard.
 *
 * Attempts to open file and read it's contents. Attempts to validate
 * contents. If successful, game has been loaded into provided data pointers.
 * Else, exits fitz.
 */
void load_game(char* saveFileName, char*** grid, int* gameData, 
        DataReadFlag* saveFlag, FILE** saveFile, int* numTiles, int* height,
        int* width) {
    *saveFile = open_file(saveFileName, saveFlag, SAVE_FILE);
    //READ CONTENTS
    char* parameters = get_params(saveFile, saveFlag); //Checks for clean line
    //Assign the data from the line into the thing
    char* currentPos = parameters; //Pointer to the string for strtol to use
    long paramVals[4]; //Valid parameters ensures 4 digits to be converted
    int index = 0;
    
    while (*currentPos != '\0') { //While there is still content
        if (isdigit(*currentPos)) { //If it's a digit, convert
            paramVals[index++] = strtol(currentPos, &currentPos, 10); 
        } else {
            currentPos++; //Sitting on a space; move pointer forward by one
        }
    }

    check_save_params(paramVals, numTiles, saveFlag);
    gameData[0] = paramVals[0];
    gameData[1] = paramVals[1]; //Hand over next tile/player
    *height = (int) paramVals[2];
    *width = (int) paramVals[3];
    create_new_grid(*height, *width, &(*grid)); 
    load_grid(grid, *height, *width, saveFlag, saveFile); 
    free(parameters);
    fclose(*saveFile);
    *saveFile = NULL; //Dangling pointer
}

/*
 * Loading function. Takes a gameboard, a height and width for the 
 * gameboard, a status flag struct, and a file pointer. Attempts to 
 * read data from file into gameboard. Exits fitz if unable to do
 * so, as per the specification checks.
 */
void load_grid(char*** grid, int height, int width, DataReadFlag* saveFlag,
        FILE** saveFile) {
    int c = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            c = fgetc(*saveFile);
            if (!check_grid_point(c)) { //Check it's a valid point
                saveFlag->returnVal = INVALID_SAVE_CONTENT;
                break;
                //Catches short lines  
            } else {
                (*grid)[i][j] = (char) c;
            }
        }
        if (!check_row_end(saveFile)) { //Check end of grid row for \n
            saveFlag->returnVal = INVALID_SAVE_CONTENT;
            break;
        } //Catches long lines 
    }
    if (fgetc(*saveFile) != EOF) { //Checks the next two chars are \n 
        //and EOF; indicating end of data block. If not, fails
        saveFlag->returnVal = INVALID_SAVE_CONTENT;
    }

    check_load_errors(*saveFlag);
}

/*
 * Checking function. Takes a single char from fgetc().  Checks a grid 
 * that is being loaded for valid chars. If char inputted is valid,
 * return 1, else return 0;
 */
int check_grid_point(int c) {
    if ((char) c != '#' && (char) c != '*' && (char) c != '.') {
        return 0; //Enforce only these 3 chars allowed
    } else {
        return 1;
    }
}

/*
 * Checking function. Takes an array of integer values derived
 * from savefile parameters, the number of tiles for this game
 * of fitz, and a status flag struct.
 * Attempts to validate the parameters as per the specification.
 * If any invalid parameters, exits fitz with relevant exit status.
 */
void check_save_params(long paramVals[4], int* numTiles, 
        DataReadFlag* saveFlag) {
    if (paramVals[1] != 1 && paramVals[1] != 0) { //# must be 1 or 0
        saveFlag->returnVal = INVALID_SAVE_CONTENT;
    }

    if (paramVals[0] > (*numTiles - 1) || paramVals[0] < 0) { //Index too big?
        saveFlag->returnVal = INVALID_SAVE_CONTENT;
    }

    if (paramVals[2] > MAX_HEIGHT || paramVals[2] < 1 || 
            paramVals[3] > MAX_WIDTH || paramVals[3] < 1) {
        saveFlag->returnVal = INVALID_SAVE_CONTENT;
    }
    check_load_errors(*saveFlag);
}

/*
 * Code reduction function. Takes a status flag struct and sets it 
 * to an invalid save file exit status, then exits fitz.
 */
void set_invalid_save(DataReadFlag* saveFlag) { 
    saveFlag->returnVal = INVALID_SAVE_CONTENT;
    check_load_errors(*saveFlag);
}


/* Takes a valid file pointer and a status flag struct. 
 * Reads data char by char from file into a string, performing 
 * various checks on the data to ensure it is in line with the 
 * required format as specified within the spec. Reads until 
 * newline, then returns the built string of data. If any errors
 * found while parsing data, program exits which status 7 (invalid
 * save file contents).
 */
char* get_params(FILE** saveFile, DataReadFlag* saveFlag) { 
    int data = 0, index = 0, numSpaces = 0;
    char prevData = ' '; //If file starts with a space its invalid 
    int bufferSize = 32;
    char* params = (char*) malloc(sizeof(char) * bufferSize);
    while (data != '\n') { //Read only until end of the first line
        data = fgetc(*saveFile); //Get the current char
        if (data == '\n') {
            params[index] = '\0';
            break;
        }
        if (data == EOF) {
            set_invalid_save(saveFlag);
        }
        if (data == ' ') { //Is it a space
            if (prevData == ' ') { //Two spaces aren't valid 
                set_invalid_save(saveFlag);
            } else {
                params[index++] = (char) data; //Put space in to signal int
                if (++numSpaces > 3) { //This signifies the end of a digit
                    set_invalid_save(saveFlag); //pre-itr numints check
                }
            }
        } else if (data == '+' || data == '-') {
            if (prevData != ' ') {
                set_invalid_save(saveFlag);
            } else { 
                params[index++] = (char) data;
            }
        } else if (!isdigit(data)) { //If it's not a number or a space 
            set_invalid_save(saveFlag);
        } else {
            params[index++] = (char) data;
        }

        prevData = (char) data; //Update previous char to check for old spaces

        if ((index) == bufferSize) {
            bufferSize *= 2;
            params = (char*) realloc(params, sizeof(char) * bufferSize);
        }
    }

    params[index] = '\0';

    if (numSpaces != 3) { //Catches too little data
        set_invalid_save(saveFlag);
    }

    return params;
}

/*
 * Takes a valid file pointer, and a pointer to
 * a fgetc() char, checking if the directly 
 * subsequent char in the given file is a newline.
 * Returns 1 on finding a newline as the next char, 0 otherwise
 */
int check_row_end(FILE** tileFile) {
    int c = fgetc(*tileFile);
    if ((char) c == '\n') {
        return 1;
    } else {
        return 0;
    }
}

/*
 * Takes a valid file pointer, and a pointer to a fgetc() char, 
 * checking if the next two chars in the given file are 
 * both newlines, or newline then EOF.
 * Returns 1 on success for finding either \n\n or \nEOF, 0 otherwise.
 */
int check_tile_end(FILE** tileFile) {
    
    int c = fgetc(*tileFile);
    if ((char) c == '\n') { //Is next char a \n?
        c = fgetc(*tileFile);
        if ((char) c == '\n') { //Is next char a \n or EOF?
            return 1; 
        } else if (c == EOF) {
            return 1;
        } else {
            return 0; 
        }
    } else {
        return 0;
    }
}
