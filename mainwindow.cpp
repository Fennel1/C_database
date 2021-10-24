#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetTable();
    SetList();
}

void MainWindow::InitDatabase()
{
    MyDatabase = (mydatabase *)malloc(sizeof(mydatabase));

    MyDatabase->num_row = MyDatabase->num_col = 10;

    MyDatabase->row = (myhead *)malloc(sizeof(myhead));
    MyDatabase->col = (myhead *)malloc(sizeof(myhead));



}

void MainWindow::SetTable()
{
        /* 创建数据模型 */
        QStandardItemModel* model = new QStandardItemModel();

        /* 设置表格标题行(输入数据为QStringList类型) */
        model->setHorizontalHeaderLabels({"ID", "User Name", "City", "Classify", "Score", "Sign"});

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

