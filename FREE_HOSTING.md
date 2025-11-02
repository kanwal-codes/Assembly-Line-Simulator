# 💰 Free Hosting Guide - 100% Free Options

Here are the **best completely FREE** ways to host your Assembly Line Simulator!

## 🏆 Best Completely Free Setup

### **Frontend: Vercel** (Always Free + Fast) ⭐⭐⭐⭐⭐
### **Backend: Railway Free Tier** (Sleeps but works) ⭐⭐⭐⭐
### **Total Cost: $0/month** 🎉

---

## ⚡ Quick Setup (100% Free)

### Step 1: Deploy Backend to Railway (Free)

1. **Push code to GitHub**
   ```bash
   git add .
   git commit -m "Ready for free hosting"
   git push origin main
   ```

2. **Sign up at Railway**
   - Go to [railway.app](https://railway.app)
   - Sign up with GitHub (free)
   - Get $5 free credit monthly

3. **Deploy**
   - Click "New Project"
   - "Deploy from GitHub repo"
   - Select your repo
   - Railway auto-detects Dockerfile ✅

4. **That's it!** You get a URL like:
   - `https://your-app.railway.app`

**Note**: Railway free tier sleeps after inactivity (adds ~30s wake-up time)

### Step 2: Deploy Frontend to Vercel (Always Free)

1. **Build frontend locally:**
   ```bash
   cd frontend
   npm run build
   ```

2. **Go to Vercel**
   - Visit [vercel.com](https://vercel.com)
   - Sign up with GitHub (free forever)

3. **Deploy**
   - "Add New Project"
   - Import your GitHub repo
   - Configure:
     - **Root Directory**: `frontend`
     - **Build Command**: `npm run build`
     - **Output Directory**: `dist`
     - **Environment Variable**:
       ```
       VITE_API_URL=https://your-app.railway.app
       ```

4. **Deploy!**
   - Vercel builds and deploys automatically
   - You get: `https://your-app.vercel.app`
   - **Always on, never sleeps!**

**Total Time**: ~10 minutes  
**Total Cost**: **$0/month** ✅

---

## 🎯 Alternative Free Options

### Option 1: All on Railway (Free)

- **Backend**: Railway free tier
- **Frontend**: Railway static site (free)
- **Pros**: Everything in one place
- **Cons**: Both may sleep after inactivity

**Setup**: Same as above, but create a second service for frontend static files.

### Option 2: Render.com (Free Tier)

1. **Backend on Render**
   - Go to [render.com](https://render.com)
   - "New Web Service"
   - Connect GitHub
   - Select Docker
   - **Free tier** (sleeps after 15min)

2. **Frontend on Render**
   - "New Static Site"
   - Point to `frontend/dist`
   - Set `VITE_API_URL` environment variable
   - **Always free!**

**Cost**: $0/month  
**Note**: Backend wakes up slowly after sleep

### Option 3: Netlify (Frontend) + Railway (Backend)

1. **Frontend on Netlify**
   - [netlify.com](https://netlify.com)
   - Free tier (100GB bandwidth/month)
   - Drag & drop `frontend/dist` or connect GitHub
   - Set environment variable:
     ```
     VITE_API_URL=https://your-api.railway.app
     ```

2. **Backend on Railway** (as above)

**Cost**: $0/month ✅

---

## 🆓 Free Tier Comparison

| Service | Free Tier | Always On? | Best For |
|---------|-----------|------------|------------------|
| **Vercel** | ✅ Unlimited | ✅ Yes | Frontend (Best!) |
| **Netlify** | ✅ 100GB/mo | ✅ Yes | Frontend |
| **Railway** | ✅ $5 credit/mo | ⚠️ Sleeps | Backend |
| **Render** | ✅ Free tier | ⚠️ Sleeps 15min | Backend |
| **Fly.io** | ✅ 3 VMs free | ✅ Yes | Backend (Best!) |
| **Cloudflare Pages** | ✅ Unlimited | ✅ Yes | Frontend |

---

## 🌟 **RECOMMENDED: Best Free Setup**

### Frontend: **Vercel** (Best choice)
- ✅ Always free
- ✅ Never sleeps
- ✅ Global CDN (super fast)
- ✅ Auto-deploy from GitHub
- ✅ SSL included

### Backend: **Fly.io** (Best for free)
- ✅ 3 VMs free (enough for your app)
- ✅ Never sleeps
- ✅ Fast worldwide
- ✅ Easy deployment

**Total**: $0/month, both always on!

### 📖 **Complete Step-by-Step Guide**

👉 **See `deploy/VERCEL_FLYIO.md` for detailed instructions!**

### Alternative Backend: Railway
- ✅ $5 free credit/month
- ⚠️ Sleeps after inactivity (~30s wake time)
- ✅ Super easy setup

---

## 🚀 Deploy to Fly.io (Free & Always On)

### Why Fly.io?

- **3 VMs free forever**
- **Never sleeps**
- **Global edge deployment**
- **Fast everywhere**

### Setup Steps:

1. **Install Fly CLI**
   ```bash
   # macOS
   brew install flyctl
   
   # Linux
   curl -L https://fly.io/install.sh | sh
   
   # Windows (PowerShell)
   iwr https://fly.io/install.ps1 -useb | iex
   ```

2. **Login**
   ```bash
   fly auth login
   ```

3. **Create Fly App**
   ```bash
   cd /path/to/Assembly-Line-Simulator
   fly launch
   ```
   
   Follow prompts:
   - App name: `assembly-line-simulator` (or auto-generated)
   - Region: Choose closest to you
   - Deploy now? Yes

4. **Fly auto-detects Dockerfile** ✅

5. **Get your URL**
   - `https://assembly-line-simulator.fly.dev`

**Done! Always on, completely free!** 🎉

---

## 📝 Environment Variables for Free Hosting

### On Railway/Render/Fly.io:

```
PORT=8000
DATABASE_PATH=/app/database/assembly_line.db
PYTHONUNBUFFERED=1
CORS_ORIGINS=https://your-frontend.vercel.app
```

### On Vercel/Netlify (Frontend):

```
VITE_API_URL=https://your-backend.railway.app
```

---

## 🎯 Quick Decision Guide

**Want everything for free?**
→ **Vercel (Frontend) + Fly.io (Backend)** = $0, always on

**Want easiest setup?**
→ **Railway (Both)** = $0, sleeps after inactivity

**Want fastest worldwide?**
→ **Vercel (Frontend) + Fly.io (Backend)** = $0, global CDN

**Just testing/learning?**
→ **Render.com (Both)** = $0, sleeps after 15min

---

## ✅ Free Hosting Checklist

- [ ] Code pushed to GitHub
- [ ] Dockerfile ready
- [ ] Environment variables documented
- [ ] Frontend uses `VITE_API_URL`
- [ ] CORS configured
- [ ] Tested locally with Docker

---

## 🆘 Troubleshooting Free Tiers

### "Backend is slow to respond"

**Cause**: Service is sleeping (Railway/Render free tier)

**Solutions**:
1. Upgrade to paid tier ($5-7/month) for always-on
2. Use Fly.io (free & always-on)
3. Add a "keep-alive" ping every 5 minutes
4. Use Vercel's serverless functions (if compatible)

### "Cannot connect to API"

**Cause**: CORS not configured or wrong API URL

**Fix**:
1. Set `CORS_ORIGINS` environment variable
2. Update `VITE_API_URL` in frontend
3. Check API is actually running

### "Database gets reset"

**Cause**: Ephemeral file system (some free tiers)

**Fix**:
1. Use Railway's persistent disk (included)
2. Use external SQLite hosting
3. Migrate to PostgreSQL (Railway free tier includes it)

---

## 💡 Pro Tips for Free Hosting

1. **Use Railway's $5 credit wisely**
   - Enough for small-medium traffic
   - Upgrade if you get popular

2. **Fly.io is best for backend**
   - Always free, never sleeps
   - 3 VMs is plenty for your app

3. **Vercel for frontend**
   - Unlimited bandwidth
   - Best CDN
   - Never sleeps

4. **Monitor usage**
   - Free tiers have limits
   - Upgrade before hitting limits

---

## 🎉 Summary

**Recommended Free Setup:**

1. **Frontend**: Vercel (free forever, always on)
2. **Backend**: Fly.io (3 VMs free, always on)
3. **Total**: **$0/month** ✅

**OR if you want easiest:**

1. **Both**: Railway (free $5 credit, sleeps)
2. **Total**: **$0/month** ✅

**Both work great and cost nothing!** 🚀

---

Need help? Check the specific guides:
- `deploy/railway.md` for Railway
- `DEPLOYMENT.md` for all options
- `QUICK_DEPLOY.md` for step-by-step

