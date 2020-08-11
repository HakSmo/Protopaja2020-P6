#pragma once
#define SND                         0xF5
#include <vector>

// BASIC RESPONSES
unsigned char CAM_ACK[8] = {0xFA,0x00,0x00,0x00,0xBC,0x7D,0x6A,0x77};
unsigned char CAM_NACK[8] = {0xFA,0x01,0x00,0x00,0xDA,0xD7,0x6A,0x85};

// SET COMMANDS

#define SET_MOD_CHANNEL             0x0E
#define SET_INTERFERENCE_DETECTION  0x11
#define SET_EDGE_DETECTION          0x10
#define SET_INT_TIME_DIST           0x00
#define SET_OPERATION_MODE          0x04
#define SET_HDR                     0x0D
#define SET_ROI                     0x02
#define SET_TEMPORAL_FILTER_WFOV    0x07
#define SET_TEMPORAL_FILTER_NFOV    0x0F
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
