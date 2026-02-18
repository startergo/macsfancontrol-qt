# Mac Fan Control for Linux (Qt5)

A Qt5 GUI application for controlling Apple SMC (System Management Controller) fans on Mac hardware running Linux.

## Features

- **Real-time monitoring**: View current fan speeds (RPM) and temperature sensors
- **Manual/Automatic control**: Switch between SMC firmware control and manual fan speed settings per fan
- **Safety enforcement**: Automatic min/max RPM limits to prevent hardware damage
- **Temperature display**: Monitor all 68 temperature sensors with color-coded values
- **Clean interface**: Intuitive Qt5 GUI similar to macsfancontrol for macOS

## Requirements

- Mac hardware (Mac Pro, iMac, MacBook, etc.) running Linux
- `applesmc` kernel driver loaded and functional
- Qt5 development libraries (Qt5 5.15 or later)
- qmake build tool
- Root/sudo access for fan control (read-only mode works without root)

### Checking Requirements

```bash
# Check if applesmc driver is loaded
lsmod | grep applesmc

# Check if SMC interface is available (traditional applesmc)
ls /sys/devices/platform/applesmc.768/
# Or on T2 Macs (ACPI-based applesmc)
ls /sys/bus/acpi/devices/APP0001:00/

# Check Qt5 installation
qmake --version

# View current fan status
sensors
```

## Building

```bash
# Navigate to project directory
cd macsfancontrol-qt

# Generate Makefile
qmake

# Compile
make

# The executable will be created in the current directory
ls -l macsfancontrol
```

## Installation

```bash
# Optional: Install to /usr/local/bin
sudo make install

# Or copy manually
sudo cp macsfancontrol /usr/local/bin/
```

## Running

### With Full Control (Recommended)

```bash
# Run with sudo for fan control
sudo ./macsfancontrol
```

### Read-Only Mode

```bash
# Run without sudo (monitoring only, no control)
./macsfancontrol
```

You'll see a warning dialog, but can continue to monitor fans without being able to change settings.

## Usage

### Fan Control

Each fan has its own control widget with three operating modes:

#### Operating Modes

- **Auto Mode** (Green indicator)
  - SMC firmware controls fan speed automatically
  - Slider is disabled
  - Fan responds to Apple's built-in thermal management

- **Manual Mode** (Blue indicator)
  - User sets a fixed fan speed using the slider
  - Slider enabled for direct RPM control
  - Fan maintains constant speed regardless of temperature

- **Sensor-Based Mode** (Orange indicator) **NEW!**
  - Fan speed automatically adjusts based on a selected temperature sensor
  - Similar to macsfancontrol for macOS
  - Configure:
    1. Select a temperature sensor from the dropdown
    2. Set minimum temperature threshold (e.g., 40°C)
    3. Set maximum temperature threshold (e.g., 80°C)
  - Fan speed calculation:
    - Below min temp → minimum RPM
    - Above max temp → maximum RPM
    - Between min/max → linear interpolation between min and max RPM
  - Current temperature of selected sensor is displayed in real-time

#### Common Controls

- **Current RPM**: Real-time display of actual fan speed (updates every second)

- **Target RPM**: In manual/sensor-based modes, shows the target speed

- **Slider**: In manual mode, drag to set desired fan speed (automatically clamped to min-max range)

#### Example: Sensor-Based Setup

For GPU cooling, you might configure:
- **Sensor**: TCAG (GPU temp sensor) - typically reading 60-85°C
- **Min Temp**: 50°C → Fan runs at minimum speed below 50°C
- **Max Temp**: 85°C → Fan runs at maximum speed above 85°C
- **Between**: At 67.5°C (midpoint), fan runs at 50% speed

### Temperature Monitoring

The right panel displays all available temperature sensors:

- **Color coding**:
  - Green: < 40°C (cool)
  - Blue: 40-60°C (moderate)
  - Orange: 60-80°C (warm)
  - Red: ≥ 80°C (hot)

- Values update every second

### Safety Features

1. **Min/Max Enforcement**: You cannot set fan speeds below minimum or above maximum RPM
2. **Auto Restoration**: All fans automatically return to Auto mode when you exit the application
3. **Permission Checks**: Application warns if running without root privileges
4. **Error Handling**: Clear error messages for any sysfs read/write failures

## Fan Information

Your Mac Pro has 6 fans:

1. **PCI** - PCI card area cooling (800-4500 RPM)
2. **PS** - Power supply fan (600-2800 RPM)
3. **EXHAUST** - Rear exhaust fan (600-2800 RPM)
4. **INTAKE** - Front intake fan (600-2800 RPM)
5. **BOOSTA** - CPU/GPU boost fan A (800-5200 RPM)
6. **BOOSTB** - CPU/GPU boost fan B (800-5200 RPM)

## Troubleshooting

### "Apple SMC interface not found"

- Make sure you're running on Mac hardware
- Check if applesmc driver is loaded: `lsmod | grep applesmc`
- Load the driver if needed: `sudo modprobe applesmc`
- On T2 Macs the interface is ACPI-based: check `ls /sys/bus/acpi/devices/APP0001:00/`

### "Cannot write to SMC interface"

- Make sure you're running with sudo: `sudo ./macsfancontrol`
- Check file permissions (traditional): `ls -l /sys/devices/platform/applesmc.768/fan1_manual`
- Check file permissions (T2 Mac): `ls -l /sys/bus/acpi/devices/APP0001:00/fan1_manual`

### Fans not responding to changes

- Wait a few seconds - fan speed changes aren't instant
- Check `sensors` output to see if sysfs values are changing
- Verify you're in Manual mode (blue indicator)
- Check status bar for error messages

### Application won't start

- Verify Qt5 is installed: `qmake --version`
- Check for missing libraries: `ldd ./macsfancontrol`
- Run from terminal to see error messages

## Technical Details

### Architecture

- **SMCInterface**: Backend class handling all sysfs I/O operations
- **FanControlWidget**: Individual fan control UI component
- **TemperaturePanel**: Temperature sensor display panel
- **MainWindow**: Main application coordinator with QTimer updates

### sysfs Interface

The SMC path is auto-detected at startup. On traditional Macs it uses the platform driver path; on T2 Macs it uses the ACPI path:

| Mac type              | Base path                                  |
|-----------------------|--------------------------------------------|
| Traditional (non-T2)  | `/sys/devices/platform/applesmc.768`       |
| T2 Mac                | `/sys/bus/acpi/devices/APP0001:00`         |

Files read/written under the base path:

- `fan*_input` - Current RPM (read-only)
- `fan*_manual` - Mode (0=auto, 1=manual)
- `fan*_output` - Target RPM (manual mode)
- `fan*_min` - Minimum safe RPM
- `fan*_max` - Maximum RPM
- `temp*_input` - Temperature sensors

## Warning

**Use at your own risk!** While this application enforces min/max safety limits, incorrect fan settings could potentially:
- Cause overheating if fans are set too low
- Increase noise levels if fans are set too high
- Reduce component lifespan with improper cooling

Always monitor temperatures when testing manual fan settings!

## License

This is free and open source software. Use and modify as you wish.

## Credits

Inspired by macsfancontrol for macOS, reimplemented in Qt5 for Linux.

Built using Qt5, targeting Mac hardware running Linux with applesmc driver support.
