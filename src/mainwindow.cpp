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
#include <QClipboard>
#include <QDateTime>
#include <QDir>
#include <QFile>

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

    QAction *copyDebugAction = new QAction("Copy &Debug Log to Clipboard", this);
    copyDebugAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(copyDebugAction, &QAction::triggered, this, &MainWindow::copyDebugLogToClipboard);
    helpMenu->addAction(copyDebugAction);

    helpMenu->addSeparator();

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

void MainWindow::copyDebugLogToClipboard()
{
    // Forward declaration — defined in main.cpp
    extern QStringList getDebugLog();

    QStringList lines;
    lines << "=== Mac Fan Control Debug Log ===";
    lines << QString("Timestamp: %1").arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    lines << QString("Mac Model:  %1").arg(smcInterface->getMacModel());
    lines << "";

    // SMC fans
    lines << "--- SMC Fans ---";
    for (const FanInfo& fan : smcInterface->getFans()) {
        lines << QString("  Fan %1 (%2): %3 RPM  [min:%4  max:%5  manual:%6]  path:%7")
                     .arg(fan.index).arg(fan.label).arg(fan.currentRPM)
                     .arg(fan.minRPM).arg(fan.maxRPM)
                     .arg(fan.isManual ? "yes" : "no")
                     .arg(fan.sysfsPath);
    }

    // HWMon fans
    lines << "";
    lines << "--- HWMon Fans ---";
    for (const HWMonFan& fan : hwmonInterface->getFans()) {
        lines << QString("  %1/%2: %3 RPM  [min:%4  max:%5  manual:%6]  path:%7")
                     .arg(fan.deviceName).arg(fan.label).arg(fan.currentRPM)
                     .arg(fan.minRPM).arg(fan.maxRPM)
                     .arg(fan.isManual ? "yes" : "no")
                     .arg(fan.devicePath);
    }

    // SMC temperatures (live read)
    lines << "";
    lines << "--- SMC Temperatures ---";
    for (const TempSensor& sensor : smcInterface->getTemperatures()) {
        lines << QString("  %1: %2 °C  (%3)")
                     .arg(sensor.label, -6)
                     .arg(sensor.temperature / 1000.0, 5, 'f', 1)
                     .arg(sensor.sysfsPath);
    }

    // HWMon temperatures (live read)
    lines << "";
    lines << "--- HWMon Temperatures ---";
    for (const HWMonSensor& sensor : hwmonInterface->getTemperatures()) {
        lines << QString("  %1/%2: %3 °C  (%4)")
                     .arg(sensor.deviceName).arg(sensor.label, -20)
                     .arg(sensor.temperature / 1000.0, 5, 'f', 1)
                     .arg(sensor.devicePath);
    }

    // Saved presets
    lines << "";
    lines << "--- Saved Presets ---";
    {
        QSettings settings("macsfancontrol", "macsfancontrol-qt");
        settings.beginGroup("Presets");
        QStringList presetNames = settings.childGroups();
        if (presetNames.isEmpty()) {
            lines << "  (none)";
        } else {
            auto modeStr = [](int m) -> QString {
                switch (m) {
                case MODE_AUTO:         return "auto";
                case MODE_MANUAL:       return "manual";
                case MODE_SENSOR_BASED: return "sensor-based";
                default:                return QString("unknown(%1)").arg(m);
                }
            };
            for (const QString& name : presetNames) {
                settings.beginGroup(name);
                int fanCount = settings.value("fanCount", 0).toInt();
                lines << QString("  Preset: %1  (%2 fans)").arg(name).arg(fanCount);
                for (int i = 0; i < fanCount; i++) {
                    settings.beginGroup(QString("Fan%1").arg(i));
                    int mode       = settings.value("mode", MODE_AUTO).toInt();
                    int targetRPM  = settings.value("targetRPM", 0).toInt();
                    int sensorIdx  = settings.value("sensorIndex", -1).toInt();
                    int minTemp    = settings.value("minTemp", 0).toInt();
                    int maxTemp    = settings.value("maxTemp", 0).toInt();
                    QString entry  = QString("    Fan%1: mode=%2").arg(i).arg(modeStr(mode));
                    if (mode == MODE_MANUAL)
                        entry += QString("  targetRPM=%1").arg(targetRPM);
                    if (mode == MODE_SENSOR_BASED)
                        entry += QString("  sensor=%1  minTemp=%2  maxTemp=%3")
                                     .arg(sensorIdx).arg(minTemp).arg(maxTemp);
                    lines << entry;
                    settings.endGroup();
                }
                settings.endGroup();
            }
        }
        settings.endGroup();
    }

    // Application log captured since startup
    lines << "";
    lines << "--- Application Log ---";
    lines << getDebugLog();

    // Raw sysfs dump
    auto readSysfsValue = [](const QString& path) -> QString {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return "(unreadable)";
        QString v = QString(f.readLine()).trimmed();
        f.close();
        return v;
    };

    lines << "";
    lines << "=== Raw Sysfs Dump ===";

    // --- hwmon devices ---
    lines << "";
    lines << "--- /sys/class/hwmon ---";
    QDir hwmonDir("/sys/class/hwmon");
    if (hwmonDir.exists()) {
        QStringList hwmonDevs = hwmonDir.entryList(QStringList() << "hwmon*", QDir::Dirs, QDir::Name);
        for (const QString& dev : hwmonDevs) {
            QString devPath = "/sys/class/hwmon/" + dev;
            QString name = readSysfsValue(devPath + "/name");
            bool isTempDup = HWMonInterface::smcDuplicateDevices.contains(name);

            lines << QString("  %1  name=%2%3")
                         .arg(dev, -8).arg(name)
                         .arg(isTempDup ? "  [SMC duplicate: temps suppressed, fans kept]" : "");

            QDir d(devPath);

            // Fan files: input, min, max, label
            QStringList fanFiles = d.entryList(
                QStringList() << "fan*_input" << "fan*_min" << "fan*_max" << "fan*_label",
                QDir::Files, QDir::Name);
            for (const QString& f : fanFiles)
                lines << QString("    %1 = %2").arg(f, -22).arg(readSysfsValue(devPath + "/" + f));

            // PWM files: value and enable mode
            QStringList pwmFiles = d.entryList(QStringList() << "pwm*", QDir::Files, QDir::Name);
            for (const QString& f : pwmFiles) {
                QString val = readSysfsValue(devPath + "/" + f);
                QString note;
                if (f.endsWith("_enable")) {
                    if (val == "0") note = "  (disabled)";
                    else if (val == "1") note = "  (manual)";
                    else if (val == "2") note = "  (auto)";
                }
                lines << QString("    %1 = %2%3").arg(f, -22).arg(val).arg(note);
            }

            // Temp files: input and label
            QStringList tempFiles = d.entryList(
                QStringList() << "temp*_input" << "temp*_label",
                QDir::Files, QDir::Name);
            for (const QString& f : tempFiles) {
                QString val = readSysfsValue(devPath + "/" + f);
                QString note = (isTempDup && f.endsWith("_input")) ? "  (suppressed)" : "";
                lines << QString("    %1 = %2%3").arg(f, -22).arg(val).arg(note);
            }

            if (fanFiles.isEmpty() && pwmFiles.isEmpty() && tempFiles.isEmpty())
                lines << "    (no fan/pwm/temp files)";
        }
    } else {
        lines << "  /sys/class/hwmon not found";
    }

    // --- applesmc raw fan + temp count ---
    lines << "";
    lines << QString("--- AppSMC: %1 ---").arg(smcInterface->getBasePath());
    QDir smcDir(smcInterface->getBasePath());
    if (smcDir.exists()) {
        QStringList smcFanFiles = smcDir.entryList(QStringList() << "fan*", QDir::Files, QDir::Name);
        for (const QString& f : smcFanFiles)
            lines << QString("  %1 = %2").arg(f, -25)
                         .arg(readSysfsValue(smcInterface->getBasePath() + "/" + f));

        int tempCount = smcDir.entryList(QStringList() << "temp*_input", QDir::Files).size();
        lines << QString("  (%1 temp*_input files — see SMC Temperatures section above)").arg(tempCount);
    } else {
        lines << "  Path not found";
    }

    QApplication::clipboard()->setText(lines.join('\n'));
    statusBar()->showMessage("Debug log copied to clipboard", 3000);
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
