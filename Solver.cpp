#include "Solver.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <climits>

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

bool Solver::gradientSearch(const Situation& start, int maxSteps) {
    nodesExpanded = 0;
    path.clear();
    moves.clear();

    Situation current = start;
    path.push_back(current);

    for (int step = 0; step < maxSteps; ++step) {
        if (current == goal) return true;

        auto nextStates = current.generateNext();
        if (nextStates.empty()) break;

        int currentScore = current.heuristic();
        int bestScore = currentScore;
        Situation bestNext;
        char bestMove = '?';

        for (auto &p : nextStates) {
            int score = p.first.heuristic();
            if (score < bestScore) {
                bestScore = score;
                bestNext = p.first;
                bestMove = p.second;
            }
        }

        // if (bestScore >= currentScore) {
        //     if (debug) std::wcout << L"[GRADIENT] Локальный минимум на шаге " << step << L"\n";
        //     return false;
        // }

        current = bestNext;
        moves.push_back(bestMove);
        path.push_back(current);
        nodesExpanded++;

        if (debug) {
            std::wcout << L"[GRADIENT] Шаг " << step
                       << L", оценка: " << bestScore
                       << L", ход: " << bestMove << L"\n";
        }
    }

    return (current == goal);
}

bool Solver::branchAndBound(const Situation& start, int maxNodes) {
    struct Node {
        Situation situation;
        int depth;          // g — длина пути
        // int heuristic;      // h — оценка
        int totalCost;      // f = g + h
        std::vector<char> path;
    };

    auto cmp = [](const Node& a, const Node& b) {
        return a.totalCost > b.totalCost;
    };

    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);

    pq.push({start, 0,/* start.heuristic(),*/ start.heuristic(), {}});
    visitedKeys.clear();
    nodesExpanded = 0;

    // bool foundSolution = false;
    int bestBound = INT_MAX;

    while (!pq.empty()) {
        Node node = pq.top();
        pq.pop();

        nodesExpanded++;

        // if (foundSolution && node.totalCost >= bestBound) continue;

        if (debug) {
            // std::wcout << L"[B&B] depth=" << node.depth
            //            << L" f=" << node.totalCost
            //            << L" h=" << node.heuristic
            //            << L" path=";
            std::wcout << L"[B&B] осталось=" << node.totalCost
                        << L" depth=" << node.depth
                       << L" путь=";
            for (char mv : node.path) std::wcout << mv;
            std::wcout << L"\n";
        }

        if (node.situation == goal) {
            // foundSolution = true;
            bestBound = node.totalCost;

            moves = node.path;
            path.clear();
            Situation s = start;
            path.push_back(s);
            for (char mv : moves) {
                s.move(mv);
                path.push_back(s);
            }

            if (debug)
                std::wcout << L"[B&B] Найдено решение с f=" << bestBound << L"\n";

            return true;
        }

        for (auto& p : node.situation.generateNext()) {
            Situation next = p.first;
            char mv = p.second;

            // int h = next.heuristic();
            // int g = node.depth + 1;
            // // int f = g + h;
            // int f = g + next.heuristic();

            // if (foundSolution && f >= bestBound) continue;

            std::vector<char> newPath = node.path;
            newPath.push_back(mv);

            pq.push({next, (node.depth + 1),/* h,*/ (node.depth + 1 + next.heuristic()), newPath});
        }
    }

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
    } else if (type == SearchType::WideSearch) {
        return wideSearch(start, depthLimit);
    } else if (type == SearchType::GradientSearch) {
        return gradientSearch(start, depthLimit);
    } else {
        return branchAndBound(start, depthLimit);
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
