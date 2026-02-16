#include "sensordescriptions.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

QMap<QString, QString> SensorDescriptions::customDescriptions;

QString SensorDescriptions::getDescription(const QString& sensorLabel, const QString& macModel)
{
    // Check custom descriptions first
    if (customDescriptions.contains(sensorLabel)) {
        return customDescriptions[sensorLabel];
    }

    // Get model-specific descriptions
    QMap<QString, QString> modelDescriptions;

    if (macModel.contains("MacPro", Qt::CaseInsensitive)) {
        modelDescriptions = getMacProDescriptions();
    } else if (macModel.contains("MacBookPro", Qt::CaseInsensitive)) {
        modelDescriptions = getMacBookProDescriptions();
    } else if (macModel.contains("iMac", Qt::CaseInsensitive)) {
        modelDescriptions = getiMacDescriptions();
    } else {
        modelDescriptions = getDefaultDescriptions();
    }

    // Return model-specific description or the label itself
    return modelDescriptions.value(sensorLabel, sensorLabel);
}

void SensorDescriptions::loadCustomDescriptions(const QString& configPath)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // Skip comments and empty lines
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        // Parse "SENSOR_CODE=Description" format
        int equalPos = line.indexOf('=');
        if (equalPos > 0) {
            QString sensorCode = line.left(equalPos).trimmed();
            QString description = line.mid(equalPos + 1).trimmed();
            customDescriptions[sensorCode] = description;
            qDebug() << "Loaded custom description:" << sensorCode << "=" << description;
        }
    }

    file.close();
}

QMap<QString, QString> SensorDescriptions::getDefaultDescriptions()
{
    return QMap<QString, QString> {
        // Ambient
        {"TA0P", "Ambient"},

        // CPU
        {"TCAC", "CPU A Core (PECI)"},
        {"TCAD", "CPU A Diode"},
        {"TCAG", "CPU A GPU"},
        {"TCAH", "CPU A Heatsink"},
        {"TCAS", "CPU A SRAM"},
        {"TCBC", "CPU B Core (PECI)"},
        {"TCBD", "CPU B Diode"},
        {"TCBG", "CPU B GPU"},
        {"TCBH", "CPU B Heatsink"},
        {"TCBS", "CPU B SRAM"},

        // Drive Bays
        {"TH1P", "Drive Bay 0"},
        {"TH2P", "Drive Bay 1"},
        {"TH3P", "Drive Bay 2"},
        {"TH4P", "Drive Bay 3"},

        // Memory (DIMM Proximity)
        {"TM1P", "DIMM Proximity 1"},
        {"TM2P", "DIMM Proximity 2"},
        {"TM3P", "DIMM Proximity 3"},
        {"TM4P", "DIMM Proximity 4"},
        {"TM5P", "DIMM Proximity 5"},
        {"TM6P", "DIMM Proximity 6"},
        {"TM7P", "DIMM Proximity 7"},
        {"TM8P", "DIMM Proximity 8"},

        // IOH (Northbridge)
        {"TN0D", "IOH Diode"},
        {"TN0H", "IOH Heatsink"},

        // PCIe/Enclosure
        {"Te1P", "PCIe Ambient"},

        // Power Supply
        {"Tp0C", "AC/DC Supply 1"},
        {"Tp1C", "AC/DC Supply 2"}
    };
}

