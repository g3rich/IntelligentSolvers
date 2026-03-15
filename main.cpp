#include <iostream>
#include <random>
#include "Situation.h"
#include "Solver.h"
#include <clocale>

using namespace std;

int main() {
    setlocale(LC_ALL, "");

    Situation goal;
    // Situation start = Situation::random(10);
    Situation start;
    start.initConcreteBoard();
    // start.move('s');
    // start.move('d');
    // start.move('s');
    // start.move('d');
    // start.move('s');

    std::wcout << L"Начальная ситуация:\n";
    start.printBoard();

    std::wcout << L"Целевая ситуация:\n";
    goal.printBoard();

    Solver solver(goal /*цель*/, /*debug=*/false);

    std::wcout << L"Выберите метод: 1 - в глубину, 2 - в ширину, 3 - по градиенту, 4 - ветвей и границ, 5 - равных цен: ";
    int choice;
    std::cin >> choice;
    // SearchType type = (choice == 2 ? SearchType::WideSearch : SearchType::DepthSearch);
    // SearchType type = SearchType::DFS;
    SearchType type;
    if (choice == 1) type = SearchType::DepthSearch;
    else if (choice == 2) type = SearchType::WideSearch;
    else if (choice == 3) type = SearchType::GradientSearch;
    else if (choice == 4) type = SearchType::BranchAndBoundSearch;
    else type = SearchType::UniformCostSearch;

    int depthLimit = 40;
    if (type == SearchType::GradientSearch) {
        std::wcout << L"Введите ограничение количества шагов (например 3): ";
        std::cin >> depthLimit;
    } else if (type == SearchType::BranchAndBoundSearch) {
        //
    } else {
        std::wcout << L"Введите ограничение глубины (например 40): ";
        std::cin >> depthLimit;
    }
    //std::cin >> depthLimit;

    bool found = solver.solve(start, type, depthLimit);
    if (found) solver.printSolution();
    else std::wcout << L"Решение не найдено (исчерпаны ресурсы/глубина/поиск).\n";

    return 0;
    // Situation game;
    // char command;
    //
    // while (true) {
    //     game.printBoard();
    //     setlocale(LC_ALL, "");
    //     wcout << L"Введите команду (w - вверх, s - вниз, a - влево, d - вправо, q - выход): ";
    //     cin >> command;
    //     if (command == 'q') break;
    //     game.move(command);
    // }

    // return 0;
}
