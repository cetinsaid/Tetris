#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "Leaderboard.h"

void Leaderboard::insert_new_entry(LeaderboardEntry *new_entry) {
        if (head_leaderboard_entry == nullptr) {
            head_leaderboard_entry = new_entry;
            size++;
        } else if (new_entry->score > head_leaderboard_entry->score ||
                   (new_entry->score == head_leaderboard_entry->score &&
                    new_entry->last_played < head_leaderboard_entry->last_played)) {
            new_entry->next_leaderboard_entry = head_leaderboard_entry;
            head_leaderboard_entry = new_entry;
            size++;
        } else if (new_entry->score == head_leaderboard_entry->score &&
                   new_entry->last_played == head_leaderboard_entry->last_played) {
            new_entry->next_leaderboard_entry = head_leaderboard_entry->next_leaderboard_entry;
            head_leaderboard_entry->next_leaderboard_entry = new_entry;
            size++;
        }
        else {
            LeaderboardEntry *current = head_leaderboard_entry;
            LeaderboardEntry *previous = nullptr;

            while (current &&
                   (current->score > new_entry->score ||
                    (current->score == new_entry->score &&
                     current->last_played < new_entry->last_played))) {
                previous = current;
                current = current->next_leaderboard_entry;
            }

            if (current && current->score == new_entry->score &&
                current->last_played == new_entry->last_played) {
                new_entry->next_leaderboard_entry = current->next_leaderboard_entry;
                current->next_leaderboard_entry = new_entry;
                size++;
            } else {
                new_entry->next_leaderboard_entry = current;
                previous->next_leaderboard_entry = new_entry;
                size++;
            }

            if (size >= 10) {
                LeaderboardEntry *temp = head_leaderboard_entry;
                while (temp->next_leaderboard_entry->next_leaderboard_entry != nullptr) {
                    temp = temp->next_leaderboard_entry;
                }
                LeaderboardEntry *willDeleted = temp->next_leaderboard_entry;
                temp->next_leaderboard_entry = nullptr;
                delete willDeleted;
                size--;
            }
        }

}


void Leaderboard::write_to_file(const string& filename) {
    std::ofstream leaderBoard(filename);
    LeaderboardEntry* temp = head_leaderboard_entry;
    while(temp != nullptr){
        leaderBoard << temp->score << " " << temp->last_played << " "<< temp->player_name << std::endl;
        temp = temp->next_leaderboard_entry;
    }
    leaderBoard.close();
    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions
}

void Leaderboard::read_from_file(const string& filename) {
    std::ifstream leaderBoard(filename);
    std::string line;
    if(leaderBoard.is_open()){
        LeaderboardEntry* temp;
        while (std::getline(leaderBoard,line) && size <= 10){
            std::istringstream iss(line);
            std::string element;
            std::vector<std::string> elements;
            while (iss >> element) {
                elements.push_back(element);
            }
            unsigned long score = std::stoul(elements[0]);
            std::string timestampString = elements[1];
            std::time_t timestamp = std::stoi(timestampString);
            std::string playerName = elements[2];
            if(head_leaderboard_entry == nullptr){
                head_leaderboard_entry = new LeaderboardEntry(score,timestamp,playerName);
                temp = head_leaderboard_entry;
                size++;
            }else{
                LeaderboardEntry* leaderboardEntry = new LeaderboardEntry(score,timestamp,playerName);
                temp->next_leaderboard_entry = leaderboardEntry;
                size++;
                temp = temp->next_leaderboard_entry;
            }
        }
        leaderBoard.close();
    }



    // TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    //       variable will point to the highest all-times score, and all other scores will be reachable from it
    //       via the "next_leaderboard_entry" member variable pointer.
}


void Leaderboard::print_leaderboard() {
    LeaderboardEntry* temp = head_leaderboard_entry;
    int number = 1;
    std::cout << "Leaderboard" << std::endl << "-----------" << std::endl;
    while (temp != nullptr){
        std::tm *timeinfo = std::gmtime(&temp->last_played);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S/%d.%m.%Y", timeinfo);
        std::cout<< number <<". " << temp->player_name << " " << temp->score << " " << buffer << std::endl;
        temp = temp->next_leaderboard_entry;
        number++;
    }

    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
}

Leaderboard::~Leaderboard() {
    LeaderboardEntry* current = head_leaderboard_entry;
    LeaderboardEntry* next;

    while (current != nullptr)
    {
        next = current->next_leaderboard_entry;
        delete current;
        current = next;
    }

    head_leaderboard_entry= nullptr;
}


    // TODO: Free dynamically allocated memory used for storing leaderboard entries

