# Project Structure

## Recommended Folder Organization

```
Assembly-Line-Simulator/
├── include/                    # Public headers (API)
│   └── seneca/
│       ├── Station.h
│       ├── Workstation.h
│       ├── CustomerOrder.h
│       ├── LineManager.h
│       ├── Utilities.h
│       ├── Logger.h
│       ├── Config.h
│       └── Exceptions.h
│
├── src/                        # Source files organized by module
│   ├── core/                   # Core simulation logic
│   │   ├── Station.cpp
│   │   ├── Workstation.cpp
│   │   ├── CustomerOrder.cpp
│   │   ├── LineManager.cpp
│   │   └── Utilities.cpp
│   ├── infrastructure/         # Infrastructure (logging, config)
│   │   ├── Logger.cpp
│   │   └── Config.cpp
│   └── main.cpp               # Application entry point
│
├── lib/                        # Third-party libraries (future)
│   └── README.md
│
├── tests/                      # Test files
│   ├── unit/                   # Unit tests
│   ├── integration/           # Integration tests
│   └── tester_*.cpp           # Existing testers
│
├── database/                 # Database files (future)
│   └── schema.sql
│
├── api/                       # REST API (future)
│   ├── handlers/
│   └── routes/
│
├── frontend/                   # Web frontend (future)
│   ├── public/
│   ├── src/
│   └── package.json
│
├── config/                     # Configuration files
│   ├── config.txt
│   └── config.example.txt
│
├── data/                       # Data files
│   ├── Stations1.txt
│   ├── Stations2.txt
│   ├── CustomerOrders.txt
│   └── AssemblyLine.txt
│
├── docs/                       # Documentation
│   ├── architecture/
│   └── api/
│
├── scripts/                    # Utility scripts
│   └── setup.sh
│
├── build/                      # Build output (gitignored)
├── logs/                       # Log files (gitignored)
│
├── CMakeLists.txt
├── Makefile
├── README.md
└── .gitignore
```

