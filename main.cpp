#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("PetriRenamer C++");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationName("Hoang Minh Anh");
    app.setStyle(QStyleFactory::create("Fusion"));
    MainWindow window;
    window.show();
    return app.exec();
}
