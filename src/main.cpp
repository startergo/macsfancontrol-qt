#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
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
