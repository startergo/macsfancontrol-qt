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

    // CPU Temperature (supports up to 12 cores on MacPro4,1/5,1/6,1)
    descriptions["TC0C"] = "CPU Core 0";
    descriptions["TC1C"] = "CPU Core 1";
    descriptions["TC2C"] = "CPU Core 2";
    descriptions["TC3C"] = "CPU Core 3";
    descriptions["TC4C"] = "CPU Core 4";
    descriptions["TC5C"] = "CPU Core 5";
    descriptions["TC6C"] = "CPU Core 6";
    descriptions["TC7C"] = "CPU Core 7";
    descriptions["TC8C"] = "CPU Core 8";
    descriptions["TC9C"] = "CPU Core 9";
    descriptions["TC10C"] = "CPU Core 10";
    descriptions["TC11C"] = "CPU Core 11";
    descriptions["TC0D"] = "CPU Diode";
    descriptions["TC1D"] = "CPU Diode 2";
    descriptions["TC0E"] = "CPU Heatsink";
    descriptions["TC0F"] = "CPU Proximity";
    descriptions["TC0H"] = "CPU Hot Spot";
    descriptions["TC0P"] = "CPU Package";
    descriptions["TCGC"] = "CPU Graphics Cluster";
    descriptions["TCSC"] = "CPU System Cluster";

    // GPU Temperature (Dual GPU support)
    descriptions["TG0D"] = "GPU 0 Diode";
    descriptions["TG1D"] = "GPU 1 Diode";
    descriptions["TG0P"] = "GPU 0 Proximity";
    descriptions["TG1P"] = "GPU 1 Proximity";
    descriptions["TG0C"] = "GPU 0 Core";
    descriptions["TG1C"] = "GPU 1 Core";
    descriptions["TG0S"] = "GPU 0 Sensor";
    descriptions["TG1S"] = "GPU 1 Sensor";
    descriptions["TG0T"] = "GPU 0 Die";
    descriptions["TG1T"] = "GPU 1 Die";
    descriptions["TG0G"] = "GPU 0 Graphics";
    descriptions["TeGG"] = "GPU Graphics Thermal Group";
    descriptions["TeRG"] = "GPU RAM Thermal Group";
    descriptions["TeGP"] = "GPU Package";
    descriptions["TeRP"] = "GPU RAM Package";

    // Memory (8 DIMM support for MacPro5,1/6,1)
    descriptions["Tm0P"] = "Memory Bank 0 Proximity";
    descriptions["Tm1P"] = "Memory Bank 1 Proximity";
    descriptions["Tm2P"] = "Memory Bank 2 Proximity";
    descriptions["Tm3P"] = "Memory Bank 3 Proximity";
    descriptions["Tm4P"] = "Memory Bank 4 Proximity";
    descriptions["Tm5P"] = "Memory Bank 5 Proximity";
    descriptions["Tm6P"] = "Memory Bank 6 Proximity";
    descriptions["Tm7P"] = "Memory Bank 7 Proximity";
    descriptions["TmAS"] = "Memory Slot A";
    descriptions["TmBS"] = "Memory Slot B";
    descriptions["TmCS"] = "Memory Slot C";
    descriptions["TmDS"] = "Memory Slot D";
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
    descriptions["TMTG"] = "Memory Thermal Group";

    // Ambient Sensors
    descriptions["TA0P"] = "Ambient Front";
    descriptions["TA1P"] = "Ambient Rear";
    descriptions["TA2P"] = "Ambient Internal";
    descriptions["TA0S"] = "Ambient Sensor";
    descriptions["TA0E"] = "Ambient Enclosure";
    descriptions["TA0T"] = "Ambient Top";

    // Drive Bays (4 bays)
    descriptions["HDD0"] = "Drive Bay 0 Temp";
    descriptions["HDD1"] = "Drive Bay 1 Temp";
    descriptions["HDD2"] = "Drive Bay 2 Temp";
    descriptions["HDD3"] = "Drive Bay 3 Temp";
    descriptions["TH1F"] = "Drive Bay 1 Front";
    descriptions["TH1V"] = "Drive Bay 1 SATA";
    descriptions["TH2F"] = "Drive Bay 2 Front";
    descriptions["TH2V"] = "Drive Bay 2 SATA";
    descriptions["TH3F"] = "Drive Bay 3 Front";
    descriptions["TH3V"] = "Drive Bay 3 SATA";
    descriptions["TH4F"] = "Drive Bay 4 Front";
    descriptions["TH4V"] = "Drive Bay 4 SATA";
    descriptions["Th0H"] = "Drive Thermal";
    descriptions["THPS"] = "HDD Power Supply";

    // PCIe Slots (5 slots)
    descriptions["Te1P"] = "PCIe Ambient";
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

    // Northbridge/PCH
    descriptions["TN0D"] = "Northbridge Diode";
    descriptions["TN0P"] = "Northbridge Proximity";
    descriptions["TN0S"] = "Northbridge Sensor";
    descriptions["TNTG"] = "Northbridge Thermal Group";

    // Power Supply
    descriptions["Tp0P"] = "Power Supply Proximity";
    descriptions["Tp0C"] = "Power Supply";
    descriptions["Tp1C"] = "Power Supply 2";
    descriptions["TpPS"] = "Power Supply Sensor";
    descriptions["TpTG"] = "Power Supply Thermal Group";
    descriptions["TV0R"] = "Voltage Regulator";

    // Thermal Groups
    descriptions["THTG"] = "Thermal Group Target";

    // Power / Voltage / Current (Mac Pro 4,1/5,1/6,1)
    descriptions["PC0C"] = "CPU Core 0 Power";
    descriptions["PC1C"] = "CPU Core 1 Power";
    descriptions["PC2C"] = "CPU Core 2 Power";
    descriptions["PC3C"] = "CPU Core 3 Power";
    descriptions["PC0R"] = "CPU Rail Power";
    descriptions["PC1R"] = "CPU Rail 2 Power";
    descriptions["PCPT"] = "CPU Package Total Power";

    return descriptions;
}

