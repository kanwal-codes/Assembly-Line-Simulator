# How "Run Simulation" Works

## Overview

When you click the **"Run Simulation"** button in the frontend dashboard, here's what happens:

## Step-by-Step Flow

### 1. **Frontend (Dashboard.tsx)**
```typescript
const runSimulation = async () => {
  await axios.post(`${API_URL}/simulation/run`)
  setTimeout(fetchStats, 1000) // Refresh stats after 1 second
}
```

- User clicks "Run Simulation" button
- Frontend sends POST request to `/simulation/run` endpoint
- Waits 1 second, then refreshes the statistics

### 2. **Backend API (main.py)**
```python
@app.post("/simulation/run")
async def run_simulation(request: SimulationRequest):
```

The API endpoint:
1. **Finds the C++ executable**: Looks for `build/assembly_line`
2. **Prepares data files**: Uses default data files:
   - `data/Stations1.txt` - Station configuration (comma-delimited)
   - `data/Stations2.txt` - Station configuration (pipe-delimited)
   - `data/CustomerOrders.txt` - Customer orders
   - `data/AssemblyLine.txt` - Assembly line configuration
3. **Runs the C++ program** as a subprocess:
   ```bash
   ./build/assembly_line data/Stations1.txt data/Stations2.txt data/CustomerOrders.txt data/AssemblyLine.txt
   ```
4. **Waits for completion**: The simulation runs until all orders are processed
5. **Returns status**: Success or error message

### 3. **C++ Simulation Core (main.cpp)**

The C++ program does the actual simulation:

1. **Initializes Services**:
   - Logger (for logging)
   - Config (loads configuration)
   - Database (SQLite connection)

2. **Loads Data**:
   - Loads workstations from `Stations1.txt` and `Stations2.txt`
   - Loads customer orders from `CustomerOrders.txt`
   - Configures assembly line from `AssemblyLine.txt`

3. **Runs Simulation**:
   - Orders are placed in `g_pending` queue
   - `LineManager` processes orders through workstations
   - Each workstation:
     - Fills items if inventory available
     - Moves orders to next station
     - Tracks serial numbers
   - Simulation continues until all orders processed

4. **Saves Results**:
   - Completed orders → `g_completed` deque
   - Incomplete orders → `g_incomplete` deque
   - All results saved to SQLite database

5. **Database Storage**:
   ```cpp
   db.saveOrderCompletion(customerName, product, isCompleted, filledItems, totalItems)
   ```
   - Each order saved with:
     - Customer name
     - Product name
     - Completion status
     - Item counts
     - Timestamps

### 4. **Frontend Updates**

After simulation completes:
- Frontend automatically refreshes statistics after 1 second
- New orders appear in the database
- Dashboard shows updated:
  - Total orders count
  - Completion rate
  - Completed vs incomplete orders
  - Station activity

## What Gets Processed

### Input Files:
- **Stations1.txt & Stations2.txt**: Define workstations with:
  - Item name
  - Starting serial number
  - Initial inventory quantity
  - Description

- **CustomerOrders.txt**: Customer orders like:
  ```
  Cornel B. | 1-Room Home Office | Office Chair|Desk|Bookcase
  ```

- **AssemblyLine.txt**: Defines station order:
  ```
  Nighttable | Desk
  Desk | Office Chair
  ```

### Output:
- Orders processed through assembly line
- Items filled with serial numbers
- Completion status tracked
- Results saved to database
- Statistics updated

## Example Flow

1. **Click "Run Simulation"**
2. **API receives request** → Starts C++ program
3. **C++ loads 5 customer orders**:
   - Cornel B. - 1-Room Home Office
   - Chris S. - Bedroom
   - John M. - Kids Bedroom
   - Hong H. - 1-Room Office
   - Rania A. - 2-Room Home Office and Bedroom

4. **Simulation processes orders**:
   - Orders move through stations
   - Items get filled if inventory available
   - Orders complete or remain incomplete

5. **Results saved**:
   - Completed: 3 orders
   - Incomplete: 2 orders
   - All saved to database

6. **Frontend refreshes** → Shows new statistics

## Key Points

✅ **Runs the actual C++ simulation** (not just a mock)  
✅ **Uses real data files** from `data/` directory  
✅ **Saves results to database** for viewing later  
✅ **Updates statistics** automatically in frontend  
✅ **Can run multiple times** - each run adds more orders to history  

## Running Multiple Times

Each time you click "Run Simulation":
- Processes the same input files again
- Creates new orders in database
- Statistics accumulate (total orders increases)
- You can see order history over time

## Error Handling

If simulation fails:
- API returns error message
- Frontend shows error
- Check:
  - Data files exist
  - C++ executable is built
  - Database is accessible
  - Files have correct format

