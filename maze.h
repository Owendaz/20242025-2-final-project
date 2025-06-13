#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <map>


struct Point {
    int x;
    int y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator<(const Point& other) const { if (y != other.y) return y < other.y; return x < other.x; }
};

struct Node {
    Point pos;
    bool wallUp = true, wallDown = true, wallLeft = true, wallRight = true;
    bool visited = false;
};

class Maze {
public:
    enum class MazeType { SinglePath, MultiPath, NoPath };
    enum class SolverState { Running, FinishedFound, FinishedNotFound };

    Maze(int width, int height);

    void generate(MazeType type = MazeType::SinglePath);

    void initSolver(const Point& start, const Point& end);
    SolverState stepDfs();
    SolverState stepBfs();
    std::stack<Point> reconstructPath();

    const std::vector<Point>& getVisitedCells() const;
    const std::queue<Point>& getBfsFrontier() const;
    const std::stack<Point>& getDfsFrontier() const;
    const Point& getStartPoint() const;
    const Point& getEndPoint() const;

    void resetVisited();
    int getWidth() const;
    int getHeight() const;
    const Node& getNode(int x, int y) const;

private:
    // 使用迭代算法代替递归
    void generateIterative();
    void removeWall(Node& a, Node& b);

    int width;
    int height;
    std::vector<std::vector<Node>> grid;

    Point start_point;
    Point end_point;

    std::stack<Point> dfs_frontier;
    std::queue<Point> bfs_frontier;
    std::map<Point, Point> came_from;
    std::vector<Point> visited_for_anim;
};

#endif // MAZE_H
