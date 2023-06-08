
#include "MacroWindow.h"

#include <QApplication>
#include <QString>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MacroWindow w;
    w.show();
    return a.exec();

    return 0;
}
