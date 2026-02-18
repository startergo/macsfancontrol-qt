# SMC Sensor Database - Per-Model Quick Reference (Intel Macs)

This document provides a quick reference for SMC sensor keys organized by Intel Mac model family.

## Table of Contents

- [MacBook Pro (Intel)](#macbook-pro-intel)
- [MacBook Air (Intel)](#macbook-air-intel)
- [iMac (Intel)](#imac-intel)
- [Mac mini (Intel)](#mac-mini-intel)
- [Mac Pro (Intel)](#mac-pro-intel)
- [SMC Key Patterns](#smc-key-patterns)

---

## MacBook Pro (Intel)

### 16-inch Models (2019–2020)

#### MacBookPro16,1 (2019) — 4× Thunderbolt 3

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC9C | CPU Cores 0-9 | fpe2 | CPU core temperatures (Comet Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TC0F | CPU Proximity | fpe2 | CPU proximity sensor |
| TG0D | GPU Diode | fpe2 | AMD Radeon Pro 5300M / 5500M |
| TG1D | GPU 2 Diode | fpe2 | Second GPU (optional) |
| Tm0P–Tm1P | Memory | fpe2 | Memory slot temperatures |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TW0P | Wireless Proximity | fpe2 | Airport/WiFi card |
| TB0T–TB3T | Thunderbolt 0-3 | flt | Thunderbolt controller temperatures |
| Th1H | Heatsink Proximity | fpe2 | Heatsink proximity |
| F0Ac–F1Ac | Left/Right Fans | ui16 | Fan RPM |

**Notes:** Intel Core i7/i9 (Comet Lake, 6 cores) · AMD Radeon Pro 5300M or 5500M · dual fans

#### MacBookPro16,2 (2020) — 2× Thunderbolt 3

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures (Ice Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel Iris Plus graphics |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TB0T–TB1T | Thunderbolt 0-1 | flt | Thunderbolt controller temperatures |
| F0Ac | Fan | ui16 | Single fan RPM |

**Notes:** Intel Core i5/i7 (Ice Lake, 4 cores) · Intel Iris Plus · single fan

#### MacBookPro16,3 / 16,4 (2020) — 13-inch

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel Iris Plus graphics |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| F0Ac | Fan | ui16 | Fan RPM |

---

## MacBook Air (Intel)

### MacBookAir9,1 (2020)

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures (Ice Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel Iris Plus graphics |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TB0T | Battery | fpe2 | Battery temperature |

**Notes:** Intel Core i3/i5/i7 (Ice Lake, 10th gen) · fanless · 2× Thunderbolt 3

### MacBookAir8,1 / 8,2 (2018–2019)

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures (Amber Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel UHD 617 |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TB0T | Battery | fpe2 | Battery temperature |
| F0Ac | Fan | ui16 | Single fan RPM |

**Notes:** Intel Core i5/i7 (Amber Lake, 8th gen) · single fan · 2× Thunderbolt 3

---

## iMac (Intel)

### 27-inch 5K (2019–2020)

#### iMac20,1 / iMac20,2 (2020)

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC9C | CPU Cores 0-9 | fpe2 | CPU core temperatures (Comet Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | GPU Diode | fpe2 | AMD Radeon Pro 5300/5500/5700 XT |
| TG1D | GPU 2 Diode | fpe2 | Second AMD GPU (optional) |
| Tm0P–Tm1P | Memory | fpe2 | Memory slots 0-1 |
| TA0P–TA1P | Ambient | fpe2 | Front/back ambient |
| Th0H | Drive Thermal | fpe2 | HDD/SSD temperature |
| F0Ac–F1Ac | Fans | ui16 | CPU/GPU fans |
| HDD0 | Hard Drive | fpe2 | 3.5" HDD temperature (Fusion Drive) |

**Notes:** Intel Core i5/i7/i9 (10th gen Comet Lake) · AMD Radeon Pro 5300/5500/5700 XT · up to 128 GB RAM

### iMac Pro (2017)

#### iMacPro1,1

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC9C | CPU Cores 0-9 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU Diode | fpe2 | Radeon Pro Vega GPU |
| TG1D | GPU 2 Diode | fpe2 | Second Vega GPU |
| Tm0P–Tm3P | Memory | fpe2 | All 4 DIMM slots |
| TA0P–TA2P | Ambient | fpe2 | Multiple ambient sensors |
| Tp0P | Power Supply | fpe2 | PSU temperature |
| F0Ac–F1Ac | Fans | ui16 | Dual-fan cooler |
| HDD0 | Hard Drive | fpe2 | Storage temperature |
| TB0T–TB1T | Thunderbolt | flt | Thunderbolt controller |

**Notes:** Intel Xeon W (8–18 cores) · AMD Radeon Pro Vega 64/64X · ECC RAM 32–128 GB · 4× Thunderbolt 3

---

## Mac mini (Intel)

### Macmini8,1 (2018) — T2 chip

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC7C | CPU Cores 0-7 | fpe2 | CPU core temperatures (Coffee Lake, up to 6-core) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0F | CPU Core Max | fpe2 | CPU core maximum |
| TC0P | CPU Proximity | fpe2 | CPU proximity |
| TCSA | CPU System Agent | fpe2 | T2: system agent temperature |
| TCXC | CPU PECI Cross Domain | fpe2 | T2: cross-domain PECI |
| TCaP | CPU Package | fpe2 | T2: CPU package |
| TCGC | CPU GPU PECI | fpe2 | Integrated GPU PECI |
| TCSC | CPU System Cluster | fpe2 | System cluster temperature |
| TIED | Intel Embedded Device | fpe2 | T2: embedded controller |
| TG0D | GPU Diode | fpe2 | Intel UHD 630 |
| TG0P | GPU Proximity | fpe2 | GPU proximity |
| TG0T | GPU Die | fpe2 | GPU die temperature |
| TM0P | Memory Proximity | fpe2 | SO-DIMM proximity |
| TM0V | Memory Virtual | fpe2 | Memory virtual sensor |
| TH0F | NVMe Front | fpe2 | T2: NVMe front temperature |
| TH0a | NVMe a | fpe2 | T2: NVMe sensor a |
| TH0b | NVMe b | fpe2 | T2: NVMe sensor b |
| TS0V | SSD Virtual | fpe2 | T2: SSD virtual sensor |
| TPCD | PCH Die | fpe2 | T2: PCH die temperature |
| TPSD | PCH SD | fpe2 | T2: PCH SD temperature |
| TTTD | Thunderbolt TD | fpe2 | T2: Thunderbolt TD |
| TTXD | Thunderbolt XD | fpe2 | T2: Thunderbolt XD |
| TW0P | Wireless Module | fpe2 | WiFi/BT proximity |
| TW1P | Wireless Module 2 | fpe2 | WiFi/BT sensor 2 |
| TW2P | Wireless Module 3 | fpe2 | WiFi/BT sensor 3 |
| TA0V | Ambient Air | fpe2 | Ambient airflow |
| TaLC | Ambient Left C | fpe2 | Left ambient connector |
| TaRC | Ambient Right C | fpe2 | Right ambient connector |
| TV0R | VRM Temperature | fpe2 | Voltage regulator |
| Tp0C | Power Supply | fpe2 | Power supply temperature |
| F0Ac | Fan | ui16 | Single exhaust fan RPM |

**Notes:** Intel Core i3/i5/i7 (Coffee Lake, 8th gen) · T2 security chip · Intel UHD 630 · up to 64 GB SO-DIMM · 4× Thunderbolt 3 · single fan

---

## Mac Pro (Intel)

### MacPro7,1 (2019) — T2 chip

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC27C | CPU Cores 0-27 | fpe2 | All CPU cores (Xeon W, up to 28 cores) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0F | CPU Proximity | fpe2 | CPU proximity sensor |
| TC0P | CPU Package | fpe2 | CPU package temperature |
| TCSA | CPU System Agent | fpe2 | T2: system agent temperature |
| TCXC | CPU PECI Cross Domain | fpe2 | T2: cross-domain PECI |
| TCaP | CPU Package | fpe2 | T2: CPU package |
| TIED | Intel Embedded Device | fpe2 | T2: embedded controller |
| TG0D–TG5D | GPU Diodes 0-5 | fpe2 | Up to 6 GPU dies (MPX Modules) |
| TG0P–TG5P | GPU Proximities 0-5 | fpe2 | GPU proximity sensors |
| TG0T–TG5T | GPU Dies 0-5 | fpe2 | GPU die temperatures |
| TM0S–TM11S | Memory Slots 0-11 | fpe2 | 12-slot ECC RDIMM |
| TA0P–TA4P | Ambient | fpe2 | Multiple ambient/enclosure sensors |
| TH0F | NVMe Front | fpe2 | T2: NVMe front temperature |
| TH0a | NVMe a | fpe2 | T2: NVMe sensor a |
| TH0b | NVMe b | fpe2 | T2: NVMe sensor b |
| TS0V | SSD Virtual | fpe2 | T2: SSD virtual sensor |
| TPCD | PCH Die | fpe2 | T2: PCH die temperature |
| TPSD | PCH SD | fpe2 | T2: PCH SD temperature |
| TTTD | Thunderbolt TD | fpe2 | T2: Thunderbolt TD |
| TTXD | Thunderbolt XD | fpe2 | T2: Thunderbolt XD |
| TW0P–TW2P | Wireless | fpe2 | WiFi/BT sensors |
| Tp0P | Power Supply Proximity | fpe2 | PSU proximity |
| Tp0C | Power Supply | fpe2 | PSU temperature |
| TV0R | VRM Temperature | fpe2 | Voltage regulator |
| TPMP | Power Supply Proximity | fpe2 | Main PSU proximity |
| Te1P–Te5S | PCIe Slots | fpe2 | PCIe slot ambient/side sensors |
| F0Ac | Exhaust Fan | ui16 | Rear exhaust fan RPM |
| F1Ac–F3Ac | Intake Fans 1-3 | ui16 | Front intake fan RPM |

**Notes:** Intel Xeon W (8–28 cores) · T2 security chip · AMD Radeon Pro W5500X/W5700X/W6900X or Vega II Duo (up to 6 GPU dies across 2 MPX modules) · ECC DDR4 up to 1.5 TB in 12 slots · 8 PCIe slots · 1 exhaust + 3 intake fans

### MacPro6,1 (2013) — Cylinder

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC11C | CPU Cores 0-11 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU 0 Diode | fpe2 | AMD FirePro D300/D500/D700 |
| TG1D | GPU 1 Diode | fpe2 | Second AMD FirePro |
| Tm0P–Tm3P | Memory | fpe2 | 4 DIMM slots |
| TA0P–TA2P | Ambient | fpe2 | Ambient sensors |
| TB0T–TB1T | Thunderbolt | flt | Thunderbolt controller |
| F0Ac–F2Ac | Fans | ui16 | Tri-fan system |

**Notes:** Intel Xeon E5 v3 (Haswell-EP) · dual AMD FirePro · 4× Thunderbolt 3

### MacPro5,1 (2010)

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC11C | CPU Cores 0-11 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU 0 Diode | fpe2 | AMD/NVIDIA GPU |
| TG1D | GPU 1 Diode | fpe2 | Second GPU |
| Tm0P–Tm7P | Memory | fpe2 | 8 DIMM slots |
| TA0P–TA2P | Ambient | fpe2 | Ambient sensors |
| TN0D–TN0P | Northbridge | fpe2 | PCH diode/proximity |
| F0Ac–F2Ac | Fans | ui16 | Tri-fan system |

### MacPro4,1 (Mid 2009)

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC11C | CPU Cores 0-11 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | GPU Diode | fpe2 | ATI Radeon HD 5870 |
| Tm0P–Tm3P | Memory | fpe2 | DDR3 ECC slots |
| TA0P–TA1P | Ambient | fpe2 | Ambient sensors |
| TN0D–TN0P | Northbridge | fpe2 | PCH diode/proximity |
| F0Ac–F2Ac | Fans | ui16 | Tri-fan system |

### MacPro3,1 (2008)

| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C–TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU Diode | fpe2 | ATI Radeon HD 2600 XT |
| TG1D | GPU 2 Diode | fpe2 | NVIDIA GeForce 8800 GT |
| Tm0P–Tm1P | Memory | fpe2 | FB-DIMM slots |
| TA0P–TA1P | Ambient | fpe2 | Ambient sensors |
| F0Ac–F2Ac | Fans | ui16 | Multiple fans |

**Notes:** Nehalem/Westmere · DDR2 FB-DIMM · no Thunderbolt

---

## SMC Key Patterns

### Temperature Sensor Keys

```text
T[C/G/B/A/T/m/h/n/p/s][0-9][C/D/E/F/G/H/P/S/T/V/a/b]
│ │ │ └── Suffix (C=Core, D=Diode, E=Heatsink, F=CoreMax, G=Graphics,
│ │ │              H=Hot, P=Package/Proximity, S=Sensor, T=Die, V=Virtual)
│ │ └──── Index (0-9, or two digits for extended ranges)
│ └────── Group (C=CPU, G=GPU, B=Battery, A=Ambient, m=Memory,
│                 h=Heatsink, N=Northbridge, p=Palm/PSU, s=Trackpad)
└──────── T prefix (Temperature)
```

### Fan Control Keys

```text
F[0-9][Ac/Mn/Mx/Tg/Md/Sf]
│ │ └── Property (Ac=Actual RPM, Mn=Min, Mx=Max, Tg=Target, Md=Mode, Sf=Safety)
│ └──── Fan number (0 = first fan)
└────── F prefix (Fan)
```

---

## Intel Mac Sensor Categories

### T2 Chip Sensors

Present on T2-equipped models (MacBookPro15,x+, Macmini8,1, MacPro7,1, MacBookAir8,x+):

| Key | Description |
|-----|-------------|
| TCSA | CPU System Agent (via T2) |
| TCXC | CPU PECI Cross Domain (via T2) |
| TCaP | CPU Package (via T2) |
| TIED | Intel Embedded Device (T2 controller) |
| TPCD | PCH Die (via T2) |
| TPSD | PCH SD (via T2) |
| TTTD | Thunderbolt TD (via T2) |
| TTXD | Thunderbolt XD (via T2) |
| TH0F / TH0a / TH0b | NVMe die temperatures (via T2) |
| TS0V | SSD virtual sensor (via T2) |

### Thunderbolt Controller Sensors (TB0T–TB3T)

- **TB0T–TB3T**: Thunderbolt controller temperatures (type: flt)
- Present on all Intel Macs with Thunderbolt 3

### Wireless / Airport Sensors

| Key | Description |
|-----|-------------|
| TW0P | Wireless proximity (primary) |
| TW1P | Wireless sensor 2 |
| TW2P | Wireless sensor 3 |

### Palm Rest / Trackpad Sensors

| Key | Description |
|-----|-------------|
| Ts0P | Trackpad / left palm rest |
| Ts1P | Trackpad actuator / right palm rest |

---

## Reading Order (Most to Least Accurate)

1. **Diode** (`TC0D`) — Most accurate CPU temperature
2. **Core** (`TC0C`) — CPU core temperature
3. **PECI** (`TCXC`, `TCSA`) — T2 cross-domain / system agent
4. **Proximity** (`TC0F`) — Proximity sensor (less accurate)
5. **Heatsink** (`TC0E`) — Heatsink temperature

---

## Conversion Examples

### Temperature (fpe2)

```text
Raw: 0x4A00
Decimal: 18944
Celsius: 18944 / 256 = 74.0°C
```

### Temperature (flt)

```text
Raw: 0x421C7AEF  (IEEE 754 float)
Value: 34.1°C
```

### Fan Speed (ui16)

```text
Raw: 0x07D0
Decimal: 2000
RPM: 2000 RPM
```

---

## Safety Thresholds

| Component    | Warning | Critical | Maximum |
|--------------|---------|----------|---------|
| CPU (Intel)  | 80°C    | 95°C     | 105°C   |
| CPU (Xeon W) | 80°C    | 90°C     | 100°C   |
| GPU (AMD)    | 85°C    | 95°C     | 105°C   |
| Memory       | 70°C    | 80°C     | 85°C    |
| Ambient      | 45°C    | 55°C     | 60°C    |

---

*Last Updated: 2026-02-17*
