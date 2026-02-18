#ifndef TEMPERATUREPANEL_H
#define TEMPERATUREPANEL_H

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include "smcinterface.h"

class TemperaturePanel : public QWidget {
    Q_OBJECT

public:
    explicit TemperaturePanel(QWidget *parent = nullptr);

    void updateTemperatures(const QVector<TempSensor>& sensors);
    void setMacModel(const QString& model) { macModel = model; }

private:
    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QGridLayout *gridLayout;
    QMap<int, QLabel*> tempLabels;  // Maps sensor index to temperature label
    QString macModel;

    QString formatTemperature(int millidegrees);
    QColor getTemperatureColor(double celsius);
};

#endif // TEMPERATUREPANEL_H
