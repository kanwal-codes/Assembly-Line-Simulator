# 🔧 Troubleshooting Guide

## Issues Fixed During Development

### 1. Backend Health Check Failing
**Problem**: Health check returned "unhealthy" when database didn't exist.

**Solution**: Updated health check to return "healthy" even when database is not initialized. Database is created automatically on first simulation run.

**Files Changed**: `api/main.py`

---

### 2. Endpoints Returning 500 Errors
**Problem**: `/orders` and `/stations` endpoints returned 500 errors when database didn't exist.

**Solution**: Added database existence check before querying. Endpoints now return empty arrays `[]` when database doesn't exist.

**Files Changed**: `api/main.py`

---

### 3. CORS Errors
**Problem**: Frontend couldn't connect to backend due to CORS policy.

**Solution**: 
- Configured CORS to allow all origins in production (`allow_origins=["*"]`)
- Set `allow_credentials=False` when using wildcard origins
- Works with any Vercel domain automatically

**Files Changed**: `api/main.py`

---

### 4. WebSocket Connection Failed
**Problem**: WebSocket URL conversion didn't handle `https` → `wss`.

**Solution**: Updated URL conversion to properly handle both `http` → `ws` and `https` → `wss`.

**Files Changed**: `frontend/src/components/Dashboard.tsx`

---

### 5. Vercel Build Failures
**Problem**: TypeScript errors preventing Vercel deployment.

**Issues Fixed**:
- Missing `import.meta.env` type definitions
- Unused imports causing errors
- `NodeJS.Timeout` type not found
- Unused variables

**Solutions**:
- Created `frontend/src/vite-env.d.ts` for environment variable types
- Removed unused imports
- Changed `NodeJS.Timeout` to `ReturnType<typeof setTimeout>`
- Fixed unused variable warnings

**Files Changed**: 
- `frontend/src/vite-env.d.ts` (created)
- `frontend/src/App.tsx`
- `frontend/src/components/Dashboard.tsx`
- `frontend/src/components/Statistics.tsx`
- `frontend/tsconfig.json`

---

### 6. Windows CI Build Failures
**Problem**: SQLite3 library not found on Windows CI.

**Solution**: 
- Disabled Windows builds (complexity not worth it)
- Production uses Docker which includes SQLite3
- CI now focuses on Linux and macOS

**Files Changed**: `.github/workflows/ci.yml`

---

### 7. Fly.io Deployment Workflow Issues
**Problem**: Deployment workflow failing due to missing app name and token handling.

**Solution**:
- Added app name to deploy command
- Added graceful handling when `FLY_API_TOKEN` is not set
- Added path filters to only deploy when backend files change

**Files Changed**: `.github/workflows/fly-deploy.yml`

---

## Common Issues & Solutions

### Backend Not Responding

**Check Status**:
```bash
fly status --app assembly-line-simulator
fly logs --app assembly-line-simulator
```

**Restart**:
```bash
fly apps restart assembly-line-simulator
```

---

### Frontend Can't Connect to Backend

**Check 1**: Verify backend is running
```bash
curl https://assembly-line-simulator.fly.dev/health
```

**Check 2**: Verify environment variable
- Go to Vercel Dashboard → Settings → Environment Variables
- Ensure `VITE_API_URL` is set to `https://assembly-line-simulator.fly.dev`
- **Important**: Redeploy after adding/changing environment variables

**Check 3**: Check browser console
- Open DevTools (F12)
- Look for CORS errors or network failures
- Check Network tab for failed requests

---

### Database Not Created

**Issue**: Database shows "not_initialized" in health check.

**Solution**: This is normal! Database is created automatically when you run the first simulation. Just click "Run Simulation" in the frontend.

---

### Build Failures

**Vercel Build Fails**:
- Check build logs in Vercel dashboard
- Verify `frontend/package.json` has all dependencies
- Ensure `frontend/vercel.json` exists

**Local Build Fails**:
```bash
# Clean and rebuild
make clean
make release
```

---

### CORS Errors

**Error**: `Access to fetch blocked by CORS policy`

**Solution**: 
- Backend is configured to allow all origins
- If error persists, check browser console for exact error message
- Verify backend is returning CORS headers:
```bash
curl -v -H "Origin: https://test.vercel.app" \
     https://assembly-line-simulator.fly.dev/stats
```

---

## Quick Fixes

### Reset Everything
```bash
# Stop all services
pkill -f uvicorn
pkill -f vite

# Clean build
make clean
make release

# Restart API
cd api && source venv/bin/activate && python main.py

# Restart frontend (new terminal)
cd frontend && npm run dev
```

### Test Connection
Use the test file: `test-connection.html` - open in browser to test all endpoints.

---

## Still Having Issues?

1. Check logs:
   - Backend: `fly logs --app assembly-line-simulator`
   - Frontend: Vercel deployment logs

2. Verify configuration:
   - Backend URL is correct
   - Environment variables are set
   - CORS is configured

3. Test endpoints directly:
   ```bash
   curl https://assembly-line-simulator.fly.dev/health
   curl https://assembly-line-simulator.fly.dev/stats
   ```
