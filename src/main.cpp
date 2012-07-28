#include <QtGui>

#include "manager.h"

int main(int argv, char **args) {
    QApplication app(argv, args);
    app.setApplicationName("QS1R_7_channels_audio_relay");

    Manager core;
    core.show();

    return app.exec();
}
