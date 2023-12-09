// Sidebar.js

import React, { useState, useEffect } from 'react';

const Sidebar = ({ isOpen, toggleSidebar }) => {
  const [sidebarOpen, setSidebarOpen] = useState(isOpen);
  const [listItems, setListItems] = useState(['GPTech DTMM', 'Netlab DTE', 'Prodev DTI']);
  const [newItemName, setNewItemName] = useState('');
  const [showInput, setShowInput] = useState(false);
  const [selectedList, setSelectedList] = useState(null);

  useEffect(() => {
    setSidebarOpen(isOpen);
  }, [isOpen]);

  const addNewItem = () => {
    if (newItemName.trim() !== '') {
      setListItems([...listItems, newItemName]);
      setNewItemName('');
      setShowInput(false);
    }
  };

  const handleInputChange = (e) => {
    setNewItemName(e.target.value);
  };

  const handleListClick = (index) => {
    setSelectedList(index); // Set the selected list index
  };

  return (
    <>
      {/* Toggle button */}
      <button
        className={`${
          sidebarOpen ? 'left-64' : 'left-0'
        } absolute top-1/2 transform -translate-y-1/2 bg-gray-700 p-2 rounded-r-md focus:outline-none z-10 transition-all duration-300`}
        onClick={toggleSidebar}
      >
        <svg
          xmlns="http://www.w3.org/2000/svg"
          className={`h-6 w-6 text-white transform ${
            sidebarOpen ? 'rotate-180' : 'rotate-0'
          } transition-transform duration-300`}
          fill="none"
          viewBox="0 0 24 24"
          stroke="currentColor"
        >
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5l7 7-7 7" />
        </svg>
      </button>

      {/* Sidebar */}
      <div
        className={`${
          sidebarOpen ? 'translate-x-0' : '-translate-x-full'
        } bg-gray-800 w-64 min-h-screen text-white fixed top-0 left-0 z-50 overflow-y-auto transform transition-transform duration-300`}
      >
        {/* Sidebar Content */}
        <div className="p-4">
          <ul>
            {listItems.map((item, index) => (
              <li className="py-2" key={index}>
                {item}
              </li>
            ))}
          </ul>
          {/* Add Item Form */}
          {showInput ? (
            <div className=" mt-4">
              <input
                type="text"
                className="border rounded-l py-1 px-2 focus:outline-none"
                placeholder="New Item Name"
                value={newItemName}
                onChange={handleInputChange}
              />
              <button
                className="bg-gray-600 mt-4 text-white rounded-r px-4 py-1 ml-1"
                onClick={addNewItem}
              >
                Add
              </button>
            </div>
          ) : (
            <button
              className="mt-4 bg-gray-600 text-white px-2 py-1 rounded"
              onClick={() => setShowInput(true)}
            >
              Add Item
            </button>
          )}
        </div>
      </div>
    </>
  );
};

export default Sidebar;
