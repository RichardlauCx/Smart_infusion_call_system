#ifndef FORM_H
#define FORM_H

#include <QTcpSocket>
#include <QWidget>

namespace Ui {
class Form;
}
class InfoData{
public:
    QString name;
    QString doctor;
    QString nurses;
    int size;
};
class Form : public QWidget
{
    Q_OBJECT
public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void paintEvent(QPaintEvent *event);
    void setNumber(QString number);
    void setSp(QString sp);
    void setTm(QString tm);
    InfoData getInfo();
    void setInfo(QString name, QString doctor, QString nurses, int cap);
    QTcpSocket *getMsocket() const;
    void setMsocket(QTcpSocket *value);
    void write_file(QStringList data);
    QStringList read_file();
private slots:
    void on_setBt_clicked();
private:
    Ui::Form *ui;
    QTcpSocket *msocket;
};

#endif // FORM_H
