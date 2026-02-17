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

### 16-inch Models (2019)

#### MacBookPro16,1 (2019) - 4 Thunderbolt 3
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC9C | CPU Cores 0-9 | fpe2 | CPU core temperatures (Comet Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TC0F | CPU Proximity | fpe2 | CPU proximity sensor |
| TG0D | GPU Diode | fpe2 | AMD Radeon Pro 5300M |
| TG1D | GPU 2 Diode | fpe2 | AMD Radeon Pro 5500M (optional) |
| Tm0P-Tm1P | Memory | fpe2 | Memory slot temperatures |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TW0P | Wireless Proximity | fpe2 | Airport/WiFi card temperature |
| TB0T-TB3T | Thunderbolt 0-3 | flt | Thunderbolt controller temperatures |
| Th0H-Th1H | Drive | fpe2 | SSD temperatures |
| F0Ac-F1Ac | Left/Right Fans | ui16 | Fan RPM |

**Notes:**
- Intel Core i7/i9 (Comet Lake, 6 cores)
- AMD Radeon Pro 5300M or 5500M GPU
- 4 Thunderbolt 3 ports
- Active cooling with dual fans

#### MacBookPro16,2 (2020) - 2 Thunderbolt 3
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures (Ice Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel Iris Plus graphics |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TB0T-TB1T | Thunderbolt 0-1 | flt | Thunderbolt controller temperatures |
| F0Ac | Fan | ui16 | Single fan RPM |

**Notes:**
- Intel Core i5/i7 (Ice Lake, 4 cores)
- Intel Iris Plus graphics
- 2 Thunderbolt 3 ports
- Single fan cooling

### 13-inch Models (2019-2020)

#### MacBookPro16,3/16,4 (2020)
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel Iris Plus graphics |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TB0T | Battery | fpe2 | Battery temperature |
| F0Ac | Fan | ui16 | Fan RPM |

**Notes:**
- Intel Core i5/i7 (Ice Lake)
- 4 Thunderbolt 3 ports (some models 2)
- Active cooling

---

## MacBook Air (Intel)

### MacBookPro16,2 (2020) - 13-inch
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel Iris Plus graphics |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TB0T | Battery | fpe2 | Battery temperature |
| F0Ac | Fan | ui16 | Fan RPM |

**Notes:**
- Intel Core i3/i5/i7 (Ice Lake)
- 10th generation Intel processors
- Retina display
- Single fan cooling

### MacBookPro13,x (2019) - Earlier 13-inch
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel Iris Plus graphics |
| Tm0P | Memory | fpe2 | Memory temperature |
| TA0P | Ambient | fpe2 | Ambient temperature |
| TB0T | Battery | fpe2 | Battery temperature |
| F0Ac | Fan | ui16 | Fan RPM |

---

## iMac (Intel)

### 27-inch 5K (2019-2020)

#### iMac20,1/iMac20,2 (2020)
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC9C | CPU Cores 0-9 | fpe2 | CPU core temperatures (Comet Lake) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | GPU Diode | fpe2 | AMD Radeon Pro 5300/5500/5700 XT |
| TG1D | GPU 2 Diode | fpe2 | Second AMD GPU (580X/5700 XT) |
| Tm0P-Tm1P | Memory | fpe2 | Memory slots 0-1 |
| TA0P-TA1P | Ambient | fpe2 | Front/back ambient |
| Th0H-Th2H | Drive | fpe2 | HDD/SSD temperatures |
| F0Ac-F1Ac | Fans | ui16 | CPU/GPU fans |
| HDD0 | Hard Drive | fpe2 | 3.5" HDD temperature (Fusion Drive) |

**Notes:**
- Intel Core i5/i7/i9 (10th gen Comet Lake)
- AMD Radeon Pro 5300/5500/5700 XT
- Up to 128GB RAM (user-upgradable)
- 4 Thunderbolt 3 ports

### iMac Pro (2017)

#### iMacPro1,1
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC9C | CPU Cores 0-9 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU Diode | fpe2 | Radeon Pro Vega GPU |
| TG1D | GPU 2 Diode | fpe2 | Second Vega GPU |
| Tm0P-Tm3P | Memory | fpe2 | All 4 DIMM slots |
| TA0P-TA2P | Ambient | fpe2 | Multiple ambient sensors |
| Tp0P | Power Supply | fpe2 | PSU temperature |
| F0Ac-F2Ac | Fans | ui16 | 3 fans (CPU/GPU/PSU) |
| HDD0 | Hard Drive | fpe2 | 3.5" HDD temperature |
| TB0T-TB1T | Thunderbolt | flt | Thunderbolt controller |

**Notes:**
- Intel Xeon W (8-18 cores)
- AMD Radeon Pro Vega 64 or 64X
- ECC RAM (32-128GB)
- 4 Thunderbolt 3 ports

---

## Mac mini (Intel)

### Macmini8,1 (2018)
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC5C | CPU Cores 0-5 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | Intel GPU | fpe2 | Intel UHD 630 |
| Tm0P-Tm1P | Memory | fpe2 | Memory slots 0-1 |
| TA0P | Ambient | fpe2 | Ambient temperature |
| F0Ac | Fan | ui16 | Single fan RPM |

**Notes:**
- Intel Core i3/i5/i7 (Coffee Lake)
- Intel UHD 630 graphics
- Up to 64GB RAM (user-upgradable)
- 4 Thunderbolt 3 ports
- Single fan cooling

---

## Mac Pro (Intel)

### MacPro7,1 (2019) - Tower Design
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC27C | CPU Cores 0-27 | fpe2 | All CPU cores (Xeon W) |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D-TG3D | GPU Diodes | fpe2 | MPX Modules (up to 4) |
| Tm0P-Tm11P | Memory | fpe2 | 12 DIMM slots (ECC DDR4) |
| TA0P-TA5P | Ambient | fpe2 | Multiple ambient sensors |
| Tp0P | Power Supply | fpe2 | 1.4kW PSU temperature |
| TB0T-TB3T | Thunderbolt | flt | Thunderbolt controller temperatures |
| F0Ac-F3Ac | Blower Fans | ui16 | 4 blower fans |
| F4Ac-F6Ac | Aux Fans | ui16 | Memory/PCI/Boost fans |
| HDD0-HDD3 | Drives | fpe2 | SSD/MPX Module temps |

**Notes:**
- Intel Xeon W (8-28 cores)
- AMD Radeon Pro W5700/W680X/W6900X
- ECC DDR4 (up to 768GB user-upgradable)
- 8 PCI Express slots
- MPX Module system
- Rack mountable design

### MacPro6,1 (2013) - Cylinder Design
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC11C | CPU Cores 0-11 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU 0 Diode | fpe2 | AMD FirePro D300/D500/D700 |
| TG1D | GPU 1 Diode | fpe2 | Second AMD FirePro |
| Tm0P-Tm3P | Memory | fpe2 | 4 DIMM slots |
| TA0P-TA2P | Ambient | fpe2 | Ambient sensors |
| TB0T-TB1T | Thunderbolt | flt | Thunderbolt controller |
| F0Ac-F2Ac | Fans | ui16 | Tri-fan system |

**Notes:**
- Intel Xeon E5 v3 (Haswell-EP)
- Dual AMD FirePro GPUs
- 4 Thunderbolt 3 ports
- Black cylinder design

### MacPro5,1 (2010) - Tower
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC11C | CPU Cores 0-11 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU 0 Diode | fpe2 | AMD FirePro D300/D500/D700 |
| TG1D | GPU 1 Diode | fpe2 | Second AMD FirePro |
| Tm0P-Tm7P | Memory | fpe2 | 8 DIMM slots (quad-channel) |
| TA0P-TA2P | Ambient | fpe2 | Ambient sensors |
| TN0D-TN0P | Northbridge | fpe2 | PCH diode/proximity |
| F0Ac-F2Ac | Fans | ui16 | Tri-fan system |

**Notes:**
- Intel Xeon E5 v2 (Ivy Bridge-EP)
- Dual AMD FirePro GPUs
- DDR3 ECC (up to 64GB)
- 6 Thunderbolt 2 ports

### MacPro4,1 (Mid 2009)
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC11C | CPU Cores 0-11 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | GPU Diode | fpe2 | ATI Radeon HD 5870 |
| Tm0P-Tm3P | Memory | fpe2 | DDR3 ECC slots |
| TA0P-TA1P | Ambient | fpe2 | Ambient sensors |
| TN0D-TN0P | Northbridge | fpe2 | PCH diode/proximity |
| F0Ac-F2Ac | Fans | ui16 | Tri-fan system |

**Notes:**
- Intel Xeon E5 (Sandy Bridge-EP)
- ATI Radeon HD 5870
- Server variant available without GPU

### MacPro3,1 (2008)
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TC0E | CPU Heatsink | fpe2 | CPU heatsink temperature |
| TG0D | GPU Diode | fpe2 | ATI Radeon HD 2600 XT |
| TG1D | GPU 2 Diode | fpe2 | NVIDIA GeForce 8800 GT |
| Tm0P-Tm1P | Memory | fpe2 | FB-DIMM slots |
| TA0P-TA1P | Ambient | fpe2 | Ambient sensors |
| F0Ac-F2Ac | Fans | ui16 | Multiple fans |

**Notes:**
- Nehalem/Westmere architecture
- DDR2 FB-DIMM memory
- PCI Express 2.0
- No Thunderbolt support

### MacPro2,1 (Early 2007)
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC3C | CPU Cores 0-3 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | GPU Diode | fpe2 | ATI X1900 XT |
| Tm0P-Tm1P | Memory | fpe2 | FB-DIMM slots |
| TA0P-TA1P | Ambient | fpe2 | Ambient sensors |
| F0Ac-F2Ac | Fans | ui16 | Multiple fans |

**Notes:**
- Harpertown/Penryn architecture
- DDR2 FB-DIMM memory
- No Thunderbolt support

### MacPro1,1 (2006-2007)
| Key | Name | Type | Description |
|-----|------|------|-------------|
| TC0C-TC1C | CPU Cores 0-1 | fpe2 | CPU core temperatures |
| TC0D | CPU Diode | fpe2 | CPU diode temperature |
| TG0D | GPU Diode | fpe2 | GeForce 7300 GT / X1900 XT |
| Tm0P-Tm1P | Memory | fpe2 | FB-DIMM slots |
| TA0P-TA1P | Ambient | fpe2 | Ambient sensors |
| F0Ac-F1Ac | Fans | ui16 | Multiple fans |

**Notes:**
- Woodcrest (65nm) architecture
- DDR2 FB-DIMM ECC
- First Intel Mac Pro

---

## SMC Key Patterns

### Temperature Sensor Keys
```
T[C/G/B/A/T/m/h/n/p/s/g/b][0-9][C/D/E/F/G/H/P/S/T/c/d/p/r]
│ │ │ │ │ │ │ │ │ │ │ └─ Type (C=Core, D=Diode, E=Heatsink, F=Prox, G=Graphics, H=Hot, P=Package, S=Sensor, T=Temp)
│ │ │ │ │ │ │ │ │ │ │ └── Number (0-9)
│ │ │ │ │ │ │ │ │ └───── Sensor Group (optional)
│ │ │ │ │ │ │ └────────── Second character (varies)
│ │ │ │ └─────────────────── First character (C=CPU, G=GPU, B=Battery, A=Ambient, etc.)
│ └─┴─┴────────────────────── T prefix (Temperature)
└──────────────────────────── Key name
```

### Fan Control Keys
```
F[0-9][Ac/Mn/Mx/Tg/Md/Sf]
│ │ └────────────────────────── Property (Ac=Actual, Mn=Min, Mx=Max, Tg=Target, Md=Mode, Sf=Safety)
│ └────────────────────────────── Fan Number (0-9)
└────────────────────────────── F prefix (Fan)
```

---

## Intel Mac Sensor Categories

### Thunderbolt Sensors (TB0T-TB3T)
- **TB0T**: Thunderbolt controller 0 temperature
- **TB1T**: Thunderbolt controller 1 temperature
- **TB2T**: Thunderbolt controller 2 temperature (if present)
- **TB3T**: Thunderbolt controller 3 temperature (if present)
- **Type**: flt (floating point)
- **Present on**: All Intel Macs with Thunderbolt 3

### Thermal Diode Array (TD00-TD24)
- **TD00-TD04**: Case diodes zone 0
- **TD10-TD14**: Case diodes zone 1
- **TD20-TD24**: Case diodes zone 2
- **TDBP**: Diode B Proximity
- **TDEL**: Diode E Left
- **TDER**: Diode E Right
- **Type**: flt (floating point)
- **Present on**: iMac Pro, Mac Pro

### Wireless/Airport Sensors (TW0P)
- **TW0P**: Wireless Proximity (Airport/WiFi card)
- **TW0T**: Wireless Temperature
- **TWBA**: Airport B
- **TWPA**: Airport P
- **Type**: fpe2
- **Present on**: Intel Macs with Airport/WiFi

### Palm Rest Sensors (Tp0P-Tp1P)
- **Tp0P**: Palm 0 Proximity (Left palm rest)
- **Tp1P**: Palm 1 Proximity (Right palm rest)
- **Type**: fpe2
- **Present on**: MacBook Pro with Touch Bar

---

## Reading Order (Most to Least Accurate)

1. **Diode** (`TC0D`) - Most accurate CPU temperature
2. **Core** (`TC0C`) - CPU core temperature
3. **Proximity** (`TC0F`) - Proximity sensor (less accurate)
4. **Heatsink** (`TC0E`) - Heatsink temperature

---

## Conversion Examples

### Temperature (fpe2)
```
Raw: 0x4A00
Decimal: 18944
Celsius: 18944 / 256 = 74.0°C
```

### Temperature (flt)
```
Raw: 0x421C7AEF (IEEE 754 float)
Value: 34.1°C
```

### Fan Speed (ui16)
```
Raw: 0x07D0
Decimal: 2000
RPM: 2000 RPM
```

---

## Safety Thresholds

| Component | Warning | Critical | Maximum |
|-----------|---------|----------|---------|
| CPU (Xeon) | 80°C | 90°C | 100°C |
| GPU (AMD) | 85°C | 95°C | 100°C |
| Memory | 70°C | 80°C | 85°C |
| Ambient | 45°C | 55°C | 60°C |

---

*Last Updated: 2025-02-16*
