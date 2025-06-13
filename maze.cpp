#include "maze.h"
#include <stdexcept>
#include <algorithm>
#include <random>
#include <chrono>

Maze::Maze(int width, int height) : width(width), height(height) {
    if (width <= 0 || height <= 0) { throw std::invalid_argument("Maze dimensions must be positive."); }
    grid.resize(height);
    for (int y = 0; y < height; ++y) {
        grid[y].resize(width);
        for (int x = 0; x < width; ++x) { grid[y][x].pos = {x, y}; }
    }
    start_point = {0, 0};
    end_point = {width > 0 ? width - 1 : 0, height > 0 ? height - 1 : 0};
}

void Maze::removeWall(Node& a, Node& b) {
    int dx = a.pos.x - b.pos.x; int dy = a.pos.y - b.pos.y;
    if (dx == 1) { a.wallLeft = false; b.wallRight = false; } else if (dx == -1) { a.wallRight = false; b.wallLeft = false; }
    if (dy == 1) { a.wallUp = false; b.wallDown = false; } else if (dy == -1) { a.wallDown = false; b.wallUp = false; }
}

// 解决大迷宫闪退问题的迭代生成算法
void Maze::generateIterative() {
    std::stack<Point> s;
    s.push({0, 0});
    grid[0][0].visited = true;
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng(static_cast<unsigned int>(seed));

    while(!s.empty()) {
        Point current = s.top();
        std::vector<Point> neighbors;
        int cx = current.x, cy = current.y;

        if (cy > 0 && !grid[cy - 1][cx].visited) neighbors.push_back({cx, cy - 1});
        if (cy < height - 1 && !grid[cy + 1][cx].visited) neighbors.push_back({cx, cy + 1});
        if (cx > 0 && !grid[cy][cx - 1].visited) neighbors.push_back({cx - 1, cy});
        if (cx < width - 1 && !grid[cy][cx + 1].visited) neighbors.push_back({cx + 1, cy});

        if (!neighbors.empty()) {
            std::shuffle(neighbors.begin(), neighbors.end(), rng);
            Point next = neighbors[0];
            removeWall(grid[cy][cx], grid[next.y][next.x]);
            grid[next.y][next.x].visited = true;
            s.push(next);
        } else {
            s.pop();
        }
    }
}

