/**
 * @file App.tsx
 * @brief Main React application component - Root of the frontend
 * 
 * PURPOSE:
 * This is the root component that sets up routing and navigation for the entire
 * frontend application. It provides the main layout structure and handles
 * client-side routing between different pages.
 * 
 * ARCHITECTURE:
 * ┌─────────────────────────────────────────┐
 * │           App.tsx (Root)                │
 * │  ┌───────────────────────────────────┐  │
 * │  │  Router (React Router)            │  │
 * │  │  ┌─────────────────────────────┐  │  │
 * │  │  │  Navigation Bar             │  │  │
 * │  │  └─────────────────────────────┘  │  │
 * │  │  ┌─────────────────────────────┐  │  │
 * │  │  │  Routes (Page Components)  │  │  │
 * │  │  │  - Dashboard               │  │  │
 * │  │  │  - Orders                  │  │  │
 * │  │  │  - Stations                │  │  │
 * │  │  │  - Statistics              │  │  │
 * │  │  └─────────────────────────────┘  │  │
 * │  └───────────────────────────────────┘  │
 * └─────────────────────────────────────────┘
 * 
 * FLOW:
 * 1. User navigates to a URL (e.g., /orders)
 * 2. React Router matches the URL to a Route
 * 3. Corresponding component (e.g., Orders) is rendered
 * 4. Component fetches data from API and displays it
 * 
 * CONNECTIONS:
 * - React Router: Handles client-side routing (no page reloads)
 * - Components: Dashboard, Orders, Stations, Statistics
 * - API: Each component makes its own API calls
 * - CSS: App.css provides global styles
 * 
 * TRIGGERS:
 * - Initial page load: Renders Dashboard component
 * - Navigation clicks: Router updates URL and renders new component
 * - Browser back/forward: Router handles history navigation
 * 
 * ROUTES:
 * - "/" → Dashboard component (main page with stats and simulation button)
 * - "/orders" → Orders component (displays order list)
 * - "/stations" → Stations component (displays station data)
 * - "/statistics" → Statistics component (displays charts and analytics)
 */

import { BrowserRouter as Router, Routes, Route, Link } from 'react-router-dom'
import Dashboard from './components/Dashboard'
import Orders from './components/Orders'
import Stations from './components/Stations'
import Statistics from './components/Statistics'
import './App.css'

/**
 * Main App component
 * 
 * This component:
 * - Sets up React Router for client-side navigation
 * - Provides navigation bar with links to all pages
 * - Renders the appropriate page component based on current URL
 * - Maintains consistent layout across all pages
 */
function App() {
  return (
    <Router>
      <div className="app">
        {/* Navigation Bar - Always visible at top of page */}
        {/* Links use React Router's Link component for client-side navigation */}
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
        
        {/* Main Content Area - Renders different components based on route */}
        {/* React Router matches the current URL and renders the matching Route */}
        <main className="main-content">
          <Routes>
            {/* Root path - Dashboard is the default/home page */}
            <Route path="/" element={<Dashboard />} />
            
            {/* Orders page - Shows list of all orders */}
            <Route path="/orders" element={<Orders />} />
            
            {/* Stations page - Shows station data and inventory */}
            <Route path="/stations" element={<Stations />} />
            
            {/* Statistics page - Shows charts and analytics */}
            <Route path="/statistics" element={<Statistics />} />
          </Routes>
        </main>
      </div>
    </Router>
  )
}

export default App

