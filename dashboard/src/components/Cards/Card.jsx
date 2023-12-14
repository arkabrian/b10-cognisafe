import React, { Component } from 'react';
import mqtt from 'mqtt';

class Card extends Component {
  state = {
    client: null,
    fallState: null,
    gasValue: null,
  };

  componentDidMount() {
    const client = mqtt.connect("mqtt://172.173.157.174:1883");
    client.on('connect', () => {
      console.log('masuk');
      client.subscribe('Gas');
      client.subscribe('Fall');
      console.log('berhasil subskrep');
    });
    client.on('message', (topic, message) => {
      switch (topic) {
        case 'Gas':
          const gasValue = parseInt(message.toString());
          this.setState({ gasValue });
          break;
        case 'Fall':
          const fallState = message.toString() === '1';
          this.setState({ fallState });
          break;
        default:
          break;
      }
    });
    this.setState({ client });
  }

  componentWillUnmount() {
    if (this.state.client) {
      this.state.client.end();
    }
  }
  
  
  render() {
    const { number } = this.props;
    const { gasValue, fallState } = this.state;

    return (
      <div className="rounded-lg bg-gray-700 mx-5 shadow-md justify-between px-4 py-3">
        <ul>
          <li className="flex justify-between my-2">
            <span className="text-white font-bold text-sm mr-2">Device {number}</span>
            <span className="text-gray-300 text-xs">Uptime: 02:14:21</span>
          </li>
          <li className="flex justify-between mt-2">
            <span className="text-gray-300 text-xs">MAC Address: 'Whatecer'</span>
          </li>
          <li className="flex justify-between mt-2">
            <span className="text-white text-sm">Gas Sensor: </span>
            <span className="text-gray-300 text-xs">{gasValue ? gasValue : '-'}</span>
          </li>
          <li className="flex justify-between mt-2">
            <span className="text-white text-sm">Fall Detection:</span>
            <span className="text-gray-300 text-xs">
              {fallState ? (fallStateConfirmed ? 'Falling' : 'Pending Confirmation') : 'Safe'}
            </span>
          </li>
          <li className="mt-2 flex">
            <div className="flex-shrink-0 bg-red-500 rounded-full w-4 h-4 mr-2"></div>
            <span className="text-gray-300 text-xs">BOCOR GAN</span>
          </li>
        </ul>
      </div>
    );
  }
}

export default Card;
