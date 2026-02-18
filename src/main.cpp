#include <QApplication>
#include <QMessageBox>
#include <QStringList>
#include "mainwindow.h"
#include <unistd.h>
#include <cstdio>

// Captured debug log messages (appended by the message handler below)
static QStringList g_debugLog;

static void debugMessageHandler(QtMsgType type, const QMessageLogContext& /*context*/, const QString& msg)
{
    const char *prefix;
    switch (type) {
    case QtDebugMsg:    prefix = "[DEBUG]"; break;
    case QtWarningMsg:  prefix = "[WARN] "; break;
    case QtCriticalMsg: prefix = "[ERROR]"; break;
    case QtFatalMsg:    prefix = "[FATAL]"; break;
    default:            prefix = "[INFO] "; break;
    }
    g_debugLog.append(QString("%1 %2").arg(prefix, msg));
    fprintf(stderr, "%s %s\n", prefix, msg.toLocal8Bit().constData());
    if (type == QtFatalMsg)
        abort();
}

// Called from MainWindow to retrieve the captured log
QStringList getDebugLog()
{
    return g_debugLog;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(debugMessageHandler);

    QApplication app(argc, argv);

    // Set application information
    app.setApplicationName("Mac Fan Control");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("macsfancontrol-qt");

    // Check for root privileges (effective user ID)
    if (geteuid() != 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Permission Required");
        msgBox.setText("This application requires root privileges to control fans.");
        msgBox.setInformativeText("You can run in read-only mode to monitor fans, "
                                  "or restart with root privileges to enable fan control.\n\n"
                                  "To run with root access:\n"
                                  "sudo macsfancontrol");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);

        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel) {
            return 0;
        }
        // If OK, continue in read-only mode
    }

    MainWindow window;
    window.show();

    return app.exec();
}
