import { useState, useEffect } from 'react'
import axios from 'axios'
import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts'
import '../App.css'

interface Station {
  station_name: string
  items_processed: number
  inventory_remaining: number
  timestamp: string
}

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000'

function Stations() {
  const [stations, setStations] = useState<Station[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    fetchStations()
  }, [])

  const fetchStations = async () => {
    try {
      const response = await axios.get(`${API_URL}/stations`)
      setStations(response.data)
      setLoading(false)
    } catch (err: any) {
      setError(err.message)
      setLoading(false)
    }
  }

  if (loading) {
    return <div className="loading">Loading stations...</div>
  }

  if (error) {
    return <div className="error">Error: {error}</div>
  }

  const chartData = stations.map(s => ({
    name: s.station_name,
    processed: s.items_processed,
    inventory: s.inventory_remaining
  }))

  return (
    <div className="fade-in">
      <div className="card">
        <h2>🏭 Stations</h2>
        <div style={{ 
          color: '#64748b', 
          marginBottom: '1.5rem',
          padding: '0.75rem 1rem',
          background: '#f8fafc',
          borderRadius: '10px',
          display: 'inline-block',
          fontWeight: 500
        }}>
          📊 Total: <strong style={{ color: '#1e293b' }}>{stations.length}</strong> stations
        </div>
      </div>

      <div className="card">
        <h2>📊 Station Activity</h2>
        <ResponsiveContainer width="100%" height={400}>
          <BarChart data={chartData}>
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
            <Bar dataKey="processed" fill="url(#processedGradient)" name="Items Processed" radius={[8, 8, 0, 0]} />
            <Bar dataKey="inventory" fill="url(#inventoryGradient)" name="Inventory Remaining" radius={[8, 8, 0, 0]} />
            <defs>
              <linearGradient id="processedGradient" x1="0" y1="0" x2="0" y2="1">
                <stop offset="0%" stopColor="#667eea" />
                <stop offset="100%" stopColor="#764ba2" />
              </linearGradient>
              <linearGradient id="inventoryGradient" x1="0" y1="0" x2="0" y2="1">
                <stop offset="0%" stopColor="#10b981" />
                <stop offset="100%" stopColor="#059669" />
              </linearGradient>
            </defs>
          </BarChart>
        </ResponsiveContainer>
      </div>

      {stations.map((station) => (
        <div key={station.station_name} className="card">
          <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'start', marginBottom: '1.5rem' }}>
            <div>
              <h3>{station.station_name}</h3>
              <p style={{ color: '#64748b', marginTop: '0.5rem', fontSize: '0.95rem' }}>
                ⏰ Last updated: {new Date(station.timestamp).toLocaleString()}
              </p>
            </div>
          </div>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(220px, 1fr))', gap: '1.5rem', marginTop: '1rem' }}>
            <div className="stat-card">
              <div className="stat-value" style={{ 
                background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
                WebkitBackgroundClip: 'text',
                WebkitTextFillColor: 'transparent',
                backgroundClip: 'text'
              }}>
                {station.items_processed}
              </div>
              <div className="stat-label">Items Processed</div>
            </div>
            <div className="stat-card">
              <div className="stat-value" style={{ 
                background: station.inventory_remaining > 0 
                  ? 'linear-gradient(135deg, #10b981 0%, #059669 100%)'
                  : 'linear-gradient(135deg, #ef4444 0%, #dc2626 100%)',
                WebkitBackgroundClip: 'text',
                WebkitTextFillColor: 'transparent',
                backgroundClip: 'text'
              }}>
                {station.inventory_remaining}
              </div>
              <div className="stat-label">Inventory Remaining</div>
            </div>
          </div>
        </div>
      ))}

      {stations.length === 0 && (
        <div className="card">
          <div style={{ textAlign: 'center', padding: '3rem 2rem' }}>
            <div style={{ fontSize: '4rem', marginBottom: '1rem' }}>🏭</div>
            <p style={{ color: '#64748b', fontSize: '1.1rem', fontWeight: 500 }}>
              No stations found. Run a simulation to generate station data.
            </p>
          </div>
        </div>
      )}
    </div>
  )
}

export default Stations

