import React, { useState } from 'react';
import Card from './Card';
import LabRegist from './LabRegist';

const Cards = () => {
  const [cardRows, setCardRows] = useState([[]]);
  const [showForm, setShowForm] = useState(true);

  const addCard = (rowIndex) => {
    const newCardRows = [...cardRows];
    if (newCardRows[rowIndex]?.length < 5) {
      newCardRows[rowIndex] = [...(newCardRows[rowIndex] || []), newCardRows[rowIndex]?.length + 1];
    } else {
      newCardRows.push([1]);
    }
    setCardRows(newCardRows);
  };

  const handleFormSubmit = (formData) => {
    // Handle form submission (you can log it for now)
    console.log('Form submitted:', formData);
    setShowForm(false); // Hide the form after submission
  };

  return (
    <div className="p-4">
      <LabRegist onClick={handleFormSubmit}></LabRegist>
      {cardRows.map((row, rowIndex) => (
        <div key={rowIndex} className="flex mb-4">
          {row.map((cardNumber) => (
            <Card key={cardNumber} number={cardNumber} />
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
