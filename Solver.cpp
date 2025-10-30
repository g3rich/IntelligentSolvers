#include "Solver.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <unordered_map>

Solver::Solver(const Situation& goalSituation, bool debugFlag) : goal(goalSituation), debug(debugFlag), nodesExpanded(0) {}

bool Solver::depthSearch_recursive(const Situation& current, int depth, int depthLimit) {
    std::string k = current.key();

    // если уже на текущем пути — цикл, отбрасываем
    if (visitedKeys.count(k)) return false;

    // добавляем в путь/visited
    visitedKeys.insert(k);
    path.push_back(current);

    nodesExpanded++;
    if (debug/* && nodesExpanded % 10 == 0*/) {
        std::wcout << L"Посмотрено вершин: " << nodesExpanded
        << L", глубина: " << depth << L", путь: ";
        for (char mv : moves) std::wcout << mv;
        std::wcout << L"\n";
    }

    // цель достигнута?
    if (current == goal) return true;

    // ходы есть?
    if (depth >= depthLimit) {
        visitedKeys.erase(k);
        path.pop_back();
        return false;
    }

    // получение следующей ситуации
    for (auto &p : current.generateNext()) {
        // новая ситуация
        Situation next = p.first;
        // ход
        char mv = p.second;

        moves.push_back(mv);
        // рекурсивно вызываем ситуацию ниже
        if (depthSearch_recursive(next, depth + 1, depthLimit)) return true;
        moves.pop_back(); // откат хода
    }

    // это тупик? — удаляем из visited и path
    visitedKeys.erase(k);
    path.pop_back();
    return false;
}

bool Solver::wideSearch(const Situation& start, int depthLimit) {
    struct Node {
        Situation situation;
        int depth;
        //std::string key;
    };

    std::queue<Node> q; // очередь
    std::unordered_set<std::string> enqueuedKeys; // ситуации в очереди
    std::unordered_map<std::string, std::string> parent;    // key -> parentKey родитель каждого ключа
    std::unordered_map<std::string, char> moveFrom;        // key -> move from parent to key - ход который привел

    std::string startKey = start.key();
    q.push({start, 0});
    enqueuedKeys.insert(startKey);
    parent[startKey] = ""; // корень — без родителя

    nodesExpanded = 0;
    visitedKeys.clear();

    while (!q.empty()) {
        Node node = q.front(); q.pop();
        std::string key = node.situation.key();

        // если обработана — пропускаем (на всякий случай)
        if (visitedKeys.count(key)) continue;

        // отмечаем как обработанную
        visitedKeys.insert(key);
        nodesExpanded++;

        // debug-вывод прогресса
        if (debug/* && nodesExpanded % 10 == 0*/) {
            std::wcout << L"[BFS] Просмотрено: " << nodesExpanded
                       << L", глубина узла: " << node.depth
                       << L", очередь: " << q.size() << L"\n";
        }

        // цель достигнута?
        if (node.situation == goal) {
            // восстановим последовательность ходов, пройдя parent-map
            std::vector<char> revMoves;
            std::string cur = key;
            while (!parent[cur].empty()) {
                revMoves.push_back(moveFrom[cur]);
                cur = parent[cur];
            }
            // обратим порядок
            std::reverse(revMoves.begin(), revMoves.end());
            moves = revMoves;

            // восстановим path: применим ходы к стартовой ситуации
            path.clear();
            Situation s = start;
            path.push_back(s);
            for (char mv : moves) {
                s.move(mv);
                path.push_back(s);
            }

            return true;
        }

        // ходы есть?
        if (node.depth >= depthLimit) continue;

        // получение ситуаций следующего уровня
        for (auto &p : node.situation.generateNext()) {
            Situation next = p.first;
            char mv = p.second;
            std::string nk = next.key();

            // не ставим в очередь если уже есть или уже обработан
            if (enqueuedKeys.count(nk) || visitedKeys.count(nk)) continue;

            // помещаем в очередь
            q.push({next, node.depth + 1});
            enqueuedKeys.insert(nk);

            // сохраняем родителя и движение
            parent[nk] = key;
            moveFrom[nk] = mv;
        }
    }

    // если очередь кончилась — решения нет
    return false;
}

bool Solver::solve(const Situation& start, SearchType type, int depthLimit) {
    path.clear();
    moves.clear();
    visitedKeys.clear();
    nodesExpanded = 0;
    // return dfs_recursive(start, 0, depthLimit);

    if (type == SearchType::DepthSearch) {
        return depthSearch_recursive(start, 0, depthLimit);
    } else {
        return wideSearch(start, depthLimit);
    }
}

void Solver::printSolution() const {
    std::wcout << L"Найден путь длиной (ходов): " << moves.size() << L"\n";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i == 0) {
            std::wcout << L"Шаг 0 (начало)\n";
        } else {
            std::wcout << L"Шаг " << i << L" — команда: " << moves[i-1] << L"\n";
        }
        path[i].printBoard();
    }
}
