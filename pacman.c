#include <stdio.h> //input - output
#include <stdlib.h> //random movement of the ghost
#include <time.h>
#include <stdbool.h> //boolean library
#include <ncurses.h>

void printMap(char matrix[22][19], int pacmanPositionColumn, int pacmanPositionRow, int GhostPositions[4][2], int invincibleMatrix[4][2], bool isPacmanInvincible);
bool isValidMovement(int pacmanPosCol, int pacmanPosRow, char map[22][19], char desiredMovement, bool * isDead, int GhostPositions[4][2]);
void pacmanMovement(int * ColumnNumber, int * RowNumber, char map[22][19], bool * isDead, int GhostPositions[4][2]);
void GhostMovement(int GhostPositionMatrix[4][2], char map[22][19], int pacmanPositionColumn, int pacmanPositionRow);
bool pacmanAteAllDots(char map[22][19]);
int manhattanDistance(int PacmanPCol, int PacmanPRow, int GhostPCol, int GhostPRow); 
bool isValidMovementGhost(int ghostPosRow, int ghostPosCol, int GhostPositionMatrix[4][2], char movement);
void pacmanMovementInvincible(int * ColumnNumber, int * RowNumber, char map[22][19]);
void GhostMovementInvincible(int GhostPositionMatrix[4][2], char map[22][19], int pacmanPositionColumn, int pacmanPositionRow, int gameTicks);
bool isMovementColision(int pacmanPosCol, int pacmanPosRow, char map[22][19], char desiredMovement);
void eatGhosts(int pacmanPosCol, int pacmanPosRow, char map[22][19], int GhostPositionMatrix[4][2]);

