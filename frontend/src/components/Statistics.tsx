import { useState, useEffect } from 'react'
import axios from 'axios'
import { PieChart, Pie, Cell, ResponsiveContainer, Legend, Tooltip } from 'recharts'
import '../App.css'

interface Stats {
  total_orders: number
  completed_orders: number
  incomplete_orders: number
  completion_rate: number
  most_active_station: string | null
}

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000'

const COLORS = ['#4caf50', '#f44336', '#ff9800']

function Statistics() {
  const [stats, setStats] = useState<Stats | null>(null)
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    fetchStats()
    const interval = setInterval(fetchStats, 5000) // Refresh every 5 seconds
    return () => clearInterval(interval)
  }, [])

  const fetchStats = async () => {
    try {
      const response = await axios.get(`${API_URL}/stats`)
      setStats(response.data)
      setLoading(false)
    } catch (err: any) {
      setError(err.message)
      setLoading(false)
    }
  }

  if (loading) {
    return <div className="loading">Loading statistics...</div>
  }

  if (error) {
    return <div className="error">Error: {error}</div>
  }

  if (!stats) {
    return <div className="error">No statistics available</div>
  }

  const pieData = [
    { name: 'Completed', value: stats.completed_orders },
    { name: 'Incomplete', value: stats.incomplete_orders }
  ]

  return (
    <div>
      <div className="card">
        <h2>Simulation Statistics</h2>
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '1rem' }}>
          <StatCard label="Total Orders" value={stats.total_orders} />
          <StatCard label="Completed" value={stats.completed_orders} color="#4caf50" />
          <StatCard label="Incomplete" value={stats.incomplete_orders} color="#f44336" />
          <StatCard label="Completion Rate" value={`${stats.completion_rate}%`} color="#2196f3" />
        </div>
      </div>

      <div className="card">
        <h2>Order Distribution</h2>
        <ResponsiveContainer width="100%" height={400}>
          <PieChart>
            <Pie
              data={pieData}
              cx="50%"
              cy="50%"
              labelLine={false}
              label={({ name, percent }) => `${name} ${(percent * 100).toFixed(0)}%`}
              outerRadius={120}
              fill="#8884d8"
              dataKey="value"
            >
              {pieData.map((entry, index) => (
                <Cell key={`cell-${index}`} fill={COLORS[index % COLORS.length]} />
              ))}
            </Pie>
            <Tooltip />
            <Legend />
          </PieChart>
        </ResponsiveContainer>
      </div>

      {stats.most_active_station && (
        <div className="card">
          <h2>Performance Highlights</h2>
          <div style={{ padding: '1rem', background: '#f5f5f5', borderRadius: '4px', marginTop: '1rem' }}>
            <div style={{ fontSize: '1.2rem', marginBottom: '0.5rem' }}>
              <strong>Most Active Station:</strong> {stats.most_active_station}
            </div>
            <div style={{ color: '#666' }}>
              This station has processed the most items in the simulation.
            </div>
          </div>
        </div>
      )}
    </div>
  )
}

function StatCard({ label, value, color = '#667eea' }: { label: string; value: string | number; color?: string }) {
  return (
    <div style={{ padding: '1.5rem', background: '#f5f5f5', borderRadius: '8px', textAlign: 'center' }}>
      <div style={{ fontSize: '2.5rem', fontWeight: 'bold', color, marginBottom: '0.5rem' }}>{value}</div>
      <div style={{ color: '#666', fontSize: '1rem' }}>{label}</div>
    </div>
  )
}

export default Statistics

