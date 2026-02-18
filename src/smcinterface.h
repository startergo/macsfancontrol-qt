#ifndef SMCINTERFACE_H
#define SMCINTERFACE_H

#include <QObject>
#include <QString>
#include <QVector>

// Fan data structure
struct FanInfo {
    int index;              // 1-6
    QString label;          // "PCI", "PS", etc.
    int currentRPM;         // Current speed
    int targetRPM;          // Target speed (manual mode)
    int minRPM;             // Minimum safe speed
    int maxRPM;             // Maximum speed
    bool isManual;          // Manual mode flag
    QString sysfsPath;      // Base path for this fan
};

// Temperature sensor structure
struct TempSensor {
    int index;              // 1-68
    QString label;          // Sensor code (e.g., "TA0P")
    int temperature;        // In millidegrees Celsius
    QString sysfsPath;      // Path to temp file
};

class SMCInterface : public QObject {
    Q_OBJECT

public:
    explicit SMCInterface(QObject *parent = nullptr);

    // Initialization
    bool initialize();
    bool hasWritePermission();

    // Fan operations
    QVector<FanInfo> getFans() const { return fans; }
    int getFanCurrentRPM(int fanIndex);
    bool setFanManualMode(int fanIndex, bool enable);
    bool setFanSpeed(int fanIndex, int rpm);

    // Temperature operations
    QVector<TempSensor> getTemperatures();

    // System information
    QString getMacModel() const { return macModel; }
    QString getBasePath() const { return basePath; }

signals:
    void error(const QString& message);
    void warning(const QString& message);

private:
    QString basePath = "/sys/devices/platform/applesmc.768";
    QVector<FanInfo> fans;
    QVector<TempSensor> sensors;
    QString macModel;

    // Helper functions for sysfs I/O
    int readSysfsInt(const QString& path);
    QString readSysfsString(const QString& path);
    bool writeSysfsInt(const QString& path, int value);

    // Discovery functions
    bool findBasePath();
    void discoverFans();
    void discoverTemperatureSensors();
    void detectMacModel();
};

#endif // SMCINTERFACE_H
