#include "hwmoninterface.h"
#include <QDebug>
#include <QDirIterator>
#include <QRegularExpression>

// Devices whose temps are always provided by the SMC on Apple hardware
const QStringList HWMonInterface::smcDuplicateDevices = {
    "coretemp",       // CPU core temps → TC*C in SMC
    "pch_cannonlake", // PCH temp → TPCD in SMC
    "pch_skylake",
    "pch_kabylake",
    "pch_cometlake",
    "nvme",           // NVMe temp → TH0* in SMC
};

HWMonInterface::HWMonInterface(QObject *parent)
    : QObject(parent),
      canWrite(false),
      smcAvailable(false),
      nextSensorIndex(1000)  // Start at 1000 to avoid conflicts with SMC indices
{
}

HWMonInterface::~HWMonInterface()
{
}

bool HWMonInterface::initialize()
{
    qDebug() << "Initializing HWMon interface...";

    // Scan for hwmon devices
    scanHWMonDevices();

    if (fans.isEmpty()) {
        qDebug() << "No hwmon fans found";
        return false;
    }

    qDebug() << "Found" << fans.size() << "hwmon fan(s)";

    // Test write permission by checking if we can read PWM enable files
    if (!fans.isEmpty()) {
        QString pwmEnablePath = getFanPWMEnablePath(0);
        QString testPath = fans[0].devicePath + "/pwm1_enable";
        QFile testFile(testPath);
        canWrite = testFile.open(QIODevice::WriteOnly);
        if (canWrite) {
            testFile.close();
        }
    }

    return true;
}

void HWMonInterface::scanHWMonDevices()
{
    QDir hwmonDir("/sys/class/hwmon");
    if (!hwmonDir.exists()) {
        qWarning() << "hwmon directory not found";
        return;
    }

    QStringList hwmonDevices = hwmonDir.entryList(QStringList() << "hwmon*", QDir::Dirs);

    for (const QString& hwmonDev : hwmonDevices) {
        QString hwmonPath = "/sys/class/hwmon/" + hwmonDev;

        // Read device name
        QString deviceName = readSysFile(hwmonPath + "/name").trimmed();
        if (deviceName.isEmpty()) {
            continue;
        }

        qDebug() << "Scanning hwmon device:" << deviceName << "at" << hwmonPath;

        // Scan for fans in all devices
        scanFansInDevice(hwmonPath, deviceName);

        // Skip temperature sensors from devices already covered by the SMC interface
        if (smcAvailable && smcDuplicateDevices.contains(deviceName)) {
            qDebug() << "Skipping SMC-duplicate hwmon sensors from:" << deviceName;
            continue;
        }
        scanSensorsInDevice(hwmonPath, deviceName);
    }
}

void HWMonInterface::scanFansInDevice(const QString& hwmonPath, const QString& deviceName)
{
    QDir dir(hwmonPath);
    QStringList fanInputFiles = dir.entryList(QStringList() << "fan*_input", QDir::Files);

    for (const QString& fanFile : fanInputFiles) {
        // Extract fan number from filename (e.g., "fan1_input" -> 1)
        QRegularExpression re("fan(\\d+)_input");
        QRegularExpressionMatch match = re.match(fanFile);
        if (!match.hasMatch()) {
            continue;
        }

        int fanNum = match.captured(1).toInt();
        QString basePath = hwmonPath + "/fan" + QString::number(fanNum);

        // Read fan properties
        int currentRPM = readIntFromFile(basePath + "_input", 0);
        int minRPM = readIntFromFile(basePath + "_min", 0);
        int maxRPM = readIntFromFile(basePath + "_max", 5000);

        // Read fan label if available
        QString label = readSysFile(basePath + "_label").trimmed();
        if (label.isEmpty()) {
            label = deviceName + " Fan " + QString::number(fanNum);
        }

        // Check if PWM control is available
        QString pwmPath = hwmonPath + "/pwm" + QString::number(fanNum);
        bool supportsPWM = QFile::exists(pwmPath);

        // Read current PWM value
        int currentPWM = readIntFromFile(pwmPath, 0);

        // Check if in manual mode
        QString pwmEnablePath = pwmPath + "_enable";
        int pwmEnable = readIntFromFile(pwmEnablePath, 2);
        bool isManual = (pwmEnable == 1);

        HWMonFan fan;
        fan.deviceName = deviceName;
        fan.devicePath = hwmonPath;
        fan.fanNumber = fanNum;
        fan.label = label;
        fan.currentRPM = currentRPM;
        fan.minRPM = minRPM;
        fan.maxRPM = maxRPM;
        fan.currentPWM = currentPWM;
        fan.supportsManualControl = supportsPWM;
        fan.isManual = isManual;

        fans.append(fan);

        qDebug() << "Found fan:" << label
                 << "RPM:" << currentRPM
                 << "Min:" << minRPM
                 << "Max:" << maxRPM
                 << "PWM:" << currentPWM
                 << "Manual:" << isManual;
    }
}

