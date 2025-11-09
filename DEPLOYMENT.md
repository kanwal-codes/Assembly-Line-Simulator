# 🚀 Deployment Guide

## Current Setup

- **Backend**: Fly.io (`https://assembly-line-simulator.fly.dev`)
- **Frontend**: Vercel
- **Database**: SQLite (embedded in Fly.io container)

---

## Backend Deployment (Fly.io)

### Prerequisites
- Fly.io account (free tier available)
- Fly CLI installed: `curl -L https://fly.io/install.sh | sh`

### Deploy Backend

```bash
# Login to Fly.io
fly auth login

# Deploy
fly deploy --app assembly-line-simulator
```

### Environment Variables

```bash
fly secrets set DATABASE_PATH=/app/database/assembly_line.db
fly secrets set PORT=8000
fly secrets set PYTHONUNBUFFERED=1
```

### Auto-Deployment

GitHub Actions automatically deploys on push to `main` branch (requires `FLY_API_TOKEN` secret).

---

## Frontend Deployment (Vercel)

### Step 1: Sign Up
1. Go to [vercel.com](https://vercel.com)
2. Sign up with GitHub

### Step 2: Import Project
1. Click "Add New Project"
2. Import `Assembly-Line-Simulator` repository

### Step 3: Configure
- **Root Directory**: `frontend`
- **Framework**: Vite (auto-detected)
- **Build Command**: `npm run build`
- **Output Directory**: `dist`

### Step 4: Environment Variable
- **Name**: `VITE_API_URL`
- **Value**: `https://assembly-line-simulator.fly.dev`
- **Environments**: Production, Preview, Development

### Step 5: Deploy
Click "Deploy" and wait for build to complete.

---

## Testing Deployment

### Test Backend
```bash
curl https://assembly-line-simulator.fly.dev/health
curl https://assembly-line-simulator.fly.dev/stats
```

### Test Frontend
1. Visit your Vercel URL
2. Open browser console (F12)
3. Run:
```javascript
fetch('https://assembly-line-simulator.fly.dev/stats')
  .then(r => r.json())
  .then(data => console.log('✅ Connected!', data))
```

---

## API Endpoints

- `GET /health` - Health check
- `GET /stats` - Statistics
- `GET /orders` - All orders
- `GET /stations` - All stations
- `POST /simulation/run` - Run simulation
- `WS /ws` - WebSocket for real-time updates

---

## Troubleshooting

See `TROUBLESHOOTING.md` for common issues and solutions.
