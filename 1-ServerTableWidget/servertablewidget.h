#ifndef SERVERTABLEWIDGET_H
#define SERVERTABLEWIDGET_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerTableWidget; }
QT_END_NAMESPACE

class ServerTableWidget : public QMainWindow
{
    Q_OBJECT
public:
    ServerTableWidget(QWidget *parent = nullptr);
    ~ServerTableWidget();

    //定时器事件
    void timerEvent(QTimerEvent *e);
    int updatetmId;
    int updateinfoId;
    QStringList read_file();
private slots:
    void new_client(); //新客户端连接对应的槽函数
    void dis_client();//客户掉线对应的槽函数
    void data_client();//读取客户端数据
    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::ServerTableWidget *ui;
    QTcpServer mserver;//监听套接字
    QVector<QTcpSocket*> sockets; //存储 所有的客户端套接字（收发数据）

};
#endif // SERVERTABLEWIDGET_H
