# 🚀 Complete Deployment Guide - Fly.io + Vercel

## ✅ Current Setup

- **Backend**: Fly.io (`https://assembly-line-simulator.fly.dev`)
- **Frontend**: Vercel (your Vercel URL)
- **Database**: SQLite (embedded in Fly.io container)

---

## 📋 Backend (Fly.io) Configuration

### Current Status
- ✅ App Name: `assembly-line-simulator`
- ✅ Region: `iad` (Washington, D.C.)
- ✅ Health Check: `/health`
- ✅ Port: `8000`
- ✅ CORS: Allows all origins in production (for Vercel)

### Environment Variables (Set via Fly.io secrets)
```bash
fly secrets set DATABASE_PATH=/app/database/assembly_line.db
fly secrets set PORT=8000
fly secrets set PYTHONUNBUFFERED=1
```

### Manual Deployment
```bash
fly deploy --app assembly-line-simulator
```

### Auto-Deployment
- GitHub Actions workflow: `.github/workflows/fly-deploy.yml`
- Triggers on push to `main` branch
- Requires `FLY_API_TOKEN` secret in GitHub

---

## 📋 Frontend (Vercel) Configuration

### Vercel Dashboard Settings

1. **Root Directory**: `frontend`
2. **Framework Preset**: Vite (auto-detected)
3. **Build Command**: `npm run build`
4. **Output Directory**: `dist`
5. **Install Command**: `npm install`

### Environment Variable (Required!)

**Name**: `VITE_API_URL`  
**Value**: `https://assembly-line-simulator.fly.dev`  
**Environments**: ☑️ Production ☑️ Preview ☑️ Development

### Vercel Configuration File
- `frontend/vercel.json` - Handles SPA routing

---

## 🔗 Connecting Frontend to Backend

### Step 1: Deploy Backend to Fly.io
```bash
# Already deployed at: https://assembly-line-simulator.fly.dev
```

### Step 2: Deploy Frontend to Vercel
1. Go to [vercel.com](https://vercel.com)
2. Import your GitHub repository
3. Set **Root Directory** to `frontend`
4. Add environment variable: `VITE_API_URL=https://assembly-line-simulator.fly.dev`
5. Deploy!

### Step 3: Verify Connection
- Frontend should connect to backend automatically
- CORS is configured to allow all origins in production

---

## 🧪 Testing

### Test Backend
```bash
curl https://assembly-line-simulator.fly.dev/health
curl https://assembly-line-simulator.fly.dev/stats
```

### Test Frontend
- Visit your Vercel URL
- Click "Run Simulation"
- Check if data loads from backend

---

## 🔧 Troubleshooting

### Backend Issues

**"App not responding"**
```bash
fly status --app assembly-line-simulator
fly logs --app assembly-line-simulator
fly apps restart assembly-line-simulator
```

**"CORS errors"**
- Backend allows all origins in production
- If issues persist, check browser console for specific error

### Frontend Issues

**"Network Error"**
- Verify `VITE_API_URL` is set in Vercel
- Check backend is running: `curl https://assembly-line-simulator.fly.dev/health`
- Verify CORS is working (backend allows all origins)

**"Build fails"**
- Check Vercel build logs
- Ensure `frontend/package.json` has all dependencies
- Verify `frontend/vercel.json` exists

---

## 📝 Quick Reference

### Backend URL
```
https://assembly-line-simulator.fly.dev
```

### API Endpoints
- Health: `GET /health`
- Stats: `GET /stats`
- Orders: `GET /orders`
- Stations: `GET /stations`
- Run Simulation: `POST /simulation/run`

### Frontend Environment Variable
```
VITE_API_URL=https://assembly-line-simulator.fly.dev
```

---

## ✅ Deployment Checklist

- [x] Backend deployed to Fly.io
- [x] Backend health check working
- [x] CORS configured for production
- [ ] Frontend deployed to Vercel
- [ ] `VITE_API_URL` set in Vercel
- [ ] Frontend connects to backend
- [ ] Simulation runs successfully

---

## 🎉 You're Done!

Once both are deployed:
- **Backend**: `https://assembly-line-simulator.fly.dev`
- **Frontend**: `https://your-app.vercel.app`

Both services will auto-deploy on every push to `main` branch!