QMap<QString, QString> SensorDescriptions::getMacBookProDescriptions()
{
    QMap<QString, QString> descriptions;

    // Ambient
    descriptions["TA0P"] = "Ambient Proximity";
    descriptions["TA1P"] = "Ambient Proximity 2";
    descriptions["TA0S"] = "Ambient Sensor";
    descriptions["TA0D"] = "Ambient Diode";
    descriptions["TA0E"] = "Ambient Enclosure";
    descriptions["TA0T"] = "Ambient Top";
    descriptions["Tals"] = "Ambient Left Side";
    descriptions["Tars"] = "Ambient Right Side";
    descriptions["Tarl"] = "Ambient Rear Left";

    // Battery (comprehensive)
    descriptions["TB0T"] = "Battery 0";
    descriptions["TB1T"] = "Battery 1";
    descriptions["TB2T"] = "Battery 2";
    descriptions["TB3T"] = "Battery 3";
    descriptions["TB0S"] = "Battery Sensor 0";
    descriptions["TB1S"] = "Battery Sensor 1";
    descriptions["TB1F"] = "Battery Front";
    descriptions["TB1M"] = "Battery Middle";
    descriptions["TB1r"] = "Battery Rear";

    // CPU Temperature (Intel)
    descriptions["TC0C"] = "CPU Core 0";
    descriptions["TC1C"] = "CPU Core 1";
    descriptions["TC2C"] = "CPU Core 2";
    descriptions["TC3C"] = "CPU Core 3";
    descriptions["TC4C"] = "CPU Core 4";
    descriptions["TC5C"] = "CPU Core 5";
    descriptions["TC6C"] = "CPU Core 6";
    descriptions["TC7C"] = "CPU Core 7";
    descriptions["TC8C"] = "CPU Core 8";
    descriptions["TC9C"] = "CPU Core 9";
    descriptions["TC0D"] = "CPU Diode";
    descriptions["TC0E"] = "CPU Core Average";
    descriptions["TC0F"] = "CPU Core Max";
    descriptions["TC0P"] = "CPU Proximity";
    descriptions["TCAH"] = "CPU Heatsink";
    descriptions["TCAD"] = "CPU Package";
    descriptions["TC0H"] = "CPU Heatsink Alt";
    descriptions["TC0G"] = "CPU Integrated GPU";
    descriptions["TCGC"] = "CPU GPU PECI";
    descriptions["TCGc"] = "CPU GPU PECI 2";
    descriptions["TCAC"] = "CPU Efficiency Cores";
    descriptions["TCBC"] = "CPU Secondary Cluster";
    descriptions["TCCD"] = "CPU Cross-Domain";
    descriptions["TCSC"] = "CPU System Cluster";

    // GPU Temperature (Intel/Discrete)
    descriptions["TG0D"] = "GPU Diode";
    descriptions["TG1D"] = "GPU Diode 2";
    descriptions["TG0P"] = "GPU Proximity";
    descriptions["TG1P"] = "GPU Proximity 2";
    descriptions["TG0T"] = "GPU Die";
    descriptions["TG1T"] = "GPU Die 2";
    descriptions["TG0C"] = "GPU Core";
    descriptions["TG1C"] = "GPU Core 2";
    descriptions["TG0G"] = "GPU Graphics";
    descriptions["TGDD"] = "GPU Desktop Discrete";

    // Memory
    descriptions["TM0P"] = "Memory Proximity";
    descriptions["TM0S"] = "Memory Slot 0";
    descriptions["TM1S"] = "Memory Slot 1";
    descriptions["TM8S"] = "Memory Slot 2";
    descriptions["TM9S"] = "Memory Slot 3";
    descriptions["TMA1"] = "Memory Bank A1";
    descriptions["TMA2"] = "Memory Bank A2";
    descriptions["TMA3"] = "Memory Bank A3";
    descriptions["TMA4"] = "Memory Bank A4";
    descriptions["TMB1"] = "Memory Bank B1";
    descriptions["TMB2"] = "Memory Bank B2";
    descriptions["TMB3"] = "Memory Bank B3";
    descriptions["TMB4"] = "Memory Bank B4";
    descriptions["Tm0P"] = "Memory Bank 0 Proximity";
    descriptions["Tm1P"] = "Memory Bank 1 Proximity";
    descriptions["Tm2P"] = "Memory Bank 2 Proximity";
    descriptions["Tm3P"] = "Memory Bank 3 Proximity";
    descriptions["TmAS"] = "Memory Slot A";
    descriptions["TmBS"] = "Memory Slot B";
    descriptions["TmCS"] = "Memory Slot C";
    descriptions["TmDS"] = "Memory Slot D";

    // Thunderbolt
    descriptions["TB0T"] = "Thunderbolt 0";
    descriptions["TB1T"] = "Thunderbolt 1";
    descriptions["TB2T"] = "Thunderbolt 2";
    descriptions["TB3T"] = "Thunderbolt 3";

    // Heatpipes
    descriptions["Th1H"] = "Heatpipe 1";
    descriptions["Th2H"] = "Heatpipe 2";

    // Hard Drive
    descriptions["TH0P"] = "HDD Proximity";
    descriptions["TH0A"] = "HDD A";
    descriptions["TH0B"] = "HDD B";
    descriptions["TH0C"] = "HDD C";

    // Northbridge/PCH
    descriptions["TN0D"] = "Northbridge Diode";
    descriptions["TN0P"] = "Northbridge Proximity";
    descriptions["TN1P"] = "Northbridge 2";
    descriptions["TPCD"] = "PCH Die";

    // Optical Drive
    descriptions["TO0P"] = "Optical Drive";

    // Power Supply
    descriptions["Tp0C"] = "Power Supply";
    descriptions["Tp0P"] = "Palm Rest";
    descriptions["Tp0D"] = "Power Supply Diode";
    descriptions["Tp1C"] = "Power Supply 2";
    descriptions["Tp1P"] = "Power Supply Proximity";
    descriptions["TV0R"] = "VRM Temperature";

    // Wireless
    descriptions["TW0P"] = "Wireless Module";
    descriptions["TW0S"] = "Wireless Sensor";
    descriptions["TWAP"] = "Wireless Alt";

    // Palm Rest / Trackpad
    descriptions["Ts0P"] = "Palm Rest Left";
    descriptions["Ts1P"] = "Palm Rest Right";
    descriptions["Ts0S"] = "Trackpad Sensor 0";
    descriptions["Ts1S"] = "Trackpad Sensor 1";

    // Airflow / Enclosure
    descriptions["Te0T"] = "Enclosure Top";
    descriptions["Te1T"] = "Enclosure Bottom 1";
    descriptions["Te2T"] = "Enclosure Bottom 2";
    descriptions["Te3T"] = "Enclosure Bottom 3";
    descriptions["Te4T"] = "Enclosure Bottom 4";
    descriptions["Te5T"] = "Enclosure Bottom 5";

    // Thermal Diodes (detailed mapping)
    descriptions["TD0P"] = "Thermal Diode 0";
    descriptions["TD1P"] = "Thermal Diode 1";
    descriptions["TD2P"] = "Thermal Diode 2";
    descriptions["TD3P"] = "Thermal Diode 3";

    return descriptions;
}