QMap<QString, QString> SensorDescriptions::getMacProDescriptions()
{
    QMap<QString, QString> descriptions = getDefaultDescriptions();

    // Mac Pro specific sensors
    descriptions["THTG"] = "Thermal Group Target";
    descriptions["TMTG"] = "Memory Thermal Group";
    descriptions["TNTG"] = "Northbridge Thermal Group";
    descriptions["TpTG"] = "Power Supply Thermal Group";
    descriptions["TeGG"] = "GPU Graphics Thermal Group";
    descriptions["TeRG"] = "GPU RAM Thermal Group";
    descriptions["TeGP"] = "GPU Package";
    descriptions["TeRP"] = "GPU RAM Package";
    descriptions["THPS"] = "HDD Power Supply";
    descriptions["TMA1"] = "Memory Bank A1";
    descriptions["TMA2"] = "Memory Bank A2";
    descriptions["TMA3"] = "Memory Bank A3";
    descriptions["TMA4"] = "Memory Bank A4";
    descriptions["TMB1"] = "Memory Bank B1";
    descriptions["TMB2"] = "Memory Bank B2";
    descriptions["TMB3"] = "Memory Bank B3";
    descriptions["TMB4"] = "Memory Bank B4";
    descriptions["TMHS"] = "Memory Heatsink";
    descriptions["TMLS"] = "Memory Low Side";
    descriptions["TMPS"] = "Memory Power Supply";
    descriptions["TMPV"] = "Memory PVDD";
    descriptions["TH1F"] = "Drive Bay 1 Front";
    descriptions["TH1V"] = "Drive Bay 1 SATA";
    descriptions["TH2F"] = "Drive Bay 2 Front";
    descriptions["TH2V"] = "Drive Bay 2 SATA";
    descriptions["TH3F"] = "Drive Bay 3 Front";
    descriptions["TH3V"] = "Drive Bay 3 SATA";
    descriptions["TH4F"] = "Drive Bay 4 Front";
    descriptions["TH4V"] = "Drive Bay 4 SATA";
    descriptions["Te1F"] = "PCIe Slot 1 Front";
    descriptions["Te1S"] = "PCIe Slot 1 Side";
    descriptions["Te2F"] = "PCIe Slot 2 Front";
    descriptions["Te2S"] = "PCIe Slot 2 Side";
    descriptions["Te3F"] = "PCIe Slot 3 Front";
    descriptions["Te3S"] = "PCIe Slot 3 Side";
    descriptions["Te4F"] = "PCIe Slot 4 Front";
    descriptions["Te4S"] = "PCIe Slot 4 Side";
    descriptions["Te5F"] = "PCIe Slot 5 Front";
    descriptions["Te5S"] = "PCIe Slot 5 Side";
    descriptions["TpPS"] = "Power Supply";

    return descriptions;
}

QMap<QString, QString> SensorDescriptions::getMacBookProDescriptions()
{
    QMap<QString, QString> descriptions;

    // MacBook Pro specific sensors
    descriptions["TA0P"] = "Ambient";
    descriptions["TB0T"] = "Battery";
    descriptions["TB1T"] = "Battery 2";
    descriptions["TB2T"] = "Battery 3";
    descriptions["TC0C"] = "CPU Core";
    descriptions["TC0D"] = "CPU Diode";
    descriptions["TC0E"] = "CPU Core 2";
    descriptions["TC0F"] = "CPU Core 3";
    descriptions["TC0P"] = "CPU Proximity";
    descriptions["TCGC"] = "GPU Core";
    descriptions["TG0D"] = "GPU Diode";
    descriptions["TG0P"] = "GPU Proximity";
    descriptions["TH0P"] = "HDD Proximity";
    descriptions["Th1H"] = "Heatpipe 1";
    descriptions["Th2H"] = "Heatpipe 2";
    descriptions["TM0P"] = "Memory Proximity";
    descriptions["TM0S"] = "Memory Slot";
    descriptions["TN0P"] = "Northbridge Proximity";
    descriptions["TN1P"] = "Northbridge 2";
    descriptions["TO0P"] = "Optical Drive";
    descriptions["Tp0C"] = "Power Supply";
    descriptions["Tp0P"] = "Palm Rest";
    descriptions["TPCD"] = "PCH Die";
    descriptions["TW0P"] = "Wireless Module";
    descriptions["Ts0P"] = "Palm Rest Left";
    descriptions["Ts1P"] = "Palm Rest Right";

    return descriptions;
}

QMap<QString, QString> SensorDescriptions::getiMacDescriptions()
{
    QMap<QString, QString> descriptions;

    // iMac specific sensors
    descriptions["TA0P"] = "Ambient";
    descriptions["TC0C"] = "CPU Core";
    descriptions["TC0D"] = "CPU Diode";
    descriptions["TC0H"] = "CPU Heatsink";
    descriptions["TC0P"] = "CPU Proximity";
    descriptions["TG0D"] = "GPU Diode";
    descriptions["TG0H"] = "GPU Heatsink";
    descriptions["TG0P"] = "GPU Proximity";
    descriptions["TH0P"] = "HDD Proximity";
    descriptions["TL0P"] = "LCD Proximity";
    descriptions["TM0P"] = "Memory Proximity";
    descriptions["TN0D"] = "Northbridge Diode";
    descriptions["TN0P"] = "Northbridge Proximity";
    descriptions["TO0P"] = "Optical Drive";
    descriptions["Tp0C"] = "Power Supply";
    descriptions["Tp0P"] = "Power Supply 2";

    return descriptions;
}