int main(){
    srand(time(NULL));
    
    // Initialize ncurses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE); // Enable special keys

    //All positions (pacman and ghosts) start from 0. Real positions are + 1

    int gameTicks = 0; // used to make pacman invincible for 12 ticks of the game

    int pacmanLocationColumn = 9; 
    int pacmanLocationRow = 12; 

    bool isPacmanDead = false;
    bool isPacmanInvincible = false;
    
    int GhostLocationMatrix[4][2] = {{8,9}, // 0 Ghost
                                     {10,9}, // 1 Ghost
                                     {10,10}, // 2 Ghost
                                     {10,8}}; // 3 ghost
                                     // each row is the position of a Ghost

    int InvincibleItemPositionMatrix[4][2] = {{1,1},
                                              {1,17},
                                              {20,1},
                                              {20,17}};
                                              
    char map[22][19]=    {{'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'}, 
                          {'#','.','.','.','.','.','.','.','.','#','.','.','.','.','.','.','.','.','#'},
                          {'#','.','#','#','.','#','#','#','.','#','.','#','#','#','.','#','#','.','#'},
                          {'#','.','#','#','.','#','#','#','.','#','.','#','#','#','.','#','#','.','#'},
                          {'#','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','#'},
                          {'#','.','#','#','.','#','.','#','#','#','#','#','.','#','.','#','#','.','#'},
                          {'#','.','.','.','.','#','.','.','.','#','.','.','.','#','.','.','.','.','#'},
                          {'#','#','#','#','.','#','#','#','.','#','.','#','#','#','.','#','#','#','#'},
                          {' ',' ',' ','#','.','#','.','.','.',' ','.','.','.','#','.','#',' ',' ',' '},
                          {'#','#','#','#','.','#','.','#','#',' ','#','#','.','#','.','#','#','#','#'},
                          {' ',' ',' ',' ','.','.','.','#',' ',' ',' ','#','.','.','.',' ',' ',' ',' '},
                          {'#','#','#','#','.','#','.','#','#',' ','#','#','.','#','.','#','#','#','#'},
                          {' ',' ',' ','#','.','#','.','.','.','.','.','.','.','#','.','#',' ',' ',' '},
                          {'#','#','#','#','.','#','.','#','#','#','#','#','.','#','.','#','#','#','#'},
                          {'#','.','.','.','.','.','.','.','.','#','.','.','.','.','.','.','.','.','#'},
                          {'#','.','#','#','.','#','#','#','.','#','.','#','#','#','.','#','#','.','#'},
                          {'#','.','.','#','.','.','.','.','.','.','.','.','.','.','.','#','.','.','#'},
                          {'#','#','.','#','.','#','.','#','#','#','#','#','.','#','.','#','.','#','#'},
                          {'#','.','.','.','.','#','.','.','.','#','.','.','.','#','.','.','.','.','#'},
                          {'#','.','#','#','#','#','#','#','.','#','.','#','#','#','#','#','#','.','#'},
                          {'#','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','#'},
                          {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'}};
       
        //print map for the first time so player can see where to make first move
        printMap(map, pacmanLocationRow, pacmanLocationColumn, GhostLocationMatrix, InvincibleItemPositionMatrix, isPacmanInvincible);

    while(!isPacmanDead && !pacmanAteAllDots(map)){
        for(int i=0; i<4; i++){ // for loop is to check that pacman is not in the same tale as a ghost before asking for a movement (the own program would detect pacman is dead, but it would be after asking for a movement). Also used to check if pacman became invincible
            if((GhostLocationMatrix[i][0]==pacmanLocationRow) && (GhostLocationMatrix[i][1]==pacmanLocationColumn) && !isPacmanInvincible){
                isPacmanDead=true;
            }
            else if((InvincibleItemPositionMatrix[i][0] == pacmanLocationRow) && (InvincibleItemPositionMatrix[i][1] == pacmanLocationColumn)){
                isPacmanInvincible = true;
                InvincibleItemPositionMatrix[i][0] = 8;
                InvincibleItemPositionMatrix[i][1] = 2;
                map[9][9]='#';
                map[11][9]='#'; //close doors of the spawn until invincibility ends
                gameTicks = 0;
            }
        }
        if(!isPacmanDead && !isPacmanInvincible){ //if we detected the situation above, don't do anything
            pacmanMovement(&pacmanLocationColumn, &pacmanLocationRow, map, &isPacmanDead, GhostLocationMatrix);
            GhostMovement(GhostLocationMatrix, map, pacmanLocationColumn, pacmanLocationRow);
            printMap(map, pacmanLocationRow, pacmanLocationColumn, GhostLocationMatrix, InvincibleItemPositionMatrix, isPacmanInvincible);
        } 
        else if(!isPacmanDead && isPacmanInvincible && (gameTicks<20)){
            pacmanMovementInvincible(&pacmanLocationColumn, &pacmanLocationRow, map);
            GhostMovementInvincible(GhostLocationMatrix, map, pacmanLocationColumn, pacmanLocationRow, gameTicks);
            eatGhosts(pacmanLocationColumn, pacmanLocationRow, map, GhostLocationMatrix);
            printMap(map, pacmanLocationRow, pacmanLocationColumn, GhostLocationMatrix, InvincibleItemPositionMatrix, isPacmanInvincible);
            gameTicks++;
        }
        else if(gameTicks==20){
            isPacmanInvincible=false;
            gameTicks=0;
            map[9][9]=' ';
            map[11][9]=' '; //open doors when invincibility ends
        }
    }
    endwin();
    return 0;
} 

void printMap(char matrix[22][19], int pacmanPositionRow, int pacmanPositionColumn, int GhostPositions[4][2], int invincibleMatrix[4][2], bool isPacmanInvincible) {
    clear();
    for (int i = 0; i < 22; i++) {
        for (int j = 0; j < 19; j++) {
            if ((pacmanPositionRow == i) && (pacmanPositionColumn == j)) {
                mvprintw(i, j, "C");
                matrix[pacmanPositionRow][pacmanPositionColumn]=' ';
            } else if ((GhostPositions[0][0] == i) && GhostPositions[0][1] == j) {
                mvprintw(i, j, "%c", isPacmanInvincible ? '@' : '0');
            } else if ((GhostPositions[1][0] == i) && GhostPositions[1][1] == j) {
                mvprintw(i, j, "%c", isPacmanInvincible ? '@' : '1');
            } else if ((GhostPositions[2][0] == i) && GhostPositions[2][1] == j) {
                mvprintw(i, j, "%c", isPacmanInvincible ? '@' : '2');
            } else if ((GhostPositions[3][0] == i) && GhostPositions[3][1] == j) {
                mvprintw(i, j, "%c", isPacmanInvincible ? '@' : '3');
            } else if ((invincibleMatrix[0][0] == i && invincibleMatrix[0][1] == j) ||
                       (invincibleMatrix[1][0] == i && invincibleMatrix[1][1] == j) ||
                       (invincibleMatrix[2][0] == i && invincibleMatrix[2][1] == j) ||
                       (invincibleMatrix[3][0] == i && invincibleMatrix[3][1] == j)) {
                mvprintw(i, j, "%%");
            } else {
                mvprintw(i, j, "%c", matrix[i][j]);
            }
        }
    }
    refresh();
}


bool isValidMovement(int pacmanPosCol, int pacmanPosRow, char map[22][19], char desiredMovement, bool * isDead, int GhostPositions[4][2]){

    switch(desiredMovement){
        case 'w': 
            //check if there is a ghost in that direction and update pacman dead/alive
            for(int i=0; (i<4) && !*isDead; i++){
                if(((pacmanPosRow-1==GhostPositions[i][0]) && (pacmanPosCol==GhostPositions[i][1])) || ((pacmanPosRow==GhostPositions[i][0]) && (pacmanPosCol==GhostPositions[i][1]))){
                    *isDead=true;
                    return false; //dying is also a not valid movement
                }
            }
            if(map[pacmanPosRow-1][pacmanPosCol]=='#'){
                return false;
            }
            break;
        case 'a':
            //check if there is a ghost in that direction and update pacman dead/alive
            for(int i=0; (i<4) && !*isDead; i++){
                if(((pacmanPosRow==GhostPositions[i][0]) && (pacmanPosCol-1==GhostPositions[i][1])) || ((pacmanPosRow==GhostPositions[i][0]) && (pacmanPosCol==GhostPositions[i][1]))){
                    * isDead=true;
                    return false; //dying is also a not valid movement
                }
            }
            if(map[pacmanPosRow][pacmanPosCol-1]=='#'){
                return false;
            }
            break;
        case 's':
            //check if there is a ghost in that direction and update pacman dead/alive
            for(int i=0; (i<4) && !*isDead; i++){
                if(((pacmanPosRow+1==GhostPositions[i][0]) && (pacmanPosCol==GhostPositions[i][1])) || ((pacmanPosRow==GhostPositions[i][0]) && (pacmanPosCol==GhostPositions[i][1]))){
                    * isDead=true;
                    return false; //dying is also a not valid movement
                }
            }
            if(map[pacmanPosRow+1][pacmanPosCol]=='#'){
                return false;
            }
            break;
        case 'd':
            //check if there is a ghost in that direction and update pacman dead/alive
            for(int i=0; (i<4) && !*isDead; i++){
                if(((pacmanPosRow==GhostPositions[i][0]) && (pacmanPosCol+1==GhostPositions[i][1])) || ((pacmanPosRow==GhostPositions[i][0]) && (pacmanPosCol==GhostPositions[i][1]))){
                    * isDead=true;
                    return false; //dying is also a not valid movement
                }
            }
            if(map[pacmanPosRow][pacmanPosCol+1]=='#'){
                return false;
            }
            break;
    }
    return true;
}

void pacmanMovement(int *ColumnNumber, int *RowNumber, char map[22][19], bool *isDead, int GhostPositions[4][2]) {
    char movement = getch();
    while (movement != 'w' && movement != 'a' && movement != 's' && movement != 'd') {
        movement = getch();
    }

    // Logic for movement
    if ((!((*RowNumber == 10) && (*ColumnNumber == 0)) || ((movement == 'w') || (movement == 's'))) &&
        (!((*RowNumber == 10) && (*ColumnNumber == 18)) || ((movement == 'w') || (movement == 's'))) &&
        !isValidMovement(*ColumnNumber, *RowNumber, map, movement, isDead, GhostPositions)) {
        movement = '<'; // Restrict movement when trying to teleport
    }

    switch (movement) {
        case '<':
            break;
        case 'w':
            *RowNumber -= 1;
            break;
        case 'a':
            if ((*RowNumber == 10) && (*ColumnNumber == 0)) {
                *ColumnNumber = 18; // Teleportation
            } else {
                *ColumnNumber -= 1;
            }
            break;
        case 's':
            *RowNumber += 1;
            break;
        case 'd':
            if ((*RowNumber == 10) && (*ColumnNumber == 18)) {
                *ColumnNumber = 0; // Teleportation
            } else {
                *ColumnNumber += 1;
            }
            break;
    }
}


void GhostMovement(int GhostPositionMatrix[4][2], char map[22][19], int pacmanPositionColumn, int pacmanPositionRow){
    //module makes every ghost move whenever it is called

    //ghost movement is determined by minimisining manhattan distance. If the position of the ghost has a movement to a position that is not a wall 
    //that also makes distance to pacman lower, then the ghost shall move that way

    
    for(int i=0; i<4; i++){//check if in all directions the ghost can move(blank space) and also minimises distance FOR ALL GHOST (use of the for loop)

    //up movement
    if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'w') && (map[GhostPositionMatrix[i][0] - 1][GhostPositionMatrix[i][1]] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) > manhattanDistance(pacmanPositionColumn, pacmanPositionRow + 1, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][0] -= 1;
    }

    //down movement
    else if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 's') && (map[GhostPositionMatrix[i][0] + 1][GhostPositionMatrix[i][1]] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) > manhattanDistance(pacmanPositionColumn, pacmanPositionRow - 1, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][0] += 1;
    }

    //left movement
    else if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'a') && (map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] - 1] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) > manhattanDistance(pacmanPositionColumn +1, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][1] -= 1;
    } 

    //right movement
    else if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'd') && (map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] + 1] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) > manhattanDistance(pacmanPositionColumn - 1, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][1] += 1;
    } 
    else{ //if no movents reduce dist., movement is random
        switch(rand() % 4){
            case 0: //UP
                if((map[GhostPositionMatrix[i][0] - 1][GhostPositionMatrix[i][1]]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'w')){
                    GhostPositionMatrix[i][0] -= 1;
                }
            case 1: //DOWN
                if((map[GhostPositionMatrix[i][0] + 1][GhostPositionMatrix[i][1]]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 's')){
                    GhostPositionMatrix[i][0] += 1;
                }
            case 2: //LEFT
                if((map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] - 1]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'a')){
                    GhostPositionMatrix[i][1] -= 1;
                }
            case 3: //RIGHT
                if((map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] + 1]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'd')){
                    GhostPositionMatrix[i][1] += 1;
                }
        }
        }
    }
}

