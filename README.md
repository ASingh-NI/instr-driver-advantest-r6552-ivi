# Advantest R6552 IVI-C Instrument Driver

IVI-C specific instrument driver (IviDmm class) for the Advantest R6552 series digital multimeters.

| | |
|---|---|
| **Driver type** | IVI-C |
| **Source folder** | [`avr6552/`](avr6552/) |
| **Prefix** | `avr6552` |
| **Manufacturer** | Advantest |
| **IVI instrument class** | IviDmm (class specification 3.0) |
| **Driver version** | 3.0 |
| **Driver vendor** | National Instruments |
| **Certification** | NI Certified |

## Supported Instruments

- Advantest R6552
- Advantest R6552T
- Advantest R6552T-R

## Supported Interfaces

- GPIB

## IviDmm Capabilities

| Group capability | Supported |
|---|---|
| IviDmmBase | Yes |
| IviDmmACMeasurement | Yes |
| IviDmmFrequencyMeasurement | Yes |
| IviDmmAutoZero | Yes |
| PowerLineFrequency, TemperatureMeasurement, Thermocouple, ResistanceTemperatureDevice, Thermistor, MultiPoint, TriggerSlope, SoftwareTrigger, DeviceInfo, AutoRangeValue | No |

Optional features: interchangeability checking, state caching, and coercion recording.

Instrument-specific functions include burst/multi-point fetch, compare, decibel, hold,
max/min, null, scale, smooth, sample mode/rate, and ripple-volts filter configuration.

## Requirements

- LabWindows/CVI (to build the driver from source)
- IVI Compliance Package 4.6.1 or later (IVI Shared Components)
- [NI-VISA](https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html) 5.2 or later
- Windows, 32-bit or 64-bit
- LabVIEW, only if you use the LabVIEW wrapper in `labview/`

## Repository Layout

```
avr6552/
├── avr6552.c / .h          Driver source and header
├── avr6552.fp / .sub       Function panels and attribute definitions
├── avr6552.prj             LabWindows/CVI project
├── avr6552.hhp, .html,     Help project and generated function reference
│   avr6552_files/
├── config/avr6552.xml      IVI Configuration Store entry (models R6552, R6552T, R6552T-R)
├── documentation/          IVI compliance document (avr6552.txt)
├── example/                C example (avr6552_example.c)
├── labview/                LabVIEW wrapper (avr6552.llb, .rc, palette .mnu files)
└── test/                   Unit test UI and C++ wrapper tests
```

## Getting Started

1. Install LabWindows/CVI, the IVI Compliance Package, and NI-VISA.
2. Clone this repository, or download it as a ZIP.
3. Open [`avr6552/avr6552.prj`](avr6552/avr6552.prj) in LabWindows/CVI and build it
   (32-bit produces `avr6552_32.dll`, 64-bit produces `avr6552_64.dll`).
4. Connect the multimeter over GPIB and find its resource name (e.g. `GPIB0::1::INSTR`)
   in NI MAX.
5. Call `avr6552_InitWithOptions` with that resource name, or with a logical name
   configured in NI MAX. Pass `"Simulate=1"` in the option string to try the driver
   without hardware. See [`avr6552/example/avr6552_example.c`](avr6552/example/avr6552_example.c).

## Known Issues

- None.

## Support

Report driver bugs and feature requests through [GitHub Issues](https://github.com/ASingh-NI/instr-driver-advantest-r6552-ivi/issues).

## Contributing

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

## Security

See [SECURITY.md](SECURITY.md). Do not report security vulnerabilities through public GitHub issues.

## License

See [LICENSE.md](LICENSE.md).
