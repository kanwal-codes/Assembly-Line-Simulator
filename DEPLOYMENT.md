# 🚀 Deployment Guide - Assembly Line Simulator

This guide covers multiple hosting options for your hybrid application (C++ core, Python FastAPI, React frontend).

## 📋 Architecture Overview

Your app consists of:
- **C++ Executable**: `build/assembly_line` (needs to be compiled for target platform)
- **Python FastAPI**: Backend API server (port 8000)
- **React Frontend**: Static files after build
- **SQLite Database**: `database/assembly_line.db`

## 🎯 Recommended Hosting Options

### **Option 1: Docker (Recommended - Most Portable)**

**Best for**: Full control, works everywhere, easy scaling

#### Setup Steps:

1. **Create Dockerfile** (see `Dockerfile` in root)
2. **Deploy to**:
   - **Railway** (railway.app) - Easy, free tier available
   - **Render** (render.com) - Simple, good free tier
   - **Fly.io** (fly.io) - Global edge deployment
   - **AWS ECS/Fargate** - Production-grade
   - **DigitalOcean App Platform** - Developer-friendly

**Pros:**
- ✅ One container runs everything
- ✅ Works on any Docker host
- ✅ Easy to replicate locally
- ✅ Consistent environment

**Cons:**
- ⚠️ Need to compile C++ for Linux
- ⚠️ Container size will be larger

---

### **Option 2: Platform-as-a-Service (Easiest)**

#### **2A. Railway.app** (⭐ Easiest)

**Why Railway:**
- Free tier available
- Automatic deployments from GitHub
- Supports Docker
- Built-in database options

**Steps:**
1. Sign up at railway.app
2. Connect GitHub repo
3. Deploy using Dockerfile
4. Add environment variables
5. Done! Get a URL like `yourapp.railway.app`

**Cost**: Free tier → $5-20/month for production

#### **2B. Render.com** (⭐ Great for Startups)

**Why Render:**
- Generous free tier
- Easy setup
- Good documentation

**Steps:**
1. Sign up at render.com
2. Create "Web Service" from Docker
3. Connect GitHub repo
4. Deploy!

**Cost**: Free tier → $7-25/month for production

#### **2C. Fly.io** (⭐ Global Edge)

**Why Fly.io:**
- Global edge deployment
- Fast worldwide
- Great for production

**Steps:**
1. Install Fly CLI
2. `fly launch`
3. Configure `fly.toml`
4. Deploy

**Cost**: Free tier → Pay-as-you-go

---

### **Option 3: Separate Services (Most Scalable)**

#### Frontend: Vercel/Netlify
#### Backend: Railway/Render/Heroku

**Why:**
- Frontend on CDN (super fast)
- Backend on platform
- Best performance

**Steps:**

1. **Build Frontend:**
   ```bash
   cd frontend
   npm run build
   ```

2. **Deploy Frontend to Vercel:**
   - Push to GitHub
   - Import repo in Vercel
   - Build command: `npm run build`
   - Output: `frontend/dist`
   - Environment variable: `VITE_API_URL=https://your-api.railway.app`

3. **Deploy Backend to Railway:**
   - Use Dockerfile
   - Set environment variables
   - Expose port 8000

**Cost**: 
- Vercel: Free tier
- Railway: Free tier → $5-20/month

---

### **Option 4: Virtual Private Server (Most Control)**

**Best for**: Learning, full control, custom needs

#### Providers:
- **DigitalOcean** ($6/month)
- **Linode** ($5/month)
- **Hetzner** (€4/month)
- **AWS EC2** (Pay-as-you-go)

**Steps:**

1. **Create Ubuntu Server** (22.04 LTS)
2. **SSH into server**
3. **Install dependencies:**
   ```bash
   sudo apt update
   sudo apt install -y build-essential python3 python3-pip python3-venv nodejs npm nginx
   ```
