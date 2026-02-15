#ifndef FANCONTROLWIDGET_H
#define FANCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QSpinBox>
#include "smcinterface.h"

enum FanMode {
    MODE_AUTO = 0,
    MODE_MANUAL = 1,
    MODE_SENSOR_BASED = 2
};

class FanControlWidget : public QWidget {
    Q_OBJECT

public:
    explicit FanControlWidget(const FanInfo& fanInfo, QWidget *parent = nullptr);

    void setCurrentRPM(int rpm);
    void updateFanInfo(const FanInfo& info);
    void setSensorList(const QVector<TempSensor>& sensors);
    void updateSensorBasedSpeed(int currentTemp);

signals:
    void manualModeRequested(int fanIndex, bool enable);
    void targetRPMChanged(int fanIndex, int rpm);
    void sensorBasedModeChanged(int fanIndex, bool enable, int sensorIndex, int minTemp, int maxTemp);

private slots:
    void onModeChanged(int mode);
    void onSliderChanged(int value);
    void onSensorSettingsChanged();

private:
    int fanIndex;
    int minRPM;
    int maxRPM;
    FanMode currentMode;
    int selectedSensorIndex;

    // UI elements
    QLabel *labelName;
    QLabel *labelCurrentRPM;
    QLabel *labelTargetRPM;
    QLabel *labelRange;
    QRadioButton *radioAuto;
    QRadioButton *radioManual;
    QRadioButton *radioSensorBased;
    QButtonGroup *modeGroup;
    QSlider *sliderRPM;
    QWidget *modeIndicator;

    // Sensor-based controls
    QWidget *sensorControls;
    QComboBox *comboSensor;
    QSpinBox *spinMinTemp;
    QSpinBox *spinMaxTemp;
    QLabel *labelCurrentTemp;

    void setupUI(const FanInfo& fanInfo);
    void updateModeIndicator(FanMode mode);
    void updateControlsVisibility();
    int calculateFanSpeed(int currentTemp, int minTemp, int maxTemp);
    QString getSensorDescription(const QString& label);
};

#endif // FANCONTROLWIDGET_H
