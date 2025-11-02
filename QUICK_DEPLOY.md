# 🚀 Quick Deploy Guide

The **fastest way** to get your app online in under 10 minutes!

## 💰 Want 100% FREE Hosting?

**👉 See `FREE_HOSTING.md` for completely free options!** ⭐

## ⚡ Option 1: Railway (Easiest - 5 minutes)

### Steps:

1. **Push to GitHub** (if not already)
   ```bash
   git add .
   git commit -m "Ready for deployment"
   git push origin main
   ```

2. **Go to Railway**
   - Visit [railway.app](https://railway.app)
   - Sign up with GitHub
   - Click "New Project"
   - Select "Deploy from GitHub repo"
   - Choose your repository

3. **Railway auto-detects Dockerfile** ✅

4. **Add Environment Variables** (if needed):
   ```
   PORT=8000
   DATABASE_PATH=/app/database/assembly_line.db
   ```

5. **Deploy!** Railway builds and deploys automatically

6. **Get your URL**: `https://your-app-name.railway.app`

**Done!** Your app is live! 🎉

---

## ⚡ Option 2: Render (Also Easy - 7 minutes)

1. **Push to GitHub**

2. **Go to Render**
   - Visit [render.com](https://render.com)
   - Sign up with GitHub
   - Click "New +" → "Web Service"
   - Connect your repository

3. **Configure**:
   - Name: `assembly-line-simulator`
   - Environment: **Docker**
   - Root Directory: `.` (root)

4. **Add Environment Variables**:
   ```
   PORT=8000
   DATABASE_PATH=/app/database/assembly_line.db
   ```

5. **Deploy!**

6. **Get URL**: `https://assembly-line-simulator.onrender.com`

**Done!** 🎉

---

## 📱 Deploy Frontend Separately (Recommended for Production)

### Frontend on Vercel (Free + Fast)

1. **Build frontend**:
   ```bash
   cd frontend
   npm run build
   ```

2. **Go to Vercel**
   - Visit [vercel.com](https://vercel.com)
   - Sign up with GitHub
   - "Add New Project"
   - Import your repository

3. **Configure**:
   - Root Directory: `frontend`
   - Build Command: `npm run build`
   - Output Directory: `dist`
   - Environment Variable:
     ```
     VITE_API_URL=https://your-api.railway.app
     ```

4. **Deploy!**

5. **Get URL**: `https://your-app.vercel.app`

**Now you have:**
- Frontend: Vercel (super fast CDN)
- Backend: Railway/Render
- Total cost: **FREE** (on free tiers)

---

## 🔧 Test Locally with Docker

Before deploying, test locally:

```bash
# Build image
docker build -t assembly-line .

# Run
docker run -p 8000:8000 assembly-line

# Test
curl http://localhost:8000/health
```

---

## ✅ Checklist Before Deploying

- [ ] Code pushed to GitHub
- [ ] Dockerfile in root directory
- [ ] `.dockerignore` file created
- [ ] Environment variables documented
- [ ] Frontend API URL uses `VITE_API_URL`
- [ ] CORS configured for production domains
- [ ] Database path set correctly

---

## 🆘 Troubleshooting

**Build fails?**
- Check Dockerfile syntax
- Verify all dependencies in requirements.txt
- Check build logs in hosting dashboard

**API not working?**
- Verify port 8000 is exposed
- Check environment variables
- Review logs for errors

**Frontend can't connect?**
- Set `VITE_API_URL` environment variable
- Update CORS origins in `api/main.py`
- Check network tab in browser console

---

## 💰 Cost Summary

| Option | Cost | Always On |
|--------|------|-----------|
| Railway Free | $0 | ⚠️ Sleeps after inactivity |
| Railway Starter | $5/mo | ✅ Yes |
| Render Free | $0 | ⚠️ Sleeps after 15min |
| Render Starter | $7/mo | ✅ Yes |
| Vercel (Frontend) | $0 | ✅ Yes |
| **Recommended** | **$5-7/mo** | ✅ Always on |

---

## 🎯 Recommended Setup for Production

**Best Performance & Cost:**

1. **Backend**: Railway Starter ($5/mo)
2. **Frontend**: Vercel (FREE)
3. **Database**: Railway's persistent disk (included)

**Total: ~$5/month** 🎉

---

## 📚 Next Steps

- See `DEPLOYMENT.md` for detailed guides
- Check `deploy/railway.md` for Railway-specific tips
- See `deploy/render.md` for Render setup
- See `deploy/vps.md` for VPS deployment

**Need help?** Check the deployment docs or open an issue!