bool pacmanAteAllDots(char map[22][19]){
    
    bool sawPoint=false;

    for(int i=0; (i<22) && !sawPoint; i++){
        for(int j=0; (j<19) && !sawPoint; j++){
            if(map[i][j]=='.'){
                sawPoint=true;
            }
        }
    }
    return !sawPoint;
}

int manhattanDistance(int PacmanPCol, int PacmanPRow, int GhostPCol, int GhostPRow){
    int distanceRow = 0;
    int distanceCol = 0;

    distanceRow = (GhostPRow<=PacmanPRow) ? (PacmanPRow-GhostPRow) : (GhostPRow - PacmanPRow);
    distanceCol = (GhostPCol<=PacmanPCol) ? (PacmanPCol-GhostPCol) : (GhostPCol - PacmanPCol);

    return distanceRow + distanceCol;
}

bool isValidMovementGhost(int ghostPosRow, int ghostPosCol, int GhostPositionMatrix[4][2], char movement){
    switch (movement){
        case 'w':
            for(int i=0; i<4; i++){
                if(ghostPosRow-1==GhostPositionMatrix[i][0] && (ghostPosCol==GhostPositionMatrix[i][1])){
                    return false;
                }
            }
            break;
        case 's':
            for(int i=0; i<4; i++){
                if((ghostPosRow+1==GhostPositionMatrix[i][0]) && (ghostPosCol==GhostPositionMatrix[i][1])){
                    return false;
                }
            }
            break;
        case 'a':
            for(int i=0; i<4; i++){
                if((ghostPosCol-1==GhostPositionMatrix[i][1]) && (ghostPosRow==GhostPositionMatrix[i][0])){
                    return false;
                }
            }
            break;
        case 'd':
            for(int i=0; i<4; i++){
                if((ghostPosCol+1==GhostPositionMatrix[i][1]) && (ghostPosRow==GhostPositionMatrix[i][0])){
                    return false;
                }
            }
            break;
    }
    return true;
}

