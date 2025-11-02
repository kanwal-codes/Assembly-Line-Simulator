# 🚀 Deploy to Vercel + Fly.io (100% Free, Always On)

Complete step-by-step guide for the **best free hosting setup**.

## 🎯 What You're Getting

- **Frontend**: Vercel (free forever, always on, global CDN)
- **Backend**: Fly.io (3 VMs free, always on, no sleep)
- **Total Cost**: $0/month
- **Performance**: ⭐⭐⭐⭐⭐

---

## 📋 Prerequisites

- GitHub account
- Code pushed to GitHub repository
- 15 minutes of time

---

## Part 1: Deploy Backend to Fly.io

### Step 1: Install Fly CLI

**macOS:**
```bash
brew install flyctl
```

**Linux:**
```bash
curl -L https://fly.io/install.sh | sh
```

**Windows (PowerShell):**
```powershell
iwr https://fly.io/install.ps1 -useb | iex
```

**Or download from:** [fly.io/install](https://fly.io/install)

### Step 2: Login to Fly.io

```bash
fly auth login
```

This opens your browser to sign up/login. Fly.io account is **free**.

### Step 3: Navigate to Your Project

```bash
cd /path/to/Assembly-Line-Simulator
```

### Step 4: Launch Your App

```bash
fly launch
```

**Follow the prompts:**

1. **App name**: `assembly-line-simulator` (or press Enter for auto-generated)
   - Must be unique, Fly will suggest if taken
   
2. **Region**: Choose closest to you (e.g., `iad` for US East, `lhr` for UK)
   - Type: `iad` (or your preferred)
   - You can see list: `fly regions list`
   
3. **Postgres?**: `n` (No, we're using SQLite)
   
4. **Redis?**: `n` (No)
   
5. **Deploy now?**: `y` (Yes)

**Fly.io will:**
- ✅ Create your app
- ✅ Detect Dockerfile
- ✅ Build the Docker image
- ✅ Deploy to edge
- ✅ Give you a URL: `https://assembly-line-simulator.fly.dev`

### Step 5: Configure Environment Variables

```bash
fly secrets set DATABASE_PATH=/app/database/assembly_line.db
fly secrets set PYTHONUNBUFFERED=1
fly secrets set PORT=8000
```

### Step 6: Get Your Backend URL

```bash
fly status
```

You'll see your URL like: `https://assembly-line-simulator.fly.dev`

**Save this URL!** You'll need it for the frontend.

---

## Part 2: Deploy Frontend to Vercel

### Step 1: Build Frontend Locally (Test First)

```bash
cd frontend

# Install dependencies (if not already)
npm install

# Build for production
npm run build
```

This creates `frontend/dist` with production-ready files.

### Step 2: Sign Up for Vercel

1. Go to [vercel.com](https://vercel.com)
2. Click **"Sign Up"**
3. Choose **"Continue with GitHub"**
4. Authorize Vercel

**Vercel account is free forever.**

### Step 3: Create New Project

1. Click **"Add New Project"**
2. Click **"Import Git Repository"**
3. Select your `Assembly-Line-Simulator` repository
4. Click **"Import"**

### Step 4: Configure Project Settings

**In the project setup screen:**

1. **Framework Preset**: `Vite`
   - Vercel auto-detects Vite
   
2. **Root Directory**: 
   - Click **"Edit"** 
   - Enter: `frontend`
   
3. **Build Command**: 
   - Should be: `npm run build`
   - (Auto-filled by Vercel)
   
4. **Output Directory**: 
   - Should be: `dist`
   - (Auto-filled by Vercel)
   
5. **Install Command**: 
   - Should be: `npm install`
   - (Auto-filled by Vercel)

### Step 5: Add Environment Variable

**Before deploying, add this:**

1. Click **"Environment Variables"** section
2. Click **"Add"**
3. Add:
   - **Name**: `VITE_API_URL`
   - **Value**: `https://assembly-line-simulator.fly.dev` 
     (Use your Fly.io URL from Step 1)
   - **Environment**: Production, Preview, Development (select all)

### Step 6: Deploy!

1. Click **"Deploy"**
2. Wait 1-2 minutes
3. Vercel builds and deploys automatically
4. You get a URL: `https://assembly-line-simulator.vercel.app`

**🎉 Frontend is live!**

---

## Part 3: Update CORS for Production

### Update Backend CORS Settings

Your backend needs to allow requests from Vercel domain.

**Option 1: Update in Fly.io secrets**

```bash
fly secrets set CORS_ORIGINS=https://your-app.vercel.app,https://your-app.vercel.app
```

Replace `your-app.vercel.app` with your actual Vercel URL.

**Option 2: Update code and redeploy**

Edit `api/main.py`:

```python
# Allow all origins (for development)
cors_origins = ["*"]

# OR for production, allow specific origins:
cors_origins = [
    "https://assembly-line-simulator.vercel.app",
    "https://your-custom-domain.com"
]
```

Then redeploy:
```bash
fly deploy
```

---

## ✅ Verification

### Test Backend

```bash
# Health check
curl https://assembly-line-simulator.fly.dev/health

# Should return: {"status":"healthy","database":"connected"}
```

### Test Frontend

1. Visit your Vercel URL
2. Open browser console (F12)
3. Check for any CORS errors
4. Click "Run Simulation" to test API connection

---

## 🔄 Updating Your App

### Update Backend

```bash
# Make changes to your code
git add .
git commit -m "Update backend"
git push origin main

# Deploy to Fly.io
fly deploy
```

**Or use auto-deploy:**

Fly.io can auto-deploy from GitHub (see GitHub Actions setup below).

### Update Frontend

1. Make changes to frontend code
2. Push to GitHub:
   ```bash
   git add .
   git commit -m "Update frontend"
   git push origin main
   ```
3. Vercel **auto-deploys** automatically! ✅
   - No manual step needed
   - Deploys on every push to main branch

---

## 🔧 Advanced: Auto-Deploy Backend

### Setup GitHub Actions for Fly.io

1. **Get Fly.io API Token:**
   ```bash
   fly auth token
   ```
   Copy the token.

2. **Add to GitHub Secrets:**
   - Go to your GitHub repo
   - Settings → Secrets and variables → Actions
   - Click "New repository secret"
   - Name: `FLY_API_TOKEN`
   - Value: (paste token from step 1)
   - Click "Add secret"

3. **The workflow file already exists!**
   - `.github/workflows/deploy-fly.yml` is ready
   - Just push to main branch
   - Auto-deploys on every push ✅

---

## 📊 Monitoring & Logs

### View Fly.io Logs

```bash
fly logs
```

### View Vercel Logs

1. Go to Vercel dashboard
2. Click your project
3. Go to "Deployments"
4. Click on a deployment
5. View "Build Logs" or "Function Logs"

### Check App Status

**Fly.io:**
```bash
fly status
fly apps list
```

**Vercel:**
- Dashboard shows all deployments
- Real-time metrics available

---

## 🆘 Troubleshooting

### Backend Issues

**"App not responding"**
```bash
# Check status
fly status

# Check logs
fly logs

# Restart app
fly apps restart assembly-line-simulator
```

**"Database path error"**
- Ensure `DATABASE_PATH` secret is set
- Check volume mounting (if using persistent storage)

**"Port binding error"**
- Verify PORT is set to 8000
- Check Dockerfile exposes correct port

### Frontend Issues

**"Cannot connect to API"**
- Verify `VITE_API_URL` environment variable is set
- Check CORS is configured correctly
- Test backend URL directly: `curl https://your-backend.fly.dev/health`

**"Build fails"**
- Check build logs in Vercel
- Ensure all dependencies in `package.json`
- Try building locally: `npm run build`

**"CORS errors"**
- Update `CORS_ORIGINS` in Fly.io secrets
- Redeploy backend: `fly deploy`
- Clear browser cache

---

## 💰 Cost Breakdown

### Fly.io Free Tier
- ✅ 3 shared-cpu VMs (256MB RAM each)
- ✅ 3GB outbound data transfer/month
- ✅ Always on (no sleep)
- ✅ Global edge network
- **Cost: $0/month**

### Vercel Free Tier
- ✅ Unlimited personal projects
- ✅ 100GB bandwidth/month
- ✅ Always on
- ✅ Automatic SSL
- ✅ Global CDN
- **Cost: $0/month**

**Total: $0/month forever** ✅

---

## 🎉 You're Done!

Your app is now:
- ✅ Live on the internet
- ✅ Always on (no sleep)
- ✅ Fast (global CDN)
- ✅ Free forever
- ✅ Auto-deploying on updates

**Frontend URL**: `https://your-app.vercel.app`  
**Backend URL**: `https://assembly-line-simulator.fly.dev`  
**API Docs**: `https://assembly-line-simulator.fly.dev/docs`

---

## 📚 Next Steps

1. **Custom Domain** (optional):
   - Vercel: Add domain in dashboard (free SSL)
   - Fly.io: `fly certs add yourdomain.com`

2. **Monitor Usage**:
   - Fly.io: `fly dashboard`
   - Vercel: Check analytics in dashboard

3. **Scale** (if needed):
   - Both platforms make scaling easy
   - Upgrade when you outgrow free tier

Enjoy your free, always-on hosting! 🚀

