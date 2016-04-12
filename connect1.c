#include "connect.h"



// Inserts a child containing the possible move, the score of that move,
// how far down we are the trie and the state of the board if that move would be made.
void trieInsert(trie* root,int curX,int curY,struct connect4* game1,int totalScore){
    int i, j;
    root->nextMove[curY] = initialize();
    root->nextMove[curY]->depth = (root->depth)+1;
    root->nextMove[curY]->x = curX;
    root->nextMove[curY]->y = curY;
    root->nextMove[curY]->value = totalScore;
    for( i = 0; i < 6; i++){
        for( j = 0; j < 7; j++)
            root->nextMove[curY]->board[i][j] = game1->board[i][j];
    }
    root->nextMove[curY]->board[curX][curY] = game1->whoseTurn;




}

// Creates a node ready to be used.
trie* initialize(){
    int i;
    trie* root = malloc(sizeof(trie));
    root->depth = 0;
    for(i = 0; i < 7; i++)
        root->nextMove[i] = NULL;
    return root;
}

// Function called from main.
int getTheMove(const struct connect4* game, int secondsLeft){

    int a,b,c,d, maxScoreIndex;
    long maxScore = 0;
    // The struct that will be part of the trie.
    trie* root = initialize();
    // Saves the struct from the con4 library.
    struct connect4* game1 = copy(game);

    // From here forwards comes all the scoring for all the moves and then bubbling up the values. Don't know how to do it
    // recursively yet. Maybe it is better this way, I don't know. Since every level get larger by a factor of 7, I add another
    // for loop. At the one with 4 for loops, I score and find the max values at the same time. When adding up scores to the parent
    // I start from the 4 for loops down to the single since you need to start adding up the cores bottom to top. They way I made it
    // it is designed to have the addition of all the scores at the surface nodes (the nodes containing the possible moves) and then
    // choosing which one has the highest score.

    score(root,game1);

    for(a = 0; a < 7; a++){
        if(root->nextMove[a] != NULL)
            score(root->nextMove[a],game1);
    }

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL)
                score(root->nextMove[a]->nextMove[b],game1);
        }
    }

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            for(c = 0; c < 7; c++){
                if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL && root->nextMove[a]->nextMove[b]->nextMove[c] != NULL)
                    score(root->nextMove[a]->nextMove[b]->nextMove[c],game1);
            }
        }
    }

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            for(c = 0; c < 7; c++){
                for(d = 0; d < 7; d++){
                    if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL && root->nextMove[a]->nextMove[b]->nextMove[c] != NULL &&
                    root->nextMove[a]->nextMove[b]->nextMove[c]->nextMove[d] != NULL){
                        score(root->nextMove[a]->nextMove[b]->nextMove[c]->nextMove[d],game1);
                        if(root->nextMove[a]->nextMove[b]->nextMove[c]->nextMove[d]->value >= maxScore)
                            maxScore = root->nextMove[a]->nextMove[b]->nextMove[c]->nextMove[d]->value;
                    }
                }
                root->nextMove[a]->nextMove[b]->nextMove[c]->value += maxScore;
                maxScore = 0;
            }
        }
    }

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            for(c = 0; c < 7; c++){
                if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL && root->nextMove[a]->nextMove[b]->nextMove[c] != NULL){
                    if(root->nextMove[a]->nextMove[b]->nextMove[c]->value >= maxScore)
                        maxScore = root->nextMove[a]->nextMove[b]->nextMove[c]->value;
                }
            }
            root->nextMove[a]->nextMove[b]->value += maxScore;
            maxScore = 0;
        }
    }

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL){
                if(root->nextMove[a]->nextMove[b]->value >= maxScore)
                    maxScore = root->nextMove[a]->nextMove[b]->value;

            }
        }
        root->nextMove[a]->value += maxScore;
        maxScore = 0;
    }

    for(a = 0; a < 7; a++){
        if(root->nextMove[a] != NULL){
            if(root->nextMove[a]->value >= maxScore){
                maxScoreIndex = a;
                maxScore = root->nextMove[a]->value;
            }
        }
    }

    freeThisMess(root);
    free(root);
    return root->nextMove[maxScoreIndex]->y;


}

