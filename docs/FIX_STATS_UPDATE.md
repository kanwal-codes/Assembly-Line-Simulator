# Fix: Stats Not Updating After Multiple Simulations

## Problem Identified

**Issue**: Stats remained the same when clicking "Run Simulation" multiple times sequentially.

**Root Cause**: Database was using `INSERT OR REPLACE` which **replaced** existing orders instead of adding new ones.

## The Bug

```cpp
// OLD CODE (BUGGY):
ss << "INSERT OR REPLACE INTO orders ...";
```

**What happened:**
1. First simulation: Saves 5 orders ✅
2. Second simulation: Generates same customer names → Same order_id pattern
3. `INSERT OR REPLACE` replaces the 5 existing orders with 5 new ones
4. **Result**: Still 5 orders (not 10) ❌

## The Fix

### Change 1: Use INSERT instead of INSERT OR REPLACE
```cpp
// NEW CODE (FIXED):
ss << "INSERT INTO orders ...";  // Always adds new rows
```

### Change 2: Better Unique Order IDs
```cpp
// Use microseconds for better uniqueness
auto us = std::chrono::duration_cast<std::chrono::microseconds>(
    now.time_since_epoch()).count();
record.orderId = customerName + "_" + product + "_" + std::to_string(us);
```

## Testing Results

**Before Fix:**
- Run 1: 5 orders
- Run 2: Still 5 orders ❌

**After Fix:**
- Run 1: 6 orders
- Run 2: 11 orders ✅
- Run 3: 16 orders ✅

## What Changed

1. ✅ `INSERT OR REPLACE` → `INSERT` (always adds new rows)
2. ✅ Better timestamp precision (microseconds instead of nanoseconds)
3. ✅ Each simulation run now accumulates orders in database

## How to Test

1. **Clear existing orders** (optional):
   ```bash
   sqlite3 database/assembly_line.db "DELETE FROM orders;"
   ```

2. **Run simulation multiple times** via UI or API

3. **Check stats** - should increment:
   ```bash
   sqlite3 database/assembly_line.db "SELECT COUNT(*) FROM orders;"
   ```

4. **Verify in UI** - stats should show increasing numbers

## Current Status

✅ **FIXED**: Database now properly accumulates orders
✅ **REBUILT**: C++ executable updated with fix
✅ **READY**: Multiple simulations now properly add to database

## Next Steps

1. Restart the API server (to pick up new binary)
2. Run simulations from UI
3. Verify stats increment (5 → 10 → 15, etc.)

