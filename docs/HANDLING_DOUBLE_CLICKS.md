# Handling Double Clicks on "Run Simulation"

## Current Behavior

### What Happens If You Click Twice?

**Scenario 1: Rapid Double Click**
- **First click**: Starts simulation, button disabled
- **Second click**: **Ignored** (button is disabled and `simulationRunning` prevents it)

**Scenario 2: Click While Simulation Running**
- Button is disabled
- Click is ignored
- First simulation continues normally

**Scenario 3: Click Right After Previous Completes**
- Previous simulation finishes → stats refresh → button re-enabled
- New click starts another simulation
- Each simulation adds more orders to database

## Protection Mechanisms

### Frontend Protection:
1. **Button Disabled State**: 
   ```typescript
   disabled={simulationRunning}
   ```
   - Button becomes unclickable while simulation runs

2. **State Check**:
   ```typescript
   if (simulationRunning) {
     return // Ignore click
   }
   ```
   - Even if somehow clicked, function returns early

3. **Visual Feedback**:
   - Button shows "⏳ Running..." 
   - Progress message appears
   - User knows simulation is active

### Backend Protection:
- Each simulation runs independently
- Multiple simulations would run sequentially if queued
- Database handles concurrent writes safely (SQLite)

## What Actually Happens

### Single Click:
1. ✅ Button disabled
2. ✅ API request sent
3. ✅ C++ simulation runs
4. ✅ Database updated
5. ✅ Stats refreshed
6. ✅ Button re-enabled

### Double Click (Rapid):
1. ✅ First click: Simulation starts
2. ✅ Second click: **Ignored** (button disabled)
3. ✅ Only one simulation runs

### Click During Execution:
1. ✅ Current simulation continues
2. ✅ New click: **Ignored** (simulationRunning = true)
3. ✅ No duplicate requests sent

## Edge Cases

### What If User Refreshes Page Mid-Simulation?
- Frontend state lost
- Backend simulation continues (subprocess runs independently)
- When page reloads, stats reflect final state

### What If Multiple Tabs Open?
- Each tab has independent state
- Each can trigger simulations
- All simulations save to same database
- Stats show cumulative results

### What If Network Is Slow?
- Request sent, waiting for response
- Button stays disabled
- User can't click again (protected)
- Stats refresh when simulation completes

## Recommendations

### Current Implementation: ✅ Safe

The current code properly handles:
- ✅ Disabled button prevents clicks
- ✅ State check prevents duplicate runs
- ✅ Clear visual feedback
- ✅ Error handling

### Possible Enhancements (Future):
- Queue system for multiple requests
- Cancel previous simulation option
- Progress bar instead of just message
- Show "X simulations running" counter

## Testing

To test double-click protection:
1. Click "Run Simulation"
2. Immediately try to click again
3. **Expected**: Button is disabled, second click ignored
4. Wait for completion
5. Click again
6. **Expected**: New simulation starts normally

## Summary

**Double clicking is safe!** The implementation prevents:
- ❌ Duplicate API requests
- ❌ Concurrent simulations (frontend prevents)
- ❌ Database conflicts (SQLite handles safely)
- ❌ UI confusion (clear feedback)

You can click multiple times in sequence (after each completes), but rapid double-clicks are ignored. This is the correct behavior!