void score(trie* root, struct connect4* game1){
    int* possibleMoves;
    int i, j, totalScore, curX, curY;
    char theirPiece;

    possibleMoves = get_possible_moves(game1);

    if(game1->whoseTurn == 'X')
        theirPiece = 'O';
    else
        theirPiece = 'X';


    for(i = 0; i < 7; i++){
        if(possibleMoves[i]){
            for(j = 0; j < 6; j++){
                curY = i;
                //Finds the row#
                if( (game1->board[j][i] == 'X') || (game1->board[j][i] == 'O') ){
                    curX = j-1;
                    break;
                }
                else
                    curX = 5;
            }
            totalScore = getScore(game1, curX, curY);
            //Creates a child for every possible move.
            trieInsert(root,curX,curY,game1,totalScore);

        }
    }
}

// As of now the function is returning 0 constantly.
int getScore(struct connect4* game1, int curX, int curY){
    //We use DX/DY arrays with only 4 values because we will be checking the negatives also,
    // which will end up taking into account the opposite directions.
    int DX[] = {-1,0,1,1};
    int DY[] = {1,1,1,0};
    int i, j, points = 0, theirStreak = 1, ourStreak = 1;
    int canWeStreak, canTheyStreak;
    char theirPiece;

    if(game1->whoseTurn == 'X')
        theirPiece = 'O';
    else
        theirPiece = 'X';

    for(j = 0; j < 4; j++){
        canWeStreak = streakPossible(game1,curX,curY,j,game1->whoseTurn,DX,DY);
        canTheyStreak = streakPossible(game1,curX,curY,j,theirPiece,DX,DY);
        printf("%d",canWeStreak);
        for(i = 1; i < 4; i++){
            if(inbounds(curX+(DX[j]*i),curY+(DY[j]*i))){
                if( game1->board[curX+(DX[j]*i)][curY+(DY[j]*i)] == '-'){
                    points += 1;
                }
                else if( game1->board[curX+(DX[j]*i)][curY+(DY[j]*i)] == theirPiece){
                    points += 2;
                    if(canTheyStreak)
                        theirStreak++;
                }
                else if(canWeStreak)
                    ourStreak++;

            }

            if(inbounds(curX+(-1*DX[j]*i),curY+(-1*DY[j]*i))){
                if( game1->board[curX+(-1*DX[j]*i)][curY+(-1*DY[j]*i)] == '-')
                    points += 1;
                else if( game1->board[curX+(-1*DX[j]*i)][curY+(-1*DY[j]*i)] == theirPiece){
                    points += 2;
                    if(canTheyStreak)
                        theirStreak++;
                }
                else if(canWeStreak)
                    ourStreak++;
            }
        }
        //If we can win, default to that. Second highest priority is them winning.
        if(ourStreak == 4)
            return 60;
        else if(theirStreak == 4)
            return 20;
        //Resets streaks before checking other directions.
        ourStreak = theirStreak = 1;
        }
    // Return points if there are no winning streaks.
    return points;
}

//A streak is only possible if there is at least one piece of your own next to the spot where
// it's going to be placed. Checks one spot ahead in a certain direction and one spot back
// in the opposite direction.
int streakPossible(struct connect4* game1, int x, int y, int j, char piece, int* DX, int* DY){
    if(inbounds(x+(DX[j]),y+(DY[j]))){
        if( (game1->board[x+(DX[j])][y+(DY[j])] == piece) )
            return 1;
    }

    if(inbounds(x+(-1*DX[j]),y+(-1*DY[j]))){
        if( (game1->board[x+(-1*DX[j])][y+(-1*DY[j])] == piece) )
            return 1;
    }

    return 0;
}

// I know how to make this one recursive, I just don't feel like testing if it's going to overflow
// yet.
void freeThisMess(trie* root){
    int a,b,c,d;

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            for(c = 0; c < 7; c++){
                for(d = 0; d < 7; d++){
                    if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL && root->nextMove[a]->nextMove[b]->nextMove[c] != NULL &&
                    root->nextMove[a]->nextMove[b]->nextMove[c]->nextMove[d] != NULL)
                        free(root->nextMove[a]->nextMove[b]->nextMove[c]->nextMove[d]);
                }
            }
        }
    }

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            for(c = 0; c < 7; c++){
                if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL && root->nextMove[a]->nextMove[b]->nextMove[c] != NULL)
                    free(root->nextMove[a]->nextMove[b]->nextMove[c]);
            }
        }
    }

    for(a = 0; a < 7; a++){
        for(b = 0; b < 7; b++){
            if(root->nextMove[a] != NULL && root->nextMove[a]->nextMove[b] != NULL)
                free(root->nextMove[a]->nextMove[b]);
        }
    }

    for(a = 0; a < 7; a++){
        if(root->nextMove[a] != NULL)
            free(root->nextMove[a]);
    }
}
