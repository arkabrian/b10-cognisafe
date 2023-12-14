import React, { useState } from 'react';

const LabRegist = ({ onSubmit }) => {
  const [formData, setFormData] = useState({
    topic: '',
    personInCharge: '',
    startTime: '',
    endTime: '',
    location: '',
  });

  const [formErrors, setFormErrors] = useState({
    topic: false,
    personInCharge: false,
    startTime: false,
    endTime: false,
    location: false,
  });

  const handleInputChange = (e) => {
    const { name, value } = e.target;
    setFormData({
      ...formData,
      [name]: value,
    });
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    const errors = {};
    for (const key in formData) {
      if (formData[key] === '') {
        errors[key] = true;
      }
    }
    if (Object.keys(errors).length === 0) {
      setFormErrors({
        topic: false,
        personInCharge: false,
        startTime: false,
        endTime: false,
        location: false,
      });
      onSubmit(formData);
    } else {
      setFormErrors(errors);
    }
  };

  return (
    <form onSubmit={handleSubmit}>
      <div>
        <label>Lab Topic:</label>
        <input
          type="text"
          name="topic"
          value={formData.topic}
          onChange={handleInputChange}
        />
        {formErrors.topic && <p className="text-red-500">Topic is required</p>}
      </div>
      {/* Other form inputs (Person in Charge, Start/End time, Location) go here */}
      <div>
        <button type="submit">Submit</button>
      </div>
    </form>
  );
};

export default LabRegist;
