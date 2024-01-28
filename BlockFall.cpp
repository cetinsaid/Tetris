#include "BlockFall.h"
#include <fstream>
#include <iostream>

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);

}

void BlockFall::read_blocks(const string &input_file) {

    std::ifstream blockFile(input_file);
    std::string line = "";
    if(blockFile.is_open()) {
        initial_block = new Block;
        Block* temp = initial_block;
        int rows1 =0;
        while (std::getline(blockFile, line)) {
            if(line.size() !=0){
                temp->shape.push_back(std::vector<bool>());
                for (int i = line.size()-1 ; i >= 0 ; --i) {
                    if(line[i] == '[' || line[i] == ']'){
                        line.erase(i,1);
                    }
                }
                for (int i = 0; i < line.size(); ++i) {
                    if(line[i] == '1'){
                        temp->shape[rows1].push_back(true);
                    }else if(line[i] == '0'){
                        temp->shape[rows1].push_back(false);
                    }
                }
                Block* firstRight = rotate_right(temp);
                Block* secondRight = rotate_right(firstRight);
                Block* left = rotate_left(temp);

                temp->right_rotation = firstRight;
                temp->left_rotation = left;

                firstRight->right_rotation = secondRight;
                firstRight->left_rotation = temp;

                secondRight->left_rotation = firstRight;
                secondRight->right_rotation = left;

                left->left_rotation = secondRight;
                left->right_rotation =temp;

                rows1++;
            }
            else if(line.size() == 0){
                Block* block = new Block;
                if(temp->shape.size() !=0){
                    temp->next_block = block;
                    temp = temp->next_block;
                }
                else{
                    delete block;
                }
                rows1 = 0;
            }
        }
        blockFile.close();
    }
    Block* temp = initial_block;
    while (temp != nullptr) {
        if (temp->right_rotation != nullptr) {
            temp->right_rotation->next_block = temp->next_block;
            if (temp->right_rotation->right_rotation != nullptr) {
                temp->right_rotation->right_rotation->next_block = temp->next_block;
                if (temp->right_rotation->right_rotation->right_rotation != nullptr) {
                    temp->right_rotation->right_rotation->right_rotation->next_block = temp->next_block;
                }
            }
        }
        temp = temp->next_block;
    }
    removeLast();
    active_rotation = initial_block;







    // TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    // TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
    //       that represents the game blocks, as explained in the PA instructions.
    // TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)
}

void BlockFall::initialize_grid(const string &input_file) {
    std::ifstream gridFile(input_file);
    std::string line = "";
    rows = 0;
    cols = 0;

    if(gridFile.is_open()) {
        while (std::getline(gridFile, line)) {
            grid.push_back(std::vector<int>());
            for (int i = 0; i < line.size(); i += 2) {
                grid[rows].push_back(static_cast<int>(line[i]) - '0');
            }
            cols = (line.size() + 1) / 2;
            rows++;
        }
        gridFile.close();
    }


    // TODO: Initialize "rows" and "cols" member variables
    // TODO: Initialize "grid" member variable using the command-line argument 1 in main
}


BlockFall::~BlockFall() {
    Block* current = initial_block;
    Block* next;
    while(current != nullptr){
        Block* rightRotation =current->right_rotation;
        Block* secondRight = current->right_rotation->right_rotation;
        Block* thirdRight = current->left_rotation;
        next = current->next_block;
        thirdRight->right_rotation = nullptr;
        thirdRight->left_rotation = nullptr;
        secondRight->right_rotation = nullptr;
        current->left_rotation = nullptr;
        delete thirdRight;

        secondRight->left_rotation = nullptr;
        rightRotation->right_rotation = nullptr;
        delete secondRight;

        rightRotation->left_rotation = nullptr;
        current->right_rotation = nullptr;
        delete rightRotation;

        delete current;
        current = next;
    }
    initial_block = nullptr;

    delete powerUp;







    // TODO: Free dynamically allocated memory used for storing game blocks
}

Block* BlockFall::rotate_right(const Block *block) {
    Block* rightRotatedBlock = new Block;
    for (int i = 0; i < block->shape[0].size(); ++i) {
        rightRotatedBlock->shape.push_back(std::vector<bool>());
        for (int j = 0; j < block->shape.size(); ++j) {
            rightRotatedBlock->shape[i].push_back(block->shape[j][i]);
        }
    }
    for (int i = 0; i < rightRotatedBlock->shape.size(); ++i) {
        for (int j = 0; j < rightRotatedBlock->shape[0].size()/2; ++j) {
            bool first = rightRotatedBlock->shape[i][j];
            bool second = rightRotatedBlock->shape[i][rightRotatedBlock->shape[0].size()-j-1];
            rightRotatedBlock->shape[i][j] = second;
            rightRotatedBlock->shape[i][rightRotatedBlock->shape[0].size()-j-1] = first;
        }
    }

    return rightRotatedBlock;
}

Block* BlockFall::rotate_left(const Block *block) {
    Block* firstRotation = rotate_right(block);
    Block* secondRotation = rotate_right(firstRotation);
    Block* finalRotation = rotate_right(secondRotation);
    return finalRotation;
}

void BlockFall::removeLast() {
    if(initial_block != nullptr){
        Block* current = initial_block;
        while (current->next_block->next_block != nullptr){
            current = current->next_block;
        }
        Block* willDeleted = current->next_block;
        Block* rightRotation =willDeleted->right_rotation;
        Block* secondRight = willDeleted->right_rotation->right_rotation;
        Block* thirdRight = willDeleted->left_rotation;

        thirdRight->right_rotation = nullptr;
        thirdRight->left_rotation = nullptr;
        secondRight->right_rotation = nullptr;
        willDeleted->left_rotation = nullptr;
        delete thirdRight;

        secondRight->left_rotation = nullptr;
        rightRotation->right_rotation = nullptr;
        delete secondRight;

        rightRotation->left_rotation = nullptr;
        willDeleted->right_rotation = nullptr;
        delete rightRotation;

        powerUp = new Block;
        powerUp->shape = current->next_block->shape;
        current->next_block = nullptr;
        delete willDeleted;
    }

}
