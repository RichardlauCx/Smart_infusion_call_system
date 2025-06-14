#include "inputdatawidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InputDataWidget w;
    w.show();
    return a.exec();
}
