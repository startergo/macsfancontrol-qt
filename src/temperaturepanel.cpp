#include "temperaturepanel.h"
#include "sensordescriptions.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QScrollBar>

TemperaturePanel::TemperaturePanel(QWidget *parent)
    : QWidget(parent)
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create title
    QLabel *title = new QLabel("<b>Temperature Sensors</b>", this);
    title->setStyleSheet("font-size: 14px; padding: 10px;");
    mainLayout->addWidget(title);

    // Create scroll area
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Create content widget
    contentWidget = new QWidget();
    gridLayout = new QGridLayout(contentWidget);
    gridLayout->setSpacing(5);
    gridLayout->setContentsMargins(10, 10, 10, 10);

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
}


void TemperaturePanel::updateTemperatures(const QVector<TempSensor>& sensors)
{
    int row = 0;
    int col = 0;
    const int maxColumns = 1;  // Display in 1 column for better scrolling

    for (const TempSensor& sensor : sensors) {
        // Skip invalid readings
        if (sensor.temperature <= -100000) {  // -100°C in millidegrees
            continue;
        }

        QString description = SensorDescriptions::getDescription(sensor.label, macModel);
        QString tempStr = formatTemperature(sensor.temperature);
        double celsius = sensor.temperature / 1000.0;
        QColor color = getTemperatureColor(celsius);

        // Create or update label if it doesn't exist
        if (!tempLabels.contains(sensor.index)) {
            // Create label row with description
            // If description is the raw label (no mapping found), don't repeat it
        QString labelText = (description == sensor.label)
            ? sensor.label + ":"
            : QString("%1 (%2):").arg(description).arg(sensor.label);
            QLabel *nameLabel = new QLabel(labelText, contentWidget);
            nameLabel->setStyleSheet("font-size: 11px;");

            QLabel *tempLabel = new QLabel(tempStr, contentWidget);
            tempLabel->setStyleSheet(QString("font-weight: bold; font-size: 11px; color: %1;")
                                     .arg(color.name()));

            gridLayout->addWidget(nameLabel, row, col * 2);
            gridLayout->addWidget(tempLabel, row, col * 2 + 1);

            tempLabels[sensor.index] = tempLabel;

            col++;
            if (col >= maxColumns) {
                col = 0;
                row++;
            }
        } else {
            // Update existing label
            QLabel *tempLabel = tempLabels[sensor.index];
            tempLabel->setText(tempStr);
            tempLabel->setStyleSheet(QString("font-weight: bold; font-size: 11px; color: %1;")
                                     .arg(color.name()));
        }
    }
}

QString TemperaturePanel::formatTemperature(int millidegrees)
{
    double celsius = millidegrees / 1000.0;
    return QString("%1°C").arg(celsius, 0, 'f', 1);
}

QColor TemperaturePanel::getTemperatureColor(double celsius)
{
    // Color coding based on temperature thresholds
    if (celsius >= 80.0) {
        return QColor("#E74C3C");  // Red - hot
    } else if (celsius >= 60.0) {
        return QColor("#F39C12");  // Orange - warm
    } else if (celsius >= 40.0) {
        return QColor("#3498DB");  // Blue - moderate
    } else {
        return QColor("#27AE60");  // Green - cool
    }
}
