#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>

#define INACTIVE 0    // represents an inactive block or game
#define ACTIVE 1      // represents an active block or game
#define GRID_ROWS 20  // number of rows in the grid
#define GRID_COLS 15  // number of columns in the grid

// Game grid where blocks will be placed
int speedIncrease = 800;           // rate at which the game speeds up
suseconds_t timeInterval = 300000;  // time between block drops 
char grid[GRID_ROWS][GRID_COLS] = {0};
int points = 0;          // player's score
char isGameActive = ACTIVE;  // tracks if the game is still running

// struct to define a block with its size and position in the grid
typedef struct {
  	char **block;
    	int size, rowPos, colPos;
} Block;

Block activeBlock;  



// 7 types of blocks (Tetris shapes)
const Block availableBlocks[7] = {
	// Each block is represented by a 2D array of 0s and 1s
	{(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3},  // L-shape
	{(char *[]){(char []){0,1,1},(char []){1,1,0}, (char []){0,0,0}}, 3},  // S-shape
        {(char *[]){(char []){1,1,0},(char []){0,1,1}, (char []){0,0,0}}, 3},  // Z-shape
    	{(char *[]){(char []){0,1,0},(char []){1,1,1}, (char []){0,0,0}}, 3},  // T-shape
    	{(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3},  // L-shape
    	{(char *[]){(char []){1,0,0},(char []){1,1,1}, (char []){0,0,0}}, 3},  // Reverse L-shape
    	{(char *[]){(char []){1,1},(char []){1,1}}, 2},                        // Square block
   	{(char *[]){(char []){0,0,0,0}, (char []){1,1,1,1}, (char []){0,0,0,0}, (char []){0,0,0,0}}, 4}  // line block
};

// clones a block creating a copy for later
Block cloneBlock(Block shape){
    	Block clonedBlock = shape;
    	char **blockCopy = shape.block;
    	clonedBlock.block = (char**)malloc(clonedBlock.size * sizeof(char*));
    	for(int i = 0; i < clonedBlock.size; i++){
        	clonedBlock.block[i] = (char*)malloc(clonedBlock.size * sizeof(char));
        	for(int j = 0; j < clonedBlock.size; j++) {
            	clonedBlock.block[i][j] = blockCopy[i][j];
        	}
    	}
    	return clonedBlock;
}

// frees the memory used by a block
void freeBlock(Block shape){
    for(int i = 0; i < shape.size; i++){
        free(shape.block[i]);
    }
    free(shape.block);
}

// checks if a block's position is valid
int validatePosition(Block shape){
    for(int i = 0; i < shape.size; i++) {
        for(int j = 0; j < shape.size; j++){
            if((shape.colPos + j < 0 || shape.colPos + j >= GRID_COLS || shape.rowPos + i >= GRID_ROWS)){
                // block is out of bounds
                if(shape.block[i][j]) 
                    return INACTIVE;
            } else if(grid[shape.rowPos + i][shape.colPos + j] && shape.block[i][j]) {
                // block overlaps with an existing block
                return INACTIVE;
            }
        }
    }
    return ACTIVE;
}

// spawns a random block at the top 
void spawnRandomBlock(){
	Block newBlock = cloneBlock(availableBlocks[rand() % 7]);

	// randomize column for block placement
	newBlock.colPos = rand() % (GRID_COLS - newBlock.size + 1);
	newBlock.rowPos = 0;  // start at the top of the grid
	freeBlock(activeBlock);
	activeBlock = newBlock;

	// if the block cannot fit in the grid end the game
	if(!validatePosition(activeBlock)){
   	    	isGameActive = INACTIVE;
	}
}

// rotates a block clockwise
void rotateBlock(Block shape){
	Block temp = cloneBlock(shape);
	int size = shape.size;
for(int i = 0; i < size; i++){
        for(int j = 0, k = size - 1; j < size; j++, k--){
            shape.block[i][j] = temp.block[k][i];
        }
    }
    freeBlock(temp);
}

// places the block in the grid and holds position
void placeBlockInGrid(){
    for(int i = 0; i < activeBlock.size; i++){
        for(int j = 0; j < activeBlock.size; j++){
            if(activeBlock.block[i][j])
                grid[activeBlock.rowPos + i][activeBlock.colPos + j] = activeBlock.block[i][j];
        }
    }
}

// removes and shifts rows down, then increase speed
void removeFullRows(){
    int rowsCleared = 0;
    for(int i = 0; i < GRID_ROWS; i++){
        int sum = 0;
        for(int j = 0; j < GRID_COLS; j++) {
            sum += grid[i][j];
        }
        if(sum == GRID_COLS){  // full row detected
            rowsCleared++;
            // shifts the rows down
            for(int k = i; k >= 1; k--)
                for(int l = 0; l < GRID_COLS; l++)
                    grid[k][l] = grid[k - 1][l];
            // clears the top row
            for(int l = 0; l < GRID_COLS; l++)
                grid[0][l] = 0;
            timeInterval -= speedIncrease--;  // speeds up the game
        }
    }
    points += 100 * rowsCleared;  // increase points based on rows cleared
}

// Displays the game grid and the score
void displayGrid(){
    char tempGrid[GRID_ROWS][GRID_COLS] = {0};  // temporary grid to include active block
    for(int i = 0; i < activeBlock.size; i++){
        for(int j = 0; j < activeBlock.size; j++){
            if(activeBlock.block[i][j])
                tempGrid[activeBlock.rowPos + i][activeBlock.colPos + j] = activeBlock.block[i][j];
        }
    }
    clear();  
    for(int i = 0; i < GRID_ROWS; i++){
        for(int j = 0; j < GRID_COLS; j++){
            printw("%c ", (grid[i][j] + tempGrid[i][j]) ? '#' : '.');  // print blocks
        }
        printw("\n");
    }
    printw("\nScore: %d\n", points);  // displays current score
}

// controls block movement and rotation based on user input
void controlBlock(int action){
    Block temp = cloneBlock(activeBlock);
    switch(action){
        case 's':  // move down
            temp.rowPos++; 
            if(validatePosition(temp))
                activeBlock.rowPos++;
            else {
                placeBlockInGrid();
                removeFullRows();
                spawnRandomBlock();  // new block after placing current one
            }
            break;
        case 'd':  // move right
            temp.colPos++; 
            if(validatePosition(temp))
                activeBlock.colPos++;
            break;
        case 'a':  // move left
            temp.colPos--; 
            if(validatePosition(temp))
                activeBlock.colPos--;
            break;
        case 'w':  // rotate
            rotateBlock(temp); 
            if(validatePosition(temp))
                rotateBlock(activeBlock);
            break;
    }
    freeBlock(temp);  // free the temporary block
    displayGrid();  // redraw the grid
}

// returns whether the block should automatically drop based on time passed
struct timeval lastUpdate, currentTime;
int needsUpdate(){
    return ((suseconds_t)(currentTime.tv_sec*1000000 + currentTime.tv_usec) -((suseconds_t)lastUpdate.tv_sec*1000000 + lastUpdate.tv_usec)) > timeInterval;
}

int main() {
    srand(time(0));  // initialize random number generator
    points = 0;      // reset points
    int input;
    initscr();  // initialize ncurses
    gettimeofday(&lastUpdate, NULL);  // record initial time
    timeout(1);  // non-blocking input with a timeout of 1ms
    spawnRandomBlock();  // spawn first block
    displayGrid();  // display initial grid

    // Main game loop
    while(isGameActive){
        if ((input = getch()) != ERR) {  // check for user input
            controlBlock(input);  // Handle input
        }
        gettimeofday(&currentTime, NULL);  // get current time
        if (needsUpdate()) {
            controlBlock('s');  // automatically move block down if needed
            gettimeofday(&lastUpdate, NULL);  // update last drop time
        }
    }

    // Game over
    freeBlock(activeBlock);  // free memory used by the active block
    endwin();  // end ncurses mode

    // Print final game state and score
    for(int i = 0; i < GRID_ROWS; i++){
        for(int j = 0; j < GRID_COLS; j++){
            printf("%c ", grid[i][j] ? '#' : '.');
        }
        printf("\n");
    }
    printf("\nGame Over!\n");
    printf("\nFinal Score: %d\n", points);
    return 0;
}
