import { useState, useEffect } from 'react'
import { BrowserRouter as Router, Routes, Route, Link } from 'react-router-dom'
import Dashboard from './components/Dashboard'
import Orders from './components/Orders'
import Stations from './components/Stations'
import Statistics from './components/Statistics'
import './App.css'

function App() {
  return (
    <Router>
      <div className="app">
        <nav className="navbar">
          <div className="nav-container">
            <h1 className="nav-title">Assembly Line Simulator</h1>
            <div className="nav-links">
              <Link to="/">📊 Dashboard</Link>
              <Link to="/orders">📦 Orders</Link>
              <Link to="/stations">🏭 Stations</Link>
              <Link to="/statistics">📈 Statistics</Link>
            </div>
          </div>
        </nav>
        
        <main className="main-content">
          <Routes>
            <Route path="/" element={<Dashboard />} />
            <Route path="/orders" element={<Orders />} />
            <Route path="/stations" element={<Stations />} />
            <Route path="/statistics" element={<Statistics />} />
          </Routes>
        </main>
      </div>
    </Router>
  )
}

export default App

