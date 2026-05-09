#include "ui/MainWindow.h"
#include "core/ImageLoader.h"
#include "core/ImageCache.h"
#include "core/ImageNavigator.h"
#include "version.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("SimplePicture");
    app.setApplicationName("SimplePicture");
    app.setApplicationVersion(EASYPIC_VERSION);
    app.setWindowIcon(QIcon(QStringLiteral(":/resources/app-icon.svg")));

    simplepic::MainWindow window(
        std::make_unique<simplepic::ImageLoader>(),
        std::make_unique<simplepic::ImageCache>(),
        std::make_unique<simplepic::ImageNavigator>());
    window.show();

    const QStringList args = app.arguments();
    if (args.size() > 1) {
        window.openFile(args.at(1));
    }

    return app.exec();
}
