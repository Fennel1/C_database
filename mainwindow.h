#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QApplication>
#include<QCoreApplication>
#include <QStandardItemModel>
#include <QFile>
#include <QDebug>
#include <stdlib.h>
#include <string>
#include <QString>
#include <iostream>
#include <QLabel>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct Node{        //行列头结点
    int flag; //0-int  1-float  2-char
    QString name;     //列标签
    int row, col;       //行号,列号

    struct data{     //数据
        int data_int;
        float data_float;
        QString data_char;
    }v;
    struct Node *right, *down;
}mynode, *node;

typedef struct Database{
    int num_row, num_col;   //行数,列数
    QString name_database;    //数据库表名称
    struct Node *row, *col;
}mydatabase, *database;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:   //槽函数
    void readorder();   //读取命令

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetTable();    //设置表格
    void AddList(QString str);     //设置列表
    void SetOrder();    //设置命令输入框

    void OpenDatabase(QString filename);    //初始化数据库
    void CloseDatabase();                   //关闭数据库并回收内存
    bool CreatDatabase(QString filename);   //创建数据库
    bool SaveDatabase();    //保存
    bool readtxt(QString filename);         //读取txt文件

    void AddColumn(QString order);  //添加列
    void AddRow();                          //添加行
    void Modify(QString order);    //修改
    bool LocateFor(QString order);   //查找
    bool DelateFor(QString order);   //删除

    struct Database *MyDatabase;
    QStandardItemModel* listmodel;
    QStandardItemModel* tablemodel;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
