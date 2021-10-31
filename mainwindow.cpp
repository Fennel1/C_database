#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(readorder()));

    MyDatabase = new Database;
    MyDatabase->row = MyDatabase->col = NULL;

    listmodel = new QStandardItemModel();
    ui->listView->setModel(listmodel);
    tablemodel = new QStandardItemModel();
    ui->tableView->setModel(tablemodel);
    SetOrder();
}

void MainWindow::OpenDatabase(QString filename)
{
    CloseDatabase();

    if(readtxt("/home/kim/qt/qtetst/" + filename + ".txt"))     SetTable();
    else AddList(filename + "open failed");
}

bool MainWindow::readtxt(QString filead)
{
    QFile file(filead);
    Node *myrow, *mycol;
    if (file.exists())  printf("file exists\n");
    else{
        printf("file not exists\n");
        return false;
    }
    if (file.open(QIODevice::ReadOnly)) printf("file open\n");
    else{
        printf("file open fail\n");
        return false;
    }

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
        Node *rowtemp = new Node;
        Node *coltemp;
        if (i == 0){
            coltemp = MyDatabase->col;
        }
        else{
            coltemp = coltemp->down;
        }
        QString line=txt.readLine();
//        qDebug() << line << endl;
        QStringList strlist=line.split(",");
        qDebug() << strlist << Qt::endl;
        Node *cp=coltemp, *rp=rowtemp;
        for (int j=0; j<MyDatabase->num_col; j++){
            Node *tempnode = new Node;
            if (cp->flag==0){
                tempnode->v.data_int = strlist[j].toInt();
                tempnode->flag = 0;
            }
            else if (cp->flag==1){
                tempnode->v.data_int = strlist[j].toFloat();
                tempnode->flag = 1;
            }
            else if (cp->flag==2){
                tempnode->v.data_char = strlist[j];
                tempnode->flag = 2;
            }
            else{
                printf("col error\n");
            }
            rp->right = tempnode;
            rp = tempnode;
            cp->down = tempnode;
            tempnode->right = NULL;
            tempnode->down = NULL;
            cp = cp->right;
        }
        if (i==0){
            MyDatabase->row = rowtemp;
            myrow = rowtemp;
            rowtemp->down = NULL;
        }
        else{
            myrow->down = rowtemp;
            myrow = rowtemp;
            rowtemp->down = NULL;
        }
    }
    file.close();
    return true;
}

void MainWindow::SetTable()
{
    QStringList hlabels;
    Node *p=MyDatabase->col;
    while(p){
        hlabels << p->name;
        p = p->right;
    }
    tablemodel->setHorizontalHeaderLabels(hlabels);

    /* 自适应所有列，让它布满空间 */
    //        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    p=MyDatabase->row;
    int i=0, j=0;
    while(p){
        Node *temp=p->right;
        j=0;
        while(temp){
            if (temp->flag==0)  tablemodel->setItem(i, j, new QStandardItem(QString::number(temp->v.data_int)));
            else if (temp->flag==1) tablemodel->setItem(i, j, new QStandardItem(QString::number(temp->v.data_float)));
            else if (temp->flag==2) tablemodel->setItem(i, j, new QStandardItem(temp->v.data_char));
            temp = temp->right;
            j++;
        }
        i++;
        p = p->down;
    } 
}

void MainWindow::CloseDatabase()
{
    Node *rowtemp = MyDatabase->row;

    while(rowtemp){
        Node *tempnode = rowtemp->right, *delrow=rowtemp;
        while (tempnode){
            Node *delnode = tempnode;
            tempnode = tempnode->right;
            free(delnode);
        }
        rowtemp = rowtemp->down;
        free(delrow);
    }

    Node *coltemp = MyDatabase->col;
    while (coltemp){
        Node *delcol=coltemp;
        coltemp = coltemp->right;
        free(delcol);
    }

    MyDatabase->row = MyDatabase->col = NULL;
}

void MainWindow::AddList(QString str)
{

    listmodel->appendRow(new QStandardItem(str));

}

void MainWindow::SetOrder()
{
    ui->lineEdit->setPlaceholderText("请在此处输入命令");
}

void MainWindow::readorder()
{
//    qDebug() << ui->lineEdit->text() << Qt::endl;
    QString order = ui->lineEdit->text();

    order = order.simplified().toLower();    //去除多余空格,并转换为小写

    if (order.isEmpty() || order.isNull())  return;
    else if (order.length() > 4 && order.left(3) == "add"){
        AddList(QString(order));
    }
    else if (order.length() > 5 && order.left(4) == "open"){
        AddList(QString(order));
        OpenDatabase(order.right(order.length()-5));
    }
    else if (order.length() > 5 && order.left(5) == "creat"){
        AddList(QString(order));
        CreatDatabase(order.right(order.length()-5));
    }
    else if (order == "clear"){
        listmodel->clear();
        tablemodel->clear();
        CloseDatabase();
        AddList("clear");
    }
    else if (order.left(10) == "locate for"){
        AddList(QString(order));

    }
    else if (order.left(10) == "delete for"){
        AddList(QString(order));
    }
    else{
        AddList(QString("Order Error")+"->"+order);
    }

    ui->lineEdit->clear();

}

bool MainWindow::CreatDatabase(QString filename)
{
    QFile file("/home/kim/qt/qtetst/" + filename + ".txt");
    if (file.exists()){
        qDebug() << "file already exists" << Qt::endl;
        return false;
    }
    file.open(QIODevice::WriteOnly);
    file.close();
    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

