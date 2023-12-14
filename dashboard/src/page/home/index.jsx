import Sidebar from "../../components/Sidebar/Sidebar";
import Cards from "../../components/Cards/Cards";
import { useState } from "react";
import {
  Backdrop,
  Box,
  Button,
  Fade,
  Modal,
  Typography,
  Grid,
  TextField,
  Container,
  makeStyles,
  InputAdornment,
} from "@mui/material";
import { LocalizationProvider } from "@mui/x-date-pickers/LocalizationProvider";
import { AdapterDayjs } from "@mui/x-date-pickers/AdapterDayjs";
import { DatePicker } from "@mui/x-date-pickers/DatePicker";
import { MobileTimePicker } from "@mui/x-date-pickers/MobileTimePicker";
import PersonIcon from "@mui/icons-material/Person";
import TopicIcon from "@mui/icons-material/Topic";
import LocationOnIcon from "@mui/icons-material/LocationOn";
import AlarmAddIcon from "@mui/icons-material/AlarmAdd";
import AlarmOnIcon from "@mui/icons-material/AlarmOn";
import AlarmAdd from "@mui/icons-material/AlarmAdd";
import AlarmOn from "@mui/icons-material/AlarmOn";

const style = {
  position: "absolute",
  top: "50%",
  left: "50%",
  transform: "translate(-50%, -50%)",
  width: 360,
  height: 480, // Set the height for scrollability
  bgcolor: "background.paper",
  border: "2px solid #000",
  boxShadow: 24,
  p: 4,
  borderRadius: "12px",
  overflowY: "auto",
  "&::-webkit-scrollbar": {
    width: "12px",
  },
  "&::-webkit-scrollbar-thumb": {
    backgroundColor: "#888",
  },
  "&::-webkit-scrollbar-track": {
    backgroundColor: "#f1f1f1",
  },
};
function HomePage() {
  const [isSidebarOpen, setIsSidebarOpen] = useState(false);
  const [createSessionOpen, setCreateSessionOpen] = useState(false);
  const [formData, setFormData] = useState({
    pic: "",
    labModul: "",
    location: "",
    date: null,
    startTime: null,
    endTime: null,
  });
  const [sessionOn, setSessionOn] = useState(false);

  const toggleSidebar = () => {
    setIsSidebarOpen(!isSidebarOpen);
  };

  const handleClose = () => setCreateSessionOpen(false);
  const handleInputChange = (field, value) => {
    setFormData({
      ...formData,
      [field]: value,
    });
  };

  // Handle date picker change
  const handleDateChange = (date) => {
    setFormData({
      ...formData,
      date,
    });
  };

  // Handle time picker change
  const handleTimeChange = (field, time) => {
    setFormData({
      ...formData,
      [field]: time,
    });
  };

  return (
    <>
      <Sidebar isOpen={isSidebarOpen} toggleSidebar={toggleSidebar} />
      {sessionOn ? (
        <div className="flex">
          {/* Your main content */}
          <div className="flex-1">{/* Content goes here */}</div>
          <Cards />
        </div>
      ) : (
        <div>
          <img src="NoLabSession.png" height={300} width={312} />
          <p className="p-3">You don't have any lab session, create one!</p>
          <button
            className="w-120 mt-12 rounded-3xl hover:border-green-600"
            onClick={() => setCreateSessionOpen(true)}
          >
            Create Lab Session
          </button>
          <div>
            <Modal
              aria-labelledby="transition-modal-title"
              aria-describedby="transition-modal-description"
              open={createSessionOpen}
              onClose={handleClose}
              closeAfterTransition
              BackdropComponent={Backdrop}
              BackdropProps={{
                timeout: 500,
              }}
            >
              <Fade in={createSessionOpen}>
                <Box sx={style}>
                  <strong className="text-black">Create Lab Session</strong>
                  <form className="mt-4">
                    <Grid container spacing={2}>
                      <Grid item xs={12}>
                        <TextField
                          label="PIC"
                          fullWidth
                          value={formData.pic}
                          onChange={(e) =>
                            handleInputChange("pic", e.target.value)
                          }
                          InputProps={{
                            endAdornment: (
                              <InputAdornment position="end">
                                <PersonIcon />
                              </InputAdornment>
                            ),
                          }}
                        />
                      </Grid>
                      <Grid item xs={12}>
                        <TextField
                          label="Lab Modul"
                          fullWidth
                          value={formData.labModul}
                          onChange={(e) =>
                            handleInputChange("labModul", e.target.value)
                          }
                          InputProps={{
                            endAdornment: (
                              <InputAdornment position="end">
                                <TopicIcon />
                              </InputAdornment>
                            ),
                          }}
                        />
                      </Grid>
                      <Grid item xs={12}>
                        <TextField
                          label="Location"
                          fullWidth
                          value={formData.location}
                          onChange={(e) =>
                            handleInputChange("location", e.target.value)
                          }
                          InputProps={{
                            endAdornment: (
                              <InputAdornment position="end">
                                <LocationOnIcon />
                              </InputAdornment>
                            ),
                          }}
                        />
                      </Grid>
                      <Grid item xs={12}>
                        <LocalizationProvider dateAdapter={AdapterDayjs}>
                          <DatePicker
                            label="Date"
                            fullWidth
                            value={formData.date}
                            onChange={(e) =>
                              handleInputChange("date", e.target.value)
                            }
                          />
                        </LocalizationProvider>
                      </Grid>
                      <Grid item xs={12}>
                        <LocalizationProvider dateAdapter={AdapterDayjs}>
                          <MobileTimePicker
                            label="Start Time"
                            fullWidth
                            value={formData.startTime}
                            onChange={(e) =>
                              handleInputChange("startTime", e.target.value)
                            }
                            InputProps={{
                              endAdornment: (
                                <InputAdornment position="end">
                                  <AlarmAdd />
                                </InputAdornment>
                              ),
                            }}
                          />
                        </LocalizationProvider>
                      </Grid>
                      <Grid item xs={12}>
                        <LocalizationProvider dateAdapter={AdapterDayjs}>
                          <MobileTimePicker
                            label="End Time"
                            fullWidth
                            value={formData.endTime}
                            onChange={(e) =>
                              handleInputChange("endTime", e.target.value)
                            }
                            InputProps={{
                              endAdornment: (
                                <InputAdornment position="end">
                                  <AlarmOn />
                                </InputAdornment>
                              ),
                            }}
                          />
                        </LocalizationProvider>
                      </Grid>
                    </Grid>
                  </form>
                  <button
                    className="w-120 mt-4 rounded-3xl self-center object-center"
                    onClick={() => {setCreateSessionOpen(false); setSessionOn(true)}}
                  >
                    Create!
                  </button>
                </Box>
              </Fade>
            </Modal>
          </div>
        </div>
      )}
    </>
  );
}

export default HomePage;
