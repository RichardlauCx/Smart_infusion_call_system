#include "servertablewidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerTableWidget w;
    w.show();
    return a.exec();
}
