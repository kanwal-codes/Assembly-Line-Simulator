"""
Assembly Line Simulator - REST API Server

PURPOSE:
This Python FastAPI server provides REST API access to simulation data stored in SQLite.
It acts as a bridge between the C++ simulation (which writes to database) and the React
frontend (which displays the data).

ARCHITECTURE:
┌─────────────┐      ┌──────────────┐      ┌─────────────┐
│   C++ Sim   │─────▶│   SQLite DB  │◀─────│  FastAPI    │
│  (writes)   │      │              │      │  (reads)    │
└─────────────┘      └──────────────┘      └──────┬──────┘
                                                    │
                                                    ▼
                                            ┌─────────────┐
                                            │  React UI    │
                                            │  (displays)  │
                                            └─────────────┘

FLOW:
1. C++ simulation runs and saves data to SQLite database
2. Frontend makes HTTP requests to this API
3. API queries database and returns JSON responses
4. Frontend renders the data in the UI

ENDPOINTS:
- GET /health - Health check (used by Fly.io)
- GET /stats - Overall statistics
- GET /orders - All orders
- GET /orders/completed - Completed orders only
- GET /orders/incomplete - Incomplete orders only
- GET /stations - Station data
- POST /simulation/run - Trigger C++ simulation
- WS /ws - WebSocket for real-time updates

CONNECTIONS:
- Reads from: database/assembly_line.db (created by C++ simulation)
- Serves to: React frontend (via HTTP/WebSocket)
- Executes: build/assembly_line (C++ executable) when simulation is triggered
"""

from fastapi import FastAPI, HTTPException, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from pydantic import BaseModel
from typing import List, Optional
import sqlite3
import json
import os
from pathlib import Path
from datetime import datetime
import subprocess
import asyncio

# ====================================================================
# FastAPI Application Initialization
# ====================================================================
# This creates the FastAPI app instance that handles all HTTP requests
# - Title and description appear in auto-generated API docs (/docs endpoint)
# - Version helps track API changes
app = FastAPI(
    title="Assembly Line Simulator API",
    description="REST API for Assembly Line Simulation System",
    version="1.0.0"
)

# ====================================================================
# CORS (Cross-Origin Resource Sharing) Configuration
# ====================================================================
# PURPOSE: Allows frontend (hosted on Vercel) to make requests to this API
# 
# WHY NEEDED:
# - Frontend runs on a different domain (vercel.app)
# - Browsers block cross-origin requests by default (security)
# - CORS headers tell browser "it's OK to allow this request"
# 
# CONFIGURATION:
# - If CORS_ORIGINS env var is set: Use specific origins (allows credentials)
# - If not set: Allow all origins (works for Vercel's dynamic domains)
# - When using ["*"], credentials MUST be False (browser security requirement)
# 
# TRIGGERS:
# - Every HTTP request from frontend includes CORS headers
# - Browser checks these headers before allowing the request
# 
# CONNECTIONS:
# - Frontend: React app makes requests to this API
# - Browser: Checks CORS headers before allowing request
cors_origins_env = os.getenv("CORS_ORIGINS", "").strip()

if cors_origins_env:
    # Specific origins mode (for production with known domains)
    # - Allows credentials (cookies, auth headers)
    # - More secure but requires knowing frontend URL
    cors_origins = [origin.strip() for origin in cors_origins_env.split(",") if origin.strip()]
    app.add_middleware(
        CORSMiddleware,
        allow_origins=cors_origins,
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )
else:
    # Wildcard mode (for Vercel's dynamic preview domains)
    # - Works with any origin (useful for preview deployments)
    # - Cannot use credentials (browser security requirement)
    # - This is the default for production deployment
    app.add_middleware(
        CORSMiddleware,
        allow_origins=["*"],
        allow_credentials=False,  # Required when using allow_origins=["*"]
        allow_methods=["*"],
        allow_headers=["*"],
    )

# ====================================================================
# Database Configuration
# ====================================================================
# PURPOSE: Locate the SQLite database file created by C++ simulation
# 
# PATH RESOLUTION:
# - Default: database/assembly_line.db (relative to project root)
# - Can be overridden via DB_PATH environment variable (used in Fly.io)
# - Path is resolved to absolute path for consistency
# 
# CONNECTIONS:
# - Created by: C++ simulation (src/main.cpp) writes data here
# - Read by: This API serves data from here
# - Shared: Both C++ and Python use the same database file
# 
# TRIGGERS:
# - All endpoints query this database
# - Database is created on first simulation run
_project_root = Path(__file__).parent.parent
DB_PATH = os.getenv("DB_PATH", str(_project_root / "database" / "assembly_line.db"))

