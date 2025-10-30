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

    Solver solver(goal /*цель*/, /*debug=*/true);

    std::wcout << L"Выберите метод: 1 - в глубину, 2 - в ширину: ";
    int choice;
    std::cin >> choice;
    SearchType type = (choice == 2 ? SearchType::WideSearch : SearchType::DepthSearch);
    // SearchType type = SearchType::DFS;

    int depthLimit = 40;
    std::wcout << L"Введите ограничение глубины (например 40): ";
    std::cin >> depthLimit;

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
