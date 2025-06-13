#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    maze = new Maze(20, 20); maze->generate();
    mazeWidget = new MazeWidget(this); mazeWidget->setMaze(maze);
    animation_timer = new QTimer(this);
    mainStatusBar = new QStatusBar(this);
    this->setStatusBar(mainStatusBar);

    connect(animation_timer, &QTimer::timeout, this, &MainWindow::onAnimationStep);
    // 连接迷宫控件的点击信号
    connect(mazeWidget, &MazeWidget::cellClicked, this, &MainWindow::onCellClicked);

    widthLabel = new QLabel("宽度(Width):", this);
    widthSpinBox = new QSpinBox(this); widthSpinBox->setRange(5, 200); widthSpinBox->setValue(20);
    heightLabel = new QLabel("高度(Height):", this);
    heightSpinBox = new QSpinBox(this); heightSpinBox->setRange(5, 200); heightSpinBox->setValue(20);
    QGroupBox *genBox = new QGroupBox("1. 生成迷宫 (Generate Maze)");
    generateSinglePathButton = new QPushButton("生成单路径迷宫\n(Single Path)", this);
    generateMultiPathButton = new QPushButton("生成多路径迷宫\n(Multiple Paths)", this);
    generateNoPathButton = new QPushButton("生成无路径迷宫\n(No Path)", this);
    QVBoxLayout *genLayout = new QVBoxLayout;
    genLayout->addWidget(generateSinglePathButton); genLayout->addWidget(generateMultiPathButton); genLayout->addWidget(generateNoPathButton);
    genBox->setLayout(genLayout);
    QGroupBox *solveBox = new QGroupBox("2. 选择算法 (Select Algorithm)");
    findPathDfsButton = new QPushButton("动画演示 (DFS)", this);
    findPathBfsButton = new QPushButton("动画演示 (BFS)", this);
    QVBoxLayout *solveLayout = new QVBoxLayout;
    solveLayout->addWidget(findPathDfsButton); solveLayout->addWidget(findPathBfsButton);
    solveBox->setLayout(solveLayout);
    QGridLayout *controlsLayout = new QGridLayout;
    controlsLayout->addWidget(widthLabel, 0, 0); controlsLayout->addWidget(widthSpinBox, 0, 1);
    controlsLayout->addWidget(heightLabel, 1, 0); controlsLayout->addWidget(heightSpinBox, 1, 1);
    controlsLayout->addWidget(genBox, 2, 0, 1, 2); controlsLayout->addWidget(solveBox, 3, 0, 1, 2);
    controlsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 4, 0);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mazeWidget, 4); mainLayout->addLayout(controlsLayout, 1);
    QWidget *centralWidget = new QWidget; centralWidget->setLayout(mainLayout); setCentralWidget(centralWidget);

    connect(generateSinglePathButton, &QPushButton::clicked, this, &MainWindow::onGenerateSinglePath);
    connect(generateMultiPathButton, &QPushButton::clicked, this, &MainWindow::onGenerateMultiPath);
    connect(generateNoPathButton, &QPushButton::clicked, this, &MainWindow::onGenerateNoPath);
    connect(findPathDfsButton, &QPushButton::clicked, this, &MainWindow::onFindPathDFS);
    connect(findPathBfsButton, &QPushButton::clicked, this, &MainWindow::onFindPathBFS);

    setWindowTitle("[Final_Project:Maze]     Developed By Owen_Chen"); resize(800, 600);
}

MainWindow::~MainWindow() { delete maze; }

void MainWindow::generateNewMaze(Maze::MazeType type) {
    stopAnimation();
    int width = widthSpinBox->value(); int height = heightSpinBox->value();
    delete maze;
    maze = new Maze(width, height);
    maze->generate(type);
    mazeWidget->setMaze(maze);
}

void MainWindow::onGenerateSinglePath() { generateNewMaze(Maze::MazeType::SinglePath); }
void MainWindow::onGenerateMultiPath() { generateNewMaze(Maze::MazeType::MultiPath); }
void MainWindow::onGenerateNoPath() { generateNewMaze(Maze::MazeType::NoPath); }

