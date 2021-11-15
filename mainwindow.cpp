#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setBaseSize(1600,800);

    ui->textEdit->setReadOnly(true);
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(readorder()));

    MyDatabase = new Database;
    MyDatabase->row = MyDatabase->col = NULL;
    MyDatabase->num_row = MyDatabase->num_col = 0;

    listmodel = new QStandardItemModel();
    ui->listView->setModel(listmodel);
    tablemodel = new QStandardItemModel();
    ui->tableView->setModel(tablemodel);
    SetOrder();
}

void MainWindow::OpenDatabase(QString filename)
{
    CloseDatabase();

    if(readtxt("D:/GitHub/C_database/" + filename + ".txt")){
        AddList("Open " + filename);
        SetTable();
    }
}

bool MainWindow::readtxt(QString filead)
{
    QFile file(filead);
    Node *myrow, *mycol;
    if (file.exists())  printf("file exists\n");
    else{
        AddList("Open error:file not exists");
        return false;
    }
    if (file.open(QIODevice::ReadOnly)) printf("file open\n");
    else{
        AddList("Open error:file open fail");
        return false;
    }

    QTextStream txt(&file);
    QString line=txt.readLine();
    QStringList strlist=line.split(",");
    MyDatabase->name_database=strlist[0];
    MyDatabase->num_row=strlist[1].toInt();
    MyDatabase->num_col=strlist[2].toInt();
    qDebug() << MyDatabase->num_row << MyDatabase->num_col << endl;
    for (int i=0; i<MyDatabase->num_col; i++){
        Node *temp = new Node;
        QString line=txt.readLine();
//        qDebug() << line << endl;
        QStringList strlist=line.split(",");
        temp->flag = strlist[0].toInt();
        temp->name = strlist[1];
        qDebug() << temp->flag << temp->name << endl;
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
        qDebug() << strlist << endl;
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
    tablemodel->clear();
    QStringList hlabels;
    Node *p=MyDatabase->col;
    while(p){
        hlabels << p->name;
//        qDebug() << p->name << endl;
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
            delete delnode;
        }
        rowtemp = rowtemp->down;
        delete delrow;
    }

    Node *coltemp = MyDatabase->col;
    while (coltemp){
        Node *delcol=coltemp;
        coltemp = coltemp->right;
        delete delcol;
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
//    qDebug() << ui->lineEdit->text() << endl;
    QString order = ui->lineEdit->text();

    order = order.simplified();    //去除多余空格,并转换为小写

    if (order.isEmpty() || order.isNull())  return;
    else if (order.length()==4 && order=="help"){

    }
    else if (order.length() == 4 && order == "save"){
        if(SaveDatabase())  AddList("Successfully saved file");
        else    AddList("Save file failed");
    }
    else if (order.length() > 4 && order.left(3) == "add"){
        if(order.mid(4,3)=="col"){
           AddColumn(order);
        }
        else if (order.mid(4,3)=="row"){
            AddRow();
        }
    }
    else if (order.length() > 4 && order.left(3)=="mod") {                            //mod (1,user name,aaa)
        Modify(order);
    }
    else if (order.length() > 5 && order.left(4) == "open"){
        OpenDatabase(order.right(order.length()-5));
    }
    else if (order.length() > 6 && order.left(5) == "creat"){
        CreatDatabase(order.right(order.length()-6));
    }
    else if (order.length() == 5 && order == "clear"){
        listmodel->clear();
        tablemodel->clear();
        CloseDatabase();
        AddList("clear");
    }
    else if (order.length() > 10 && order.left(10) == "locate for"){
        if (LocateFor(order.right(order.length()-10)))  AddList(order);
        else    AddList("Locate error");
    }
    else if (order.length() > 10 && order.left(10) == "delete for"){
        if (DelateFor(order.right(order.length()-10)))  AddList(order);
        else    AddList("Delete error");
    }
    else{
        AddList("Order Error:" + order);
    }

    ui->lineEdit->clear();

}

void MainWindow::AddColumn(QString order)
{
    if (MyDatabase->row == 0){
        AddList("Add error:num of row is 0");
        return;
    }
    int type;
    QString name;
    if(order.mid(8,3)=="int"){
        name = order.mid(12);
        type = 0;
    }
    else if (order.mid(8,4)=="char") {
        name = order.mid(13);
        type = 2;
    }
    else if (order.mid(8,5)=="float") {
        name = order.mid(14);
        type = 1;
    }

    if(name==NULL)
    {
        return;
    }
    MyDatabase->num_col++;
    Node *temp = new Node;
    Node* p=MyDatabase->col;
    while(p->right){
        p=p->right;
    }
    temp->name = name;
    temp->flag = type;
    temp->right=NULL;
    temp->down=NULL;
    p->right=temp;
    Node* c=p;
    for (int i=0;i<MyDatabase->num_row;i++) {
        c=c->down;
        Node *tem = new Node;
        tem->v.data_int=0;
        tem->v.data_char="None";
        tem->v.data_float=0.0;
        tem->flag=type;
        tem->right=NULL;
        tem->down=NULL;
        temp->down=tem;
        c->right=tem;
        temp=tem;
    }
    SetTable();
    AddList("Add col");
}

void MainWindow::AddRow()
{
    if (MyDatabase->col == 0){
        AddList("Add error:num of col is 0");
        return;
    }
    MyDatabase->num_row++;
    Node* f=MyDatabase->col;
    Node* p=MyDatabase->row;
    Node* prep=MyDatabase->row;
    while(p->down){
        p=p->down;
    }
    Node *head = new Node;
    Node *h;
    h=head;
    for (int i=2;i<MyDatabase->num_row;i++) {
        prep=prep->down;
    }
    prep=prep->right;
    for(int i=0;i<MyDatabase->num_col;i++){
        Node *temp = new Node;
        temp->v.data_int=0;
        temp->v.data_char="None";
        temp->v.data_float=0.0;
        temp->flag=f->flag;
        f=f->right;
        prep->down=temp;
        prep=prep->right;
        h->right=temp;
        h=temp;
    }
    h->right=NULL;
    head->down=NULL;
    p->down=head;
    SetTable();
    AddList("Add row");
}

void MainWindow::Modify(QString order)
{
    int i1=5,i2,i3, k=4;
    int n1=0,n2=0,n3=0;
    while(order[k]==" ")    k++;
    if(order[k]=='('){
        while(order[i1]!=','){
            i1++;
            n1++;
            if (i1>order.length()){
                AddList(QString("Modify Error:order Error"));
                return;
            }
        }
        i2=i1+1;
        while(order[i2]!=','){
            i2++;
            n2++;
            if (i2>order.length()){
                AddList(QString("Modify Error:order Error"));
                return;
            }
        }
        i3=i2+1;
        while(order[i3]!=')'){
            i3++;
            n3++;
            if (i3>order.length()){
                AddList(QString("Modify Error:order Error"));
                return;
            }
        }
    }
    else{
        AddList(QString("Modify Error:order Error"));
        return;
    }
    int row = order.mid(i1-n1,n1).toInt();
    QString col = order.mid(i2-n2,n2).simplified();
    QString change = order.mid(i3-n3,n3).simplified();

    qDebug() << row << col << change << endl;
    Node *f=MyDatabase->col, *p=MyDatabase->col;
    if (row > MyDatabase->num_row){
        AddList(QString("Modify Error:exceed the rowlimit"));
        return;
    }
    for (int i=0;i<row;i++) {
        f=f->down;
    }
    while (p&&p->name!=col){
        f = f->right;
        p = p->right;
    }
    if (f==NULL){
        AddList(QString("Modify Error:colname not find"));
        return;
    }

    if(f->flag==0){
        for (int i=0;change[i]!='\0';i++) {
            if(change[i]<'0'||change[i]>'9'){
                AddList("Modify error:value type error");
                return;
            }
        }
        f->v.data_int=change.toInt();
    }
    else if (f->flag==1) {
        for (int i=0;change[i]!='\0';i++) {
            if(change[i]<'0'||change[i]>'9'){
                AddList("Modify error:value type error");
                return;
            }
        }
        f->v.data_float=change.toFloat();
    }
    else if (f->flag==2) {
        f->v.data_char=change;
    }
    SetTable();
}

bool MainWindow::CreatDatabase(QString filename)
{
    QFile file("/home/kim/qt/qtetst/" + filename + ".txt");
    if (file.exists()){
        AddList("Creat error:file already exists");
        return false;
    }
    file.open(QIODevice::WriteOnly);
    file.write(filename.toLocal8Bit());
    file.write(QString(",1,1\n").toLocal8Bit());
    file.write(QString("0,ID\n").toLocal8Bit());
    file.write(QString("10000\n").toLocal8Bit());
    file.close();
    OpenDatabase(filename);
    AddList("Creat file");
    return true;
}

bool MainWindow::SaveDatabase()
{
    QFile file("/home/kim/qt/qtetst/" + MyDatabase->name_database + ".txt");
    if (!file.exists()){
        qDebug() << "file not exists" << endl;
        return false;
    }
    QTextStream txt(&file);
    file.open(QIODevice::WriteOnly| QIODevice::Text);
    txt << MyDatabase->name_database << "," << QString::number(MyDatabase->num_row)
                   << "," << QString::number(MyDatabase->num_col) << "\n";
    Node *coltemp = MyDatabase->col;
    while (coltemp){
        txt << QString::number(coltemp->flag) << "," << coltemp->name << "\n";
        coltemp = coltemp->right;
    }
    Node *rowtemp = MyDatabase->row;
    while(rowtemp){
        Node *nodetemp=rowtemp->right;
        if (nodetemp){
            if (nodetemp->flag == 0)        txt << QString::number(nodetemp->v.data_int);
            else if (nodetemp->flag == 1)   txt << QString::number(nodetemp->v.data_float);
            else if (nodetemp->flag == 2)   txt << nodetemp->v.data_char;
            nodetemp = nodetemp->right;
        }
        while(nodetemp){
            if (nodetemp->flag == 0)        txt << "," << QString::number(nodetemp->v.data_int);
            else if (nodetemp->flag == 1)   txt << "," << QString::number(nodetemp->v.data_float);
            else if (nodetemp->flag == 2)   txt << "," << nodetemp->v.data_char;
            nodetemp = nodetemp->right;
        }
        txt << "\n";
        rowtemp = rowtemp->down;
    }
    file.close();
    return true;
}

bool MainWindow::LocateFor(QString order)
{
    QStringList strlist;
    QString op;
    if (order.contains(">=")){
        strlist = order.split(">=");
        op = ">=";
    }
    else if (order.contains(">")){
        strlist = order.split(">");
        op = ">";
    }
    else if (order.contains("<=")){
        strlist = order.split("<=");
        op = "<=";
    }
    else if (order.contains("<")){
        strlist = order.split("<");
        op = "<";
    }
    else if (order.contains("==")){
        strlist = order.split("==");
        op = "==";
    }
    else if (order.contains("!=")){
        strlist = order.split("!=");
        op = "!=";
    }
    else{
        return false;
    }
    QString colname = strlist[0].trimmed();
    QString value = strlist[1].trimmed();
    qDebug() << colname << value << op << endl;

    Node *coltemp = MyDatabase->col;
    while(coltemp && coltemp->name!=colname){
        coltemp = coltemp->right;
    }
    if (coltemp==NULL){
        qDebug() << "colname not find" << endl;
        return false;
    }
    if ((op==">=" || op==">" || op=="<=" || op=="<") && coltemp->flag == 2){
        qDebug() << "col type error";
        return false;
    }

    coltemp = coltemp->down;
    Node *rowtemp = MyDatabase->row, *prerow=NULL;
    bool isfirstrow=true;
    if (op == "=="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (QString::number(coltemp->v.data_int) == value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (QString::number(coltemp->v.data_float) == value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            while(rowtemp){
                if (coltemp->v.data_char == value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
    }
    else if (op == "!="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (QString::number(coltemp->v.data_int) != value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;


                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (QString::number(coltemp->v.data_float) != value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            while(rowtemp){
                if (coltemp->v.data_char != value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
    }
    else if (op == ">="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int >= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float >= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }
    else if (op == ">"){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int > value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float > value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }
    else if (op == "<="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int <= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float <= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }
    else if (op == "<"){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int < value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float < value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }

    SetTable();
    return true;
}

bool MainWindow::DelateFor(QString order)
{
    QStringList strlist;
    QString op;
    if (order.contains(">=")){
        strlist = order.split(">=");
        op = ">=";
    }
    else if (order.contains(">")){
        strlist = order.split(">");
        op = ">";
    }
    else if (order.contains("<=")){
        strlist = order.split("<=");
        op = "<=";
    }
    else if (order.contains("<")){
        strlist = order.split("<");
        op = "<";
    }
    else if (order.contains("==")){
        strlist = order.split("==");
        op = "==";
    }
    else if (order.contains("!=")){
        strlist = order.split("!=");
        op = "!=";
    }
    else{
        return false;
    }
    QString colname = strlist[0].trimmed();
    QString value = strlist[1].trimmed();
    qDebug() << colname << value << op << endl;

    Node *coltemp = MyDatabase->col;
    while(coltemp && coltemp->name!=colname){
        coltemp = coltemp->right;
    }
    if (coltemp==NULL){
        qDebug() << "colname not find" << endl;
        return false;
    }
    if ((op==">=" || op==">" || op=="<=" || op=="<") && coltemp->flag == 2){
        qDebug() << "col type error";
        return false;
    }

    coltemp = coltemp->down;
    Node *rowtemp = MyDatabase->row, *prerow=NULL;
    bool isfirstrow=true;
    if (op == "=="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (QString::number(coltemp->v.data_int) != value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (QString::number(coltemp->v.data_float) != value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            while(rowtemp){
                if (coltemp->v.data_char != value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
    }
    else if (op == "!="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (QString::number(coltemp->v.data_int) == value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (QString::number(coltemp->v.data_float) == value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            while(rowtemp){
                if (coltemp->v.data_char == value){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
    }
    else if (op == ">="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int < value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float < value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }
    else if (op == ">"){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int <= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float <= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }
    else if (op == "<="){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int > value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float > value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }
    else if (op == "<"){
        if (coltemp->flag == 0){
            while(rowtemp){
                if (coltemp->v.data_int >= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 1){
            while(rowtemp){
                if (coltemp->v.data_float >= value.toInt()){
                    if (isfirstrow){
                        Node *cp = MyDatabase->col, *rp=rowtemp->right;
                        while(cp){
                            cp->down = rp;
                            cp = cp->right;
                            rp = rp->right;
                        }
                        MyDatabase->row = rowtemp;
                        prerow = rowtemp;
                        isfirstrow = false;
                    }
                    else{
                       Node *cp = prerow->right, *rp=rowtemp->right;
                       while(cp){
                           cp->down = rp;
                           cp = cp->right;
                           rp = rp->right;
                       }
                       prerow->down = rowtemp;
                       prerow = rowtemp;
                    }
                }
                else{
                    MyDatabase->num_row--;
                }
                coltemp = coltemp->down;
                rowtemp = rowtemp->down;
            }
            Node *rp=prerow;
            if (rp==NULL){
                Node *rowtemp = MyDatabase->row;
                while(rowtemp){
                    Node *tempnode = rowtemp->right, *delrow=rowtemp;
                    while (tempnode){
                        Node *delnode = tempnode;
                        tempnode = tempnode->right;
                        delete delnode;
                    }
                    rowtemp = rowtemp->down;
                    delete delrow;
                }
                MyDatabase->row = NULL;
                Node *coltemp = MyDatabase->col;
                while(coltemp){
                    coltemp->down = NULL;
                    coltemp = coltemp->right;
                }
            }
            else{
                while(rp){
                    rp->down = NULL;
                    rp = rp->right;
                }
            }
        }
        else if (coltemp->flag == 2){
            return false;
        }
    }

    SetTable();
    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
    CloseDatabase();
    free(MyDatabase);
}

