# 🔗 Frontend-Backend Connection Guide

## Current Setup

- **Backend URL**: `https://assembly-line-simulator.fly.dev`
- **Frontend**: Vercel (your Vercel URL)

## ✅ Connection Checklist

### 1. Backend is Running
```bash
curl https://assembly-line-simulator.fly.dev/health
```
Should return: `{"status":"healthy",...}`

### 2. CORS is Configured
- Backend allows all origins (`allow_origins=["*"]`)
- No credentials needed (simpler setup)
- Works with any Vercel domain

### 3. Frontend Environment Variable
In Vercel Dashboard → Settings → Environment Variables:
- **Name**: `VITE_API_URL`
- **Value**: `https://assembly-line-simulator.fly.dev`
- **Environments**: Production, Preview, Development

### 4. Test Connection

#### Test from Browser Console
1. Open your Vercel frontend URL
2. Open Browser DevTools (F12)
3. Go to Console tab
4. Run:
```javascript
fetch('https://assembly-line-simulator.fly.dev/stats')
  .then(r => r.json())
  .then(data => console.log('✅ Connected!', data))
  .catch(err => console.error('❌ Error:', err))
```

#### Test from Terminal
```bash
# Test CORS
curl -H "Origin: https://your-app.vercel.app" \
     https://assembly-line-simulator.fly.dev/stats

# Should return JSON data
```

## 🔧 Troubleshooting

### Issue: "Network Error" or CORS Error

**Check 1: Backend is running**
```bash
curl https://assembly-line-simulator.fly.dev/health
```

**Check 2: CORS headers**
```bash
curl -v -H "Origin: https://your-app.vercel.app" \
     https://assembly-line-simulator.fly.dev/stats 2>&1 | grep -i "access-control"
```

Should see:
- `access-control-allow-origin: *`
- `access-control-allow-methods: *`

**Check 3: Frontend environment variable**
- Go to Vercel Dashboard
- Settings → Environment Variables
- Verify `VITE_API_URL` is set correctly
- Redeploy if you just added it

**Check 4: Browser console**
- Open DevTools (F12)
- Check Console for errors
- Check Network tab for failed requests

### Issue: "404 Not Found"

- Verify backend URL is correct: `https://assembly-line-simulator.fly.dev`
- Check endpoint exists: `/stats`, `/orders`, `/health`

### Issue: WebSocket Connection Failed

- WebSocket URL should be: `wss://assembly-line-simulator.fly.dev/ws`
- Check browser console for WebSocket errors
- WebSocket is optional - app works without it

## 🧪 Quick Test Script

Save this as `test-connection.html` and open in browser:

```html
<!DOCTYPE html>
<html>
<head>
    <title>Backend Connection Test</title>
</head>
<body>
    <h1>Testing Backend Connection</h1>
    <div id="result"></div>
    <script>
        const backendUrl = 'https://assembly-line-simulator.fly.dev';
        
        async function testConnection() {
            const result = document.getElementById('result');
            result.innerHTML = 'Testing...';
            
            try {
                // Test health endpoint
                const health = await fetch(`${backendUrl}/health`);
                const healthData = await health.json();
                console.log('Health:', healthData);
                
                // Test stats endpoint
                const stats = await fetch(`${backendUrl}/stats`);
                const statsData = await stats.json();
                console.log('Stats:', statsData);
                
                result.innerHTML = `
                    <h2>✅ Connection Successful!</h2>
                    <pre>${JSON.stringify(statsData, null, 2)}</pre>
                `;
            } catch (error) {
                result.innerHTML = `
                    <h2>❌ Connection Failed</h2>
                    <pre>${error.message}</pre>
                `;
                console.error('Error:', error);
            }
        }
        
        testConnection();
    </script>
</body>
</html>
```

## 📝 Common Issues & Solutions

### 1. CORS Error in Browser
**Error**: `Access to fetch at '...' from origin '...' has been blocked by CORS policy`

**Solution**: 
- Backend CORS is configured to allow all origins
- If error persists, check browser console for exact error
- Verify backend is returning CORS headers

### 2. Environment Variable Not Working
**Issue**: Frontend still uses `http://localhost:8000`

**Solution**:
- In Vercel, go to Settings → Environment Variables
- Verify `VITE_API_URL` is set
- **Important**: Redeploy after adding/changing environment variables
- Check build logs to verify variable is included

### 3. Backend Returns 404
**Issue**: API endpoints return 404

**Solution**:
- Verify backend is running: `curl https://assembly-line-simulator.fly.dev/health`
- Check endpoint path is correct
- Verify backend was deployed successfully

## ✅ Success Indicators

When everything works:
1. ✅ Frontend loads without errors
2. ✅ Dashboard shows stats (even if 0)
3. ✅ "Run Simulation" button works
4. ✅ Orders/Stations pages load data
5. ✅ No CORS errors in browser console
6. ✅ Network tab shows successful API calls

## 🚀 Next Steps

Once connection is working:
1. Run a simulation from frontend
2. Check that data appears in dashboard
3. Verify orders are saved to database
4. Test all frontend pages

