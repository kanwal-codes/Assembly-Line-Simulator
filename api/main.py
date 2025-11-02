"""
Assembly Line Simulator - REST API Server
Python FastAPI server that provides REST API access to the simulation data
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

# Initialize FastAPI app
app = FastAPI(
    title="Assembly Line Simulator API",
    description="REST API for Assembly Line Simulation System",
    version="1.0.0"
)

# CORS middleware for frontend
# Allow origins from environment variable or default to localhost for development
cors_origins = os.getenv("CORS_ORIGINS", "http://localhost:3000,http://localhost:5173").split(",")
app.add_middleware(
    CORSMiddleware,
    allow_origins=cors_origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Database path - use absolute path from project root
_project_root = Path(__file__).parent.parent
DB_PATH = os.getenv("DB_PATH", str(_project_root / "database" / "assembly_line.db"))

# Pydantic models
class OrderRecord(BaseModel):
    order_id: str
    customer_name: str
    product: str
    is_completed: bool
    total_items: int
    filled_items: int
    created_at: str
    completed_at: Optional[str] = None

class StationRecord(BaseModel):
    station_name: str
    items_processed: int
    inventory_remaining: int
    timestamp: str

class SimulationStats(BaseModel):
    total_orders: int
    completed_orders: int
    incomplete_orders: int
    completion_rate: float
    most_active_station: Optional[str] = None

class SimulationRequest(BaseModel):
    stations_file_1: str = "data/Stations1.txt"
    stations_file_2: str = "data/Stations2.txt"
    customer_orders_file: str = "data/CustomerOrders.txt"
    assembly_line_file: str = "data/AssemblyLine.txt"

# Helper functions
def get_db_connection():
    """Get SQLite database connection"""
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
    """Health check endpoint"""
    try:
        conn = get_db_connection()
        conn.close()
        return {"status": "healthy", "database": "connected"}
    except Exception as e:
        return {"status": "unhealthy", "error": str(e)}

# Order endpoints
@app.get("/orders", response_model=List[OrderRecord])
async def get_orders(limit: int = 100):
    """Get order history"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders ORDER BY created_at DESC LIMIT ?",
            (limit,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Specific routes must come before parameterized routes to avoid route conflicts
@app.get("/orders/completed", response_model=List[OrderRecord])
async def get_completed_orders(limit: int = 100):
    """Get completed orders"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders WHERE is_completed = 1 ORDER BY completed_at DESC LIMIT ?",
            (limit,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/orders/incomplete", response_model=List[OrderRecord])
async def get_incomplete_orders(limit: int = 100):
    """Get incomplete orders"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders WHERE is_completed = 0 ORDER BY created_at DESC LIMIT ?",
            (limit,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/orders/customer/{customer_name}", response_model=List[OrderRecord])
async def get_orders_by_customer(customer_name: str):
    """Get orders for a specific customer"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            "SELECT * FROM orders WHERE customer_name = ? ORDER BY created_at DESC",
            (customer_name,)
        )
        rows = cursor.fetchall()
        conn.close()
        return [row_to_order(row) for row in rows]
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
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/stations/{station_name}/history", response_model=List[StationRecord])
async def get_station_history(station_name: str, limit: int = 100):
    """Get history for a specific station"""
    try:
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
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Statistics endpoint
@app.get("/stats", response_model=SimulationStats)
async def get_statistics():
    """Get simulation statistics"""
    try:
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
    """Run the simulation (executes the C++ program)"""
    try:
        # Build path to executable
        exe_path = Path(__file__).parent.parent / "build" / "assembly_line"
        if not exe_path.exists():
            raise HTTPException(
                status_code=404,
                detail="Simulation executable not found. Please build the project first."
            )
        
        # Prepare data file paths
        data_dir = Path(__file__).parent.parent / "data"
        stations1 = data_dir / request.stations_file_1.split("/")[-1]
        stations2 = data_dir / request.stations_file_2.split("/")[-1]
        orders = data_dir / request.customer_orders_file.split("/")[-1]
        assembly = data_dir / request.assembly_line_file.split("/")[-1]
        
        # Run simulation
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

# WebSocket endpoint for real-time updates
@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    """WebSocket endpoint for real-time updates"""
    await websocket.accept()
    try:
        while True:
            # Send statistics every second
            stats = await get_statistics()
            await websocket.send_json(stats.dict())
            await asyncio.sleep(1)
    except WebSocketDisconnect:
        pass

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)

