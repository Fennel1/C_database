#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetTable();
    SetList();
    InitDatabase();
}

void MainWindow::InitDatabase()
{
    MyDatabase = (mydatabase *)malloc(sizeof(mydatabase));

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
    qDebug() << MyDatabase->num_row << MyDatabase->num_col << endl;
    for (int i=0; i<MyDatabase->num_col; i++){
        Node *temp = (mynode *)malloc(sizeof(mynode));
        QString line=txt.readLine();
//        qDebug() << line << endl;
        QStringList strlist=line.split(",");
//        qDebug() << strlist << endl;
        temp->flag = strlist[0].toInt();
        QByteArray ba = strlist[1].toLatin1();
        temp->name = ba.data();
        qDebug() << temp->flag << temp->name << endl;
        if (i==1){
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
        Node *temp = (mynode *)malloc(sizeof(mynode));
        QString line=txt.readLine();
//        qDebug() << line << endl;
        QStringList strlist=line.split(",");
        qDebug() << strlist << endl;
        Node *p = MyDatabase->col;
        for (int j=0; j<MyDatabase->num_col; j++){
            if (p->flag==0){

            }
            else if (p->flag==1){

            }
            else if (p->flag==2){

            }
            else{
                printf("col error\n");
            }
        }
        if (i==1){
            MyDatabase->row = temp;
            myrow = temp;
            temp->down = NULL;
        }
        else{
            mycol->down = temp;
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

        /* 设置表格标题行(输入数据为QStringList类型) */
        model->setHorizontalHeaderLabels({"ID", "User Name", "City", "Classify", "Score", "Sign","7","8"});

        /* 自适应所有列，让它布满空间 */
//        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        /* 加载共10行数据，并每行有6列数据 */
        for (int i = 0; i < 100; i++) {
            /* 加载第一列(ID)数据 */
            model->setItem(i, 0, new QStandardItem(QString("1000%1").arg(i)));
            /* 加载第二列(User Name)数据 */
            model->setItem(i, 1, new QStandardItem(QString("User%1").arg(i)));
            /* 加载第三列(City)数据 */
            model->setItem(i, 2, new QStandardItem("Shanghai"));
            /* 加载第四列(Classify)数据 */
            model->setItem(i, 3, new QStandardItem("Engineer"));
            /* 加载第五列(Score)数据 */
            model->setItem(i, 4, new QStandardItem("80"));
            /* 加载第六列(Sign)数据 */
            model->setItem(i, 5, new QStandardItem("Hello world!"));
            model->setItem(i, 6, new QStandardItem("Hello world!"));
            model->setItem(i, 7, new QStandardItem("Hello world!"));
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

