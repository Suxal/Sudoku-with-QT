#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QToolBar>
#include <QString>
#include <iostream>
#include <random>
#include <chrono>
void StartGame();
void GameOver(bool,bool);
using namespace std;
const int EMPTY_VALUE = 0;
bool isactiveelement=false;
const int PUZZLE_SIZE = 9;
QMainWindow *mainRootFrame;
QLabel *lifePlay;
array<int, 9> values{1, 2, 3, 4, 5, 6, 7, 8, 9};
int numberOfSolution = 1;
int difficulty=1;
#define PUZZLE_SIZE 9
class Sudoku {
public:
    int mat[PUZZLE_SIZE][PUZZLE_SIZE];
    int life=3;
    bool isValid(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE], int row, int col, int value)
    {
        for (int c = 0; c < PUZZLE_SIZE; c++)
        {
            if (puzzle[row][c] == value)
                return false;
        }
        for (int r = 0; r < PUZZLE_SIZE; r++)
        {
            if (puzzle[r][col] == value)
                return false;
        }
        int startRow = floor(row / 3) * 3, startCol = floor(col / 3) * 3;
        for (int r = startRow; r < startRow + 3; r++)
        {
            for (int c = startCol; c < startCol + 3; c++)
            {
                if (puzzle[r][c] == value)
                    return false;
            }
        }
        return true;
    }
    bool hasEmptyCell(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE])
    {
        for (int r = 0; r < PUZZLE_SIZE; r++)
        {
            for (int c = 0; c < PUZZLE_SIZE; c++)
            {
                if (puzzle[r][c] == EMPTY_VALUE)
                    return true;
            }
        }
        return false;
    }
    bool fillPuzzle(int (&puzzle)[PUZZLE_SIZE][PUZZLE_SIZE])
    {
        int row, col;
        for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++)
        {
            row = floor(i / PUZZLE_SIZE);
            col = i % PUZZLE_SIZE;
            if (puzzle[row][col] == EMPTY_VALUE)
            {
                unsigned seed = chrono::system_clock::now().time_since_epoch().count();
                shuffle(values.begin(), values.end(), default_random_engine(seed));
                for (int j = 0; j < PUZZLE_SIZE; j++)
                {
                    if (isValid(puzzle, row, col, values[j]))
                    {
                        puzzle[row][col] = values[j];
                        if (!hasEmptyCell(puzzle) || fillPuzzle(puzzle))
                        {
                            return true;
                        }
                    }
                }
                break;
            }
        }
        puzzle[row][col] = EMPTY_VALUE;
        return false;
    }
    bool solveSudoku(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE], bool visualize = false)
    {
        int row, col;
        for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++)
        {
            row = floor(i / PUZZLE_SIZE);
            col = i % PUZZLE_SIZE;
            if (puzzle[row][col] == EMPTY_VALUE)
            {
                for (int value = 1; value <= PUZZLE_SIZE; value++)
                {
                    if (isValid(puzzle, row, col, value))
                    {
                        puzzle[row][col] = value;
                        if (!hasEmptyCell(puzzle))
                        {
                            numberOfSolution++;
                            if (visualize)
                            {
                                return true;
                            }
                            break;
                        }
                        else if (solveSudoku(puzzle, visualize))
                        {
                            return true;
                        }
                    }
                }
                break;
            }
        }
        puzzle[row][col] = EMPTY_VALUE;

        return false;
    }
    void generatePuzzle(int (&puzzle)[PUZZLE_SIZE][PUZZLE_SIZE])
    {
        for (int i = 0; i < PUZZLE_SIZE; i++)
        {
            for (int j = 0; j < PUZZLE_SIZE; j++)
            {
                puzzle[i][j] = EMPTY_VALUE;
            }
        }
        fillPuzzle(puzzle);
        srand((unsigned)time(0));
        int attempt = difficulty;
        int maxempty=0;
        while (attempt > 0)
        {
            int row = floor(rand() % PUZZLE_SIZE);
            int col = floor(rand() % PUZZLE_SIZE);
            while (puzzle[row][col] == EMPTY_VALUE)
            {
                row = floor(rand() % PUZZLE_SIZE);
                col = floor(rand() % PUZZLE_SIZE);
            }
            int backupValue = puzzle[row][col];
            puzzle[row][col] = EMPTY_VALUE;
            maxempty++;
            numberOfSolution = 0;
            solveSudoku(puzzle);
            if (numberOfSolution != 1)
            {
                puzzle[row][col] = backupValue;
                attempt--;
            }
            if(maxempty>=difficulty*10){
                break;
            }
        }
    }
};

