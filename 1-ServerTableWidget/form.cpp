#include "form.h"
#include "ui_form.h"

#include <QFile>
#include <QInputDialog>
#include <QPainter>
#include <QStyleOption>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    this->msocket = NULL;
}

Form::~Form()
{
    delete ui;
}

void Form::paintEvent(QPaintEvent *event)
{
    QStyleOption  opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt, &painter,this);
}

void Form::setNumber(QString number)
{
    ui->number->setText(number);
}

void Form::setSp(QString sp)
{
    ui->spvalue->setText(sp);
}

void Form::setTm(QString tm)
{
    ui->timevale->setText(tm);
}

InfoData Form::getInfo()
{
    InfoData data;
    data.name = ui->name->text();
    data.doctor = ui->docvalue->text();
    data.nurses = ui->nurvalue->text();
    data.size = ui->sizevalue->text().remove("ml").toInt();
    return data;
}

void Form::setInfo(QString name, QString doctor, QString nurses, int cap)
{
        ui->name->setText(name);
        ui->docvalue->setText(doctor);
        ui->nurvalue->setText(nurses);
        ui->sizevalue->setText(QString::number(cap)+"ml");
}

void Form::on_setBt_clicked()
{
    QString name = ui->name->text();
    QString doctor = ui->docvalue->text();
    QString nurses = ui->nurvalue->text();
    QString sizecap = ui->sizevalue->text();

    QString tmpname = QInputDialog::getText(this, "输入提示","请输入住院者姓名");
    if(!tmpname.isEmpty()) name = tmpname;
    tmpname = QInputDialog::getText(this, "输入提示","请输主治医师名称");
    if(!tmpname.isEmpty()) doctor = tmpname;
    tmpname = QInputDialog::getText(this, "输入提示","请输护士名称");
    if(!tmpname.isEmpty()) nurses = tmpname;
    tmpname = QInputDialog::getText(this, "输入提示","请输输液容量");
    if(!tmpname.isEmpty()) sizecap = tmpname;

    QStringList list = read_file();
    //修改数据
    QString data = QString("%1:%2:%3:%4:%5\n")
            .arg(ui->number->text())
            .arg(name)
            .arg(doctor)
            .arg(nurses)
            .arg(sizecap);
    int number = ui->number->text().toUInt();//行号--床位号
    if(number<list.count())
    {
        list.removeAt(number-1);//删除
        list.insert(number-1, data);//插入
    }else
    {
        list<<data; //追加
    }

    write_file(list);

    ui->name->setText(name);
    ui->docvalue->setText(doctor);
    ui->nurvalue->setText(nurses);
    ui->sizevalue->setText(sizecap+"ml");

    if(msocket == NULL) return;

    //打包数据发送给客户端
    QString reply=QString("{\"name\":\"%1\",\"doctor\":\"%2\",\"nurses\":\"%3\",\"capacity\":%4,\"flag\":0}")
            .arg(ui->name->text())
            .arg(ui->docvalue->text())
            .arg(ui->nurvalue->text())
            .arg(sizecap);
    msocket->write(reply.toUtf8());
}

QTcpSocket *Form::getMsocket() const
{
    return msocket;
}

void Form::setMsocket(QTcpSocket *value)
{
    msocket = value;
}

void Form::write_file(QStringList data)
{
    //创建QFile对象
   QFile file("F:/Computer/UI/QT/Project/project_1_2/file.txt"); //注意路径不要错了
    //打开文件
    bool flag = file.open(QIODevice::WriteOnly);
    if(!flag)
    {
        qDebug()<<"打开文件失败";
        exit(1);
    }
    //写文件
    for(int i=0; i<data.count();  i++){
        int size = file.write(data.at(i).toUtf8());
    }
    //关闭文件
    file.close();
}

QStringList Form::read_file()
{
    //创建QFile对象
    QFile file("F:/Computer/UI/QT/Project/project_1_2/file.txt"); //注意路径不要错了
    //打开文件
    bool flag = file.open(QIODevice::ReadOnly); //只读
    if(!flag)
    {
        qDebug()<<"打开文件失败";
        exit(1);
    }
    QStringList datalist;
    while(!file.atEnd())
    {
        QString data = file.readLine(); //读取一行数据
        datalist<<data;//把读到的数据添加到列表中
    }
    //关闭文件
    file.close();
    return datalist;
}
