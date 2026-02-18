#ifndef HWMONINTERFACE_H
#define HWMONINTERFACE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QTextStream>

struct HWMonFan {
    QString deviceName;      // e.g., "amdgpu"
    QString devicePath;      // e.g., "/sys/class/hwmon/hwmon0"
    int fanNumber;           // 1 for fan1, 2 for fan2, etc.
    QString label;           // Fan label/name
    int currentRPM;          // Current fan speed
    int minRPM;              // Minimum RPM
    int maxRPM;              // Maximum RPM
    int currentPWM;          // Current PWM value (0-255)
    bool supportsManualControl;  // Whether manual control is supported
    bool isManual;           // Current mode (manual or auto)
};

struct HWMonSensor {
    QString deviceName;
    QString devicePath;
    QString label;
    int temperature;         // Temperature in millidegrees Celsius
    int index;              // Unique index for this sensor
};

class HWMonInterface : public QObject {
    Q_OBJECT

public:
    explicit HWMonInterface(QObject *parent = nullptr);
    ~HWMonInterface();

    bool initialize();
    bool hasWritePermission() const { return canWrite; }
    void setSmcAvailable(bool available) { smcAvailable = available; }

    QVector<HWMonFan> getFans() const;
    QVector<HWMonSensor> getTemperatures() const;

    int getFanCurrentRPM(int fanIndex);
    int getFanCurrentPWM(int fanIndex);
    bool setFanManualMode(int fanIndex, bool manual);
    bool setFanPWM(int fanIndex, int pwm);  // PWM: 0-255
    bool setFanSpeed(int fanIndex, int rpm);

signals:
    void error(const QString& message);
    void warning(const QString& message);

private:
    QVector<HWMonFan> fans;
    QVector<HWMonSensor> sensors;
    bool canWrite;
    bool smcAvailable;
    int nextSensorIndex;

    // Device names whose temperature sensors are duplicated by the SMC interface
    static const QStringList smcDuplicateDevices;

    void scanHWMonDevices();
    void scanFansInDevice(const QString& hwmonPath, const QString& deviceName);
    void scanSensorsInDevice(const QString& hwmonPath, const QString& deviceName);

    QString readSysFile(const QString& path) const;
    bool writeSysFile(const QString& path, const QString& value);
    int readIntFromFile(const QString& path, int defaultValue = -1) const;
    bool writeIntToFile(const QString& path, int value);

    QString getFanInputPath(int fanIndex);
    QString getFanMinPath(int fanIndex);
    QString getFanMaxPath(int fanIndex);
    QString getFanPWMPath(int fanIndex);
    QString getFanPWMEnablePath(int fanIndex);
};

#endif // HWMONINTERFACE_H
