#include "mainwindow.h"
#include "sensordescriptions.h"
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
#include <QInputDialog>
#include <QSettings>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      smcInterface(new SMCInterface(this)),
      hwmonInterface(new HWMonInterface(this)),
      tempPanel(new TemperaturePanel(this)),
      updateTimer(new QTimer(this))
{
    bool smcAvailable = false;
    bool hwmonAvailable = false;

    // Initialize SMC interface
    if (smcInterface->initialize()) {
        smcAvailable = true;
        qDebug() << "SMC interface initialized";
    } else {
        qWarning() << "SMC interface not available";
    }

    // Initialize HWMon interface
    hwmonInterface->setSmcAvailable(smcAvailable);
    if (hwmonInterface->initialize()) {
        hwmonAvailable = true;
        qDebug() << "HWMon interface initialized";
    } else {
        qWarning() << "HWMon interface not available";
    }

    // Check if at least one interface is available
    if (!smcAvailable && !hwmonAvailable) {
        QMessageBox::critical(this, "Initialization Error",
                            "No fan control interfaces found.\n"
                            "Make sure you're running on compatible hardware with "
                            "applesmc or hwmon drivers loaded.");
        QTimer::singleShot(0, qApp, &QApplication::quit);
        return;
    }

    // Check for write permissions
    bool canWriteSMC = smcAvailable && smcInterface->hasWritePermission();
    bool canWriteHWMon = hwmonAvailable && hwmonInterface->hasWritePermission();

    if ((smcAvailable && !canWriteSMC) || (hwmonAvailable && !canWriteHWMon)) {
        QMessageBox::warning(this, "Permission Warning",
                           "Application does not have write permissions to all fan interfaces.\n"
                           "You may be able to monitor some fans but not control them.\n"
                           "Run with: sudo macsfancontrol");
    }

    setupUI();
    createMenuBar();
    connectSignals();

    // Load custom sensor descriptions if available
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
                        "/macsfancontrol/sensor_descriptions.conf";
    SensorDescriptions::loadCustomDescriptions(configPath);

    // Load saved settings
    loadSettings();

    // Start update timer (1 second interval)
    updateTimer->start(1000);

    // Initial update
    updateSensorData();

    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    // Save current settings before exit
    saveSettings();

    // Restore all fans to automatic mode on exit
    restoreAutoMode();
}

