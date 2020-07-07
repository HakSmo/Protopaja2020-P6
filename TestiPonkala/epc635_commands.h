#define SND                         0xF5

// SET COMMANDS

#define SET_MOD_CHANNEL             0x0E
#define SET_INT_TIME_DIST           0x00
#define SET_OPERATION_MODE          0x04
#define SET_HDR                     0x0D
#define SET_ROI                     0x02
#define SET_TEMPORAL_FILTER_WFOW    0x07
#define SET_TEMPORAL_FILTER_NFOW    0x0F
#define SET_AVERAGE_FILTER          0x0A
#define SET_MEDIAN_FILTER           0x0B
#define SET_FRAME_RATE              0x0C
#define SET_AMPLITUDE_LIMIT         0x09
#define STOP_STREAM                 0x28
#define SET_COMPENSATION            0x55
#define SET_ILLUMINATION_POWER      0x6C
#define SET_DLL_STEP                0x06

// GET COMMANDS

#define GET_DIST                    0x20
#define GET_DIST_GS                 0x29
#define GET_DIST_AMPLITUDE          0x22
#define GET_GS                      0x24
#define GET_DCS                     0x25
#define GET_CALIBRATION_INFO        0x57

// MISC COMMANDS

#define SET_OUTPUT                  0x51
#define GET_INPUT                   0x52
#define GET_TEMPERATURE             0x4A
#define GET_TOFCOS_VERSION          0x49
#define GET_CHIP_INFORMATION        0x48
#define GET_PROD_DATE               0x50
#define IDENTIFY                    0x47
#define GET_ERROR                   0x53

#define SINGLE_PIC_SIZE             19288

void stream_on(){
  unsigned char cmd[14] = {SND, GET_DIST, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xAC, 0xA8, 0xCC};
  Serial1.write(cmd, 14);
}
