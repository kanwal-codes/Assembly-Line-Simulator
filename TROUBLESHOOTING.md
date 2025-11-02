# Troubleshooting Guide

## Issue: Frontend Shows "Loading" Forever

### ✅ FIXED: API Syntax Error
- **Problem**: Syntax error in `api/main.py` line 103
- **Solution**: Fixed string literal error
- **Status**: ✅ API is now running

### Current Status

**API Server**: ✅ Running on http://127.0.0.1:8000
- Health: http://127.0.0.1:8000/health
- Stats: http://127.0.0.1:8000/stats

**Frontend**: Check if running on port 3000

### Quick Fix Steps

1. **Check API is running:**
```bash
curl http://127.0.0.1:8000/health
```

Should return: `{"status":"healthy","database":"connected"}`

2. **If API is not running, start it:**
```bash
cd api
source venv/bin/activate
uvicorn main:app --host 127.0.0.1 --port 8000 --reload
```

3. **Check Frontend:**
- Open browser console (F12)
- Look for errors in Network tab
- Check if requests to http://localhost:8000 are failing

4. **Common Issues:**

**CORS Error:**
- The API has CORS enabled for localhost:3000
- If using different port, update CORS in `api/main.py`

**Connection Refused:**
- API not running
- Wrong port (should be 8000)
- Firewall blocking connection

**Database Not Found:**
- Run simulation first: `./build/assembly_line data/Stations1.txt data/Stations2.txt data/CustomerOrders.txt data/AssemblyLine.txt`

5. **Test API Directly:**
```bash
# Health check
curl http://127.0.0.1:8000/health

# Get stats
curl http://127.0.0.1:8000/stats

# Get orders
curl http://127.0.0.1:8000/orders
```

### Debug Frontend

1. Open browser console
2. Check for errors like:
   - `Network Error`
   - `CORS policy`
   - `404 Not Found`
3. In Network tab, check if requests to `/stats` or `/orders` are:
   - Pending (waiting)
   - Failed (red)
   - 200 OK (green)

### Restart Everything

```bash
# Stop all processes
pkill -f uvicorn
pkill -f vite

# Start API
cd api
source venv/bin/activate
uvicorn main:app --host 127.0.0.1 --port 8000 --reload &

# Start Frontend (in another terminal)
cd frontend
npm run dev
```

### Verify Database

```bash
# Check if database exists
ls -la database/assembly_line.db

# If missing, run simulation
./build/assembly_line data/Stations1.txt data/Stations2.txt data/CustomerOrders.txt data/AssemblyLine.txt
```

