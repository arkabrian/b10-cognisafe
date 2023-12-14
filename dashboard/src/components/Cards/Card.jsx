import React, { Component } from "react";
import { FaUser, FaInfoCircle } from "react-icons/fa";

class Card extends Component {
  state = {
    fallStatus: 0,
    gasStatus: 1,
    uptime: 0, // Add uptime state
  };

  componentDidMount() {
    fetch("http://localhost:4141/mqtt/subscribe")
    .then(response => {
      if (response.ok) {
        // If the response is successful (status code 200), set subscribe success to true
        this.setState({ subscribeSuccess: true });
      }
    })
    .catch(error => {
      console.error("Error subscribing:", error);
    });

    this.intervalM = setInterval(() => {
      if (this.state.subscribeSuccess) {
        fetch("http://localhost:4141/mqtt/getData")
          .then(response => response.json())
          .then(data => {
            // Update state with the received data (assuming your API returns fall and gas fields)
            this.setState({
              fallStatus: data.fall,
              gasStatus: data.gas
            });
          })
          .catch(error => {
            console.error("Error fetching data:", error);
          });
      }
    }, 1000);

    // Function to update uptime every second
    this.intervalU = setInterval(() => {
      this.setState((prevState) => ({ uptime: prevState.uptime + 1 }));
    }, 1000);


  }

  componentWillUnmount() {
    clearInterval(this.intervalU);
    clearInterval(this.intervalM);
  }

  formatUptime(seconds) {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const remainingSeconds = seconds % 60;

    const formattedHours = hours < 10 ? `0${hours}` : hours;
    const formattedMinutes = minutes < 10 ? `0${minutes}` : minutes;
    const formattedSeconds =
      remainingSeconds < 10 ? `0${remainingSeconds}` : remainingSeconds;

    return `${formattedHours}:${formattedMinutes}:${formattedSeconds}`;
  }

  render() {
    const { fallStatus, gasStatus, uptime } = this.state;
    const { number, ipAddress, macAddress } = this.props;

    return (
      <div className="rounded-lg bg-gray-700 mx-5 shadow-md justify-between px-4 py-3">
        {/* Person 1 */}
        <div className="text-white font-bold text-xl mb-2">{`Person ${number}`}</div>

        {/* Uptime */}
        <div className="text-gray-300 mb-2">{`Uptime ${this.formatUptime(
          uptime
        )}`}</div>

        {/* Centered person icon */}
        <div className="flex justify-center mb-2">
          <FaUser size={40} color="#fff" />
        </div>

        {/* IP Address and Mac Address Info */}
        <div className="text-gray-300 mb-2">
          <FaInfoCircle className="mb-3" />
          <table className="w-full border-collapse">
            <tbody>
              <tr className="border-t border-white text-left text-sm">
                <td className="py-2 pr-4">IP Addr.</td>
                <td className="py-2">{ipAddress}</td>
              </tr>
              <tr className="border-t border-b border-white text-left text-sm">
                <td className="py-2 pr-4">MAC Addr.</td>
                <td className="py-2">{macAddress}</td>
              </tr>
            </tbody>
          </table>
        </div>

        {/* Gas and Fall status */}
        <div className="flex justify-between mt-4 mb-2 ">
          <div className="status-box">
            <div className="status-title">Gas Status</div>
            <div
              className={`status-circle ${
                gasStatus === 0 ? "status-green" : "status-red"
              }`}
            ></div>
          </div>
          <div className="status-box">
            <div className="status-title">Fall Status</div>
            <div
              className={`status-circle ${
                fallStatus === 0 ? "status-green" : "status-red"
              }`}
            ></div>
          </div>
        </div>
      </div>
    );
  }
}

export default Card;
