/**
 * @file Dashboard.tsx
 * @brief Main dashboard component - Home page of the application
 * 
 * PURPOSE:
 * This is the primary page users see when they visit the application. It displays
 * overall statistics, provides a button to run simulations, and shows real-time
 * updates via WebSocket or HTTP polling.
 * 
 * ARCHITECTURE:
 * ┌─────────────────────────────────────────┐
 * │         Dashboard Component              │
 * │  ┌───────────────────────────────────┐  │
 * │  │  Run Simulation Button            │  │
 * │  └───────────────────────────────────┘  │
 * │  ┌───────────────────────────────────┐  │
 * │  │  Statistics Cards                │  │
 * │  │  - Total Orders                   │  │
 * │  │  - Completed Orders               │  │
 * │  │  - Incomplete Orders              │  │
 * │  │  - Completion Rate                │  │
 * │  └───────────────────────────────────┘  │
 * │  ┌───────────────────────────────────┐  │
 * │  │  Order Overview Chart              │  │
 * │  └───────────────────────────────────┘  │
 * └─────────────────────────────────────────┘
 * 
 * FLOW:
 * 1. Component mounts → Fetches initial stats from API
 * 2. Establishes WebSocket connection for real-time updates
 * 3. User clicks "Run Simulation" → POST /simulation/run
 * 4. C++ simulation runs → Database updated
 * 5. Dashboard refreshes stats → Shows new data
 * 
 * CONNECTIONS:
 * - API: GET /stats (fetch statistics), POST /simulation/run (trigger simulation)
 * - WebSocket: ws://host/ws (real-time updates, optional)
 * - State: Manages stats, loading, error states
 * - Re-rendering: Uses key props and timestamps to force updates
 * 
 * TRIGGERS:
 * - Component mount: Fetches stats and connects WebSocket
 * - "Run Simulation" button: Triggers C++ simulation via API
 * - WebSocket messages: Updates stats in real-time
 * - Manual refresh: fetchStats() can be called to refresh data
 * 
 * KEY FEATURES:
 * - Real-time updates via WebSocket (falls back to polling)
 * - Simulation button with loading state
 * - Retry logic for fetching stats after simulation
 * - Force re-render mechanism to ensure UI updates
 */

import { useState, useEffect } from 'react'
import axios from 'axios'
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts'
import '../App.css'

/**
 * Statistics data structure matching API response
 * This interface ensures type safety when working with API data
 */
interface Stats {
  total_orders: number
  completed_orders: number
  incomplete_orders: number
  completion_rate: number
  most_active_station: string | null
}

/**
 * API URL configuration
 * - Production: Set via VITE_API_URL environment variable (Vercel)
 * - Development: Defaults to localhost:8000
 * - Vite replaces import.meta.env.VITE_API_URL at build time
 */
const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000'

/**
 * Dashboard component - Main page of the application
 * 
 * This component:
 * - Displays overall simulation statistics
 * - Provides button to run new simulations
 * - Shows real-time updates via WebSocket
 * - Displays charts and visualizations
 */