void Maze::generate(MazeType type) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            grid[y][x].wallUp = true; grid[y][x].wallDown = true;
            grid[y][x].wallLeft = true; grid[y][x].wallRight = true;
            grid[y][x].visited = false;
        }
    }
    //调用新的迭代生成器
    generateIterative();

    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng(static_cast<unsigned int>(seed));
    std::uniform_int_distribution<int> x_dist(0, width - 1);
    std::uniform_int_distribution<int> y_dist(0, height - 1);
    if (type == MazeType::MultiPath) {
        int walls_to_remove = (width * height) / 10;
        for (int i = 0; i < walls_to_remove; ) {
            int x = x_dist(rng); int y = y_dist(rng);
            if (rng() % 2 == 0) { if (x < width - 1) { grid[y][x].wallRight = false; grid[y][x+1].wallLeft = false; i++; } }
            else { if (y < height - 1) { grid[y][x].wallDown = false; grid[y+1][x].wallUp = false; i++; } }
        }
    } else if (type == MazeType::NoPath) {
        int walls_to_add = (width + height) / 4;
        for (int i = 0; i < walls_to_add; ) {
            int x = x_dist(rng); int y = y_dist(rng);
            if (rng() % 2 == 0) { if (x < width - 1) { grid[y][x].wallRight = true; grid[y][x+1].wallLeft = true; i++; } }
            else { if (y < height - 1) { grid[y][x].wallDown = true; grid[y+1][x].wallUp = true; i++; } }
        }
    }
}
void Maze::resetVisited() { for (int y = 0; y < height; ++y) for (int x = 0; x < width; ++x) grid[y][x].visited = false; }
int Maze::getWidth() const { return width; }
int Maze::getHeight() const { return height; }
const Node& Maze::getNode(int x, int y) const { if (x < 0 || x >= width || y < 0 || y >= height) throw std::out_of_range("Node out of bounds."); return grid[y][x]; }
void Maze::initSolver(const Point& start, const Point& end) { resetVisited(); start_point = start; end_point = end; dfs_frontier = std::stack<Point>(); bfs_frontier = std::queue<Point>(); came_from.clear(); visited_for_anim.clear(); grid[start.y][start.x].visited = true; visited_for_anim.push_back(start); }
Maze::SolverState Maze::stepDfs() { if (dfs_frontier.empty() && visited_for_anim.size() == 1) { dfs_frontier.push(start_point); } if(dfs_frontier.empty()) return SolverState::FinishedNotFound; Point current = dfs_frontier.top(); dfs_frontier.pop(); if (current == end_point) return SolverState::FinishedFound; const Node& node = getNode(current.x, current.y); if (current.y > 0 && !node.wallUp && !grid[current.y-1][current.x].visited) { Point next = {current.x, current.y-1}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); dfs_frontier.push(next); came_from[next] = current; } if (current.x < width-1 && !node.wallRight && !grid[current.y][current.x+1].visited) { Point next = {current.x+1, current.y}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); dfs_frontier.push(next); came_from[next] = current; } if (current.y < height-1 && !node.wallDown && !grid[current.y+1][current.x].visited) { Point next = {current.x, current.y+1}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); dfs_frontier.push(next); came_from[next] = current; } if (current.x > 0 && !node.wallLeft && !grid[current.y][current.x-1].visited) { Point next = {current.x-1, current.y}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); dfs_frontier.push(next); came_from[next] = current; } if (dfs_frontier.empty()) return SolverState::FinishedNotFound; return SolverState::Running; }
Maze::SolverState Maze::stepBfs() { if (bfs_frontier.empty() && visited_for_anim.size() == 1) { bfs_frontier.push(start_point); } if(bfs_frontier.empty()) return SolverState::FinishedNotFound; Point current = bfs_frontier.front(); bfs_frontier.pop(); if (current == end_point) return SolverState::FinishedFound; const Node& node = getNode(current.x, current.y); if (current.y > 0 && !node.wallUp && !grid[current.y-1][current.x].visited) { Point next = {current.x, current.y-1}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); bfs_frontier.push(next); came_from[next] = current; } if (current.x < width-1 && !node.wallRight && !grid[current.y][current.x+1].visited) { Point next = {current.x+1, current.y}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); bfs_frontier.push(next); came_from[next] = current; } if (current.y < height-1 && !node.wallDown && !grid[current.y+1][current.x].visited) { Point next = {current.x, current.y+1}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); bfs_frontier.push(next); came_from[next] = current; } if (current.x > 0 && !node.wallLeft && !grid[current.y][current.x-1].visited) { Point next = {current.x-1, current.y}; grid[next.y][next.x].visited = true; visited_for_anim.push_back(next); bfs_frontier.push(next); came_from[next] = current; } if (bfs_frontier.empty()) return SolverState::FinishedNotFound; return SolverState::Running; }
std::stack<Point> Maze::reconstructPath() { std::stack<Point> path; Point at = end_point; while (!(at == start_point)) { if (came_from.find(at) == came_from.end()) return std::stack<Point>(); path.push(at); at = came_from[at]; } path.push(start_point); return path; }
const std::vector<Point>& Maze::getVisitedCells() const { return visited_for_anim; }
const std::queue<Point>& Maze::getBfsFrontier() const { return bfs_frontier; }
const std::stack<Point>& Maze::getDfsFrontier() const { return dfs_frontier; }
const Point& Maze::getStartPoint() const { return start_point; }
const Point& Maze::getEndPoint() const { return end_point; }
