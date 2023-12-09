// MainContent.js

import React, { useState } from 'react';

const Cards = () => {
  const [cardRows, setCardRows] = useState([[]]);

  const addCard = (rowIndex) => {
    const newCardRows = [...cardRows];
    if (newCardRows[rowIndex]?.length < 5) {
      newCardRows[rowIndex] = [...(newCardRows[rowIndex] || []), newCardRows[rowIndex]?.length + 1];
    } else {
      newCardRows.push([1]);
    }
    setCardRows(newCardRows);
  };

  return (
    <div className="p-4">
      {cardRows.map((row, rowIndex) => (
        <div key={rowIndex} className="flex mb-4">
          {row.map((cardNumber) => (
            <div
              key={cardNumber}
              className="w-24 h-24 bg-gray-700 rounded-md flex items-center justify-center"
            >
              Card {cardNumber}
            </div>
          ))}
        </div>
      ))}
      <button
        className="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded"
        onClick={() => addCard(cardRows.length - 1)}
      >
        Add Card
      </button>
    </div>
  );
};

export default Cards;
