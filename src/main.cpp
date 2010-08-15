#include <QApplication>

#include "qpod.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qPod *pencere =  new qPod;

    pencere->show();
    return app.exec();
}



