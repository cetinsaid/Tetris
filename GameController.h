#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    bool play(BlockFall &game, const string &commands_file); // Function that implements the gameplay
    void moveRight(BlockFall * block);
    void moveLeft(BlockFall * block);
    void drop(BlockFall * block);
    void rotateRight(BlockFall * block);
    void rotateLeft(BlockFall * block);
    void pushBlock(Block* currentBlock, BlockFall* game);
    void printGrid(BlockFall* game);
    void switchGravity(BlockFall* game);
    void testDrop(BlockFall* game);
    int score;
    bool canEnter;
    bool gameStatue;

};


#endif //PA2_GAMECONTROLLER_H
