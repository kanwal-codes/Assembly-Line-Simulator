# 📝 Vercel Deployment Configuration

Use these exact settings when setting up your project on Vercel.

## 🎯 Vercel Deployment Settings

### Step 1: Import Repository
- Go to [vercel.com](https://vercel.com)
- Click **"Add New Project"**
- Click **"Import Git Repository"**
- Select: `Assembly-Line-Simulator`
- Click **"Import"**

### Step 2: Configure Project Settings

#### **Framework Preset**
- Select: **"Vite"** (auto-detected)

#### **Root Directory**
- Click **"Edit"** button
- Enter: `frontend`
- This tells Vercel where your frontend code is

#### **Build and Output Settings**
```
Build Command: npm run build
Output Directory: dist
Install Command: npm install
```

These should auto-fill, but verify they're correct.

#### **Environment Variables**
**Click "Add" and add this variable:**

| Name | Value | Environments |
|------|-------|--------------|
| `VITE_API_URL` | `https://your-backend.fly.dev` | ☑️ Production ☑️ Preview ☑️ Development |

**Important:** Replace `your-backend.fly.dev` with your actual Fly.io backend URL!

If you haven't deployed backend yet, you can:
1. Set it to `http://localhost:8000` for now
2. Update it later in Vercel settings

### Step 3: Deploy!

1. Click **"Deploy"** button
2. Wait 1-2 minutes for build
3. Vercel will show you the live URL!

---

## 📋 Quick Checklist for Vercel

- [ ] Root Directory: `frontend`
- [ ] Framework: Vite (auto-detected)
- [ ] Build Command: `npm run build`
- [ ] Output Directory: `dist`
- [ ] Environment Variable: `VITE_API_URL` set to your backend URL
- [ ] Deployed successfully ✅

---

## 🔧 After Deployment

### Get Your Vercel URL
After deployment, Vercel gives you:
- **Production URL**: `https://your-app.vercel.app`
- This is your live frontend!

### Update Backend CORS
Once you have your Vercel URL, update backend CORS:

**On Fly.io:**
```bash
fly secrets set CORS_ORIGINS=https://your-app.vercel.app
```

Replace `your-app.vercel.app` with your actual Vercel URL.

---

## 🆘 Troubleshooting

### "Build Failed"
- Check build logs in Vercel
- Ensure `frontend/package.json` exists
- Verify Node.js version (Vercel uses 18.x by default)

### "Cannot find module"
- Make sure `frontend/node_modules` exists (or will be installed)
- Check `package.json` has all dependencies

### "API connection failed"
- Verify `VITE_API_URL` environment variable is set
- Check backend is running on Fly.io
- Test backend URL: `curl https://your-backend.fly.dev/health`

---

## ✅ That's It!

Once deployed, every time you push to GitHub:
- Vercel **automatically deploys** your changes
- No manual steps needed! 🎉

