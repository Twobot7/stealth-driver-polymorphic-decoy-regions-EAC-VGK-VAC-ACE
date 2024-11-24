# Advanced Decoy Protection System

A sophisticated anti-tampering and memory protection system using decoy regions and threat detection, integrated with Dear ImGui for visualization.

## ⚠️ Warning

This software is designed for advanced memory protection and anti-debugging purposes. Please note:

1. NO SECURITY GUARANTEE - Do not use this on untrusted systems or in production without thorough testing
2. This system performs low-level memory operations that could potentially crash your system
3. Some features may trigger antivirus software
4. Use at your own risk

## Features

### 1. Decoy Protection System
- Dynamic decoy memory regions generation
- Polymorphic transformations
- Behavioral emulation
- Configurable decoy sizes and counts
- Automatic updates and relocations

### 2. Threat Detection
- Memory scanning detection
- Pattern matching detection
- Suspicious access monitoring
- Integrity violation checks
- Debug detection
- Threat event logging and correlation
- Configurable threat levels and thresholds

### 3. Memory Protection
- Advanced memory region protection
- Memory encryption
- Guard pages implementation
- Memory page shuffling
- Integrity checking

### 4. Visual Debugging Interface
- Real-time decoy monitoring
- Threat visualization
- Memory region inspection
- Interactive controls
- Threat history display

## Installation

1. Clone the repository
2. Build using CMake:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Usage

1. Initialize the loader with configuration:
```c
LOADER_CONFIG config = {
    .DecoyConfig = {
        .MinDecoySize = 1024,
        .MaxDecoySize = 4096,
        .DecoyCount = 10,
        .EnablePolymorphic = TRUE,
        .EnableBehavioralEmulation = TRUE,
        .UpdateInterval = 5000
    },
    .AutoStart = TRUE
};
```

2. Start the protection system:
```c
LOADER_CONTEXT loaderContext = NULL;
if (!InitializeLoader(&config, &loaderContext)) {
    // Handle error
}
```

## Configuration Options

### Decoy Manager Configuration
- MinDecoySize: Minimum size of decoy regions
- MaxDecoySize: Maximum size of decoy regions
- DecoyCount: Number of decoy regions to create
- EnablePolymorphic: Enable polymorphic transformations
- EnableBehavioralEmulation: Enable behavioral emulation
- UpdateInterval: Interval for decoy updates (ms)

### Threat Detection Configuration
- MAX_THREAT_HISTORY: Maximum number of threat events to store
- THREAT_CORRELATION_WINDOW: Time window for correlating threats
- SUSPICIOUS_ACCESS_THRESHOLD: Threshold for suspicious access detection

## Code References

### Main Implementation
- Core initialization and setup
- Memory protection integration
- Decoy system management
- ImGui visualization setup

### Threat Detection System
- Real-time monitoring
- Pattern matching
- Behavioral analysis
- Event correlation

### Decoy Protection
- Dynamic region generation
- Polymorphic transformations
- Memory encryption
- Guard page implementation

## Dependencies

- Dear ImGui (included)
- Windows SDK
- CMake 3.10+
- C/C++ Compiler with C11 support

## License

This project uses multiple components with different licenses:

1. Dear ImGui - MIT License
2. Core Protection System - Proprietary
3. STB Libraries - MIT/Public Domain

## Security Considerations

1. The system performs low-level memory operations
2. Anti-debug features may conflict with development tools
3. Memory encryption may impact performance
4. Some antivirus software may flag the behaviors
5. System crashes possible if memory protection fails

## Contributing

1. Fork the repository
2. Create a feature branch
3. Submit a pull request
4. Ensure all tests pass
5. Follow coding standards

## Support

For issues and support:
1. Check the documentation
2. Review the demo code
3. Submit issues through the issue tracker
4. Read the threat detection logs

## Acknowledgments

- Dear ImGui contributors
- STB library authors
- Security research community
- created by "onbot."
