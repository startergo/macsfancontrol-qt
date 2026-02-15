#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include "smcinterface.h"
#include "fancontrolwidget.h"
#include "temperaturepanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateSensorData();
    void showError(const QString& message);
    void showWarning(const QString& message);
    void onSensorBasedModeChanged(int fanIndex, bool enable, int sensorIndex, int minTemp, int maxTemp);

private:
    SMCInterface *smcInterface;
    QVector<FanControlWidget*> fanWidgets;
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
};

#endif // MAINWINDOW_H
