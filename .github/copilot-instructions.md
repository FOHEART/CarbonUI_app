# MyUI - GitHub Copilot Instructions

## Project Overview
 a Qt-based desktop application . It's a cross-platform tool (Linux, macOS, Windows).
### Key Technologies
- **Language**: C++ (C++17 standard)
- **GUI Framework**: Qt5 (Widgets, OpenGL, WebSockets, SVG, PrintSupport, Concurrent, Xml, Network)
- **Build System**: CMake (minimum version 3.16)
- **Scripting**: 
- **Additional Libraries**: 
- **ROS Support**: 


## Coding Standards

### Code Style
- **Style Guide**: Based on Google C++ Style Guide with customizations
- **Formatter**: clang-format (configuration in `.clang-format`)
- **Line Length**: 100 characters maximum
- **Indentation**: 4 spaces (no tabs)
- **Braces**: Custom style with braces on new lines for classes, functions, structs, enums, namespaces
- **Naming Conventions**:
  - Classes: PascalCase (e.g., `PlotWidget`, `DataLoader`)
  - Functions/Methods: camelCase (e.g., `getData()`, `transformData()`)
  - Member variables: Use underscore suffix convention where applicable
  - Pointers: `Type* pointer` (pointer binds to type)
  
  ### File Headers
All source files should include the MPL-2.0 license header:
```cpp
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
```


## Important Notes

### Dependencies Management
- Use system packages when possible (apt, brew, etc.)
- Conan or vcpkg for Windows builds
- CMake's FetchContent for some third-party libraries
- Git submodules avoided (empty .gitmodules file)


### Qt Specifics
- Uses Qt5 (not Qt6)
- MOC (Meta-Object Compiler) enabled via `CMAKE_AUTOMOC`
- UIC (User Interface Compiler) enabled via `CMAKE_AUTOUIC`
- Resource files compiled automatically



### Cross-Platform Considerations
- Windows-specific: `WIN32` flag, special handling for executables
- macOS-specific: Homebrew Qt paths, RPATH settings
- Linux-specific: Address Sanitizer support, backward-cpp for stack traces