// 点击寻路按钮现在进入“点选模式”
void MainWindow::onFindPathDFS() {
    stopAnimation();
    active_solver = SolverType::DFS;
    selection_state = SelectionState::SelectStart;
    mazeWidget->clearUserSelection();
    mainStatusBar->showMessage("请左键点击选择起点 (Please left-click to select a starting point)");
}
void MainWindow::onFindPathBFS() {
    stopAnimation();
    active_solver = SolverType::BFS;
    selection_state = SelectionState::SelectStart;
    mazeWidget->clearUserSelection();
    mainStatusBar->showMessage("请左键点击选择起点 (Please left-click to select a starting point)");
}

// [新增] 处理鼠标点击的核心逻辑
// [NEW] Core logic for handling mouse clicks.
void MainWindow::onCellClicked(const Point& cellPos) {
    if (selection_state == SelectionState::SelectStart) {
        selected_start = cellPos;
        selection_state = SelectionState::SelectEnd;
        mazeWidget->setUserSelectionPoints(selected_start, {-1, -1});
        mainStatusBar->showMessage("请右键点击选择终点 (Please right-click to select an ending point)");
    } else if (selection_state == SelectionState::SelectEnd) {
        selected_end = cellPos;
        mazeWidget->setUserSelectionPoints(selected_start, selected_end);
        mainStatusBar->showMessage("起点和终点已选择。开始寻路... (Start and end points selected. Solving...)");
        selection_state = SelectionState::None;
        startSolver(active_solver == SolverType::DFS);
    }
}

void MainWindow::startSolver(bool is_dfs) {
    if (selected_start.x == -1 || selected_end.x == -1) return; // 未选择点
    stopAnimation();
    mazeWidget->clearAnimation();
    maze->initSolver(selected_start, selected_end);
    active_solver = is_dfs ? SolverType::DFS : SolverType::BFS;
    setControlsEnabled(false);
    animation_timer->start(20); // 减慢速度以便观察
}

void MainWindow::stopAnimation() {
    animation_timer->stop();
    // 不重置 active_solver，以便知道最终路径的类型
    setControlsEnabled(true);
    mainStatusBar->clearMessage();
}

void MainWindow::setControlsEnabled(bool enabled) {
    generateSinglePathButton->setEnabled(enabled);
    generateMultiPathButton->setEnabled(enabled);
    generateNoPathButton->setEnabled(enabled);
    findPathDfsButton->setEnabled(enabled);
    findPathBfsButton->setEnabled(enabled);
    widthSpinBox->setEnabled(enabled);
    heightSpinBox->setEnabled(enabled);
}

void MainWindow::onAnimationStep() {
    if (!maze || active_solver == SolverType::None) { stopAnimation(); return; }
    Maze::SolverState state = (active_solver == SolverType::DFS) ? maze->stepDfs() : maze->stepBfs();
    const auto& std_visited = maze->getVisitedCells();
    QVector<Point> visited(std_visited.begin(), std_visited.end());
    QVector<Point> frontier_vec;
    if (active_solver == SolverType::DFS) {
        auto frontier_copy = maze->getDfsFrontier();
        while(!frontier_copy.empty()) { frontier_vec.push_back(frontier_copy.top()); frontier_copy.pop(); }
    } else {
        auto frontier_copy = maze->getBfsFrontier();
        while(!frontier_copy.empty()) { frontier_vec.push_back(frontier_copy.front()); frontier_copy.pop(); }
    }
    mazeWidget->setAnimationState(visited, frontier_vec);

    if (state != Maze::SolverState::Running) {
        stopAnimation();
        if (state == Maze::SolverState::FinishedFound) {
            std::stack<Point> path = maze->reconstructPath();
            mazeWidget->setSolutionPath(path, active_solver == SolverType::DFS ? MazeWidget::PathType::DFS : MazeWidget::PathType::BFS);
        } else {
            QMessageBox::information(this, "寻路结果", "未找到路径！\nNo path found!");
        }
    }
}