# ====================================================================
# Pydantic Data Models
# ====================================================================
# PURPOSE: Define the structure of data sent/received by API endpoints
# 
# WHY PYDANTIC:
# - Automatic validation of request/response data
# - Type checking and conversion
# - Auto-generated API documentation
# - Prevents invalid data from reaching endpoints
# 
# USAGE:
# - Request bodies: FastAPI validates incoming JSON against these models
# - Response bodies: FastAPI serializes data to JSON using these models
# - Type safety: IDE can autocomplete and catch errors

class OrderRecord(BaseModel):
    """Represents a single order in the database"""
    order_id: str
    customer_name: str
    product: str
    is_completed: bool
    total_items: int
    filled_items: int
    created_at: str
    completed_at: Optional[str] = None

class StationRecord(BaseModel):
    """Represents a station's status at a point in time"""
    station_name: str
    items_processed: int
    inventory_remaining: int
    timestamp: str

class SimulationStats(BaseModel):
    """Overall statistics about the simulation"""
    total_orders: int
    completed_orders: int
    incomplete_orders: int
    completion_rate: float
    most_active_station: Optional[str] = None

class SimulationRequest(BaseModel):
    """Request body for running a simulation"""
    stations_file_1: str = "data/Stations1.txt"
    stations_file_2: str = "data/Stations2.txt"
    customer_orders_file: str = "data/CustomerOrders.txt"
    assembly_line_file: str = "data/AssemblyLine.txt"

# ====================================================================
# Helper Functions
# ====================================================================

def get_db_connection():
    """
    Get SQLite database connection
    
    PURPOSE:
    Creates a connection to the SQLite database for querying data.
    
    HOW IT WORKS:
    1. Checks if database file exists
    2. If not found, raises HTTPException (404)
    3. If found, creates and returns connection
    
    TRIGGERS:
    - Called by all endpoints that need to query the database
    - Database is created by C++ simulation on first run
    
    CONNECTIONS:
    - Database file: Created by C++ simulation (src/main.cpp)
    - Used by: All GET endpoints that return data
    
    @return sqlite3.Connection object
    @raises HTTPException if database file doesn't exist
    """
    db_file = Path(DB_PATH)
    if not db_file.exists():
        raise HTTPException(
            status_code=404, 
            detail=f"Database not found at {db_file}. Run simulation first to create database."
        )
    return sqlite3.connect(str(db_file))

def row_to_order(row):
    """Convert database row to OrderRecord"""
    return OrderRecord(
        order_id=row[1],
        customer_name=row[2],
        product=row[3],
        is_completed=bool(row[4]),
        total_items=row[5],
        filled_items=row[6],
        created_at=row[7],
        completed_at=row[8] if len(row) > 8 else None
    )

# API Endpoints

@app.get("/")
async def root():
    """API root endpoint"""
    return {
        "message": "Assembly Line Simulator API",
        "version": "1.0.0",
        "endpoints": {
            "orders": "/orders",
            "orders_by_customer": "/orders/customer/{name}",
            "completed_orders": "/orders/completed",
            "incomplete_orders": "/orders/incomplete",
            "stations": "/stations",
            "stats": "/stats",
            "simulation": "/simulation"
        }
    }

@app.get("/health")
async def health_check():
    """
    Health check endpoint
    
    PURPOSE:
    Used by Fly.io (and other platforms) to verify the API is running correctly.
    This endpoint is called periodically to ensure the service is healthy.
    
    HOW IT WORKS:
    1. Checks if database file exists
    2. If exists, tries to connect (verifies database is accessible)
    3. Returns status indicating health
    
    TRIGGERS:
    - Fly.io health checks (every 10 seconds based on fly.toml)
    - Manual checks: curl https://assembly-line-simulator.fly.dev/health
    - Frontend can check this to verify backend is available
    
    CONNECTIONS:
    - Fly.io: Uses this to determine if app should be restarted
    - Frontend: Can call this to check backend availability
    
    @return JSON with status and database connection state
    """
    try:
        db_file = Path(DB_PATH)
        if db_file.exists():
            conn = get_db_connection()
            conn.close()
            return {"status": "healthy", "database": "connected"}
        else:
            # Database doesn't exist yet, but API is running - this is OK
            # Database will be created when first simulation runs
            return {"status": "healthy", "database": "not_initialized", "message": "Database will be created on first simulation run"}
    except Exception as e:
        return {"status": "unhealthy", "error": str(e)}

