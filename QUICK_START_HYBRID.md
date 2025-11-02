# Quick Start Guide - Hybrid Architecture

## 🚀 Get Everything Running in 5 Minutes

### Step 1: Build C++ Core (if not already built)

```bash
make release
```

### Step 2: Run Simulation Once (to create database)

```bash
./build/assembly_line data/Stations1.txt data/Stations2.txt data/CustomerOrders.txt data/AssemblyLine.txt
```

This will create the SQLite database at `database/assembly_line.db`

### Step 3: Start Python API (Terminal 1)

```bash
cd api
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install -r requirements.txt
python main.py
```

✅ API running at: http://localhost:8000  
📚 API Docs: http://localhost:8000/docs

### Step 4: Start React Frontend (Terminal 2)

```bash
cd frontend
npm install
npm run dev
```

✅ Frontend running at: http://localhost:3000

### Step 5: Open Browser

Visit: **http://localhost:3000**

You should see the Assembly Line Simulator dashboard!

## 🎯 What You Get

- ✅ **Dashboard**: Real-time statistics and charts
- ✅ **Orders**: View all orders with filtering
- ✅ **Stations**: Monitor station activity
- ✅ **Statistics**: Analytics and visualizations
- ✅ **Run Simulation**: Button to trigger new simulations

## 🔧 Troubleshooting

**"Database not found"**: Run Step 2 first to create the database

**"Module not found"**: Make sure you activated the Python venv and installed requirements

**"Cannot connect to API"**: Make sure the Python API is running on port 8000

**Frontend build errors**: Run `npm install` in the frontend directory

## 📝 Next Steps

1. Run simulations from the dashboard
2. View order history
3. Monitor station performance
4. Explore the API at http://localhost:8000/docs

Enjoy your hybrid architecture! 🎉

