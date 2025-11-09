# 📝 Step-by-Step: Deploy Frontend to Vercel

## ✅ Prerequisites (Already Done!)

- ✅ Code pushed to GitHub
- ✅ Backend deployed to Fly.io: `https://assembly-line-simulator.fly.dev`

---

## 🚀 Step-by-Step Instructions

### **Step 1: Go to Vercel**

1. Open your browser
2. Go to: **[vercel.com](https://vercel.com)**
3. Click **"Sign Up"** or **"Log In"** (top right)
4. Choose **"Continue with GitHub"**
5. Authorize Vercel to access your GitHub account

---

### **Step 2: Create New Project**

1. After logging in, you'll see your dashboard
2. Click the big **"Add New..."** button (top right)
3. Or click **"New Project"** button

---

### **Step 3: Import Your Repository**

1. You'll see a list of your GitHub repositories
2. Find: **`kanwal-codes/Assembly-Line-Simulator`**
3. Click **"Import"** button next to it

---

### **Step 4: Configure Project Settings**

Now you're on the project configuration page. **IMPORTANT:** Change these settings:

#### **4A. Framework Preset**
- Should auto-detect: **"Vite"**
- ✅ Leave as is (or select "Vite" from dropdown)

#### **4B. Root Directory** ⚠️ **IMPORTANT!**
- You'll see: **Root Directory: `.`**
- Click **"Edit"** link next to it
- **Change from:** `.`
- **Change to:** `frontend`
- Click outside or press Enter
- ✅ This tells Vercel where your frontend code is!

#### **4C. Build Settings** (Should auto-fill, just verify)
```
Build Command: npm run build
Output Directory: dist
Install Command: npm install
```

✅ These should be correct already

---

### **Step 5: Add Environment Variable** ⚠️ **CRITICAL!**

**Before clicking Deploy:**

1. Scroll down to **"Environment Variables"** section
2. Click **"Add"** button
3. In the popup, enter:
   - **Name:** `VITE_API_URL`
   - **Value:** `https://assembly-line-simulator.fly.dev`
   - **Environments:** Check **ALL THREE** boxes:
     - ☑️ Production
     - ☑️ Preview
     - ☑️ Development
4. Click **"Add"** button to save

---

### **Step 6: Deploy!**

1. Scroll to the bottom of the page
2. Review your settings one more time:
   - ✅ Root Directory: `frontend`
   - ✅ Environment Variable: `VITE_API_URL` added
3. Click the big blue **"Deploy"** button
4. **Wait 1-2 minutes** while Vercel:
   - Installs dependencies
   - Builds your app
   - Deploys to CDN

---

### **Step 7: Get Your Live URL**

After deployment completes:
1. You'll see: **"Congratulations!"** message
2. Vercel gives you a URL like:
   - `https://assembly-line-simulator.vercel.app`
3. **Click the URL** to visit your live app!

---

## ✅ Verification Checklist

After deployment, verify:

- [ ] Can visit your Vercel URL
- [ ] Frontend loads without errors
- [ ] Open browser console (F12) - no CORS errors
- [ ] Try clicking "Run Simulation" button
- [ ] Stats should load from backend

---

## 🔧 Update Backend CORS (After Getting Vercel URL)

Once you have your Vercel URL, update backend to allow requests:

```bash
fly secrets set CORS_ORIGINS=https://assembly-line-simulator.vercel.app
```

**Replace `assembly-line-simulator.vercel.app` with your actual Vercel URL!**

---

## 📋 Quick Settings Summary

Copy this for reference:

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
VERCEL CONFIGURATION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Repository: kanwal-codes/Assembly-Line-Simulator
Framework: Vite
Root Directory: frontend
Build Command: npm run build
Output Directory: dist

Environment Variable:
  Name: VITE_API_URL
  Value: https://assembly-line-simulator.fly.dev
  Environments: ✅ All three

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

## 🆘 Troubleshooting

### "Build Failed"

**Check:**
- Root Directory is set to `frontend` (not `.`)
- `frontend/package.json` exists
- Check build logs in Vercel dashboard

### "Cannot connect to API"

**Check:**
- Environment variable `VITE_API_URL` is set correctly
- Backend is running: `curl https://assembly-line-simulator.fly.dev/health`
- CORS is configured (update with Vercel URL)

### "Module not found"

**Fix:**
- Vercel should auto-install dependencies
- Check build logs for missing packages
- Verify `frontend/package.json` has all dependencies

---

## 🎉 You're Done!

Once deployed:
- ✅ Frontend: `https://your-app.vercel.app`
- ✅ Backend: `https://assembly-line-simulator.fly.dev`
- ✅ API Docs: `https://assembly-line-simulator.fly.dev/docs`
- ✅ **Total Cost: $0/month!**

**Your app is live and ready to use!** 🚀




