# Project Organization

All project files are organized within the `Assembly-Line-Simulator/` directory.

## Clean Root Directory

The root contains only essential files:
- `README.md` - Main project documentation
- `Makefile` - Build configuration
- `CMakeLists.txt` - CMake configuration
- `.gitignore` - Git ignore rules

## Directory Structure

```
Assembly-Line-Simulator/
│
├── 📂 include/seneca/          # Public API headers
├── 📂 src/                     # Source code
│   ├── core/                   # Core simulation logic
│   ├── infrastructure/        # Infrastructure services
│   └── main.cpp               # Entry point
│
├── 📂 tests/                   # Test files
├── 📂 config/                  # Configuration files
├── 📂 data/                    # Input data files
├── 📂 docs/                    # All documentation
│   ├── planning/               # Planning documents
│   ├── guides/                 # User/developer guides
│   ├── architecture/           # Architecture docs
│   └── api/                    # API docs (future)
│
├── 📂 build/                   # Build output (gitignored)
├── 📂 logs/                    # Log files (gitignored)
├── 📂 database/                # Database code (future)
├── 📂 api/                     # REST API (future)
├── 📂 frontend/                # Web frontend (future)
├── 📂 lib/                     # Third-party libraries (future)
└── 📂 scripts/                 # Utility scripts
```

## File Organization Rules

### Code Files
- **Headers**: `include/seneca/`
- **Core Sources**: `src/core/`
- **Infrastructure**: `src/infrastructure/`
- **Main**: `src/main.cpp`

### Documentation
- **All docs**: `docs/` folder
  - Planning: `docs/planning/`
  - Guides: `docs/guides/`
  - Architecture: `docs/architecture/`
  - API: `docs/api/`

### Configuration
- Config files: `config/`

### Data
- Input data: `data/`

### Tests
- Test files: `tests/`
  - Unit tests: `tests/unit/`
  - Integration: `tests/integration/`

## Benefits

✅ **Clean root** - Only essential files at top level  
✅ **Organized docs** - All documentation in one place  
✅ **Clear structure** - Easy to find any file  
✅ **Scalable** - Ready for future expansion  
✅ **Professional** - Follows industry standards  

## Finding Files

| Need | Location |
|------|----------|
| Code headers | `include/seneca/` |
| Source code | `src/core/` or `src/infrastructure/` |
| Documentation | `docs/` |
| Configuration | `config/` |
| Data files | `data/` |
| Tests | `tests/` |
| Build output | `build/` (gitignored) |

All project-related files are contained within this directory structure!