void pacmanMovementInvincible(int * ColumnNumber, int * RowNumber, char map[22][19]){

    char movement = getch();

    while (movement != 'w' && movement != 'a' && movement != 's' && movement != 'd') {
        movement = getch();
    }

    bool isValidVerticalMovement = (movement == 'w' || movement == 's');

    if(((((*RowNumber==10) && (*ColumnNumber==0)) && !isValidVerticalMovement) && (((*RowNumber==10) || (*ColumnNumber==18)) && !isValidVerticalMovement)) || isMovementColision(*ColumnNumber, *RowNumber, map, movement)){
            movement='<';
    }//this if statement serves as 1. let pacman move only to left or right when we are triyng to teleport 2. check that the movement is valid (no walls)

    switch(movement){
        case '<':
            break;
        case 'w':
            *RowNumber-=1;
            break;
        case 'a':
            if((*RowNumber==10) && (*ColumnNumber==0)){ //teleporting for programmers :)
                *ColumnNumber=18;
            } else{
                *ColumnNumber-=1;
            }
            break;
        case 's':
            *RowNumber+=1;
            break;
        case 'd':
            if((*RowNumber==10) && (*ColumnNumber==18)){
                *ColumnNumber=0;
            } else{
                *ColumnNumber+=1;
            }
            break;
    }
}

