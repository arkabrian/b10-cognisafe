import React, { useState } from 'react';
import Card from './Card';
import LabRegist from './LabRegist';

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
            <Card ipAddress="192.168.1.1" macAddress="00:0a:95:9d:68:16" number={cardNumber} />
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