Sudoku *mainPuzzlesudoku;
class CostumLabel : public QLabel
{


public:
    void mouseReleaseEvent(QMouseEvent * event);
    using QLabel::QLabel;
    QString initialstyle="";
    //position of box in board
    int x;
    int y;
    int value;
    //default constructor
    bool initialfilled=false;
    bool correct=false;
    CostumLabel():QLabel(){}
    //parameterized constructor
    CostumLabel(int x,int y,int value):QLabel(){
        this->x=x;
        this->y=y;
        this->value=value;
        if(value)
        {
            initialfilled=true;
            this->setText(QString::fromStdString(to_string(value)));
        }

    }
    setClass(){
        this->setObjectName("active");
        string styles=initialstyle.toStdString()+"background:skyblue;";
        this->setStyleSheet(QString::fromStdString(styles));
    }
    removeClass(){
        this->setObjectName("");
        this->defaultStyle();
    }
    defaultStyle(){
        if(this->value){
            this->setStyleSheet(initialstyle);
        }else{
            string styles=initialstyle.toStdString()+"background:white;";
            this->setStyleSheet(QString::fromStdString(styles));
        }
        if(!this->initialfilled && this->value){
            if(!this->correct){
                string styles=initialstyle.toStdString()+"background:#ffffffb8;color:red;";
                this->setStyleSheet(QString::fromStdString(styles));
            }else{
                string styles=initialstyle.toStdString()+"background:#ffffffb8;color:green;";
                this->setStyleSheet(QString::fromStdString(styles));
            }
        }
    }
    clearValue(){
        if(this->initialfilled)
        {this->value=0;}
    }
    setValue(int value){
        this->value=value;
        this->setText(QString::fromStdString(to_string(value)));
        this->correct=mainPuzzlesudoku->isValid(mainPuzzlesudoku->mat,this->y,this->x,value);
        if(!this->correct){
            mainPuzzlesudoku->mat[this->y][this->x]=0;
            mainPuzzlesudoku->life--;
            lifePlay->setText(QString::fromStdString("  Life : "+to_string(mainPuzzlesudoku->life)+"   "));
            if(mainPuzzlesudoku->life==0){
                GameOver(false,true);
            }
        }else{
            mainPuzzlesudoku->mat[this->y][this->x]=value;
            if(!mainPuzzlesudoku->hasEmptyCell(mainPuzzlesudoku->mat)){
                GameOver(true,true);
            }
        }
        this->setClass();
    }

};
CostumLabel *mainGridMap[9][9];
CostumLabel *currentActiveGrid;
class NumberPickLevel : public QLabel
{
public:
    using QLabel::QLabel;
    bool isnumric=true;
    int value;
protected:
    void mouseReleaseEvent(QMouseEvent * event)
    {
        if (event->button()==Qt::LeftButton)
        // process only clicks on the left button
        {
            if(isnumric){
                if(isactiveelement){
                    currentActiveGrid->setValue(this->value);
                }
            }
        }
        QLabel::mouseReleaseEvent(event);
    }
};
void CostumLabel::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->button()==1)
    // process only clicks on the left button
    {
        if(!this->initialfilled){
            isactiveelement=true;
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    mainGridMap[i][j]->removeClass();
                }
            }
            currentActiveGrid=this;
            this->setClass();
        }else{
            isactiveelement=false;
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    mainGridMap[i][j]->removeClass();
                }
            }
        }
    }
    QLabel::mouseReleaseEvent(event);
}
class PuzzleSolver{

public:
    QWidget *window;
    Sudoku *sudoku;
    PuzzleSolver(){
        window = new QWidget;
        sudoku=new Sudoku;
        mainPuzzlesudoku=sudoku;
        sudoku->generatePuzzle(sudoku->mat);
        QHBoxLayout *layout = new QHBoxLayout(window);
        window->setLayout(layout);
        window->layout()->setContentsMargins(0,0,0,0);
        window->layout()->setSpacing(0);
        int count=0;
        //creating grids
        for(int i=0;i<3;i++){
            QWidget *widget =new QWidget(window);
            QVBoxLayout *layHor = new QVBoxLayout(widget);
            widget->setLayout(layHor);
            layout->addWidget(widget);
            widget->layout()->setContentsMargins(0,0,0,0);
            widget->layout()->setSpacing(0);

            for(int j=0;j<3;j++){
                QWidget *widget_inner =new QWidget(widget);
                QHBoxLayout *layVer = new QHBoxLayout(widget_inner);
                layHor->addWidget(widget_inner);
                widget_inner->setLayout(layVer);
                widget_inner->layout()->setContentsMargins(0,0,0,0);
                widget_inner->layout()->setSpacing(0);

                for(int x=0;x<3;x++){
                    QWidget *widget_small =new QWidget(widget_inner);
                    QVBoxLayout *laysmall = new QVBoxLayout(widget_small);
                    widget_small->setLayout(laysmall);
                    widget_small->layout()->setContentsMargins(0,0,0,0);
                    layVer->addWidget(widget_small);
                    widget_small->layout()->setSpacing(0);
                    widget_small->setStyleSheet("QLabel#active{background:skyblue;}");
                    for (int y=0;y<3;y++){
                        //calculation of position of grid
                        int x1=i*3+x;
                        int y1=j*3+y;
                        count++;
                        CostumLabel *button = new CostumLabel(x1,y1,sudoku->mat[y1][x1]);
                        mainGridMap[y1][x1]=button;
                        laysmall->addWidget(button);
                        button->setContentsMargins(0,0,0,0);
                        button->setAlignment(Qt::AlignCenter);
                        string style="border:1px solid lightgray;";
                        if(x==0 && i==0){
                            style+="border-left:2px solid black;";
                        }
                        if(x==2){
                            style+="border-right:2px solid black;";
                        }
                        if(y==0 && j==0){
                            style+="border-top:2px solid black;";
                        }
                        if(y==2){
                            style+="border-bottom:2px solid black;";
                        }
                        style=style+"font-size:20px;";
                        button->initialstyle=QString::fromStdString(style);
                        button->defaultStyle();

                    }
                }
            }
        }
        window->setFixedSize(400,400);
    }
};
void StartGame(QString level_game){
    PuzzleSolver *solver=new PuzzleSolver;
    QWidget *win =solver->window;
    QWidget *window=new QWidget(mainRootFrame);
    QVBoxLayout *mainlayout=new QVBoxLayout(window);
    mainlayout->addWidget(win);
    mainRootFrame->setCentralWidget(window);
    QToolBar *toolbar=new QToolBar(window);
    window->layout()->addWidget(toolbar);
    window->setStyleSheet("QLabel#number{font-size:17px;padding:0px 5px;background:skyblue;"
                          "margin:1px;color:white;}\n"
                          "QLabel#number:hover{background:blue;}"
                          "QLabel#life{font-size:17px;padding:10px;background:lightsteelblue;"
                          "margin:1px;color:white;}");
    for (int var = 0; var < 9; ++var) {
        NumberPickLevel *label= new NumberPickLevel(QString::fromStdString(to_string(var+1)));
        toolbar->addWidget(label);
        label->value=var+1;
        label->setObjectName("number");
        label->setCursor(Qt::PointingHandCursor);
    }
    QLabel *label=new QLabel(QString::fromStdString("  Life : 3   "));
    toolbar->addWidget(label);
    label->setObjectName("life");
    label->setAlignment(Qt::AlignCenter);
    lifePlay=label;
    // mainRootFrame->setFixedSize(410,500);
    mainRootFrame->setWindowTitle(QString::fromStdString(level_game.toStdString()+" - Sudoku Game C++"));
}
void GameOver(bool win=false, bool overgm=true){
    QWidget *widget = new QWidget;
    QVBoxLayout *lay=new QVBoxLayout(widget);
    widget->setLayout(lay);
    widget->setStyleSheet("QPushButton{font-size:20px;background:skyblue;color:white;}"
                          "QPushButton:hover{font-size:20px;background:blue;}QLabel{font-size:20px;}");
    widget->layout()->setContentsMargins(0,0,0,0);
    widget->layout()->setSpacing(0);
    if(overgm)
    {
        QLabel *over=new QLabel("Game Over");
        over->setAlignment(Qt::AlignCenter);
        widget->layout()->addWidget(over);
        QLabel *over1=new QLabel("You Loose!");
        over->setStyleSheet("color:blue;");
        over1->setStyleSheet("color:red");
        over1->setAlignment(Qt::AlignCenter);
        widget->layout()->addWidget(over1);
        if(win){
            over1->setStyleSheet("color:green;");
            over1->setText("You Win!");
        }
    }
    QLabel *NewGame=new QLabel("Start NewGame!");
    NewGame->setAlignment(Qt::AlignCenter);
    NewGame->setStyleSheet("color:#1bb3b1");
    widget->layout()->addWidget(NewGame);
    QPushButton *button =new QPushButton;
    button->setMinimumSize(200,50);
    button->setText("Beginner");
    QObject::connect(button, &QPushButton::clicked, [=]() {
        difficulty=1;
        StartGame(button->text());
    });
    widget->layout()->addWidget(button);
    QPushButton *button1 =new QPushButton;
    button1->setMinimumSize(200,50);
    button1->setText("Easy");
    QObject::connect(button1, &QPushButton::clicked, [=]() {
        difficulty=2;
        StartGame(button1->text());
    });
    widget->layout()->addWidget(button1);
    QPushButton *button2 =new QPushButton;
    button2->setMinimumSize(200,50);
    button2->setText("Medium");
    QObject::connect(button2, &QPushButton::clicked, [=]() {
        difficulty=2.5;
        StartGame(button2->text());
    });
    widget->layout()->addWidget(button2);
    QPushButton *button3 =new QPushButton;
    button3->setMinimumSize(200,50);
    button3->setText("Hard");
    QObject::connect(button3, &QPushButton::clicked, [=]() {
        difficulty=3;
        StartGame(button3->text());
    });
    widget->layout()->addWidget(button3);
    QPushButton *button4 =new QPushButton;
    button4->setMinimumSize(200,50);
    button4->setText("Expert");
    QObject::connect(button4, &QPushButton::clicked, [=]() {
        difficulty=4;
        StartGame(button4->text());
    });
    widget->layout()->addWidget(button4);
    mainRootFrame->setCentralWidget(widget);
    mainRootFrame->setWindowTitle(QString::fromStdString("Sudoku Game C++"));
    // mainRootFrame->setFixedSize(410,500);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow *main = new QMainWindow;
    mainRootFrame=main;
    GameOver(false,false);
    mainRootFrame->setFixedSize(410,500);
    mainRootFrame->setWindowTitle(QString::fromStdString("Sudoku Game C++"));
    main->show();
    return a.exec();
}
