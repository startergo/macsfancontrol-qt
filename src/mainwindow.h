#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QSettings>
#include "smcinterface.h"
#include "hwmoninterface.h"
#include "fancontrolwidget.h"
#include "temperaturepanel.h"

enum FanSource {
    FAN_SOURCE_SMC = 0,
    FAN_SOURCE_HWMON = 1
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateSensorData();
    void showError(const QString& message);
    void showWarning(const QString& message);
    void onManualModeRequested(int fanWidgetIndex, bool enable);
    void onTargetRPMChanged(int fanWidgetIndex, int rpm);
    void onSensorBasedModeChanged(int fanWidgetIndex, bool enable, int sensorIndex, int minTemp, int maxTemp);
    void savePreset();
    void loadPreset();
    void deletePreset();

private:
    SMCInterface *smcInterface;
    HWMonInterface *hwmonInterface;
    QVector<FanControlWidget*> fanWidgets;
    QVector<FanSource> fanSources;  // Track which interface each fan belongs to
    QVector<int> fanSourceIndices;  // Index within the source interface
    TemperaturePanel *tempPanel;
    QTimer *updateTimer;

    // Sensor-based control settings
    struct SensorBasedSettings {
        bool enabled;
        int sensorIndex;
        int minTemp;
        int maxTemp;
    };
    QVector<SensorBasedSettings> sensorSettings;

    void setupUI();
    void createMenuBar();
    void connectSignals();
    void restoreAutoMode();
    void updateSensorListInFanWidgets();

    // Settings management
    void saveSettings();
    void loadSettings();
    void savePresetToSettings(const QString& presetName);
    void loadPresetFromSettings(const QString& presetName);
    void applyFanSettings(int fanIndex, FanMode mode, int targetRPM, int sensorIndex, int minTemp, int maxTemp);
};

#endif // MAINWINDOW_H
