# Assembly Line Simulator - Hybrid Architecture

This project uses a hybrid architecture combining:
- **C++ Core**: High-performance simulation engine
- **Python API**: FastAPI REST server for easy API development
- **React Frontend**: Modern web dashboard

## Architecture Overview

```
┌─────────────────────────────────┐
│   React Frontend (Port 3000)   │
│   - Dashboard                   │
│   - Order Management            │
│   - Station Monitoring          │
│   - Statistics & Charts        │
└──────────────┬──────────────────┘
               │ HTTP/WebSocket
┌──────────────▼──────────────────┐
│   Python FastAPI (Port 8000)    │
│   - REST API                    │
│   - Database Access             │
│   - WebSocket Updates           │
└──────────────┬──────────────────┘
               │ SQLite + Subprocess
┌──────────────▼──────────────────┐
│   C++ Simulation Core           │
│   - Assembly Line Logic         │
│   - Order Processing            │
│   - SQLite Database             │
└─────────────────────────────────┘
```

## Quick Start

### 1. Build C++ Core

```bash
make release
```

This creates the simulation executable at `build/assembly_line`.

### 2. Run Simulation (to populate database)

```bash
./build/assembly_line data/Stations1.txt data/Stations2.txt data/CustomerOrders.txt data/AssemblyLine.txt
```

### 3. Start Python API

```bash
cd api
python3 -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate
pip install -r requirements.txt
python main.py
```

API will be available at `http://localhost:8000`

### 4. Start React Frontend

```bash
cd frontend
npm install
npm run dev
```

Frontend will be available at `http://localhost:3000`

## Project Structure

```
Assembly-Line-Simulator/
├── src/                    # C++ source code
│   ├── core/              # Simulation logic
│   └── infrastructure/    # Database, logging
├── api/                    # Python FastAPI
│   ├── main.py           # API server
│   └── requirements.txt
├── frontend/               # React app
│   ├── src/
│   │   ├── components/   # React components
│   │   └── App.tsx
│   └── package.json
├── build/                  # Compiled C++ binaries
├── database/               # SQLite database
└── data/                   # Input data files
```

## API Endpoints

- `GET /` - API information
- `GET /orders` - Get all orders
- `GET /orders/completed` - Get completed orders
- `GET /orders/incomplete` - Get incomplete orders
- `GET /stations` - Get station statistics
- `GET /stats` - Get overall statistics
- `POST /simulation/run` - Run simulation
- `WS /ws` - WebSocket for real-time updates

Full API documentation: http://localhost:8000/docs

## Why This Architecture?

1. **C++ Core**: Performance-critical simulation logic stays fast
2. **Python API**: Rapid development, modern ecosystem, easy maintenance
3. **React Frontend**: Modern UI, great developer experience
4. **Industry Standard**: Many companies use this hybrid pattern (Spotify, Netflix, Dropbox)

## Development Workflow

1. Modify C++ simulation code → Rebuild with `make release`
2. Modify Python API → Changes apply immediately (with auto-reload)
3. Modify React frontend → Hot reload enabled

## Benefits

✅ Best performance where it matters (simulation)  
✅ Fast API development (Python)  
✅ Modern web interface (React)  
✅ Easy to maintain and extend  
✅ Great portfolio piece showing full-stack skills  

## Troubleshooting

**Database not found**: Run the simulation first to create the database

**API can't connect to database**: Check `DB_PATH` environment variable or ensure database exists

**Frontend can't connect to API**: Ensure API is running on port 8000 and CORS is enabled

