#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QTime>
#include <QApplication>
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      smcInterface(new SMCInterface(this)),
      tempPanel(new TemperaturePanel(this)),
      updateTimer(new QTimer(this))
{
    // Initialize SMC interface
    if (!smcInterface->initialize()) {
        QMessageBox::critical(this, "Initialization Error",
                            "Failed to initialize Apple SMC interface.\n"
                            "Make sure you're running on a Mac with applesmc driver loaded.");
        QTimer::singleShot(0, qApp, &QApplication::quit);
        return;
    }

    // Check for write permissions
    if (!smcInterface->hasWritePermission()) {
        QMessageBox::warning(this, "Permission Warning",
                           "Application does not have write permissions to SMC.\n"
                           "You will be able to monitor fans but not control them.\n"
                           "Run with: sudo macsfancontrol");
    }

    setupUI();
    createMenuBar();
    connectSignals();

    // Start update timer (1 second interval)
    updateTimer->start(1000);

    // Initial update
    updateSensorData();

    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    // Restore all fans to automatic mode on exit
    restoreAutoMode();
}

void MainWindow::setupUI()
{
    setWindowTitle("Mac Fan Control");
    resize(800, 600);

    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create main horizontal layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Left panel container
    QWidget *leftPanelContainer = new QWidget(this);
    QVBoxLayout *leftContainerLayout = new QVBoxLayout(leftPanelContainer);
    leftContainerLayout->setContentsMargins(0, 0, 0, 0);

    // Add title for fan section
    QLabel *fanTitle = new QLabel("<b>Fan Controls</b>", this);
    fanTitle->setStyleSheet("font-size: 14px; padding: 10px;");
    leftContainerLayout->addWidget(fanTitle);

    // Create scroll area for fan controls
    QScrollArea *fanScrollArea = new QScrollArea(this);
    fanScrollArea->setWidgetResizable(true);
    fanScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    fanScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Fan controls content widget
    QWidget *fanContentWidget = new QWidget();
    QVBoxLayout *fanLayout = new QVBoxLayout(fanContentWidget);
    fanLayout->setSpacing(10);
    fanLayout->setContentsMargins(10, 10, 10, 10);

    // Create fan control widgets
    QVector<FanInfo> fans = smcInterface->getFans();
    for (const FanInfo& fan : fans) {
        FanControlWidget *fanWidget = new FanControlWidget(fan, this);
        fanWidgets.append(fanWidget);
        fanLayout->addWidget(fanWidget);

        // Initialize sensor-based settings
        SensorBasedSettings settings = {false, -1, 40, 80};
        sensorSettings.append(settings);

        // Connect fan widget signals to SMC interface
        connect(fanWidget, &FanControlWidget::manualModeRequested,
                smcInterface, &SMCInterface::setFanManualMode);
        connect(fanWidget, &FanControlWidget::targetRPMChanged,
                smcInterface, &SMCInterface::setFanSpeed);
        connect(fanWidget, &FanControlWidget::sensorBasedModeChanged,
                this, &MainWindow::onSensorBasedModeChanged);
    }

    // Initialize sensor list in fan widgets
    updateSensorListInFanWidgets();

    fanLayout->addStretch();

    // Add content to scroll area
    fanScrollArea->setWidget(fanContentWidget);
    leftContainerLayout->addWidget(fanScrollArea);

    // Right panel: Temperature display
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addWidget(tempPanel);

    // Add panels to main layout
    mainLayout->addWidget(leftPanelContainer, 1);
    mainLayout->addWidget(rightPanel, 1);

    // Create status bar
    statusBar()->showMessage("Initializing...");
}

void MainWindow::createMenuBar()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About Mac Fan Control",
                         "Mac Fan Control for Linux\n\n"
                         "A Qt5 application for controlling Apple SMC fans.\n\n"
                         "Controls fans on Mac hardware running Linux with applesmc driver.\n\n"
                         "Features:\n"
                         "- Real-time fan speed monitoring\n"
                         "- Manual and automatic fan control\n"
                         "- Temperature sensor monitoring\n"
                         "- Safety enforcement (min/max RPM limits)");
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::connectSignals()
{
    // Connect timer to update function
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateSensorData);

    // Connect SMC interface signals
    connect(smcInterface, &SMCInterface::error, this, &MainWindow::showError);
    connect(smcInterface, &SMCInterface::warning, this, &MainWindow::showWarning);
}

void MainWindow::updateSensorData()
{
    // Get temperature readings first
    QVector<TempSensor> temps = smcInterface->getTemperatures();

    // Update all fan RPMs
    for (int i = 0; i < fanWidgets.size(); i++) {
        int rpm = smcInterface->getFanCurrentRPM(i);
        if (rpm >= 0) {  // Valid reading
            fanWidgets[i]->setCurrentRPM(rpm);
        }

        // Update sensor-based fans
        if (sensorSettings[i].enabled && sensorSettings[i].sensorIndex >= 0) {
            // Find the temperature for the selected sensor
            for (const TempSensor& sensor : temps) {
                if (sensor.index == sensorSettings[i].sensorIndex) {
                    fanWidgets[i]->updateSensorBasedSpeed(sensor.temperature);
                    break;
                }
            }
        }
    }

    // Update temperature panel
    tempPanel->updateTemperatures(temps);

    // Update sensor list in fan widgets periodically (every 10 seconds)
    static int updateCount = 0;
    if (++updateCount % 10 == 0) {
        updateSensorListInFanWidgets();
    }

    // Update status bar
    statusBar()->showMessage(QString("Last update: %1").arg(QTime::currentTime().toString("hh:mm:ss")));
}

void MainWindow::showError(const QString& message)
{
    // Show error in status bar
    statusBar()->showMessage("Error: " + message, 5000);

    // Log to stderr
    qWarning() << "SMC Error:" << message;

    // For critical permission errors, show dialog
    if (message.contains("permission", Qt::CaseInsensitive) ||
        message.contains("root", Qt::CaseInsensitive)) {
        QMessageBox::critical(this, "Permission Error",
                            "Cannot write to SMC interface.\n\n" + message +
                            "\n\nPlease run with: sudo macsfancontrol");
    }
}

void MainWindow::showWarning(const QString& message)
{
    // Show warning in status bar
    statusBar()->showMessage("Warning: " + message, 3000);

    // Log to stderr
    qWarning() << "SMC Warning:" << message;
}

void MainWindow::restoreAutoMode()
{
    // Restore all fans to automatic mode
    for (int i = 0; i < fanWidgets.size(); i++) {
        smcInterface->setFanManualMode(i, false);
    }
}

void MainWindow::onSensorBasedModeChanged(int fanIndex, bool enable, int sensorIndex, int minTemp, int maxTemp)
{
    if (fanIndex < 0 || fanIndex >= sensorSettings.size()) {
        return;
    }

    // Update sensor-based settings
    sensorSettings[fanIndex].enabled = enable;
    sensorSettings[fanIndex].sensorIndex = sensorIndex;
    sensorSettings[fanIndex].minTemp = minTemp;
    sensorSettings[fanIndex].maxTemp = maxTemp;

    // Log the change
    if (enable) {
        qDebug() << "Fan" << fanIndex << "sensor-based mode enabled:"
                 << "sensor" << sensorIndex
                 << "temp range" << minTemp << "-" << maxTemp << "°C";
    }
}

void MainWindow::updateSensorListInFanWidgets()
{
    // Get current temperature sensors
    QVector<TempSensor> temps = smcInterface->getTemperatures();

    // Update sensor list in each fan widget
    for (FanControlWidget* fanWidget : fanWidgets) {
        fanWidget->setSensorList(temps);
    }
}
