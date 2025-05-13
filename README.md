# esp-screen-lib

**esp-screen-lib** is a modular display driver and rendering library for ESP32-based devices, designed to support a wide range of TFT and LCD panels. Built with ESP-IDF, it aims to provide a flexible and efficient framework for handling screen drivers, SPI transport layers, and high-performance rendering workflows.

## Key Features (Planned)
- Unified transport abstraction for SPI-based displays
- Modular driver system for different LCD/TFT controllers (e.g., ILI9341, ST7789)
- Support for command scripts for easy driver customization
- Optimized partial and full-screen updates using DMA transfers
- Integration with layout/UI libraries like Clay UI
- Platform-agnostic APIs for easy integration into ESP32 projects

## Status
> **🚧 This project is under active development.**
> 
> Features are incomplete and APIs are subject to change. Expect breaking changes and incomplete documentation. Contributions and testing feedback are welcome.

## Getting Started
The project is being structured as an ESP-IDF component. Basic usage instructions and examples will be added as development progresses.

For now, you can clone the repository and include it in your ESP-IDF project:

```bash
git clone https://github.com/yourusername/esp-screen-lib.git
```

## Repository Structure

esp-screen-lib/
├── include/        # Public headers (user-facing API)
├── src/            # Internal source files
├── private_include/# Private headers (internal only)
├── examples/       # Example applications (coming soon)
├── components.yml  # ESP-IDF component configuration
└── README.md       # This file

## Roadmap

- [ ] Basic SPI transport layer abstraction
- [ ] Initial ILI9341 and ST7789 driver implementations
- [ ] Command script framework for driver initialization
- [ ] DMA-based rendering pipeline
- [ ] Clay UI integration demo
- [ ] Documentation and usage examples

## License

This project will be released under the MIT License (to be finalized).