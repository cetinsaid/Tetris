#include <fstream>
#include <iostream>
#include <algorithm>
#include "GameController.h"

bool GameController::play(BlockFall& game, const string& commands_file){
    gameStatue = true;
    game.current_score = 0;
    std::ifstream commands(commands_file);
    std::string line;
    if(commands.is_open()){
        pushBlock(game.active_rotation,&game);
        while (std::getline(commands, line) && game.active_rotation->next_block != nullptr && canEnter){
            if(line == "DROP"){
                testDrop(&game);
                game.active_rotation = game.active_rotation->next_block;
                if(canEnter){
                    pushBlock(game.active_rotation , &game);
                }

            }else if(line == "MOVE_RIGHT"){
                moveRight(&game);

            }else if(line == "MOVE_LEFT"){
                moveLeft(&game);
            }else if(line == "ROTATE_RIGHT"){
                rotateRight(&game);

            }else if(line == "ROTATE_LEFT"){
                rotateLeft(&game);

            }else if(line == "PRINT_GRID"){
                printGrid(&game);
            }else if (line == "GRAVITY_SWITCH"){
                game.gravity_mode_on = !game.gravity_mode_on;
                if(game.gravity_mode_on){
                    switchGravity(&game);
                }
            }else{
                std:: cout << "UNKNOWN COMMAND: " << line << std::endl;
            }
        }
        for (int i = 0; i < game.active_rotation->blockLocation.size(); ++i) {
            if(game.active_rotation->shape[i/(game.active_rotation->shape[0].size())][i%game.active_rotation->shape[0].size()] ==1
            && game.grid[game.active_rotation->blockLocation[i].first][game.active_rotation->blockLocation[i].second]== 1){
                game.grid[game.active_rotation->blockLocation[i].first][game.active_rotation->blockLocation[i].second] = 0;
            }
        }
        if(!canEnter){
            std::cout << "GAME OVER!" << std::endl << "Next block that couldn't fit:" << std::endl;
            for (const auto& row : game.active_rotation->shape) {
                for (int value : row) {
                    if(value == 1){
                        std::cout<<occupiedCellChar;
                    }
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            gameStatue = false;
        }
        else if(!std::getline(commands,line)){
            std::cout << "GAME FINISHED!"<< std::endl << "No more commands." << std::endl;
            gameStatue = true;
        }else{
            std::cout << "YOU WIN!"<< std::endl << "No more blocks." << std::endl;
            gameStatue = true;
        }
        std::cout << "Final grid and score:" << std::endl << std::endl;
        std::cout << "Score: " << game.current_score << std::endl;
        std::cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << std::endl;
        for (const auto& row : game.grid) {
            for (int value : row) {
                if(value == 1){
                    std::cout<<occupiedCellChar;
                }else{
                    std::cout << unoccupiedCellChar;
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        LeaderboardEntry* leaderboardEntry = new LeaderboardEntry(game.current_score , time_t(nullptr) , game.player_name);
        game.leaderboard.insert_new_entry(leaderboardEntry);
        game.leaderboard.print_leaderboard();
        commands.close();
    }
    return gameStatue;
    // TODO: Implement the gameplay here while reading the commands from the input file given as the 3rd command-line
    //       argument. The return value represents if the gameplay was successful or not: false if game over,
    //       true otherwise.

}



void GameController::moveRight(BlockFall *block) {
    Block* block1 = block->active_rotation;
    bool isMovable = true;
    if(block1->blockLocation[block1->blockLocation.size()-1].second+1 >= block->grid[0].size()){
        isMovable = false;
    }
    for (int i = 0; i < block1->shape.size(); ++i) {
        int blockI = block1->blockLocation[(i+1)*block1->shape[0].size()-1].first;
        int blockJ = block1->blockLocation[(i+1)*block1->shape[0].size()-1].second;
        if(block1->shape[i][block1->shape[0].size()-1] == 1 &&
        block->grid[blockI][blockJ+1] == 1 ){
            isMovable = false;
        }

    }

    if(isMovable){
        for (int i = 1; i < block1->blockLocation.size()+1; ++i) {
            int blockI = block1->blockLocation[i].first;
            int blockJ = block1->blockLocation[i].second;

            if((i-1)%block1->shape[0].size() != (block1->shape[0].size() -1)
               && block1->shape[(i-1)/(block1->shape[0].size())][(i-1)%block1->shape[0].size()] == 1
               && block1->shape[i/(block1->shape[0].size())][i%block1->shape[0].size()] == 0
               && block->grid[blockI][blockJ] == 1){
                isMovable = false;
            }
        }
    }
    if(isMovable){
        for (int i = 0; i < block1->blockLocation.size(); ++i) {
            int blockI = block1->blockLocation[i].first;
            int blockJ = block1->blockLocation[i].second;
            block->grid[blockI][blockJ] = 0;
        }

        for (int i = 0; i < block1->blockLocation.size(); ++i) {
            int blockI = block1->blockLocation[i].first;
            int blockJ = block1->blockLocation[i].second;
            if(block1->shape[i/(block1->shape[0].size())][i%block1->shape[0].size()] ==1){
                block->grid[blockI][blockJ+1] = 1;
            }
            block1->blockLocation[i] = std::make_pair(blockI, blockJ+1);
        }
    }
}

void GameController::moveLeft(BlockFall *block) {
    Block* block1 = block->active_rotation;
    bool isMovable = true;

    if (block1->blockLocation[0].second - 1 < 0) {
        isMovable = false;
    }

    for (int i = 0; i < block1->shape.size(); ++i) {
        int blockI = block1->blockLocation[i * block1->shape[0].size()].first;
        int blockJ = block1->blockLocation[i * block1->shape[0].size()].second;

        if (block1->shape[i][0] == 1 && block->grid[blockI][blockJ - 1] == 1) {
            isMovable = false;
        }
    }

    if (isMovable) {
        for (int i = 0; i < block1->blockLocation.size(); ++i) {
            int blockI = block1->blockLocation[i].first;
            int blockJ = block1->blockLocation[i].second;

            if (i % block1->shape[0].size() != 0 &&
                block1->shape[i / block1->shape[0].size()][i % block1->shape[0].size()] == 1 &&
                block1->shape[i / block1->shape[0].size()][(i - 1) % block1->shape[0].size()] == 0 &&
                block->grid[blockI][blockJ - 1] == 1) {
                isMovable = false;
            }
        }
    }

    if (isMovable) {
        for (int i = 0; i < block1->blockLocation.size(); ++i) {
            int blockI = block1->blockLocation[i].first;
            int blockJ = block1->blockLocation[i].second;
            block->grid[blockI][blockJ] = 0;
        }

        for (int i = 0; i < block1->blockLocation.size(); ++i) {
            int blockI = block1->blockLocation[i].first;
            int blockJ = block1->blockLocation[i].second;

            if (block1->shape[i / block1->shape[0].size()][i % block1->shape[0].size()] == 1) {
                block->grid[blockI][blockJ - 1] = 1;  // Fix: set the grid value to 1
            }

            block1->blockLocation[i] = std::make_pair(blockI, blockJ - 1);
        }
    }
}


void GameController::rotateRight(BlockFall *game) {
    Block* block = game->active_rotation;
    Block* rightRotatedBlock = block->right_rotation;
    int movementCount = block->blockLocation[0].second;
    rightRotatedBlock->blockLocation.clear();
    for (int i = 0; i < block->blockLocation.size(); ++i) {
        int blockI = block->blockLocation[i].first;
        int blockJ = block->blockLocation[i].second;
        rightRotatedBlock->blockLocation.push_back(std::make_pair(blockJ - movementCount, block->shape.size()-blockI-1 + movementCount));
        if(block->shape[i/(block->shape[0].size())][i%block->shape[0].size()] ==1){
            game->grid[blockI][blockJ] = 0;
        }
    }
    std::sort(rightRotatedBlock->blockLocation.begin(),rightRotatedBlock->blockLocation.end());

    bool canRotatable = true;
    for (int i = 0; i < rightRotatedBlock->blockLocation.size(); ++i) {
        int rotatedBlockI = rightRotatedBlock->blockLocation[i].first;
        int rotatedBlockJ = rightRotatedBlock->blockLocation[i].second;

        if(rotatedBlockI >= game->grid.size() || rotatedBlockJ >= game->grid[0].size()){
            std::cout << "cant rotate out of boundry" << std::endl;
            canRotatable = false;
            break;
        }else if(game->grid[rotatedBlockI][rotatedBlockJ] == 1
                 && rightRotatedBlock->shape[i/(rightRotatedBlock->shape[0].size())][i%rightRotatedBlock->shape[0].size()] ==1){
            std::cout << "there are other block" << std::endl;
            canRotatable = false;
            break;}
    }
    if(canRotatable){
        for (int i = 0; i < rightRotatedBlock->blockLocation.size(); ++i) {
            int rotatedBlockI = rightRotatedBlock->blockLocation[i].first;
            int rotatedBlockJ = rightRotatedBlock->blockLocation[i].second;
            if(rightRotatedBlock->shape[i/(rightRotatedBlock->shape[0].size())][i%rightRotatedBlock->shape[0].size()] ==1){
                game->grid[rotatedBlockI][rotatedBlockJ] = 1;}}
        game->active_rotation = game->active_rotation->right_rotation;
    }else{
        for (int i = 0; i < block->blockLocation.size(); ++i) {
            int blockI = block->blockLocation[i].first;
            int blockJ = block->blockLocation[i].second;
            if(block->shape[i/(block->shape[0].size())][i%block->shape[0].size()] ==1){
                game->grid[blockI][blockJ] = 1;
            }}}

}

void GameController::rotateLeft(BlockFall *game) {
    Block* block = game->active_rotation;
    Block* rightRotatedBlock = block->right_rotation->right_rotation->right_rotation;

    int movementCount = block->blockLocation[0].second;
    rightRotatedBlock->blockLocation.clear();
    for (int i = 0; i < block->blockLocation.size(); ++i) {
        int blockI = block->blockLocation[i].first;
        int blockJ = block->blockLocation[i].second;
        rightRotatedBlock->blockLocation.push_back(std::make_pair(blockJ - movementCount, block->shape.size()-blockI-1 + movementCount));
        if(block->shape[i/(block->shape[0].size())][i%block->shape[0].size()] ==1){
            game->grid[blockI][blockJ] = 0;
        }
    }
    std::sort(rightRotatedBlock->blockLocation.begin(),rightRotatedBlock->blockLocation.end());

    bool canRotatable = true;
    for (int i = 0; i < rightRotatedBlock->blockLocation.size(); ++i) {
        int rotatedBlockI = rightRotatedBlock->blockLocation[i].first;
        int rotatedBlockJ = rightRotatedBlock->blockLocation[i].second;

        if(rotatedBlockI >= game->grid.size() || rotatedBlockJ >= game->grid[0].size()){
            canRotatable = false;
            break;
        }else if(game->grid[rotatedBlockI][rotatedBlockJ] == 1
                 && rightRotatedBlock->shape[i/(rightRotatedBlock->shape[0].size())][i%rightRotatedBlock->shape[0].size()] ==1){
            canRotatable = false;
            break;}
    }
    if(canRotatable){
        for (int i = 0; i < rightRotatedBlock->blockLocation.size(); ++i) {
            int rotatedBlockI = rightRotatedBlock->blockLocation[i].first;
            int rotatedBlockJ = rightRotatedBlock->blockLocation[i].second;
            if(rightRotatedBlock->shape[i/(rightRotatedBlock->shape[0].size())][i%rightRotatedBlock->shape[0].size()] ==1){
                game->grid[rotatedBlockI][rotatedBlockJ] = 1;}}
        game->active_rotation = game->active_rotation->right_rotation->right_rotation->right_rotation;
    }else{
        for (int i = 0; i < block->blockLocation.size(); ++i) {
            int blockI = block->blockLocation[i].first;
            int blockJ = block->blockLocation[i].second;
            if(block->shape[i/(block->shape[0].size())][i%block->shape[0].size()] ==1){
                game->grid[blockI][blockJ] = 1;
            }}}
}

void GameController::pushBlock(Block *currentBlock, BlockFall *game) {
    canEnter = true;
    currentBlock = game->active_rotation;
    if(game->grid[0][0] == 1 ){
        canEnter = false;
    }
    else {
        int currRow = currentBlock->shape.size();
        int currCol = currentBlock->shape[0].size();

        for (int i = currRow - 1; i >= 0; --i) {
            for (int j = 0; j < currCol; ++j) {
                for (int k = 0; k < i + 1; ++k) {
                    if (currentBlock->shape[i][j] == 1 && game->grid[k][j] == 1) {
                        canEnter = false;}
                }}}

        for (int i = 0; i < currRow; ++i) {
            for (int j = 0; j < currCol; ++j) {
                if (canEnter) {
                    if(currentBlock->shape[i][j] == 1){
                        game->grid[i][j] = currentBlock->shape[i][j];
                    }
                    currentBlock->blockLocation.push_back(std::make_pair(i,j));
                }
            }}
    }
}

void GameController::printGrid(BlockFall *game) {
    std::cout << "Score: " << game->current_score << std::endl;
    std::cout << "High Score: " << game->leaderboard.head_leaderboard_entry->score << std::endl;
    for (const auto& row : game->grid) {
        for (int value : row) {
            if(value == 1){
                std::cout<<occupiedCellChar;
            }else{
                std::cout << unoccupiedCellChar;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;


}

void GameController::switchGravity(BlockFall *game) {
    for (int i = 0; i < game->active_rotation->blockLocation.size(); ++i) {
        if(game->active_rotation->shape[i/(game->active_rotation->shape[0].size())][i%game->active_rotation->shape[0].size()] ==1
           && game->grid[game->active_rotation->blockLocation[i].first][game->active_rotation->blockLocation[i].second]== 1){
            game->grid[game->active_rotation->blockLocation[i].first][game->active_rotation->blockLocation[i].second] = 0;
        }
    }


    for (int i = game->grid.size()-1; i >= 0 ; --i) {
        for (int j = 0; j < game->grid[0].size(); ++j) {
            if(game->grid[i][j] == 1){
                int dropAmount = 0;
                while (i+dropAmount < game->grid.size()-1 && game->grid[i + dropAmount + 1][j] != 1){
                    dropAmount++;}
                if(dropAmount != 0){
                    game->grid[i][j] = 0;
                    game->grid[i+dropAmount][j] = 1;}
            }}}

    for (int i = 0; i < game->active_rotation->blockLocation.size(); ++i) {
        if(game->active_rotation->shape[i/(game->active_rotation->shape[0].size())][i%game->active_rotation->shape[0].size()] ==1
           && game->grid[game->active_rotation->blockLocation[i].first][game->active_rotation->blockLocation[i].second]== 0){
            game->grid[game->active_rotation->blockLocation[i].first][game->active_rotation->blockLocation[i].second] = 1;
        }
    }



}

void GameController::testDrop(BlockFall *game) {
    Block* activeBlock = game->active_rotation;
    int movementCount = activeBlock->blockLocation[0].second;
    std::vector<std::pair<int , std::pair<int,int> >> testCoordination;
    for (int i = 0; i< activeBlock->shape.size(); ++i) {
        for (int j = 0; j < activeBlock->shape[0].size(); ++j) {
            if(activeBlock->shape[i][j] == 1){
                for (int k = i+1; k < game->rows; ++k) {
                    if(game->grid[k][j+movementCount] == 1){
                        testCoordination.push_back(std::make_pair((k-i-1),std::make_pair(i,j)));
                    }
                }}}}
    std::sort(testCoordination.begin(),testCoordination.end());
    for (int i = testCoordination.size()-1; i >=0; --i) {
        if(testCoordination[i].first < activeBlock->shape.size() - testCoordination[i].second.first-1){
            testCoordination.erase(testCoordination.begin() + i);
        }
    }
    int dropAmount = 0;
    if(testCoordination.empty()){
        dropAmount = game->rows - activeBlock->shape.size();
    }else{
        dropAmount = testCoordination[0].first;
    }
    for (int i = 0; i < activeBlock->blockLocation.size(); ++i) {
        int blockI = activeBlock->blockLocation[i].first;
        int blockJ = activeBlock->blockLocation[i].second;
        if(dropAmount > 0){
            game->grid[blockI][blockJ] = 0;
        }
    }
    int filledBlocks = 0;
    for (int i = 0; i < activeBlock->blockLocation.size(); ++i) {
        int blockI = activeBlock->blockLocation[i].first;
        int blockJ = activeBlock->blockLocation[i].second;

        if (activeBlock->shape[i / activeBlock->shape[0].size()][i % activeBlock->shape[0].size()] == 1) {
            filledBlocks++;
            game->grid[blockI + dropAmount][blockJ] = 1;  // Fix: set the grid value to 1
        }
        activeBlock->blockLocation[i] = std::make_pair(blockI + dropAmount, blockJ);
    }
    game->current_score += filledBlocks * dropAmount;
    if(game->gravity_mode_on){
        for (int i = activeBlock->blockLocation.size()-1; i >=0; --i) {
            int blockI = activeBlock->blockLocation[i].first;
            int blockJ = activeBlock->blockLocation[i].second;
            int extraDrop = 0;
            if (activeBlock->shape[i / activeBlock->shape[0].size()][i % activeBlock->shape[0].size()] == 1 && blockI != game->grid.size() -1) {
                while (blockI+extraDrop < game->grid.size()-1 && game->grid[blockI + extraDrop + 1][blockJ] != 1){
                    extraDrop++;}
                if(extraDrop >0){
                    game->grid[blockI][blockJ] = 0;
                    game->grid[blockI +extraDrop][blockJ] = 1;
                }
            }
        }
    }
    bool found = false;
    for (int row = 0; row < game->grid.size() - game->powerUp->shape.size() + 1; ++row) {
        for (int col = 0; col < game->grid[0].size() - game->powerUp->shape[0].size() + 1; ++col) {
            found = true;
            for (int i = 0; i < game->powerUp->shape.size() && found; ++i) {
                for (int j = 0; j < game->powerUp->shape[0].size(); ++j) {
                    if (game->powerUp->shape[i][j] != game->grid[row + i][col + j]) {
                        found = false;
                        break;
                    }
                }
            }
            if(found){
                game->current_score +=1000;
                for (int i = 0; i < game->grid.size(); ++i) {
                    for (int j = 0; j < game->grid[0].size(); ++j) {
                        if(game->grid[i][j] ==1){
                            game->current_score++;
                            game->grid[i][j] = 0;
                        }
                    }
                }
            }
        }}




    bool willClear = false;
    for (int i = game->grid.size() - 1 ; i >= 0 ; --i) {
        int oneCount = 0 ;
        for (int j = 0; j < game->grid[0].size(); ++j) {
            if(game->grid[i][j] == 1){
                oneCount++;
            }
        }
        if(oneCount == game->grid[0].size()){
            willClear = true;
        }
    }
    if(willClear){
        std::cout << "Before clearing:" << std::endl;
        for (const auto& row : game->grid) {
            for (int value : row) {
                if(value == 1){
                    std::cout<<occupiedCellChar;
                }else{
                    std::cout << unoccupiedCellChar;
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
        for (int i = game->grid.size() - 1 ; i >= 0 ; --i) {
            int oneCount = 0 ;
            for (int j = 0; j < game->grid[0].size(); ++j) {
                if(game->grid[i][j] == 1){
                    oneCount++;
                }
            }
            if(oneCount == game->grid[0].size()){
                game->current_score += game->grid[0].size();
                for (int j = i-1; j > 0; --j) {
                    game->grid[j+1] = game->grid[j];
                }
                i++;
            }
        }

    }

}



