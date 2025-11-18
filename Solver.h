#ifndef SOLVER_H
#define SOLVER_H

#include "Situation.h"
#include <vector>
#include <unordered_set>

enum class SearchType {
    DepthSearch,
    WideSearch,
    GradientSearch,
    BranchAndBoundSearch
};

class Solver {
private:
    Situation goal;
    std::vector<Situation> path;   // вершины на текущем пути (если найдено — итоговый путь)
    std::vector<char> moves;       // команды между вершинами
    std::unordered_set<std::string> visitedKeys; // для DFS: ключи на текущем пути

    bool debug;
    long long nodesExpanded;

    bool depthSearch_recursive(const Situation& current, int depth, int depthLimit);
    bool wideSearch(const Situation& start, int depthLimit);
    bool gradientSearch(const Situation& start, int maxSteps);
    bool branchAndBound(const Situation& start, int maxNodes);

public:
    Solver(const Situation& goalSituation, bool debug = false);

    // depthLimit — опциональное ограничение глубины для DFS
    bool solve(const Situation& start, SearchType type = SearchType::DepthSearch, int depthLimit = 1000000);
    void printSolution() const;
};

#endif // SOLVER_H
