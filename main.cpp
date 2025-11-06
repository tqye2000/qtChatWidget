#include <QApplication>
#include "DemoWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DemoWindow demo;
    demo.show();

    return a.exec();
}
