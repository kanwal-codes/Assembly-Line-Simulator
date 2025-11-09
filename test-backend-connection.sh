#!/bin/bash

echo "🔍 Testing Backend Connection..."
echo "================================"
echo ""

BACKEND_URL="https://assembly-line-simulator.fly.dev"

echo "1. Testing Health Endpoint..."
HEALTH=$(curl -s "$BACKEND_URL/health")
if echo "$HEALTH" | grep -q "healthy"; then
    echo "   ✅ Health check passed"
    echo "$HEALTH" | python3 -m json.tool 2>/dev/null || echo "$HEALTH"
else
    echo "   ❌ Health check failed"
    echo "$HEALTH"
fi
echo ""

echo "2. Testing Stats Endpoint..."
STATS=$(curl -s "$BACKEND_URL/stats")
if echo "$STATS" | grep -q "total_orders"; then
    echo "   ✅ Stats endpoint working"
    echo "$STATS" | python3 -m json.tool 2>/dev/null || echo "$STATS"
else
    echo "   ❌ Stats endpoint failed"
    echo "$STATS"
fi
echo ""

echo "3. Testing CORS Headers..."
CORS=$(curl -s -H "Origin: https://test.vercel.app" \
     -H "Access-Control-Request-Method: GET" \
     -X OPTIONS "$BACKEND_URL/stats" -v 2>&1 | grep -i "access-control")
if echo "$CORS" | grep -qi "access-control-allow-origin"; then
    echo "   ✅ CORS headers present"
    echo "$CORS"
else
    echo "   ⚠️  CORS headers not found"
fi
echo ""

echo "4. Testing from Browser-like Request..."
BROWSER_TEST=$(curl -s -H "Origin: https://test.vercel.app" \
     -H "Content-Type: application/json" \
     "$BACKEND_URL/stats")
if echo "$BROWSER_TEST" | grep -q "total_orders"; then
    echo "   ✅ Browser-like request successful"
    echo "$BROWSER_TEST" | python3 -m json.tool 2>/dev/null || echo "$BROWSER_TEST"
else
    echo "   ❌ Browser-like request failed"
    echo "$BROWSER_TEST"
fi
echo ""

echo "================================"
echo "✅ Backend Connection Test Complete!"
echo ""
echo "If all tests passed, your backend is ready for frontend connection."
echo "Make sure your Vercel frontend has:"
echo "  - Environment variable: VITE_API_URL=$BACKEND_URL"
echo "  - Deployed and accessible"