void MainWindow::setupUI()
{
    setWindowTitle("Fan Control");
    resize(800, 600);

    // Pass Mac model to temperature panel for sensor description lookup
    tempPanel->setMacModel(smcInterface->getMacModel());

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

    // Create fan control widgets for SMC fans
    QVector<FanInfo> smcFans = smcInterface->getFans();
    QString macModel = smcInterface->getMacModel();
    for (const FanInfo& fan : smcFans) {
        FanControlWidget *fanWidget = new FanControlWidget(fan, this);
        fanWidget->setMacModel(macModel);  // Set Mac model for sensor descriptions
        fanWidgets.append(fanWidget);
        fanSources.append(FAN_SOURCE_SMC);
        fanSourceIndices.append(fan.index - 1);  // SMC uses 1-based index
        fanLayout->addWidget(fanWidget);

        // Initialize sensor-based settings
        SensorBasedSettings settings = {false, -1, 40, 80};
        sensorSettings.append(settings);

        // Connect fan widget signals
        connect(fanWidget, &FanControlWidget::manualModeRequested,
                this, &MainWindow::onManualModeRequested);
        connect(fanWidget, &FanControlWidget::targetRPMChanged,
                this, &MainWindow::onTargetRPMChanged);
        connect(fanWidget, &FanControlWidget::sensorBasedModeChanged,
                this, &MainWindow::onSensorBasedModeChanged);
    }

    // Create fan control widgets for HWMon fans
    QVector<HWMonFan> hwmonFans = hwmonInterface->getFans();
    for (int i = 0; i < hwmonFans.size(); i++) {
        const HWMonFan& hwFan = hwmonFans[i];

        // Convert HWMonFan to FanInfo
        FanInfo fan;
        fan.index = fanWidgets.size() + 1;  // Sequential index
        fan.label = hwFan.label;
        fan.currentRPM = hwFan.currentRPM;
        fan.targetRPM = hwFan.currentRPM;
        fan.minRPM = hwFan.minRPM;
        fan.maxRPM = hwFan.maxRPM;
        fan.isManual = hwFan.isManual;
        fan.sysfsPath = hwFan.devicePath;

        FanControlWidget *fanWidget = new FanControlWidget(fan, this);
        fanWidgets.append(fanWidget);
        fanSources.append(FAN_SOURCE_HWMON);
        fanSourceIndices.append(i);  // HWMon index
        fanLayout->addWidget(fanWidget);

        // Initialize sensor-based settings
        SensorBasedSettings settings = {false, -1, 40, 80};
        sensorSettings.append(settings);

        // Connect fan widget signals
        connect(fanWidget, &FanControlWidget::manualModeRequested,
                this, &MainWindow::onManualModeRequested);
        connect(fanWidget, &FanControlWidget::targetRPMChanged,
                this, &MainWindow::onTargetRPMChanged);
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

    // Presets menu
    QMenu *presetsMenu = menuBar()->addMenu("&Presets");

    QAction *savePresetAction = new QAction("&Save Preset...", this);
    savePresetAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(savePresetAction, &QAction::triggered, this, &MainWindow::savePreset);
    presetsMenu->addAction(savePresetAction);

    QAction *loadPresetAction = new QAction("&Load Preset...", this);
    loadPresetAction->setShortcut(QKeySequence("Ctrl+L"));
    connect(loadPresetAction, &QAction::triggered, this, &MainWindow::loadPreset);
    presetsMenu->addAction(loadPresetAction);

    QAction *deletePresetAction = new QAction("&Delete Preset...", this);
    connect(deletePresetAction, &QAction::triggered, this, &MainWindow::deletePreset);
    presetsMenu->addAction(deletePresetAction);

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About Mac Fan Control",
                         "Mac Fan Control for Linux\n\n"
                         "A Qt5 application for controlling fans on Linux.\n\n"
                         "Supports Apple SMC fans (applesmc) and hwmon devices (AMD/NVIDIA GPUs, etc.).\n\n"
                         "Features:\n"
                         "- Real-time fan speed monitoring\n"
                         "- Manual and automatic fan control\n"
                         "- Temperature sensor monitoring\n"
                         "- Sensor-based automatic control\n"
                         "- Save and load presets\n"
                         "- Settings persistence\n"
                         "- Support for multiple fan types (SMC + hwmon)\n"
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
    // Get temperature readings from both sources
    QVector<TempSensor> smcTemps = smcInterface->getTemperatures();
    QVector<HWMonSensor> hwmonSensors = hwmonInterface->getTemperatures();

    // Convert hwmon sensors to TempSensor format
    QVector<TempSensor> temps = smcTemps;
    for (const HWMonSensor& hwSensor : hwmonSensors) {
        TempSensor sensor;
        sensor.index = hwSensor.index;
        sensor.label = hwSensor.label;
        sensor.temperature = hwSensor.temperature;
        sensor.sysfsPath = hwSensor.devicePath;
        temps.append(sensor);
    }

    // Update all fan RPMs
    for (int i = 0; i < fanWidgets.size(); i++) {
        FanSource source = fanSources[i];
        int sourceIndex = fanSourceIndices[i];
        int rpm = -1;

        if (source == FAN_SOURCE_SMC) {
            rpm = smcInterface->getFanCurrentRPM(sourceIndex);
        } else if (source == FAN_SOURCE_HWMON) {
            rpm = hwmonInterface->getFanCurrentRPM(sourceIndex);
        }

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
        FanSource source = fanSources[i];
        int sourceIndex = fanSourceIndices[i];

        if (source == FAN_SOURCE_SMC) {
            smcInterface->setFanManualMode(sourceIndex, false);
        } else if (source == FAN_SOURCE_HWMON) {
            hwmonInterface->setFanManualMode(sourceIndex, false);
        }
    }
}

void MainWindow::onManualModeRequested(int fanWidgetIndex, bool enable)
{
    if (fanWidgetIndex < 0 || fanWidgetIndex >= fanWidgets.size()) {
        return;
    }

    FanSource source = fanSources[fanWidgetIndex];
    int sourceIndex = fanSourceIndices[fanWidgetIndex];

    if (source == FAN_SOURCE_SMC) {
        smcInterface->setFanManualMode(sourceIndex, enable);
    } else if (source == FAN_SOURCE_HWMON) {
        hwmonInterface->setFanManualMode(sourceIndex, enable);
    }
}

void MainWindow::onTargetRPMChanged(int fanWidgetIndex, int rpm)
{
    if (fanWidgetIndex < 0 || fanWidgetIndex >= fanWidgets.size()) {
        return;
    }

    FanSource source = fanSources[fanWidgetIndex];
    int sourceIndex = fanSourceIndices[fanWidgetIndex];

    if (source == FAN_SOURCE_SMC) {
        smcInterface->setFanSpeed(sourceIndex, rpm);
    } else if (source == FAN_SOURCE_HWMON) {
        hwmonInterface->setFanSpeed(sourceIndex, rpm);
    }
}

void MainWindow::onSensorBasedModeChanged(int fanWidgetIndex, bool enable, int sensorIndex, int minTemp, int maxTemp)
{
    if (fanWidgetIndex < 0 || fanWidgetIndex >= sensorSettings.size()) {
        return;
    }

    // Update sensor-based settings
    sensorSettings[fanWidgetIndex].enabled = enable;
    sensorSettings[fanWidgetIndex].sensorIndex = sensorIndex;
    sensorSettings[fanWidgetIndex].minTemp = minTemp;
    sensorSettings[fanWidgetIndex].maxTemp = maxTemp;

    // Log the change
    if (enable) {
        qDebug() << "Fan" << fanWidgetIndex << "sensor-based mode enabled:"
                 << "sensor" << sensorIndex
                 << "temp range" << minTemp << "-" << maxTemp << "°C";
    }
}

void MainWindow::updateSensorListInFanWidgets()
{
    // Get current temperature sensors from both sources
    QVector<TempSensor> smcTemps = smcInterface->getTemperatures();
    QVector<HWMonSensor> hwmonSensors = hwmonInterface->getTemperatures();

    // Convert hwmon sensors to TempSensor format
    QVector<TempSensor> temps = smcTemps;
    for (const HWMonSensor& hwSensor : hwmonSensors) {
        TempSensor sensor;
        sensor.index = hwSensor.index;
        sensor.label = hwSensor.label;
        sensor.temperature = hwSensor.temperature;
        sensor.sysfsPath = hwSensor.devicePath;
        temps.append(sensor);
    }

    // Update sensor list in each fan widget
    for (FanControlWidget* fanWidget : fanWidgets) {
        fanWidget->setSensorList(temps);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings("macsfancontrol", "macsfancontrol-qt");

    settings.beginGroup("LastSession");
    settings.setValue("fanCount", fanWidgets.size());

    for (int i = 0; i < fanWidgets.size(); i++) {
        settings.beginGroup(QString("Fan%1").arg(i));
        settings.setValue("mode", static_cast<int>(fanWidgets[i]->getCurrentMode()));
        settings.setValue("targetRPM", fanWidgets[i]->getTargetRPM());
        settings.setValue("sensorIndex", fanWidgets[i]->getSelectedSensorIndex());
        settings.setValue("minTemp", fanWidgets[i]->getMinTemp());
        settings.setValue("maxTemp", fanWidgets[i]->getMaxTemp());
        settings.endGroup();
    }

    settings.endGroup();
    qDebug() << "Settings saved";
}

void MainWindow::loadSettings()
{
    QSettings settings("macsfancontrol", "macsfancontrol-qt");

    settings.beginGroup("LastSession");
    int savedFanCount = settings.value("fanCount", 0).toInt();

    if (savedFanCount != fanWidgets.size()) {
        qDebug() << "Fan count mismatch, skipping settings load";
        settings.endGroup();
        return;
    }

    for (int i = 0; i < fanWidgets.size(); i++) {
        settings.beginGroup(QString("Fan%1").arg(i));

        FanMode mode = static_cast<FanMode>(settings.value("mode", MODE_AUTO).toInt());
        int targetRPM = settings.value("targetRPM", 2000).toInt();
        int sensorIndex = settings.value("sensorIndex", -1).toInt();
        int minTemp = settings.value("minTemp", 40).toInt();
        int maxTemp = settings.value("maxTemp", 80).toInt();

        applyFanSettings(i, mode, targetRPM, sensorIndex, minTemp, maxTemp);

        settings.endGroup();
    }

    settings.endGroup();
    qDebug() << "Settings loaded";
}

void MainWindow::savePresetToSettings(const QString& presetName)
{
    QSettings settings("macsfancontrol", "macsfancontrol-qt");

    settings.beginGroup("Presets");
    settings.beginGroup(presetName);
    settings.setValue("fanCount", fanWidgets.size());

    for (int i = 0; i < fanWidgets.size(); i++) {
        settings.beginGroup(QString("Fan%1").arg(i));
        settings.setValue("mode", static_cast<int>(fanWidgets[i]->getCurrentMode()));
        settings.setValue("targetRPM", fanWidgets[i]->getTargetRPM());
        settings.setValue("sensorIndex", fanWidgets[i]->getSelectedSensorIndex());
        settings.setValue("minTemp", fanWidgets[i]->getMinTemp());
        settings.setValue("maxTemp", fanWidgets[i]->getMaxTemp());
        settings.endGroup();
    }

    settings.endGroup();
    settings.endGroup();
    qDebug() << "Preset saved:" << presetName;
}

void MainWindow::loadPresetFromSettings(const QString& presetName)
{
    QSettings settings("macsfancontrol", "macsfancontrol-qt");

    settings.beginGroup("Presets");
    settings.beginGroup(presetName);
    int savedFanCount = settings.value("fanCount", 0).toInt();

    if (savedFanCount != fanWidgets.size()) {
        QMessageBox::warning(this, "Preset Error",
                           "This preset was saved with a different fan configuration.");
        settings.endGroup();
        settings.endGroup();
        return;
    }

    for (int i = 0; i < fanWidgets.size(); i++) {
        settings.beginGroup(QString("Fan%1").arg(i));

        FanMode mode = static_cast<FanMode>(settings.value("mode", MODE_AUTO).toInt());
        int targetRPM = settings.value("targetRPM", 2000).toInt();
        int sensorIndex = settings.value("sensorIndex", -1).toInt();
        int minTemp = settings.value("minTemp", 40).toInt();
        int maxTemp = settings.value("maxTemp", 80).toInt();

        applyFanSettings(i, mode, targetRPM, sensorIndex, minTemp, maxTemp);

        settings.endGroup();
    }

    settings.endGroup();
    settings.endGroup();
    qDebug() << "Preset loaded:" << presetName;
}

void MainWindow::applyFanSettings(int fanIndex, FanMode mode, int targetRPM, int sensorIndex, int minTemp, int maxTemp)
{
    if (fanIndex < 0 || fanIndex >= fanWidgets.size()) {
        return;
    }

    FanSource source = fanSources[fanIndex];
    int sourceIndex = fanSourceIndices[fanIndex];

    // Apply settings to widget
    fanWidgets[fanIndex]->setMode(mode);
    fanWidgets[fanIndex]->setTargetRPM(targetRPM);

    if (mode == MODE_SENSOR_BASED) {
        fanWidgets[fanIndex]->setSensorBasedSettings(sensorIndex, minTemp, maxTemp);

        // Update sensor-based settings
        sensorSettings[fanIndex].enabled = true;
        sensorSettings[fanIndex].sensorIndex = sensorIndex;
        sensorSettings[fanIndex].minTemp = minTemp;
        sensorSettings[fanIndex].maxTemp = maxTemp;
    } else {
        sensorSettings[fanIndex].enabled = false;
    }

    // Apply to the correct interface
    if (mode == MODE_AUTO) {
        if (source == FAN_SOURCE_SMC) {
            smcInterface->setFanManualMode(sourceIndex, false);
        } else if (source == FAN_SOURCE_HWMON) {
            hwmonInterface->setFanManualMode(sourceIndex, false);
        }
    } else if (mode == MODE_MANUAL) {
        if (source == FAN_SOURCE_SMC) {
            smcInterface->setFanManualMode(sourceIndex, true);
            smcInterface->setFanSpeed(sourceIndex, targetRPM);
        } else if (source == FAN_SOURCE_HWMON) {
            hwmonInterface->setFanManualMode(sourceIndex, true);
            hwmonInterface->setFanSpeed(sourceIndex, targetRPM);
        }
    } else if (mode == MODE_SENSOR_BASED) {
        if (source == FAN_SOURCE_SMC) {
            smcInterface->setFanManualMode(sourceIndex, true);
        } else if (source == FAN_SOURCE_HWMON) {
            hwmonInterface->setFanManualMode(sourceIndex, true);
        }
    }
}

void MainWindow::savePreset()
{
    bool ok;
    QString presetName = QInputDialog::getText(this, "Save Preset",
                                               "Enter preset name:",
                                               QLineEdit::Normal,
                                               "", &ok);

    if (ok && !presetName.isEmpty()) {
        savePresetToSettings(presetName);
        statusBar()->showMessage(QString("Preset '%1' saved").arg(presetName), 3000);
    }
}

void MainWindow::loadPreset()
{
    QSettings settings("macsfancontrol", "macsfancontrol-qt");
    settings.beginGroup("Presets");
    QStringList presets = settings.childGroups();
    settings.endGroup();

    if (presets.isEmpty()) {
        QMessageBox::information(this, "Load Preset",
                               "No saved presets found.\nUse 'Save Preset' to create one.");
        return;
    }

    bool ok;
    QString presetName = QInputDialog::getItem(this, "Load Preset",
                                              "Select preset to load:",
                                              presets, 0, false, &ok);

    if (ok && !presetName.isEmpty()) {
        loadPresetFromSettings(presetName);
        statusBar()->showMessage(QString("Preset '%1' loaded").arg(presetName), 3000);
    }
}

void MainWindow::deletePreset()
{
    QSettings settings("macsfancontrol", "macsfancontrol-qt");
    settings.beginGroup("Presets");
    QStringList presets = settings.childGroups();
    settings.endGroup();

    if (presets.isEmpty()) {
        QMessageBox::information(this, "Delete Preset",
                               "No saved presets found.");
        return;
    }

    bool ok;
    QString presetName = QInputDialog::getItem(this, "Delete Preset",
                                              "Select preset to delete:",
                                              presets, 0, false, &ok);

    if (ok && !presetName.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
                                                                  QString("Delete preset '%1'?").arg(presetName),
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            settings.beginGroup("Presets");
            settings.remove(presetName);
            settings.endGroup();
            statusBar()->showMessage(QString("Preset '%1' deleted").arg(presetName), 3000);
            qDebug() << "Preset deleted:" << presetName;
        }
    }
}
