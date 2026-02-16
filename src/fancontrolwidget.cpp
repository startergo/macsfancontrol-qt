#include "fancontrolwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QGroupBox>

FanControlWidget::FanControlWidget(const FanInfo& fanInfo, QWidget *parent)
    : QWidget(parent),
      fanIndex(fanInfo.index - 1),  // Convert to 0-based index
      minRPM(fanInfo.minRPM),
      maxRPM(fanInfo.maxRPM),
      currentMode(MODE_AUTO),
      selectedSensorIndex(-1)
{
    setupUI(fanInfo);
}

void FanControlWidget::setupUI(const FanInfo& fanInfo)
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Create a frame for better visual separation
    QFrame *frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Raised);
    frame->setLineWidth(1);

    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    // Top row: Fan name and mode indicator
    QHBoxLayout *topRow = new QHBoxLayout();
    labelName = new QLabel(QString("<b>%1 Fan</b>").arg(fanInfo.label), this);
    labelName->setStyleSheet("font-size: 14px;");
    topRow->addWidget(labelName);

    topRow->addStretch();

    // Mode indicator (color-coded square)
    modeIndicator = new QWidget(this);
    modeIndicator->setFixedSize(20, 20);
    modeIndicator->setStyleSheet("background-color: #7ED321; border-radius: 3px;"); // Green for auto
    topRow->addWidget(modeIndicator);

    frameLayout->addLayout(topRow);

    // Mode selection row
    QHBoxLayout *modeRow = new QHBoxLayout();
    radioAuto = new QRadioButton("Auto", this);
    radioManual = new QRadioButton("Manual", this);
    radioSensorBased = new QRadioButton("Sensor", this);

    modeGroup = new QButtonGroup(this);
    modeGroup->addButton(radioAuto, MODE_AUTO);
    modeGroup->addButton(radioManual, MODE_MANUAL);
    modeGroup->addButton(radioSensorBased, MODE_SENSOR_BASED);

    // Set initial mode
    if (fanInfo.isManual) {
        radioManual->setChecked(true);
        currentMode = MODE_MANUAL;
    } else {
        radioAuto->setChecked(true);
        currentMode = MODE_AUTO;
    }

    modeRow->addWidget(radioAuto);
    modeRow->addWidget(radioManual);
    modeRow->addWidget(radioSensorBased);

    frameLayout->addLayout(modeRow);

    // Current RPM display
    QHBoxLayout *currentRow = new QHBoxLayout();
    currentRow->addWidget(new QLabel("Current:", this));
    labelCurrentRPM = new QLabel(QString("%1 RPM").arg(fanInfo.currentRPM), this);
    labelCurrentRPM->setStyleSheet("font-weight: bold; color: #333;");
    currentRow->addWidget(labelCurrentRPM);
    currentRow->addStretch();

    frameLayout->addLayout(currentRow);

    // Target RPM display (only visible in manual mode)
    QHBoxLayout *targetRow = new QHBoxLayout();
    targetRow->addWidget(new QLabel("Target:", this));
    labelTargetRPM = new QLabel(QString("%1 RPM").arg(fanInfo.targetRPM), this);
    labelTargetRPM->setStyleSheet("font-weight: bold; color: #4A90E2;");
    targetRow->addWidget(labelTargetRPM);
    targetRow->addStretch();

    frameLayout->addLayout(targetRow);

    // Sensor-based controls (hidden by default)
    sensorControls = new QWidget(this);
    QVBoxLayout *sensorLayout = new QVBoxLayout(sensorControls);
    sensorLayout->setContentsMargins(0, 5, 0, 5);

    // Sensor selection
    QHBoxLayout *sensorRow = new QHBoxLayout();
    sensorRow->addWidget(new QLabel("Sensor:", this));
    comboSensor = new QComboBox(this);
    comboSensor->addItem("(Select sensor)", -1);
    sensorRow->addWidget(comboSensor, 1);
    sensorLayout->addLayout(sensorRow);

    // Temperature thresholds
    QGridLayout *tempGrid = new QGridLayout();
    tempGrid->addWidget(new QLabel("Min Temp:", this), 0, 0);
    spinMinTemp = new QSpinBox(this);
    spinMinTemp->setRange(0, 100);
    spinMinTemp->setValue(40);
    spinMinTemp->setSuffix("°C");
    tempGrid->addWidget(spinMinTemp, 0, 1);

    tempGrid->addWidget(new QLabel("Max Temp:", this), 0, 2);
    spinMaxTemp = new QSpinBox(this);
    spinMaxTemp->setRange(0, 120);
    spinMaxTemp->setValue(80);
    spinMaxTemp->setSuffix("°C");
    tempGrid->addWidget(spinMaxTemp, 0, 3);

    sensorLayout->addLayout(tempGrid);

    // Current temperature display
    QHBoxLayout *currentTempRow = new QHBoxLayout();
    currentTempRow->addWidget(new QLabel("Current Temp:", this));
    labelCurrentTemp = new QLabel("--°C", this);
    labelCurrentTemp->setStyleSheet("font-weight: bold; color: #F39C12;");
    currentTempRow->addWidget(labelCurrentTemp);
    currentTempRow->addStretch();
    sensorLayout->addLayout(currentTempRow);

    sensorControls->setVisible(false);
    frameLayout->addWidget(sensorControls);

    // RPM slider
    sliderRPM = new QSlider(Qt::Horizontal, this);
    sliderRPM->setMinimum(minRPM);
    sliderRPM->setMaximum(maxRPM);
    sliderRPM->setValue(fanInfo.isManual ? fanInfo.targetRPM : fanInfo.currentRPM);
    sliderRPM->setTickPosition(QSlider::TicksBelow);
    sliderRPM->setTickInterval((maxRPM - minRPM) / 10);
    sliderRPM->setEnabled(fanInfo.isManual);

    frameLayout->addWidget(sliderRPM);

    // Range display
    labelRange = new QLabel(QString("%1 - %2 RPM").arg(minRPM).arg(maxRPM), this);
    labelRange->setStyleSheet("font-size: 10px; color: #666;");
    labelRange->setAlignment(Qt::AlignCenter);

    frameLayout->addWidget(labelRange);

    mainLayout->addWidget(frame);

    // Connect signals
    connect(modeGroup, QOverload<int>::of(&QButtonGroup::idClicked),
            this, &FanControlWidget::onModeChanged);
    connect(sliderRPM, &QSlider::valueChanged, this, &FanControlWidget::onSliderChanged);
    connect(comboSensor, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FanControlWidget::onSensorSettingsChanged);
    connect(spinMinTemp, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FanControlWidget::onSensorSettingsChanged);
    connect(spinMaxTemp, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FanControlWidget::onSensorSettingsChanged);

    // Update initial mode indicator
    updateModeIndicator(currentMode);
    updateControlsVisibility();
}

