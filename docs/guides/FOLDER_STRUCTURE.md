# Project Folder Structure

## 📁 Current Organization

```
Assembly-Line-Simulator/
│
├── 📂 include/                 # Public API headers
│   └── seneca/                  # Seneca namespace
│       ├── Station.h
│       ├── Workstation.h
│       ├── CustomerOrder.h
│       ├── LineManager.h
│       ├── Utilities.h
│       ├── Logger.h
│       ├── Config.h
│       └── Exceptions.h
│
├── 📂 src/                      # Source implementation
│   ├── core/                    # Core simulation logic
│   │   ├── Station.cpp
│   │   ├── Workstation.cpp
│   │   ├── CustomerOrder.cpp
│   │   ├── LineManager.cpp
│   │   └── Utilities.cpp
│   ├── infrastructure/          # Infrastructure services
│   │   ├── Logger.cpp
│   │   └── Config.cpp
│   └── main.cpp                 # Application entry point
│
├── 📂 tests/                    # Test files
│   ├── unit/                    # Unit tests (future)
│   ├── integration/             # Integration tests (future)
│   ├── tester_1.cpp
│   ├── tester_2.cpp
│   └── tester_3.cpp
│
├── 📂 config/                   # Configuration files
│   └── config.txt
│
├── 📂 data/                     # Input data files
│   ├── Stations1.txt
│   ├── Stations2.txt
│   ├── CustomerOrders.txt
│   └── AssemblyLine.txt
│
├── 📂 build/                    # Build output (gitignored)
│   └── obj/                     # Object files
│
├── 📂 logs/                     # Log files (gitignored)
│
├── 📂 docs/                     # Documentation
│   ├── architecture/
│   └── api/
│
├── 📂 database/                 # Database code (future)
│   └── schema.sql
│
├── 📂 api/                      # REST API (future)
│   ├── handlers/
│   └── routes/
│
├── 📂 frontend/                 # Web frontend (future)
│   ├── public/
│   ├── src/
│   └── package.json
│
├── 📂 lib/                      # Third-party libraries (future)
│
├── 📂 scripts/                   # Utility scripts
│
├── 📄 Makefile                  # Build configuration
├── 📄 CMakeLists.txt            # CMake configuration
├── 📄 README.md                 # Main documentation
└── 📄 .gitignore                # Git ignore rules
```

## 📋 File Naming Conventions

### Headers
- **Format**: `ClassName.h` (PascalCase)
- **Location**: `include/seneca/`
- **Example**: `Station.h`, `CustomerOrder.h`

### Source Files
- **Format**: `ClassName.cpp` (matches header)
- **Location**: 
  - Core: `src/core/`
  - Infrastructure: `src/infrastructure/`

### Test Files
- **Format**: `test_*.cpp` or `*_test.cpp`
- **Location**: `tests/unit/` or `tests/integration/`

### Configuration Files
- **Format**: `config.txt`, `config.example.txt`
- **Location**: `config/`

### Data Files
- **Format**: `DescriptiveName.txt`
- **Location**: `data/`

## 🎯 Module Organization

### Core Modules (`src/core/`)
Core simulation logic - the business logic:
- `Station.cpp` - Base station class
- `Workstation.cpp` - Workstation implementation
- `CustomerOrder.cpp` - Order management
- `LineManager.cpp` - Assembly line orchestration
- `Utilities.cpp` - Utility functions

### Infrastructure (`src/infrastructure/`)
Supporting infrastructure - cross-cutting concerns:
- `Logger.cpp` - Logging system
- `Config.cpp` - Configuration management

### Future Modules

**Database** (`database/`)
- Schema files
- Migration scripts
- Database utilities

**API** (`api/`)
- `handlers/` - Request handlers
- `routes/` - Route definitions
- Middleware

**Frontend** (`frontend/`)
- React/TypeScript application
- Components, pages, utilities

## 🔍 Finding Files

| Need | Look In |
|------|---------|
| Headers/API | `include/seneca/` |
| Core Logic | `src/core/` |
| Logging/Config | `src/infrastructure/` |
| Tests | `tests/` |
| Config Files | `config/` |
| Data Files | `data/` |
| Documentation | `docs/` |
| Build Output | `build/` (gitignored) |
| Log Files | `logs/` (gitignored) |

## ✅ Benefits of This Structure

1. **Clear Separation**: Headers separate from implementation
2. **Modular**: Easy to find files by purpose
3. **Scalable**: Ready for expansion (API, frontend, database)
4. **Professional**: Follows industry standards
5. **Maintainable**: Logical organization
6. **Build-Friendly**: Makefile handles structure automatically

## 🚀 Adding New Features

When adding new components:

1. **New Core Class** → `src/core/` + `include/seneca/`
2. **New Infrastructure** → `src/infrastructure/` + `include/seneca/`
3. **API Endpoint** → `api/handlers/` or `api/routes/`
4. **Database Code** → `database/`
5. **Frontend Component** → `frontend/src/components/`
6. **Unit Test** → `tests/unit/`
7. **Integration Test** → `tests/integration/`
8. **Documentation** → `docs/`

## 📝 Include Paths

All includes use namespace paths:
```cpp
#include "seneca/Station.h"
#include "seneca/Logger.h"
```

Build system adds `-Iinclude` so paths resolve correctly.

---

**This structure ensures the project stays organized as it grows!** 🎉

