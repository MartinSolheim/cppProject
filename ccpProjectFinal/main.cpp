#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //Henter inn applikasjonen og kjører slik at applikasjonen blir vist
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
