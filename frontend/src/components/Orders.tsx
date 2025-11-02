import { useState, useEffect } from 'react'
import axios from 'axios'
import '../App.css'

interface Order {
  order_id: string
  customer_name: string
  product: string
  is_completed: boolean
  total_items: number
  filled_items: number
  created_at: string
  completed_at: string | null
}

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000'

function Orders() {
  const [orders, setOrders] = useState<Order[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)
  const [filter, setFilter] = useState<'all' | 'completed' | 'incomplete'>('all')

  useEffect(() => {
    fetchOrders()
  }, [filter])

  const fetchOrders = async () => {
    try {
      setLoading(true)
      let endpoint = '/orders'
      if (filter === 'completed') endpoint = '/orders/completed'
      if (filter === 'incomplete') endpoint = '/orders/incomplete'
      
      const response = await axios.get(`${API_URL}${endpoint}`)
      setOrders(response.data)
      setLoading(false)
    } catch (err: any) {
      console.error('Error fetching orders:', err)
      setError(err.message || 'Failed to fetch orders')
      setLoading(false)
    }
  }

  if (loading) {
    return <div className="loading">Loading orders...</div>
  }

  if (error) {
    return <div className="error">Error: {error}</div>
  }

  return (
    <div className="fade-in">
      <div className="card">
        <h2>📦 Orders</h2>
        <div style={{ marginBottom: '1.5rem', display: 'flex', gap: '0.75rem', flexWrap: 'wrap' }}>
          <button
            onClick={() => setFilter('all')}
            className={`filter-btn ${filter === 'all' ? 'active' : ''}`}
          >
            📋 All Orders
          </button>
          <button
            onClick={() => setFilter('completed')}
            className={`filter-btn ${filter === 'completed' ? 'active' : ''}`}
          >
            ✅ Completed
          </button>
          <button
            onClick={() => setFilter('incomplete')}
            className={`filter-btn ${filter === 'incomplete' ? 'active' : ''}`}
          >
            ⚠️ Incomplete
          </button>
        </div>
        <div style={{ 
          color: '#64748b', 
          fontSize: '0.95rem',
          fontWeight: 500,
          padding: '0.75rem 1rem',
          background: '#f8fafc',
          borderRadius: '10px',
          display: 'inline-block'
        }}>
          📊 Total: <strong style={{ color: '#1e293b' }}>{orders.length}</strong> {filter === 'all' ? 'orders' : filter === 'completed' ? 'completed orders' : 'incomplete orders'}
        </div>
      </div>

      {orders.map((order) => (
        <div key={order.order_id} className="card">
          <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'start', marginBottom: '1.5rem' }}>
            <div>
              <h3 style={{ marginBottom: '0.5rem' }}>{order.customer_name}</h3>
              <p style={{ color: '#64748b', fontSize: '1rem' }}>{order.product}</p>
            </div>
            <span className={`badge ${order.is_completed ? 'badge-success' : 'badge-danger'}`}>
              {order.is_completed ? '✅ Completed' : '⚠️ Incomplete'}
            </span>
          </div>
          <div style={{ 
            display: 'grid', 
            gridTemplateColumns: 'repeat(auto-fit, minmax(180px, 1fr))', 
            gap: '1.25rem',
            marginBottom: '1.5rem'
          }}>
            <div style={{ padding: '0.75rem', background: '#f8fafc', borderRadius: '10px' }}>
              <div style={{ fontSize: '0.875rem', color: '#64748b', marginBottom: '0.25rem' }}>Order ID</div>
              <div style={{ fontWeight: 600, color: '#1e293b', wordBreak: 'break-all' }}>{order.order_id}</div>
            </div>
            <div style={{ padding: '0.75rem', background: '#f8fafc', borderRadius: '10px' }}>
              <div style={{ fontSize: '0.875rem', color: '#64748b', marginBottom: '0.25rem' }}>Items</div>
              <div style={{ fontWeight: 600, color: '#1e293b' }}>
                {order.filled_items} / {order.total_items}
              </div>
            </div>
            <div style={{ padding: '0.75rem', background: '#f8fafc', borderRadius: '10px' }}>
              <div style={{ fontSize: '0.875rem', color: '#64748b', marginBottom: '0.25rem' }}>Created</div>
              <div style={{ fontWeight: 600, color: '#1e293b', fontSize: '0.95rem' }}>
                {new Date(order.created_at).toLocaleString()}
              </div>
            </div>
            {order.completed_at && (
              <div style={{ padding: '0.75rem', background: '#f8fafc', borderRadius: '10px' }}>
                <div style={{ fontSize: '0.875rem', color: '#64748b', marginBottom: '0.25rem' }}>Completed</div>
                <div style={{ fontWeight: 600, color: '#1e293b', fontSize: '0.95rem' }}>
                  {new Date(order.completed_at).toLocaleString()}
                </div>
              </div>
            )}
          </div>
          <div style={{ marginTop: '1rem' }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '0.5rem' }}>
              <strong style={{ color: '#1e293b' }}>Progress</strong>
              <span style={{ color: '#64748b', fontSize: '0.875rem', fontWeight: 500 }}>
                {Math.round((order.filled_items / order.total_items) * 100)}%
              </span>
            </div>
            <div className="progress-bar">
              <div
                className="progress-fill"
                style={{
                  width: `${(order.filled_items / order.total_items) * 100}%`,
                  background: order.is_completed 
                    ? 'linear-gradient(135deg, #10b981 0%, #059669 100%)'
                    : 'linear-gradient(135deg, #f59e0b 0%, #d97706 100%)'
                }}
              />
            </div>
          </div>
        </div>
      ))}

      {orders.length === 0 && (
        <div className="card">
          <div style={{ textAlign: 'center', padding: '3rem 2rem' }}>
            <div style={{ fontSize: '4rem', marginBottom: '1rem' }}>📦</div>
            <p style={{ color: '#64748b', fontSize: '1.1rem', fontWeight: 500 }}>
              No orders found. Run a simulation to generate orders.
            </p>
          </div>
        </div>
      )}
    </div>
  )
}

export default Orders

