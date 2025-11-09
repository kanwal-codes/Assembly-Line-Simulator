# 🚀 Deploy Backend First (Recommended Order)

## Why Deploy Backend First?

✅ You get the backend URL immediately  
✅ Frontend can use the real API URL  
✅ Avoids CORS issues  
✅ Easier to test everything works

---

## Step-by-Step: Deploy Backend to Fly.io

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

### Step 2: Login to Fly.io

```bash
fly auth login
```

This opens your browser to sign up/login (free account).

### Step 3: Navigate to Project

```bash
cd /Users/kanwal/Projects/Assembly-Line-Simulator
```

### Step 4: Launch App

```bash
fly launch
```

**Follow the prompts:**

1. **App name**: Type `assembly-line-simulator` (or press Enter for auto-generated)
   - If taken, Fly will suggest an alternative

2. **Region**: Choose closest to you
   - US East: `iad` (Washington DC)
   - US West: `sjc` (San Jose)
   - Europe: `lhr` (London)
   - Type your choice or press Enter for default

3. **Postgres?**: Type `n` (No, we're using SQLite)

4. **Redis?**: Type `n` (No)

5. **Deploy now?**: Type `y` (Yes)

**Fly.io will:**
- ✅ Create your app
- ✅ Detect Dockerfile automatically
- ✅ Build Docker image
- ✅ Deploy to edge network
- ✅ Give you a URL

### Step 5: Wait for Deployment

Takes 3-5 minutes. You'll see progress in terminal.

### Step 6: Get Your Backend URL

After deployment completes, you'll see:
```
App: assembly-line-simulator
URL: https://assembly-line-simulator.fly.dev
```

**Save this URL!** You'll need it for Vercel.

### Step 7: Configure Environment Variables

```bash
fly secrets set DATABASE_PATH=/app/database/assembly_line.db
fly secrets set PYTHONUNBUFFERED=1
fly secrets set PORT=8000
```

### Step 8: Test Backend

```bash
# Test health endpoint
curl https://assembly-line-simulator.fly.dev/health

# Should return: {"status":"healthy","database":"connected"}
```

**If you see that, backend is working!** ✅

---

## ✅ Backend Deployed!

You now have:
- ✅ Backend URL: `https://assembly-line-simulator.fly.dev`
- ✅ API Docs: `https://assembly-line-simulator.fly.dev/docs`
- ✅ Health Check: `https://assembly-line-simulator.fly.dev/health`

---

## 📝 Next: Deploy Frontend to Vercel

Now use your backend URL in Vercel:

1. Go to [vercel.com](https://vercel.com)
2. Import repository: `kanwal-codes/Assembly-Line-Simulator`
3. **Root Directory**: `frontend`
4. **Environment Variable**:
   - Name: `VITE_API_URL`
   - Value: `https://assembly-line-simulator.fly.dev` ← Use your actual URL!
5. Deploy!

---

## 🆘 Troubleshooting Backend

**"App failed to build"**
```bash
# Check logs
fly logs

# Common issues:
# - Dockerfile syntax error
# - Missing dependencies
```

**"App not responding"**
```bash
# Check status
fly status

# Restart app
fly apps restart assembly-line-simulator
```

**"Database error"**
- Verify DATABASE_PATH secret is set
- Check SQLite file permissions

**"Port binding error"**
- Ensure PORT=8000 is set
- Check Dockerfile exposes port 8000

---

## 🎯 Quick Commands Reference

```bash
# Deploy
fly deploy

# View logs
fly logs

# Check status
fly status

# Restart app
fly apps restart assembly-line-simulator

# View secrets
fly secrets list

# Open in browser
fly open
```

---

**Once backend is deployed, proceed to deploy frontend!** 🚀




