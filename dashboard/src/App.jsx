import { useState } from 'react'
import './App.css'
import Sidebar from './components/Sidebar/Sidebar';
import Cards from './components/Cards/Cards';

const App = () => {
  const [isSidebarOpen, setIsSidebarOpen] = useState(true);

  const toggleSidebar = () => {
    setIsSidebarOpen(!isSidebarOpen);
  };

  return (
    <>
      <div className="flex">
      {/* Your main content */}
      <div className="flex-1">
        {/* Content goes here */}
      </div>
      {/* Sidebar */}
      <Sidebar isOpen={isSidebarOpen} toggleSidebar={toggleSidebar} />
      <Cards />
    </div>
    </>
  )
}

export default App
