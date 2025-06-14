#include "inputdatawidget.h"
#include "ui_inputdatawidget.h"
#include <QDebug>
#include <QFile>
InputDataWidget::InputDataWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::InputDataWidget)
{
    ui->setupUi(this);
    dataList.clear();
    dataList = read_file();

}

InputDataWidget::~InputDataWidget()
{
    delete ui;
}
void InputDataWidget::write_file(QStringList data)
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

QStringList InputDataWidget::read_file()
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

void InputDataWidget::on_addBt_clicked()
{
    //提取数据
    QString data = QString("%1:%2:%3:%4:%5\n")
            .arg(ui->number->text())
            .arg(ui->name->text())
            .arg(ui->doctor->text())
            .arg(ui->nurses->text())
            .arg(ui->captain->text());
    int number = ui->number->text().toUInt();//行号--床位号
    if(number<dataList.count())
    {
        dataList.removeAt(number-1);//删除
        dataList.insert(number-1, data);//插入
    }else
    {
        dataList<<data; //追加
    }

}

void InputDataWidget::on_yesBt_clicked()
{
    write_file(dataList);
}
