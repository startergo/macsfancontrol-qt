#include "smcinterface.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QtMath>
#include <QDebug>

SMCInterface::SMCInterface(QObject *parent)
    : QObject(parent)
{
}

bool SMCInterface::initialize()
{
    // Check if SMC path exists
    QDir smcDir(basePath);
    if (!smcDir.exists()) {
        emit error("Apple SMC interface not found at " + basePath);
        return false;
    }

    // Detect Mac model
    detectMacModel();

    // Discover available fans and sensors
    discoverFans();
    discoverTemperatureSensors();

    if (fans.isEmpty()) {
        emit error("No fans discovered!");
        return false;
    }

    return true;
}

bool SMCInterface::hasWritePermission()
{
    // Try to open fan1_manual for writing (without actually writing)
    QFile file(basePath + "/fan1_manual");
    bool canWrite = file.open(QIODevice::WriteOnly | QIODevice::Append);
    if (canWrite) {
        file.close();
    }
    return canWrite;
}

void SMCInterface::discoverFans()
{
    fans.clear();

    for (int i = 1; i <= 6; i++) {
        QString fanBase = QString("%1/fan%2").arg(basePath).arg(i);
        QString inputPath = fanBase + "_input";

        // Check if this fan exists
        if (QFile::exists(inputPath)) {
            FanInfo fan;
            fan.index = i;
            fan.label = readSysfsString(fanBase + "_label").trimmed();
            fan.minRPM = readSysfsInt(fanBase + "_min");
            fan.maxRPM = readSysfsInt(fanBase + "_max");
            fan.currentRPM = readSysfsInt(inputPath);
            fan.isManual = (readSysfsInt(fanBase + "_manual") == 1);
            fan.targetRPM = readSysfsInt(fanBase + "_output");
            fan.sysfsPath = fanBase;

            fans.append(fan);
        }
    }
}

void SMCInterface::discoverTemperatureSensors()
{
    sensors.clear();

    for (int i = 1; i <= 68; i++) {
        QString tempBase = QString("%1/temp%2").arg(basePath).arg(i);
        QString inputPath = tempBase + "_input";

        // Check if this sensor exists
        if (QFile::exists(inputPath)) {
            TempSensor sensor;
            sensor.index = i;
            sensor.label = readSysfsString(tempBase + "_label").trimmed();
            sensor.temperature = readSysfsInt(inputPath);
            sensor.sysfsPath = inputPath;

            // Only add sensors with valid readings (skip -128°C and similar invalid values)
            if (sensor.temperature > -100000) {  // -100°C in millidegrees
                sensors.append(sensor);
            }
        }
    }
}

int SMCInterface::readSysfsInt(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit error(QString("Cannot read %1: %2").arg(path).arg(file.errorString()));
        return -1;
    }

    QTextStream in(&file);
    QString value = in.readLine().trimmed();
    file.close();

    bool ok;
    int result = value.toInt(&ok);
    if (!ok) {
        emit error(QString("Invalid integer value in %1: %2").arg(path).arg(value));
        return -1;
    }

    return result;
}

QString SMCInterface::readSysfsString(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit error(QString("Cannot read %1: %2").arg(path).arg(file.errorString()));
        return QString();
    }

    QTextStream in(&file);
    QString value = in.readLine();
    file.close();

    return value;
}

bool SMCInterface::writeSysfsInt(const QString& path, int value)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        emit error(QString("Cannot write to %1: %2 (need root?)").arg(path).arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << value;
    file.close();

    if (file.error() != QFile::NoError) {
        emit error(QString("Error writing to %1").arg(path));
        return false;
    }

    return true;
}

int SMCInterface::getFanCurrentRPM(int fanIndex)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        emit error("Invalid fan index");
        return -1;
    }

    int rpm = readSysfsInt(fans[fanIndex].sysfsPath + "_input");
    fans[fanIndex].currentRPM = rpm;
    return rpm;
}

bool SMCInterface::setFanManualMode(int fanIndex, bool enable)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        emit error("Invalid fan index");
        return false;
    }

    QString path = fans[fanIndex].sysfsPath + "_manual";
    bool success = writeSysfsInt(path, enable ? 1 : 0);

    if (success) {
        fans[fanIndex].isManual = enable;
    }

    return success;
}

bool SMCInterface::setFanSpeed(int fanIndex, int rpm)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        emit error("Invalid fan index");
        return false;
    }

    FanInfo& fan = fans[fanIndex];

    // Clamp to safe range
    int safeRPM = qBound(fan.minRPM, rpm, fan.maxRPM);

    if (safeRPM != rpm) {
        emit warning(QString("RPM %1 out of range [%2-%3], clamping to %4")
                     .arg(rpm).arg(fan.minRPM).arg(fan.maxRPM).arg(safeRPM));
    }

    QString path = fan.sysfsPath + "_output";
    bool success = writeSysfsInt(path, safeRPM);

    if (success) {
        fan.targetRPM = safeRPM;
    }

    return success;
}

QVector<TempSensor> SMCInterface::getTemperatures()
{
    // Update temperature readings
    for (int i = 0; i < sensors.size(); i++) {
        sensors[i].temperature = readSysfsInt(sensors[i].sysfsPath);
    }

    return sensors;
}

void SMCInterface::detectMacModel()
{
    // Try to read Mac model from DMI information
    QFile productNameFile("/sys/devices/virtual/dmi/id/product_name");
    if (productNameFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&productNameFile);
        macModel = in.readLine().trimmed();
        productNameFile.close();
    }

    // If that fails, try to read from board_name
    if (macModel.isEmpty()) {
        QFile boardNameFile("/sys/devices/virtual/dmi/id/board_name");
        if (boardNameFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&boardNameFile);
            macModel = in.readLine().trimmed();
            boardNameFile.close();
        }
    }

    // Default to "Unknown" if we couldn't detect
    if (macModel.isEmpty()) {
        macModel = "Unknown Mac";
    }

    qDebug() << "Detected Mac model:" << macModel;
}
