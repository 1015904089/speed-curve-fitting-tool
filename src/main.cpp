#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Speed Curve Fitting Tool");
    QApplication::setOrganizationName("drawplot");

    MainWindow window;
    window.show();

    return app.exec();
}
