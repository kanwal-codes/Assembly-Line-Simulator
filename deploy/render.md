# 🎨 Deploy to Render.com

Render offers a simple, developer-friendly deployment experience.

## Step-by-Step Guide

### 1. Prepare Your Project

- ✅ Push code to GitHub
- ✅ Have Dockerfile ready
- ✅ Set up environment variables

### 2. Sign Up

1. Go to [render.com](https://render.com)
2. Sign up with GitHub
3. Click "New +"

### 3. Create Web Service

1. Select "Web Service"
2. Connect your GitHub repository
3. Choose:
   - **Name**: assembly-line-simulator
   - **Environment**: Docker
   - **Region**: Choose closest to you
   - **Branch**: main
   - **Root Directory**: . (root)

### 4. Configure Build

Render auto-detects Dockerfile, but you can customize:

**Build Command**: (leave empty - Docker handles it)

**Start Command**: (leave empty - CMD in Dockerfile)

### 5. Environment Variables

Add these in the dashboard:

```env
PORT=8000
DATABASE_PATH=/app/database/assembly_line.db
PYTHONUNBUFFERED=1
```

### 6. Deploy

1. Click "Create Web Service"
2. Render will build and deploy automatically
3. Wait for "Live" status

### 7. Get Your URL

Render provides:
- `https://assembly-line-simulator.onrender.com`

## Frontend Deployment

### Option 1: Static Site (Recommended)

1. Build frontend locally:
   ```bash
   cd frontend
   npm run build
   ```

2. In Render, create "Static Site"
3. Point to `frontend/dist` directory
4. Set environment variable:
   ```
   VITE_API_URL=https://your-api.onrender.com
   ```

### Option 2: Same Service

Modify Dockerfile to serve static files from Python API.

## Cost

- **Free Tier**: 
  - Web services sleep after 15min inactivity
  - Static sites always free
- **Starter**: $7/month (always-on)
- **Professional**: $25/month

## Pros & Cons

**Pros:**
- ✅ Easy setup
- ✅ Free tier available
- ✅ Automatic SSL
- ✅ GitHub integration

**Cons:**
- ⚠️ Free tier sleeps (adds cold start time)
- ⚠️ Limited resources on free tier

## Tips

- Use Render's persistent disk for SQLite database
- Upgrade to Starter for always-on service
- Monitor logs in Render dashboard

