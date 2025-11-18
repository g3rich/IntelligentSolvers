#ifndef SITUATION_H
#define SITUATION_H

#include <vector>
#include <random>
#include <chrono>
#include <string>

class Situation {
public:
    static constexpr int SIZE = 4;

private:
    std::vector<std::vector<int>> board;
    int emptyX{SIZE-1}, emptyY{SIZE-1};
    std::mt19937 rng;

public:
    // default: упорядоченная (целевая) ситуация
    Situation();

    // построить из готовой доски
    explicit Situation(const std::vector<std::vector<int>>& b);

    // фабрика: случайная ситуация (перемешивание)
    static Situation random(int shuffleSteps = 1000);

    void initBoard();
    void initConcreteBoard();
    void shuffleBoard(int steps = 1000);
    void printBoard() const;

    // изменить текущую ситуацию (попытка хода)
    bool move(char command);

    bool isGoal() const;

    // генератор следующих ситуаций: возвращает пару (новая ситуация, команда)
    std::vector<std::pair<Situation,char>> generateNext() const;

    // ключ для хеширования/сравнения (используется в Solver)
    std::string key() const;

    int heuristic() const;

    std::vector<std::vector<int>> getBoard() const { return board; }

    bool operator==(const Situation& other) const { return board == other.board; }
    bool operator!=(const Situation& other) const { return !(*this == other); }
};

#endif // SITUATION_H
