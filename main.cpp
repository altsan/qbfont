#include <QApplication>

#include "mainwindow.h"

int main( int argc, char *argv[] )
{
    int rc;
    QApplication app( argc, argv );
    FontEditor *qfe = new FontEditor;
    qfe->show();
    rc = app.exec();
    delete qfe;
    return rc;
}