QMap<QString, QString> SensorDescriptions::getiMacDescriptions()
{
    QMap<QString, QString> descriptions;

    // Ambient
    descriptions["TA0P"] = "Ambient Proximity";
    descriptions["TA1P"] = "Ambient Proximity 2";
    descriptions["TA0S"] = "Ambient Sensor";
    descriptions["TA0D"] = "Ambient Diode";
    descriptions["TA0E"] = "Ambient Enclosure";
    descriptions["TA0T"] = "Ambient Top";

    // CPU Temperature (Intel)
    descriptions["TC0C"] = "CPU Core 0";
    descriptions["TC1C"] = "CPU Core 1";
    descriptions["TC2C"] = "CPU Core 2";
    descriptions["TC3C"] = "CPU Core 3";
    descriptions["TC4C"] = "CPU Core 4";
    descriptions["TC5C"] = "CPU Core 5";
    descriptions["TC6C"] = "CPU Core 6";
    descriptions["TC7C"] = "CPU Core 7";
    descriptions["TC8C"] = "CPU Core 8";
    descriptions["TC9C"] = "CPU Core 9";
    descriptions["TC0D"] = "CPU Diode";
    descriptions["TC0E"] = "CPU Core Average";
    descriptions["TC0F"] = "CPU Core Max";
    descriptions["TC0H"] = "CPU Heatsink";
    descriptions["TC0P"] = "CPU Proximity";
    descriptions["TCAD"] = "CPU Package";
    descriptions["TCAH"] = "CPU Heatsink Alt";
    descriptions["TC0G"] = "CPU Integrated GPU";
    descriptions["TCGC"] = "CPU GPU PECI";
    descriptions["TCAC"] = "CPU Efficiency Cores";
    descriptions["TCBC"] = "CPU Secondary Cluster";
    descriptions["TCSC"] = "CPU System Cluster";

    // GPU Temperature (Intel/Discrete)
    descriptions["TG0D"] = "GPU Diode";
    descriptions["TG1D"] = "GPU Diode 2";
    descriptions["TG0H"] = "GPU Heatsink";
    descriptions["TG0P"] = "GPU Proximity";
    descriptions["TG1P"] = "GPU Proximity 2";
    descriptions["TG0T"] = "GPU Die";
    descriptions["TG0C"] = "GPU Core";
    descriptions["TGDD"] = "GPU Desktop Discrete";

    // Memory
    descriptions["TM0P"] = "Memory Proximity";
    descriptions["TM1P"] = "Memory Proximity 2";
    descriptions["TM0S"] = "Memory Slot 0";
    descriptions["TM1S"] = "Memory Slot 1";
    descriptions["TM8S"] = "Memory Slot 2";
    descriptions["TM9S"] = "Memory Slot 3";
    descriptions["Tm0P"] = "Memory Bank 0 Proximity";
    descriptions["Tm1P"] = "Memory Bank 1 Proximity";
    descriptions["Tm2P"] = "Memory Bank 2 Proximity";
    descriptions["Tm3P"] = "Memory Bank 3 Proximity";
    descriptions["TmAS"] = "Memory Slot A";
    descriptions["TmBS"] = "Memory Slot B";
    descriptions["TmCS"] = "Memory Slot C";
    descriptions["TmDS"] = "Memory Slot D";

    // Thunderbolt
    descriptions["TB0T"] = "Thunderbolt 0";
    descriptions["TB1T"] = "Thunderbolt 1";
    descriptions["TB2T"] = "Thunderbolt 2";
    descriptions["TB3T"] = "Thunderbolt 3";

    // Hard Drive
    descriptions["HDD0"] = "Drive 0 Temp";
    descriptions["HDD1"] = "Drive 1 Temp";
    descriptions["TH0P"] = "HDD Proximity";
    descriptions["TH1P"] = "HDD Proximity 2";
    descriptions["TH0A"] = "HDD A";
    descriptions["TH0B"] = "HDD B";
    descriptions["Th0H"] = "Drive Thermal";

    // LCD
    descriptions["TL0P"] = "LCD Proximity";
    descriptions["TL1P"] = "LCD Proximity 2";

    // Northbridge/PCH
    descriptions["TN0D"] = "Northbridge Diode";
    descriptions["TN0H"] = "Northbridge Heatsink";
    descriptions["TN0P"] = "Northbridge Proximity";
    descriptions["TPCD"] = "PCH Die";

    // Optical Drive
    descriptions["TO0P"] = "Optical Drive";

    // Power Supply
    descriptions["Tp0C"] = "Power Supply";
    descriptions["Tp0P"] = "Power Supply Proximity";
    descriptions["Tp0D"] = "Power Supply Diode";
    descriptions["Tp1C"] = "Power Supply 2";
    descriptions["Tp1P"] = "Power Supply Proximity 2";
    descriptions["TV0R"] = "VRM Temperature";

    // Wireless
    descriptions["TW0P"] = "Wireless Module";
    descriptions["TW0S"] = "Wireless Sensor";

    // Enclosure
    descriptions["Te0T"] = "Enclosure Top";
    descriptions["Te1T"] = "Enclosure Bottom 1";

    // Thermal Diodes
    descriptions["TD0P"] = "Thermal Diode 0";
    descriptions["TD1P"] = "Thermal Diode 1";
    descriptions["TD2P"] = "Thermal Diode 2";
    descriptions["TD3P"] = "Thermal Diode 3";

    return descriptions;
}