void GhostMovementInvincible(int GhostPositionMatrix[4][2], char map[22][19], int pacmanPositionColumn, int pacmanPositionRow, int gameTicks){
    
    if(gameTicks%2==0){
    for(int i=0; i<4; i++){//check if in all directions the ghost can move(blank space) and also maximises distance FOR ALL GHOST (use of the for loop)
        if((GhostPositionMatrix[i][0]==pacmanPositionRow) && GhostPositionMatrix[i][1]==pacmanPositionColumn){
            continue;
        } // if pacman is in the same tale as the current ghost, then this ghost shall not move and we'll check for the next one
    //up movement
    if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'w') && (map[GhostPositionMatrix[i][0] - 1][GhostPositionMatrix[i][1]] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) < manhattanDistance(pacmanPositionColumn, pacmanPositionRow + 1, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][0] -= 1;
    }

    //down movement
    else if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 's') && (map[GhostPositionMatrix[i][0] + 1][GhostPositionMatrix[i][1]] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) < manhattanDistance(pacmanPositionColumn, pacmanPositionRow - 1, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][0] += 1;
    }

    //left movement
    else if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'a') && (map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] - 1] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) < manhattanDistance(pacmanPositionColumn +1, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][1] -= 1;
    } 

    //right movement
    else if(isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'd') && (map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] + 1] != '#') && (manhattanDistance(pacmanPositionColumn, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]) < manhattanDistance(pacmanPositionColumn - 1, pacmanPositionRow, GhostPositionMatrix[i][1], GhostPositionMatrix[i][0]))){
        GhostPositionMatrix[i][1] += 1;
    } 
    else{ //if no movents reduce dist., movement is random
        switch(rand() % 4){
            case 0: //UP
                if((map[GhostPositionMatrix[i][0] - 1][GhostPositionMatrix[i][1]]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'w')){
                    GhostPositionMatrix[i][0] -= 1;
                }
            case 1: //DOWN
                if((map[GhostPositionMatrix[i][0] + 1][GhostPositionMatrix[i][1]]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 's')){
                    GhostPositionMatrix[i][0] += 1;
                }
            case 2: //LEFT
                if((map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] - 1]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'a')){
                    GhostPositionMatrix[i][1] -= 1;
                }
            case 3: //RIGHT
                if((map[GhostPositionMatrix[i][0]][GhostPositionMatrix[i][1] + 1]!='#') && isValidMovementGhost(GhostPositionMatrix[i][0], GhostPositionMatrix[i][1], GhostPositionMatrix, 'd')){
                    GhostPositionMatrix[i][1] += 1;
                }
        }
        }
    }
    }
}

bool isMovementColision(int pacmanPosCol, int pacmanPosRow, char map[22][19], char desiredMovement){
    switch(desiredMovement){
        case 'w':
            if(map[pacmanPosRow - 1][pacmanPosCol]=='#'){
                return true;
            }
            break;       
        case 'a':
            if(map[pacmanPosRow][pacmanPosCol - 1]=='#'){
                return true;
            }
            break;       
        case 's':
            if(map[pacmanPosRow + 1][pacmanPosCol]=='#'){
                return true;
            }
            break;       
        case 'd':
            if(map[pacmanPosRow][pacmanPosCol + 1]=='#'){
                return true;
            }
            break;       

    }
    return false;
}

void eatGhosts(int pacmanPosCol, int pacmanPosRow, char map[22][19], int GhostPositionMatrix[4][2]){
    for(int i=0; i<4; i++){
        if((pacmanPosRow==GhostPositionMatrix[i][0]) && (pacmanPosCol==GhostPositionMatrix[i][1])){
            GhostPositionMatrix[i][0] = 10;
            GhostPositionMatrix[i][1] = 8 + (rand() % 3);
        }
    }
}