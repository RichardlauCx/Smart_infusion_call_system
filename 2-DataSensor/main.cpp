#include "datesensor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DateSensor w;
    w.show();
    return a.exec();
}
