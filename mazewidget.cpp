#include "mazewidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QColor>

MazeWidget::MazeWidget(QWidget *parent) : QWidget(parent) {
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void MazeWidget::setMaze(Maze *maze) { this->maze_ptr = maze; clearAnimation(); clearUserSelection(); }
void MazeWidget::setSolutionPath(const std::stack<Point>& path, PathType type) { this->solution_path = path; this->path_type = type; update(); }
void MazeWidget::setAnimationState(const QVector<Point>& visited, const QVector<Point>& frontier) { anim_visited_cells = visited; anim_frontier_cells = frontier; update(); }
void MazeWidget::setUserSelectionPoints(const Point& start, const Point& end) { user_start = start; user_end = end; update(); }
void MazeWidget::clearUserSelection() { user_start = {-1, -1}; user_end = {-1, -1}; update(); }

void MazeWidget::clearAnimation() {
    solution_path = std::stack<Point>();
    path_type = PathType::None;
    anim_visited_cells.clear();
    anim_frontier_cells.clear();
    update();
}

void MazeWidget::mousePressEvent(QMouseEvent *event) {
    if (!maze_ptr) return;
    int cellX = event->x() / ((float)width() / maze_ptr->getWidth());
    int cellY = event->y() / ((float)height() / maze_ptr->getHeight());

    if (cellX >= 0 && cellX < maze_ptr->getWidth() && cellY >= 0 && cellY < maze_ptr->getHeight()) {
        emit cellClicked({cellX, cellY});
    }
}

void MazeWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if (!maze_ptr) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int mazeWidth = maze_ptr->getWidth(); int mazeHeight = maze_ptr->getHeight();
    if (mazeWidth == 0 || mazeHeight == 0) return;
    float cellWidth = (float)this->width() / mazeWidth; float cellHeight = (float)this->height() / mazeHeight;

    const Point& solver_start = maze_ptr->getStartPoint();
    const Point& solver_end = maze_ptr->getEndPoint();

    // 绘制动画过程
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(220, 220, 220));
    for (const auto& p : anim_visited_cells) { if (!(p == solver_start) && !(p == solver_end)) painter.drawRect(QRectF(p.x*cellWidth, p.y*cellHeight, cellWidth, cellHeight)); }
    painter.setBrush(QColor(255, 165, 0, 150));
    for (const auto& p : anim_frontier_cells) { if (!(p == solver_start) && !(p == solver_end)) painter.drawRect(QRectF(p.x*cellWidth, p.y*cellHeight, cellWidth, cellHeight)); }

    // 绘制最终路径
    if (!solution_path.empty()) {
        QColor path_color = (path_type == PathType::DFS) ? QColor(135, 206, 250) : QColor(218, 112, 214);
        painter.setBrush(path_color);
        auto temp_path = solution_path;
        while (!temp_path.empty()) {
            Point p = temp_path.top(); temp_path.pop();
            if (!(p == solver_start) && !(p == solver_end)) painter.drawRect(QRectF(p.x*cellWidth, p.y*cellHeight, cellWidth, cellHeight));
        }
    }

    // 绘制起点和终点
    if (path_type != PathType::None || !anim_visited_cells.isEmpty()) {
        painter.setBrush(QColor(60, 179, 113)); painter.drawRect(QRectF(solver_start.x*cellWidth, solver_start.y*cellHeight, cellWidth, cellHeight));
        painter.setBrush(QColor(220, 20, 60)); painter.drawRect(QRectF(solver_end.x*cellWidth, solver_end.y*cellHeight, cellWidth, cellHeight));
    }

    // 绘制用户选择的点
    painter.setPen(QPen(Qt::blue, 3));
    painter.setBrush(Qt::NoBrush);
    if(user_start.x != -1) { painter.drawEllipse(QRectF(user_start.x*cellWidth+cellWidth*0.2, user_start.y*cellHeight+cellHeight*0.2, cellWidth*0.6, cellHeight*0.6)); }
    if(user_end.x != -1) { painter.drawRect(QRectF(user_end.x*cellWidth+cellWidth*0.2, user_end.y*cellHeight+cellHeight*0.2, cellWidth*0.6, cellHeight*0.6)); }

    // 绘制墙壁
    QPen wallPen(Qt::black); wallPen.setWidth(2);
    painter.setPen(wallPen); painter.setBrush(Qt::NoBrush);
    for (int y = 0; y < mazeHeight; ++y) {
        for (int x = 0; x < mazeWidth; ++x) {
            const Node& node = maze_ptr->getNode(x, y);
            float x1 = x * cellWidth, y1 = y * cellHeight, x2 = (x+1)*cellWidth, y2 = (y+1)*cellHeight;
            if (node.wallDown) painter.drawLine(QPointF(x1, y2), QPointF(x2, y2));
            if (node.wallRight) painter.drawLine(QPointF(x2, y1), QPointF(x2, y2));
        }
    }
    painter.drawRect(0, 0, this->width()-1, this->height()-1);
}