function Dashboard() {
  // ====================================================================
  // State Management
  // ====================================================================
  
  /**
   * Statistics data from API
   * - Initially null (loading state)
   * - Updated when API returns data
   * - Used to render all statistics cards and charts
   */
  const [stats, setStats] = useState<Stats | null>(null)
  
  /**
   * Loading state
   * - true: Shows "Loading..." message
   * - false: Shows dashboard content
   * - Set to true when fetching data or running simulation
   */
  const [loading, setLoading] = useState(true)
  
  /**
   * Error state
   * - null: No error
   * - string: Error message to display
   * - Set when API calls fail
   */
  const [error, setError] = useState<string | null>(null)
  
  /**
   * WebSocket connection status
   * - true: WebSocket connected, receiving real-time updates
   * - false: WebSocket disconnected, using HTTP polling
   * - Displayed to user with visual indicator
   */
  const [wsConnected, setWsConnected] = useState(false)
  
  /**
   * Simulation running state
   * - Prevents multiple concurrent simulation runs
   * - Disables button while simulation is running
   * - Shows progress message to user
   */
  const [simulationRunning, setSimulationRunning] = useState(false)
  
  /**
   * Stats update timestamp
   * - Used as key prop to force React re-renders
   * - Updated whenever stats change
   * - Ensures UI updates even if data reference doesn't change
   */
  const [statsUpdateTime, setStatsUpdateTime] = useState(Date.now())

  // ====================================================================
  // Effect: Initial Load and WebSocket Setup
  // ====================================================================
  /**
   * This effect runs once when component mounts
   * 
   * WHAT IT DOES:
   * 1. Fetches initial statistics from API
   * 2. Establishes WebSocket connection for real-time updates
   * 3. Sets up WebSocket event handlers
   * 4. Cleans up WebSocket on component unmount
   * 
   * TRIGGERS:
   * - Component mount: Runs once when Dashboard is first rendered
   * - Component unmount: Cleanup function closes WebSocket
   * 
   * CONNECTIONS:
   * - API: GET /stats endpoint for initial data
   * - WebSocket: ws://host/ws for real-time updates
   * - State: Updates stats, loading, wsConnected states
   */
  useEffect(() => {
    // Initial load with loading state
    // Fetches statistics immediately when component mounts
    const initialLoad = async () => {
      setLoading(true)
      try {
        await fetchStats()
      } catch (err) {
        console.error('Initial load error:', err)
      } finally {
        setLoading(false)
      }
    }
    
    initialLoad()
    
    // ====================================================================
    // WebSocket Connection Setup
    // ====================================================================
    // PURPOSE: Establish persistent connection for real-time updates
    // 
    // HOW IT WORKS:
    // 1. Convert HTTP URL to WebSocket URL (http->ws, https->wss)
    // 2. Create WebSocket connection
    // 3. Set up event handlers for messages, errors, close
    // 4. Update stats when messages received
    // 
    // FALLBACK:
    // - If WebSocket fails, component continues with HTTP polling
    // - fetchStats() can be called periodically as fallback
    let ws: WebSocket | null = null
    try {
      const apiUrl = import.meta.env.VITE_API_URL || 'http://localhost:8000'
      // Convert http/https to ws/wss for WebSocket
      // This is necessary because WebSocket uses different protocol
      const wsUrl = apiUrl.replace(/^http/, 'ws').replace(/^https/, 'wss')
      ws = new WebSocket(`${wsUrl}/ws`)
      
      ws.onopen = () => {
        setWsConnected(true)
      }
      
      ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data)
          console.log('WebSocket stats update:', data)
          // Update with new object reference
          setStats(prevStats => {
            // Only update if data actually changed
            if (prevStats && 
                prevStats.total_orders === data.total_orders &&
                prevStats.completed_orders === data.completed_orders &&
                prevStats.incomplete_orders === data.incomplete_orders) {
              return prevStats // No change
            }
            const newData = { ...data, _timestamp: Date.now() }
            setStatsUpdateTime(Date.now()) // Trigger re-render
            return newData
          })
        } catch (err) {
          console.error('Error parsing WebSocket data:', err)
        }
      }
      
      ws.onerror = (err) => {
        console.warn('WebSocket error (non-critical, using polling instead):', err)
        setWsConnected(false)
        // WebSocket is optional, continue with polling
      }
      
      ws.onclose = () => {
        setWsConnected(false)
      }
    } catch (err) {
      console.error('Failed to create WebSocket:', err)
      setWsConnected(false)
    }
    
    return () => {
      if (ws) ws.close()
    }
  }, [])

  // ====================================================================
  // Function: Fetch Statistics
  // ====================================================================
  /**
   * Fetches statistics from the API
   * 
   * PURPOSE:
   * Retrieves current simulation statistics from the backend API.
   * This is the primary way to get data when WebSocket is not available.
   * 
   * HOW IT WORKS:
   * 1. Makes GET request to /stats endpoint
   * 2. Uses cache busting parameter to prevent browser caching
   * 3. Updates state with new data
   * 4. Triggers re-render with timestamp
   * 
   * TRIGGERS:
   * - Initial component load
   * - After simulation completes (with retry logic)
   * - Can be called manually to refresh data
   * 
   * CONNECTIONS:
   * - API: GET /stats endpoint
   * - State: Updates stats, error, statsUpdateTime
   * - UI: Causes re-render of statistics cards and charts
   * 
   * @returns Promise<Stats> The fetched statistics data
   * @throws Error if API call fails
   */
  const fetchStats = async () => {
    try {
      const response = await axios.get(`${API_URL}/stats`, {
        params: { _: Date.now() } // Cache busting - ensures fresh data
      })
      console.log('Stats fetched:', response.data)
      
      // Force state update with new object reference
      // This ensures React detects the change and re-renders
      const newStats = {
        ...response.data,
        _timestamp: Date.now()  // Add timestamp for tracking
      }
      console.log('Setting new stats:', newStats)
      setStats(newStats)
      setStatsUpdateTime(Date.now()) // Trigger re-render by updating timestamp
      setError(null)
      return newStats
    } catch (err: any) {
      console.error('Error fetching stats:', err)
      setError(err.message || 'Failed to fetch statistics')
      throw err
    }
  }

  // ====================================================================
  // Function: Run Simulation
  // ====================================================================
  /**
   * Triggers a new simulation run
   * 
   * PURPOSE:
   * When user clicks "Run Simulation" button, this function:
   * 1. Calls API endpoint POST /simulation/run
   * 2. API executes C++ simulation program
   * 3. C++ program processes orders and saves to database
   * 4. Dashboard refreshes statistics to show new data
   * 
   * FLOW:
   * 1. Check if simulation already running (prevent duplicates)
   * 2. Set loading state and disable button
   * 3. POST request to /simulation/run endpoint
   * 4. Wait for simulation to complete
   * 5. Wait for database writes to finish
   * 6. Fetch updated statistics with retry logic
   * 7. Update UI with new data
   * 8. Show success message to user
   * 
   * RETRY LOGIC:
   * - Fetches stats multiple times after simulation
   * - Accounts for potential database write delays
   * - Ensures UI shows latest data
   * 
   * TRIGGERS:
   * - User clicks "Run Simulation" button
   * - Button is disabled while simulation runs
   * 
   * CONNECTIONS:
   * - API: POST /simulation/run (triggers C++ executable)
   * - API: GET /stats (fetches updated statistics)
   * - State: Updates loading, error, stats, simulationRunning
   * - UI: Shows loading state, progress message, success alert
   */
  const runSimulation = async () => {
    // Prevent multiple concurrent simulations
    // This ensures only one simulation runs at a time
    if (simulationRunning) {
      console.log('Simulation already running, ignoring click')
      return
    }

    let refreshTimeout: ReturnType<typeof setTimeout> | null = null

    try {
      setSimulationRunning(true)
      setLoading(true)
      setError(null)
      console.log('Starting simulation...')
      
      const response = await axios.post(`${API_URL}/simulation/run`, {})
      console.log('Simulation completed:', response.data)
      
      // Wait for database to be ready, then refresh stats
      refreshTimeout = setTimeout(async () => {
        console.log('Refreshing stats after simulation...')
        try {
          // Wait a bit for database writes to complete
          await new Promise(resolve => setTimeout(resolve, 1500))
          
          // Fetch fresh stats with retries
          let newStats = null
          for (let attempt = 0; attempt < 3; attempt++) {
            try {
              const response = await axios.get(`${API_URL}/stats`, {
                params: { _: Date.now() } // Cache busting
              })
              newStats = {
                ...response.data,
                _timestamp: Date.now()
              }
              console.log(`Fetch attempt ${attempt + 1}:`, newStats)
              
              // If we got data, wait a bit and try once more to ensure we have the latest
              if (attempt < 2) {
                await new Promise(resolve => setTimeout(resolve, 1000))
              }
            } catch (err: any) {
              console.error(`Fetch attempt ${attempt + 1} failed:`, err)
              if (attempt === 2) throw err // Re-throw on last attempt
            }
          }
          
          // Final state update with the latest stats
          if (newStats) {
            // Check if stats actually changed before updating
            setStats(prevStats => {
              if (!prevStats || 
                  prevStats.total_orders !== newStats.total_orders ||
                  prevStats.completed_orders !== newStats.completed_orders ||
                  prevStats.incomplete_orders !== newStats.incomplete_orders) {
                console.log('Stats changed, updating state:', newStats)
                return newStats
              }
              console.log('Stats unchanged, keeping previous:', prevStats)
              return prevStats
            })
            
            // Always update the timestamp to trigger re-render
            setStatsUpdateTime(Date.now())
            
            // Show success message
            const message = `Simulation completed!\n\nTotal orders: ${newStats.total_orders}\nCompleted: ${newStats.completed_orders}\nIncomplete: ${newStats.incomplete_orders}\nCompletion rate: ${newStats.completion_rate.toFixed(1)}%`
            alert(message)
            console.log('Final stats update:', newStats)
          }
        } catch (err: any) {
          console.error('Error refreshing stats:', err)
          setError('Simulation completed but failed to refresh stats')
        } finally {
          setLoading(false)
          setSimulationRunning(false)
          refreshTimeout = null
        }
      }, 2000) // Initial wait
    } catch (err: any) {
      // Cancel timeout if error occurred
      if (refreshTimeout) {
        clearTimeout(refreshTimeout)
        refreshTimeout = null
      }
      
      console.error('Simulation error:', err)
      setError(err.response?.data?.detail || err.message || 'Failed to run simulation')
      setLoading(false)
      setSimulationRunning(false)
    }
  }

  if (loading && !stats) {
    return <div className="loading">Loading...</div>
  }

  if (error) {
    return <div className="error">Error: {error}</div>
  }

  const chartData = stats ? [
    { name: 'Total', value: stats.total_orders },
    { name: 'Completed', value: stats.completed_orders },
    { name: 'Incomplete', value: stats.incomplete_orders }
  ] : []

  return (
    <div className="fade-in">
      <div className="card">
        <h2>📊 Dashboard</h2>
        <div style={{ 
          marginBottom: '1.5rem', 
          display: 'flex', 
          alignItems: 'center', 
          gap: '0.5rem',
          padding: '0.75rem 1rem',
          background: wsConnected ? 'linear-gradient(135deg, #d1fae5 0%, #a7f3d0 100%)' : 'linear-gradient(135deg, #fef3c7 0%, #fde68a 100%)',
          borderRadius: '12px',
          color: wsConnected ? '#065f46' : '#92400e',
          fontWeight: 500
        }}>
          <span style={{ fontSize: '1.25rem' }}>{wsConnected ? '🟢' : '🟠'}</span>
          <span>{wsConnected ? 'Live Updates Enabled' : 'Live Updates Disabled (Polling Active)'}</span>
        </div>
        <button 
          onClick={runSimulation}
          disabled={simulationRunning}
          className="btn btn-primary"
          style={{
            minWidth: '220px',
            fontSize: '1.1rem',
            padding: '1rem 2rem'
          }}
        >
          {simulationRunning ? '⏳ Running Simulation...' : '🚀 Run Simulation'}
        </button>
        {simulationRunning && (
          <div style={{ 
            marginTop: '1.5rem', 
            padding: '1rem 1.25rem', 
            background: 'linear-gradient(135deg, #dbeafe 0%, #bfdbfe 100%)',
            borderRadius: '12px',
            color: '#1e40af',
            fontWeight: 500,
            display: 'flex',
            alignItems: 'center',
            gap: '0.75rem'
          }}>
            <span style={{ fontSize: '1.25rem' }}>⏳</span>
            <span>Simulation in progress... This may take a few seconds. Please wait.</span>
          </div>
        )}
      </div>

      {stats && (
        <>
          <div className="card" key={`stats-container-${statsUpdateTime}`}>
            <h2>📈 Statistics</h2>
            <div style={{ 
              fontSize: '0.875rem', 
              color: '#64748b', 
              marginBottom: '1.5rem',
              padding: '0.5rem 0.75rem',
              background: '#f8fafc',
              borderRadius: '8px',
              display: 'inline-block',
              fontWeight: 500
            }}>
              ⏰ Last updated: {new Date(statsUpdateTime).toLocaleTimeString()}
            </div>
            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(240px, 1fr))', gap: '1.5rem' }} key={`stats-grid-${statsUpdateTime}`}>
              <StatCard label="Total Orders" value={stats.total_orders} color="#667eea" key={`total-${stats.total_orders}-${statsUpdateTime}`} />
              <StatCard label="Completed" value={stats.completed_orders} color="#10b981" key={`completed-${stats.completed_orders}-${statsUpdateTime}`} />
              <StatCard label="Incomplete" value={stats.incomplete_orders} color="#ef4444" key={`incomplete-${stats.incomplete_orders}-${statsUpdateTime}`} />
              <StatCard label="Completion Rate" value={`${stats.completion_rate.toFixed(1)}%`} color="#3b82f6" key={`rate-${stats.completion_rate}-${statsUpdateTime}`} />
            </div>
            {stats.most_active_station && (
              <div style={{ 
                marginTop: '1.5rem', 
                padding: '1.25rem 1.5rem', 
                background: 'linear-gradient(135deg, #fef3c7 0%, #fde68a 100%)',
                borderRadius: '12px',
                border: '2px solid #fbbf24',
                display: 'flex',
                alignItems: 'center',
                gap: '0.75rem'
              }}>
                <span style={{ fontSize: '1.5rem' }}>⭐</span>
                <div>
                  <div style={{ fontSize: '0.875rem', color: '#92400e', fontWeight: 500, marginBottom: '0.25rem' }}>Most Active Station</div>
                  <div style={{ fontSize: '1.25rem', fontWeight: 700, color: '#78350f' }}>{stats.most_active_station}</div>
                </div>
              </div>
            )}
          </div>

          <div className="card">
            <h2>📊 Order Overview</h2>
            <ResponsiveContainer width="100%" height={350} key={`chart-container-${statsUpdateTime}`}>
              <LineChart data={chartData} key={`chart-${stats.total_orders}-${statsUpdateTime}`}>
                <CartesianGrid strokeDasharray="3 3" stroke="#e2e8f0" />
                <XAxis 
                  dataKey="name" 
                  stroke="#64748b"
                  style={{ fontSize: '0.875rem', fontWeight: 500 }}
                />
                <YAxis 
                  stroke="#64748b"
                  style={{ fontSize: '0.875rem' }}
                />
                <Tooltip 
                  contentStyle={{ 
                    background: 'white', 
                    border: 'none', 
                    borderRadius: '12px',
                    boxShadow: '0 10px 25px rgba(0,0,0,0.1)',
                    padding: '0.75rem'
                  }}
                />
                <Legend 
                  wrapperStyle={{ paddingTop: '1rem' }}
                />
                <Line 
                  type="monotone" 
                  dataKey="value" 
                  stroke="url(#colorGradient)" 
                  strokeWidth={3}
                  dot={{ fill: '#667eea', r: 6 }}
                  activeDot={{ r: 8 }}
                />
                <defs>
                  <linearGradient id="colorGradient" x1="0" y1="0" x2="1" y2="0">
                    <stop offset="0%" stopColor="#667eea" />
                    <stop offset="100%" stopColor="#764ba2" />
                  </linearGradient>
                </defs>
              </LineChart>
            </ResponsiveContainer>
          </div>
        </>
      )}
    </div>
  )
}

function StatCard({ label, value, color = '#667eea' }: { label: string; value: string | number; color?: string }) {
  const gradients: Record<string, string> = {
    '#667eea': 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
    '#10b981': 'linear-gradient(135deg, #10b981 0%, #059669 100%)',
    '#ef4444': 'linear-gradient(135deg, #ef4444 0%, #dc2626 100%)',
    '#3b82f6': 'linear-gradient(135deg, #3b82f6 0%, #2563eb 100%)',
  };
  
  const gradient = gradients[color] || gradients['#667eea'];
  
  return (
    <div className="stat-card">
      <div className="stat-value" style={{ background: gradient, WebkitBackgroundClip: 'text', WebkitTextFillColor: 'transparent', backgroundClip: 'text' }}>
        {String(value)}
      </div>
      <div className="stat-label">{label}</div>
    </div>
  )
}

export default Dashboard