void HWMonInterface::scanSensorsInDevice(const QString& hwmonPath, const QString& deviceName)
{
    QDir dir(hwmonPath);
    QStringList tempInputFiles = dir.entryList(QStringList() << "temp*_input", QDir::Files);

    for (const QString& tempFile : tempInputFiles) {
        // Extract temp number from filename
        QRegularExpression re("temp(\\d+)_input");
        QRegularExpressionMatch match = re.match(tempFile);
        if (!match.hasMatch()) {
            continue;
        }

        int tempNum = match.captured(1).toInt();
        QString basePath = hwmonPath + "/temp" + QString::number(tempNum);

        // Read temperature
        int temp = readIntFromFile(basePath + "_input", 0);

        // Read label if available
        QString label = readSysFile(basePath + "_label").trimmed();
        if (label.isEmpty()) {
            label = deviceName + " Temp " + QString::number(tempNum);
        }

        HWMonSensor sensor;
        sensor.deviceName = deviceName;
        sensor.devicePath = hwmonPath;
        sensor.label = label;
        sensor.temperature = temp;
        sensor.index = nextSensorIndex++;

        sensors.append(sensor);
    }
}

QString HWMonInterface::readSysFile(const QString& path) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readLine();
    file.close();
    return content;
}

bool HWMonInterface::writeSysFile(const QString& path, const QString& value)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        emit error("Cannot write to " + path + ": " + file.errorString());
        return false;
    }

    QTextStream out(&file);
    out << value;
    file.close();
    return true;
}

int HWMonInterface::readIntFromFile(const QString& path, int defaultValue) const
{
    QString content = readSysFile(path);
    if (content.isEmpty()) {
        return defaultValue;
    }

    bool ok;
    int value = content.toInt(&ok);
    return ok ? value : defaultValue;
}

bool HWMonInterface::writeIntToFile(const QString& path, int value)
{
    return writeSysFile(path, QString::number(value));
}

QVector<HWMonFan> HWMonInterface::getFans() const
{
    return fans;
}

QVector<HWMonSensor> HWMonInterface::getTemperatures() const
{
    // Update temperature readings
    QVector<HWMonSensor> currentSensors = sensors;
    for (int i = 0; i < currentSensors.size(); i++) {
        QString tempPath = currentSensors[i].devicePath + "/temp";
        // Find the temp number by scanning for it
        QDir dir(currentSensors[i].devicePath);
        QStringList tempFiles = dir.entryList(QStringList() << "temp*_input", QDir::Files);
        for (const QString& tempFile : tempFiles) {
            QString labelPath = currentSensors[i].devicePath + "/" +
                               tempFile.left(tempFile.indexOf("_input")) + "_label";
            QString label = readSysFile(labelPath).trimmed();
            if (label == currentSensors[i].label || label.isEmpty()) {
                QString inputPath = currentSensors[i].devicePath + "/" + tempFile;
                currentSensors[i].temperature = readIntFromFile(inputPath, 0);
                break;
            }
        }
    }
    return currentSensors;
}

