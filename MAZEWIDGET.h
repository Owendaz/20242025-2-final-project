#ifndef MAZEWIDGET_H
#define MAZEWIDGET_H

#include <QWidget>
#include <QVector>
#include <stack>
#include "maze.h"

class MazeWidget : public QWidget {
    Q_OBJECT

public:
    enum class PathType { None, DFS, BFS };
    explicit MazeWidget(QWidget *parent = nullptr);

    void setMaze(Maze *maze);
    void setSolutionPath(const std::stack<Point>& path, PathType type);
    void setAnimationState(const QVector<Point>& visited, const QVector<Point>& frontier);

    // 管理用户选择的点
    void setUserSelectionPoints(const Point& start, const Point& end);
    void clearUserSelection();
    void clearAnimation();

signals:
    // 当用户点击单元格时发射此信号
    void cellClicked(const Point& cellPos);

protected:
    void paintEvent(QPaintEvent *event) override;
    //覆盖鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;

private:
    Maze *maze_ptr = nullptr;
    std::stack<Point> solution_path;
    PathType path_type = PathType::None;

    QVector<Point> anim_visited_cells;
    QVector<Point> anim_frontier_cells;

    //存储用户选择的起点和终点
    Point user_start = {-1, -1};
    Point user_end = {-1, -1};
};

#endif // MAZEWIDGET_H
