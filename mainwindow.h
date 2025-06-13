#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "maze.h"
#include "mazewidget.h"

class QPushButton; class QSpinBox; class QLabel; class QStatusBar;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onGenerateSinglePath(); void onGenerateMultiPath(); void onGenerateNoPath();
    void onFindPathDFS(); void onFindPathBFS();
    void onAnimationStep();
    void onCellClicked(const Point& cellPos);
private:
    void startSolver(bool is_dfs);
    void stopAnimation();
    void setControlsEnabled(bool enabled);
    void generateNewMaze(Maze::MazeType type);

    enum class SolverType { None, DFS, BFS };
    //用于管理点选状态
    enum class SelectionState { None, SelectStart, SelectEnd };

    SolverType active_solver = SolverType::None;
    SelectionState selection_state = SelectionState::None;

    Point selected_start = {-1, -1};
    Point selected_end = {-1, -1};

    Maze *maze;
    MazeWidget *mazeWidget;
    QTimer *animation_timer;

    //UI 控件
    QPushButton *generateSinglePathButton, *generateMultiPathButton, *generateNoPathButton;
    QPushButton *findPathDfsButton, *findPathBfsButton;
    QSpinBox *widthSpinBox, *heightSpinBox;
    QLabel *widthLabel, *heightLabel;
    QStatusBar *mainStatusBar;
};
#endif // MAINWINDOW_H