4. **Clone and build:**
   ```bash
   git clone https://github.com/yourusername/Assembly-Line-Simulator.git
   cd Assembly-Line-Simulator
   make release
   ```
5. **Setup Python API:**
   ```bash
   cd api
   python3 -m venv venv
   source venv/bin/activate
   pip install -r requirements.txt
   ```
6. **Use PM2 or systemd** to run services:
   ```bash
   # Install PM2
   npm install -g pm2
   
   # Start API
   cd api
   pm2 start "uvicorn main:app --host 0.0.0.0 --port 8000" --name api
   
   # Start frontend (production mode)
   cd frontend
   npm run build
   pm2 serve dist 3000 --name frontend
   ```
7. **Setup Nginx** as reverse proxy:
   ```nginx
   server {
       listen 80;
       server_name yourdomain.com;
       
       # Frontend
       location / {
           proxy_pass http://localhost:3000;
       }
       
       # API
       location /api {
           proxy_pass http://localhost:8000;
       }
   }
   ```
8. **Add SSL** with Let's Encrypt:
   ```bash
   sudo apt install certbot python3-certbot-nginx
   sudo certbot --nginx -d yourdomain.com
   ```

**Cost**: $5-12/month

---

## 🐳 Docker Deployment (Recommended)

### Quick Start with Docker:

1. **Build Docker image:**
   ```bash
   docker build -t assembly-line-simulator .
   ```

2. **Run locally:**
   ```bash
   docker run -p 8000:8000 -p 3000:3000 assembly-line-simulator
   ```

3. **Deploy to Railway/Render:**
   - Push to GitHub
   - Connect to Railway/Render
   - Select Docker deployment
   - Deploy!

---

## 🔧 Configuration for Production

### Environment Variables:

Create `.env` file or set in hosting platform:

```env
# API Configuration
API_HOST=0.0.0.0
API_PORT=8000

# Database
DATABASE_PATH=/app/database/assembly_line.db

# Frontend
VITE_API_URL=https://your-api-url.com

# CORS (if frontend on different domain)
CORS_ORIGIN=https://your-frontend-url.com
```

### Update Frontend API URL:

In `frontend/src/components/Dashboard.tsx`:
```typescript
const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000'
```

---

## 📊 Comparison Table

| Option | Ease | Cost | Scalability | Best For |
|--------|------|------|-------------|----------|
| **Railway** | ⭐⭐⭐⭐⭐ | Free-$20/mo | ⭐⭐⭐ | Quick deployment |
| **Render** | ⭐⭐⭐⭐⭐ | Free-$25/mo | ⭐⭐⭐ | Startups |
| **Docker+VPS** | ⭐⭐⭐ | $5-12/mo | ⭐⭐⭐⭐ | Learning/Control |
| **Separate** | ⭐⭐⭐ | Free-$20/mo | ⭐⭐⭐⭐⭐ | Production |
| **Fly.io** | ⭐⭐⭐⭐ | Pay-as-you-go | ⭐⭐⭐⭐⭐ | Global scale |

---

## 🎯 My Recommendation

**For FREE Hosting**: See `FREE_HOSTING.md` ⭐ (100% free options)
**For Quick Start**: Use **Railway.app** with Docker
**For Production**: Use **Separate Services** (Vercel + Railway)
**For Learning**: Use **VPS** (DigitalOcean)

## 💰 Want 100% Free Hosting?

**Check `FREE_HOSTING.md`** for:
- ✅ Completely free options
- ✅ Always-on free services
- ✅ Step-by-step free deployment
- ✅ Best free combinations

---

## 📝 Next Steps

1. Choose your hosting option
2. See the specific deployment file:
   - `docker/Dockerfile` - Docker setup
   - `deploy/railway.md` - Railway guide
   - `deploy/render.md` - Render guide
   - `deploy/vps.md` - VPS guide

Need help? Check the troubleshooting section or open an issue!

