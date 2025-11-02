# 🚂 Deploy to Railway.app

Railway is the easiest way to deploy your full-stack application.

## Step-by-Step Guide

### 1. Prepare Your Project

Make sure you have:
- ✅ `Dockerfile` in root directory
- ✅ All dependencies listed
- ✅ `.dockerignore` file
- ✅ Git repository pushed to GitHub

### 2. Sign Up

1. Go to [railway.app](https://railway.app)
2. Sign up with GitHub
3. Click "New Project"

### 3. Deploy from GitHub

1. Click "Deploy from GitHub repo"
2. Select your repository
3. Railway will auto-detect the Dockerfile

### 4. Configure Environment Variables

In Railway dashboard, go to "Variables" and add:

```env
PORT=8000
DATABASE_PATH=/app/database/assembly_line.db
PYTHONUNBUFFERED=1
```

### 5. Deploy Frontend (Optional - Separate Service)

If deploying frontend separately:

1. Build frontend locally:
   ```bash
   cd frontend
   npm run build
   ```

2. Create new service in Railway
3. Use static site deployment
4. Point to `frontend/dist`
5. Set environment variable:
   ```
   VITE_API_URL=https://your-api.railway.app
   ```

### 6. Get Your URL

Railway will provide:
- `https://your-app-name.railway.app`

### 7. Update CORS (if frontend on different domain)

In `api/main.py`, update CORS origins:
```python
app.add_middleware(
    CORSMiddleware,
    allow_origins=["https://your-frontend.railway.app"],
    ...
)
```

## Cost

- **Free Tier**: $5 credit/month (enough for small projects)
- **Hobby**: $5-20/month
- **Pro**: $20+/month

## Tips

- Use Railway's built-in PostgreSQL instead of SQLite for production
- Enable "Deploy on Push" for auto-deployments
- Use Railway's CLI for easy management

## Troubleshooting

**Build fails?**
- Check Dockerfile syntax
- Ensure all dependencies are listed
- Check build logs in Railway dashboard

**API not accessible?**
- Check port is exposed (8000)
- Verify environment variables
- Check logs for errors