# Order endpoints
@app.get("/orders", response_model=List[OrderRecord])
async def get_orders(limit: int = 100):
    """Get order history"""
    try:
        db_file = Path(DB_PATH)
        if not db_file.exists():
            return []  # Return empty list if database doesn't exist yet
        
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders ORDER BY created_at DESC LIMIT ?",
            (limit,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Specific routes must come before parameterized routes to avoid route conflicts
@app.get("/orders/completed", response_model=List[OrderRecord])
async def get_completed_orders(limit: int = 100):
    """Get completed orders"""
    try:
        db_file = Path(DB_PATH)
        if not db_file.exists():
            return []  # Return empty list if database doesn't exist yet
        
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders WHERE is_completed = 1 ORDER BY completed_at DESC LIMIT ?",
            (limit,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/orders/incomplete", response_model=List[OrderRecord])
async def get_incomplete_orders(limit: int = 100):
    """Get incomplete orders"""
    try:
        db_file = Path(DB_PATH)
        if not db_file.exists():
            return []  # Return empty list if database doesn't exist yet
        
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders WHERE is_completed = 0 ORDER BY created_at DESC LIMIT ?",
            (limit,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/orders/customer/{customer_name}", response_model=List[OrderRecord])
async def get_orders_by_customer(customer_name: str):
    """Get orders for a specific customer"""
    try:
        db_file = Path(DB_PATH)
        if not db_file.exists():
            return []  # Return empty list if database doesn't exist yet
        
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders WHERE customer_name = ? ORDER BY created_at DESC",
            (customer_name,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/orders/{order_id}", response_model=OrderRecord)
async def get_order(order_id: str):
    """Get specific order by ID"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute("SELECT * FROM orders WHERE order_id = ?", (order_id,))
        row = cursor.fetchone()
        conn.close()
        if not row:
            raise HTTPException(status_code=404, detail="Order not found")
        return row_to_order(row)
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Station endpoints
@app.get("/stations", response_model=List[StationRecord])
async def get_stations(limit: int = 100):
    """Get station history"""
    try:
        db_file = Path(DB_PATH)
        if not db_file.exists():
            return []  # Return empty list if database doesn't exist yet
        
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            """SELECT station_name, SUM(items_processed) as total_processed,
               MAX(inventory_remaining) as current_inventory,
               MAX(timestamp) as last_update
               FROM station_history
               GROUP BY station_name
               ORDER BY total_processed DESC
               LIMIT ?""",
            (limit,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [
            StationRecord(
                station_name=row[0],
                items_processed=row[1] or 0,
                inventory_remaining=row[2] or 0,
                timestamp=row[3] or ""
            )
            for row in rows
        ]
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/stations/{station_name}/history", response_model=List[StationRecord])
async def get_station_history(station_name: str, limit: int = 100):
    """Get history for a specific station"""
    try:
        db_file = Path(DB_PATH)
        if not db_file.exists():
            return []  # Return empty list if database doesn't exist yet
        
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM station_history WHERE station_name = ? ORDER BY timestamp DESC LIMIT ?",
            (station_name, limit)
        )
        rows = cursor.fetchall()
        conn.close()
        return [
            StationRecord(
                station_name=row[1],
                items_processed=row[2],
                inventory_remaining=row[3],
                timestamp=row[4]
            )
            for row in rows
        ]
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Statistics endpoint
@app.get("/stats", response_model=SimulationStats)
async def get_statistics():
    """Get simulation statistics"""
    try:
        db_file = Path(DB_PATH)
        if not db_file.exists():
            # Return empty stats if database doesn't exist yet
            return SimulationStats(
                total_orders=0,
                completed_orders=0,
                incomplete_orders=0,
                completion_rate=0.0,
                most_active_station=None
            )
        
        conn = get_db_connection()
        cursor = conn.cursor()
        
        # Total orders
        cursor.execute("SELECT COUNT(*) FROM orders")
        total = cursor.fetchone()[0]
        
        # Completed orders
        cursor.execute("SELECT COUNT(*) FROM orders WHERE is_completed = 1")
        completed = cursor.fetchone()[0]
        
        incomplete = total - completed
        completion_rate = (completed / total * 100) if total > 0 else 0.0
        
        # Most active station
        cursor.execute("""
            SELECT station_name, SUM(items_processed) as total
            FROM station_history
            GROUP BY station_name
            ORDER BY total DESC
            LIMIT 1
        """)
        most_active = cursor.fetchone()
        most_active_name = most_active[0] if most_active else None
        
        conn.close()
        
        return SimulationStats(
            total_orders=total,
            completed_orders=completed,
            incomplete_orders=incomplete,
            completion_rate=round(completion_rate, 2),
            most_active_station=most_active_name
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Simulation control endpoint
@app.post("/simulation/run")
async def run_simulation(request: SimulationRequest = SimulationRequest()):
    """
    Run the simulation (executes the C++ program)
    
    PURPOSE:
    Triggers the C++ simulation executable, which processes orders and saves
    results to the database. This is the main action endpoint.
    
    FLOW:
    1. Locates the C++ executable (build/assembly_line)
    2. Prepares data file paths
    3. Executes the C++ program with subprocess
    4. C++ program runs simulation and saves to database
    5. Returns success/failure status
    
    HOW IT WORKS:
    - Uses subprocess.run() to execute the C++ binary
    - Passes data file paths as command-line arguments
    - Captures stdout/stderr for error reporting
    - C++ program writes results directly to SQLite database
    
    TRIGGERS:
    - Called when user clicks "Run Simulation" button in frontend
    - Frontend: Dashboard.tsx -> POST /simulation/run
    - This triggers: C++ executable runs -> Database gets populated
    
    CONNECTIONS:
    - Frontend: Dashboard component calls this endpoint
    - C++ Executable: build/assembly_line (created by make/cmake)
    - Database: C++ program writes results here
    - Data Files: data/Stations1.txt, Stations2.txt, etc.
    
    @param request SimulationRequest with data file paths (optional, has defaults)
    @return JSON with status and simulation output
    @raises HTTPException if executable not found or simulation fails
    """
    try:
        # Build path to executable
        # Executable is created by: make release or cmake build
        exe_path = Path(__file__).parent.parent / "build" / "assembly_line"
        if not exe_path.exists():
            raise HTTPException(
                status_code=404,
                detail="Simulation executable not found. Please build the project first."
            )
        
        # Prepare data file paths
        # Extract just filename from path (security: prevent path traversal)
        data_dir = Path(__file__).parent.parent / "data"
        stations1 = data_dir / request.stations_file_1.split("/")[-1]
        stations2 = data_dir / request.stations_file_2.split("/")[-1]
        orders = data_dir / request.customer_orders_file.split("/")[-1]
        assembly = data_dir / request.assembly_line_file.split("/")[-1]
        
        # Run simulation
        # This executes the C++ program which:
        # 1. Loads stations and orders from data files
        # 2. Processes orders through assembly line
        # 3. Saves results to database
        result = subprocess.run(
            [str(exe_path), str(stations1), str(stations2), str(orders), str(assembly)],
            capture_output=True,
            text=True,
            cwd=exe_path.parent
        )
        
        if result.returncode != 0:
            raise HTTPException(
                status_code=500,
                detail=f"Simulation failed: {result.stderr}"
            )
        
        return {
            "status": "success",
            "message": "Simulation completed successfully",
            "output": result.stdout
        }
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# ====================================================================
# WebSocket Endpoint for Real-Time Updates
# ====================================================================
@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    """
    WebSocket endpoint for real-time updates
    
    PURPOSE:
    Provides a persistent connection for real-time data updates without polling.
    Frontend can receive live statistics updates as they change.
    
    HOW IT WORKS:
    1. Client connects via WebSocket (ws:// or wss://)
    2. Server accepts connection
    3. Server sends statistics every second
    4. Connection stays open until client disconnects
    
    TRIGGERS:
    - Frontend: Dashboard.tsx connects to this on component mount
    - Sends updates: Every 1 second automatically
    - Disconnects: When frontend component unmounts or page closes
    
    CONNECTIONS:
    - Frontend: Dashboard component connects to ws://host/ws
    - URL Conversion: http -> ws, https -> wss (handled in frontend)
    - Data: Sends SimulationStats JSON every second
    
    BENEFITS:
    - Real-time updates without constant HTTP polling
    - Lower latency than polling
    - Efficient: Single connection vs many HTTP requests
    
    NOTE:
    - If WebSocket fails, frontend falls back to HTTP polling
    - This is optional - app works fine without WebSocket
    """
    await websocket.accept()
    try:
        while True:
            # Send statistics every second
            # This keeps the frontend dashboard updated in real-time
            stats = await get_statistics()
            await websocket.send_json(stats.dict())
            await asyncio.sleep(1)
    except WebSocketDisconnect:
        # Client disconnected - this is normal, just exit
        pass

if __name__ == "__main__":
    import uvicorn
    port = int(os.getenv("PORT", "8000"))
    uvicorn.run(app, host="0.0.0.0", port=port)

