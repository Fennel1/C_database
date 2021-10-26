#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitDatabase();
    SetTable();
    SetList();
}

void MainWindow::InitDatabase()
{
    MyDatabase = new Database;

    Node *myrow, *mycol;
    QFile file("/home/kim/qt/qtetst/file.txt");
    if (file.exists())  printf("file exists\n");
    else    printf("file error\n");
//    file.open(QIODevice::ReadOnly);
    if (file.open(QIODevice::ReadOnly)) printf("file open\n");
    else    printf("file open fail\n");

    QTextStream txt(&file);
    QString line=txt.readLine();
    QStringList strlist=line.split(",");
    MyDatabase->num_row=strlist[0].toInt();
    MyDatabase->num_col=strlist[1].toInt();
    qDebug() << MyDatabase->num_row << MyDatabase->num_col << Qt::endl;
    for (int i=0; i<MyDatabase->num_col; i++){
        Node *temp = new Node;
        QString line=txt.readLine();
//        qDebug() << line << endl;
        QStringList strlist=line.split(",");
        temp->flag = strlist[0].toInt();
        temp->name = strlist[1];
//        QByteArray ba = strlist[1].toLatin1();
//        temp->name = ba.data();
        qDebug() << temp->flag << temp->name << Qt::endl;
        if (i==0){
            MyDatabase->col = temp;
            mycol = temp;
            temp->right = NULL;
        }
        else{
            mycol->right = temp;
            mycol = temp;
            temp->right = NULL;
        }
    }
    for (int i=0; i<MyDatabase->num_row; i++){
        Node *temp = new Node;
        QString line=txt.readLine();
//        qDebug() << line << endl;
        QStringList strlist=line.split(",");
        qDebug() << strlist << Qt::endl;
        Node *p=MyDatabase->col, *tp=temp;
        for (int j=0; j<MyDatabase->num_col; j++){
            Node *tempnode = new Node;
            if (p->flag==0){
                tempnode->v.data_int = strlist[j].toInt();
                tempnode->flag = 0;
            }
            else if (p->flag==1){
                tempnode->v.data_int = strlist[j].toFloat();
                tempnode->flag = 1;
            }
            else if (p->flag==2){
                tempnode->v.data_char = strlist[j];
                tempnode->flag = 2;
            }
            else{
                printf("col error\n");
            }
            tp->right = tempnode;
            tp = tempnode;
            tempnode->right = NULL;
            p = p->right;
        }
        if (i==0){
            MyDatabase->row = temp;
            myrow = temp;
            temp->down = NULL;
        }
        else{
            myrow->down = temp;
            myrow = temp;
            temp->down = NULL;
        }
    }
    file.close();
}

void MainWindow::SetTable()
{
    /* 创建数据模型 */
    QStandardItemModel* model = new QStandardItemModel();

    QStringList hlabels;
    Node *p=MyDatabase->col;
    while(p){
        hlabels << p->name;
        //            qDebug() << p->flag << p->name << Qt::endl;
        p = p->right;
    }
    model->setHorizontalHeaderLabels(hlabels);

    /* 自适应所有列，让它布满空间 */
    //        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    p=MyDatabase->row;
    int i=0, j=0;
    while(p){
        Node *temp=p->right;
        j=0;
        while(temp){
            if (temp->flag==0)  model->setItem(i, j, new QStandardItem(QString::number(temp->v.data_int)));
            else if (temp->flag==1) model->setItem(i, j, new QStandardItem(QString::number(temp->v.data_float)));
            else if (temp->flag==2) model->setItem(i, j, new QStandardItem(temp->v.data_char));
            temp = temp->right;
            j++;
        }
        i++;
        p = p->down;
    }

    ui->tableView->setModel(model);
}

void MainWindow::SetList()
{
    QStandardItemModel* model = new QStandardItemModel();

    model->appendRow(new QStandardItem("Database1"));

    ui->listView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