void FanControlWidget::setCurrentRPM(int rpm)
{
    labelCurrentRPM->setText(QString("%1 RPM").arg(rpm));
}

void FanControlWidget::updateFanInfo(const FanInfo& info)
{
    setCurrentRPM(info.currentRPM);
    labelTargetRPM->setText(QString("%1 RPM").arg(info.targetRPM));

    if (info.isManual && currentMode != MODE_SENSOR_BASED) {
        radioManual->setChecked(true);
        currentMode = MODE_MANUAL;
    } else if (currentMode != MODE_SENSOR_BASED) {
        radioAuto->setChecked(true);
        currentMode = MODE_AUTO;
    }

    updateModeIndicator(currentMode);
}

QString FanControlWidget::getSensorDescription(const QString& label)
{
    // Map Apple SMC sensor codes to human-readable descriptions
    // Based on macsfancontrol sensor mappings
    static const QMap<QString, QString> sensorMap = {
        // Ambient
        {"TA0P", "Ambient"},

        // CPU A
        {"TCAC", "CPU A Core (PECI)"},
        {"TCAD", "CPU A Diode"},
        {"TCAG", "CPU A GPU"},
        {"TCAH", "CPU A Heatsink"},
        {"TCAS", "CPU A SRAM"},

        // CPU B
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

    // Return description if found, otherwise return the label itself
    return sensorMap.value(label, label);
}

void FanControlWidget::setSensorList(const QVector<TempSensor>& sensors)
{
    // Save the currently selected sensor index before clearing
    int currentSelection = comboSensor->currentData().toInt();

    // Block signals to prevent triggering onSensorSettingsChanged during rebuild
    comboSensor->blockSignals(true);

    comboSensor->clear();
    comboSensor->addItem("(Select sensor)", -1);

    for (const TempSensor& sensor : sensors) {
        QString description = getSensorDescription(sensor.label);
        QString displayText = QString("%1 - %2 (%3°C)")
            .arg(sensor.label)
            .arg(description)
            .arg(sensor.temperature / 1000.0, 0, 'f', 1);
        comboSensor->addItem(displayText, sensor.index);
    }

    // Restore the previous selection if it still exists
    if (currentSelection >= 0) {
        for (int i = 0; i < comboSensor->count(); i++) {
            if (comboSensor->itemData(i).toInt() == currentSelection) {
                comboSensor->setCurrentIndex(i);
                break;
            }
        }
    }

    // Re-enable signals
    comboSensor->blockSignals(false);
}

void FanControlWidget::updateSensorBasedSpeed(int currentTemp)
{
    if (currentMode != MODE_SENSOR_BASED) {
        return;
    }

    // Update current temperature display
    labelCurrentTemp->setText(QString("%1°C").arg(currentTemp / 1000.0, 0, 'f', 1));

    // Calculate and emit new fan speed
    int targetSpeed = calculateFanSpeed(currentTemp / 1000, spinMinTemp->value(), spinMaxTemp->value());
    sliderRPM->setValue(targetSpeed);
    labelTargetRPM->setText(QString("%1 RPM").arg(targetSpeed));
    emit targetRPMChanged(fanIndex, targetSpeed);
}

void FanControlWidget::onModeChanged(int mode)
{
    currentMode = static_cast<FanMode>(mode);

    // Update mode indicator
    updateModeIndicator(currentMode);
    updateControlsVisibility();

    // Emit appropriate signals
    if (currentMode == MODE_AUTO) {
        emit manualModeRequested(fanIndex, false);
    } else if (currentMode == MODE_MANUAL) {
        emit manualModeRequested(fanIndex, true);
        emit targetRPMChanged(fanIndex, sliderRPM->value());
    } else if (currentMode == MODE_SENSOR_BASED) {
        emit manualModeRequested(fanIndex, true);  // Sensor-based uses manual control
        onSensorSettingsChanged();  // Trigger initial calculation
    }
}

void FanControlWidget::onSliderChanged(int value)
{
    // Update target label
    labelTargetRPM->setText(QString("%1 RPM").arg(value));

    // Only emit signal if in manual mode (not auto, not sensor-based)
    if (currentMode == MODE_MANUAL) {
        emit targetRPMChanged(fanIndex, value);
    }
}

void FanControlWidget::onSensorSettingsChanged()
{
    if (currentMode != MODE_SENSOR_BASED) {
        return;
    }

    int sensorIdx = comboSensor->currentData().toInt();
    if (sensorIdx < 0) {
        return;  // No sensor selected
    }

    selectedSensorIndex = sensorIdx;

    // Emit signal with sensor-based settings
    emit sensorBasedModeChanged(fanIndex, true, selectedSensorIndex,
                                 spinMinTemp->value(), spinMaxTemp->value());
}

void FanControlWidget::updateControlsVisibility()
{
    // Show/hide controls based on mode
    sliderRPM->setEnabled(currentMode == MODE_MANUAL);
    sensorControls->setVisible(currentMode == MODE_SENSOR_BASED);

    // Target RPM label visibility
    bool showTarget = (currentMode == MODE_MANUAL || currentMode == MODE_SENSOR_BASED);
    labelTargetRPM->setVisible(showTarget);
}

void FanControlWidget::updateModeIndicator(FanMode mode)
{
    switch (mode) {
        case MODE_AUTO:
            // Green for auto mode
            modeIndicator->setStyleSheet("background-color: #7ED321; border-radius: 3px;");
            break;
        case MODE_MANUAL:
            // Blue for manual mode
            modeIndicator->setStyleSheet("background-color: #4A90E2; border-radius: 3px;");
            break;
        case MODE_SENSOR_BASED:
            // Orange for sensor-based mode
            modeIndicator->setStyleSheet("background-color: #F39C12; border-radius: 3px;");
            break;
    }
}

int FanControlWidget::calculateFanSpeed(int currentTemp, int minTemp, int maxTemp)
{
    // Ensure min < max
    if (minTemp >= maxTemp) {
        return minRPM;  // Safe fallback
    }

    // Clamp current temperature to range
    if (currentTemp <= minTemp) {
        return minRPM;
    } else if (currentTemp >= maxTemp) {
        return maxRPM;
    }

    // Linear interpolation between min and max
    double tempRatio = static_cast<double>(currentTemp - minTemp) / (maxTemp - minTemp);
    int targetRPM = minRPM + static_cast<int>(tempRatio * (maxRPM - minRPM));

    return targetRPM;
}

void FanControlWidget::setMode(FanMode mode)
{
    currentMode = mode;

    // Update radio buttons
    switch (mode) {
        case MODE_AUTO:
            radioAuto->setChecked(true);
            break;
        case MODE_MANUAL:
            radioManual->setChecked(true);
            break;
        case MODE_SENSOR_BASED:
            radioSensorBased->setChecked(true);
            break;
    }

    // Update UI
    updateModeIndicator(mode);
    updateControlsVisibility();
}

void FanControlWidget::setTargetRPM(int rpm)
{
    // Clamp to valid range
    rpm = qBound(minRPM, rpm, maxRPM);
    sliderRPM->setValue(rpm);
    labelTargetRPM->setText(QString("%1 RPM").arg(rpm));
}

void FanControlWidget::setSensorBasedSettings(int sensorIndex, int minTemp, int maxTemp)
{
    selectedSensorIndex = sensorIndex;
    spinMinTemp->setValue(minTemp);
    spinMaxTemp->setValue(maxTemp);

    // Update combo box to show the selected sensor
    for (int i = 0; i < comboSensor->count(); i++) {
        if (comboSensor->itemData(i).toInt() == sensorIndex) {
            comboSensor->setCurrentIndex(i);
            break;
        }
    }
}
