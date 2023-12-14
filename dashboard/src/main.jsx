import React, { StrictMode } from 'react'
import ReactDOM from 'react-dom/client'
import App from './App.jsx'
import './index.css'
import { BrowserRouter } from 'react-router-dom'
import HomePage from './page/home/index.jsx'

ReactDOM.createRoot(document.getElementById('root')).render(
  <StrictMode>
    {/* <BrowserRouter> */}
      <HomePage />
    {/* </BrowserRouter> */}
  </StrictMode>
)
