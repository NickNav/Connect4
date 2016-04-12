#ifndef CON4
#include "con4lib.h"
#endif

#ifndef GROUP
#define GROUP
#endif




typedef struct trie{
    int x;
    int y;
    long value;
    int depth;
    char board[6][7];
    struct trie* nextMove[7];
}trie;



void trieInsert(trie* root,int curX,int curY,struct connect4* game1,int totalScore);
trie* initialize();
int getTheMove(const struct connect4* game, int secondsLeft);
void score(trie* root, struct connect4* game1);
int getScore(struct connect4* game1, int curX, int curY);
int streakPossible(struct connect4* game1, int x, int y, int j, char piece, int* DX, int* DY);
void freeThisMess(trie* root);
