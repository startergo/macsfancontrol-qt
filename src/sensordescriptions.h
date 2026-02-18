#ifndef SENSORDESCRIPTIONS_H
#define SENSORDESCRIPTIONS_H

#include <QString>
#include <QMap>

class SensorDescriptions {
public:
    static QString getDescription(const QString& sensorLabel, const QString& macModel);
    static void loadCustomDescriptions(const QString& configPath);

private:
    static QMap<QString, QString> getDefaultDescriptions();
    static QMap<QString, QString> getMacProDescriptions();
    static QMap<QString, QString> getMacBookProDescriptions();
    static QMap<QString, QString> getMacMiniDescriptions();
    static QMap<QString, QString> getiMacDescriptions();
    static QMap<QString, QString> customDescriptions;
};

#endif // SENSORDESCRIPTIONS_H
