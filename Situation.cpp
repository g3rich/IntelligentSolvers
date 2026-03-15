#include "Situation.h"
#include <iostream>
#include <algorithm>
#include <sstream>

Situation::Situation() {
    rng.seed(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));
    initBoard();
}

Situation::Situation(const std::vector<std::vector<int>>& b) {
    rng.seed(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));
    board = b;
    // найти пустую клетку
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == 0) {
                emptyX = i;
                emptyY = j;
            }
}

Situation Situation::random(int shuffleSteps) {
    Situation s;
    s.shuffleBoard(shuffleSteps);
    return s;
}

void Situation::initBoard() {
    board.assign(SIZE, std::vector<int>(SIZE));
    int k = 1;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j) {
            if (i == SIZE - 1 && j == SIZE - 1) {
                board[i][j] = 0;
                emptyX = i; emptyY = j;
            } else {
                board[i][j] = k++;
            }
        }
}

void Situation::initConcreteBoard() {
    board.assign(SIZE, std::vector<int>(SIZE));
    // board = {
    //     { 1,  2,  3,  4},
    //     { 5,  7,  0,  8},  // 0 — пустая клетка
    //     { 9, 6, 14, 11},
    //     { 13, 15, 10, 12}
    // };
    // board = {
    //     { 1,  2,  3,  4},
    //     { 5,  0,  7,  8},  // 0 — пустая клетка
    //     { 9, 6, 14, 11},
    //     { 13, 15, 10, 12}
    // };
    board = {
        { 1,  6,  2,  3},
        { 9,  5,  7,  4},  // 0 — пустая клетка
        { 13, 0, 12, 8},
        { 14, 10, 11, 15}
    };

    // найти и запомнить позицию пустой клетки
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (board[i][j] == 0) {
                emptyX = i;
                emptyY = j;
                return;
            }
        }
    }
}

void Situation::shuffleBoard(int steps) {
    const std::vector<std::pair<int,int>> directions = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int step = 0; step < steps; ++step) {
        std::vector<std::pair<int,int>> moves;
        for (auto [dx, dy] : directions) {
            int nx = emptyX + dx, ny = emptyY + dy;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE)
                moves.push_back({nx, ny});
        }
        if (!moves.empty()) {
            auto p = moves[rng() % moves.size()];
            int nx = p.first, ny = p.second;
            std::swap(board[emptyX][emptyY], board[nx][ny]);
            emptyX = nx; emptyY = ny;
        }
    }
}

void Situation::printBoard() const {
    std::cout << "============\n";
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (board[i][j] == 0) std::cout << "   ";
            else std::cout << (board[i][j] < 10 ? " " : "") << board[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "============\n";
}

bool Situation::move(char command) {
    int newX = emptyX, newY = emptyY;
    // ВАЖНО: здесь использована та же семантика (как в ранних версиях)
    if (command == 'w') newX++;
    else if (command == 's') newX--;
    else if (command == 'a') newY++;
    else if (command == 'd') newY--;
    else return false;

    if (newX >= 0 && newX < SIZE && newY >= 0 && newY < SIZE) {
        std::swap(board[emptyX][emptyY], board[newX][newY]);
        emptyX = newX; emptyY = newY;
        return true;
    }
    return false;
}

bool Situation::isGoal() const {
    int k = 1;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j) {
            if (i == SIZE - 1 && j == SIZE - 1) {
                if (board[i][j] != 0) return false;
            } else {
                if (board[i][j] != k++) return false;
            }
        }
    return true;
}

std::vector<std::pair<Situation,char>> Situation::generateNext() const {
    std::vector<std::pair<Situation,char>> next;
    static const std::vector<std::pair<char,std::pair<int,int>>> dirs = {
        {'w',{1,0}}, {'s',{-1,0}}, {'a',{0,1}}, {'d',{0,-1}}
    };

    for (auto &d : dirs) {
        char cmd = d.first;
        int nx = emptyX + d.second.first;
        int ny = emptyY + d.second.second;
        if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
            auto newBoard = board;
            std::swap(newBoard[emptyX][emptyY], newBoard[nx][ny]);
            next.push_back({Situation(newBoard), cmd});
        }
    }
    return next;
}

std::string Situation::key() const {
    std::string s;
    s.reserve(SIZE*SIZE*3);
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j) {
            s += std::to_string(board[i][j]);
            s.push_back(',');
        }
    return s;
}

int Situation::heuristic() const {
    int distance = 0;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] != 0) {
                int correct_i = (board[i][j] - 1) / SIZE;
                int correct_j = (board[i][j] - 1) % SIZE;
                distance += abs(i - correct_i) + abs(j - correct_j);
            }
    return distance;
}

