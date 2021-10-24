#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QApplication>
#include <QStandardItemModel>
#include <stdlib.h>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct Node{    //普通结点
    int flag; //0-int  1-float  2-char
    union data{     //数据
        int data_int;
        float data_float;
        char *data_char;
    };
    struct Node *right, *down;
}node;

typedef struct Head{        //行列头结点
    int flag; //0-int  1-float  2-char
    char *name;     //列标签
    int row, col;       //行号,列号
    struct Node *right, *down;
}myhead, *head;

typedef struct Database{
    int num_row, num_col;   //行数,列数
    char *name_database;    //数据库表名称
    struct Head *row, *col;
}mydatabase, *database;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetTable();    //设置表格
    void SetList();     //设置列表

    void InitDatabase();    //初始化数据库

    struct Database *MyDatabase;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
