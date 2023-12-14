import './App.css'
import {
  Route,
  Routes,
} from "react-router-dom";
import HomePage from "./page/home";

const App = () => {
  <>
  <HomePage />
      {/* <AuthProvider> */}
        {/* <Routes>
          <Route path="/login" element={<LoginForm />} />
          <Route path="/" element={<HomePage />} />
        </Routes> */}
      {/* </AuthProvider> */}
    </>  
}

export default App
