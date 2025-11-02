# Assembly Line Simulator - REST API

Python FastAPI server providing REST API access to the simulation data.

## Setup

```bash
# Create virtual environment (recommended)
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt
```

## Running

```bash
# Run the API server
python main.py

# Or with uvicorn directly
uvicorn main:app --reload --host 0.0.0.0 --port 8000
```

The API will be available at `http://localhost:8000`

## API Documentation

Once running, visit:
- Swagger UI: http://localhost:8000/docs
- ReDoc: http://localhost:8000/redoc

## Endpoints

### Orders
- `GET /orders` - Get all orders
- `GET /orders/{order_id}` - Get specific order
- `GET /orders/customer/{name}` - Get orders by customer
- `GET /orders/completed` - Get completed orders
- `GET /orders/incomplete` - Get incomplete orders

### Stations
- `GET /stations` - Get all stations with statistics
- `GET /stations/{name}/history` - Get station history

### Statistics
- `GET /stats` - Get simulation statistics

### Simulation
- `POST /simulation/run` - Run the simulation

### WebSocket
- `WS /ws` - Real-time statistics updates

## Environment Variables

- `DB_PATH` - Path to SQLite database (default: `database/assembly_line.db`)