QString HWMonInterface::getFanInputPath(int fanIndex)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        return QString();
    }
    return fans[fanIndex].devicePath + "/fan" + QString::number(fans[fanIndex].fanNumber) + "_input";
}

QString HWMonInterface::getFanMinPath(int fanIndex)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        return QString();
    }
    return fans[fanIndex].devicePath + "/fan" + QString::number(fans[fanIndex].fanNumber) + "_min";
}

QString HWMonInterface::getFanMaxPath(int fanIndex)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        return QString();
    }
    return fans[fanIndex].devicePath + "/fan" + QString::number(fans[fanIndex].fanNumber) + "_max";
}

QString HWMonInterface::getFanPWMPath(int fanIndex)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        return QString();
    }
    return fans[fanIndex].devicePath + "/pwm" + QString::number(fans[fanIndex].fanNumber);
}

QString HWMonInterface::getFanPWMEnablePath(int fanIndex)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        return QString();
    }
    return fans[fanIndex].devicePath + "/pwm" + QString::number(fans[fanIndex].fanNumber) + "_enable";
}

int HWMonInterface::getFanCurrentRPM(int fanIndex)
{
    QString path = getFanInputPath(fanIndex);
    if (path.isEmpty()) {
        return -1;
    }

    int rpm = readIntFromFile(path, -1);
    if (rpm >= 0 && fanIndex >= 0 && fanIndex < fans.size()) {
        fans[fanIndex].currentRPM = rpm;
    }
    return rpm;
}

int HWMonInterface::getFanCurrentPWM(int fanIndex)
{
    QString path = getFanPWMPath(fanIndex);
    if (path.isEmpty()) {
        return -1;
    }

    int pwm = readIntFromFile(path, -1);
    if (pwm >= 0 && fanIndex >= 0 && fanIndex < fans.size()) {
        fans[fanIndex].currentPWM = pwm;
    }
    return pwm;
}

bool HWMonInterface::setFanManualMode(int fanIndex, bool manual)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        emit error("Invalid fan index");
        return false;
    }

    if (!fans[fanIndex].supportsManualControl) {
        emit warning("Fan does not support manual control");
        return false;
    }

    QString path = getFanPWMEnablePath(fanIndex);
    int value = manual ? 1 : 2;  // 1 = manual, 2 = automatic

    if (writeIntToFile(path, value)) {
        fans[fanIndex].isManual = manual;
        qDebug() << "Set fan" << fanIndex << "to" << (manual ? "manual" : "auto") << "mode";
        return true;
    }

    emit error("Failed to set fan mode");
    return false;
}

bool HWMonInterface::setFanPWM(int fanIndex, int pwm)
{
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        emit error("Invalid fan index");
        return false;
    }

    if (!fans[fanIndex].supportsManualControl) {
        emit warning("Fan does not support manual control");
        return false;
    }

    // Clamp PWM to valid range (0-255)
    pwm = qBound(0, pwm, 255);

    QString path = getFanPWMPath(fanIndex);
    if (writeIntToFile(path, pwm)) {
        fans[fanIndex].currentPWM = pwm;
        qDebug() << "Set fan" << fanIndex << "PWM to" << pwm;
        return true;
    }

    emit error("Failed to set fan PWM");
    return false;
}

bool HWMonInterface::setFanSpeed(int fanIndex, int rpm)
{
    // Convert RPM to PWM (0-255)
    if (fanIndex < 0 || fanIndex >= fans.size()) {
        return false;
    }

    const HWMonFan& fan = fans[fanIndex];

    // Calculate PWM from RPM using linear mapping
    int pwm;
    if (fan.maxRPM > fan.minRPM) {
        double ratio = static_cast<double>(rpm - fan.minRPM) / (fan.maxRPM - fan.minRPM);
        pwm = static_cast<int>(ratio * 255);
    } else {
        // If we don't have min/max info, use a simple percentage
        pwm = (rpm * 255) / 5000;  // Assume max 5000 RPM
    }

    pwm = qBound(0, pwm, 255);

    return setFanPWM(fanIndex, pwm);
}
