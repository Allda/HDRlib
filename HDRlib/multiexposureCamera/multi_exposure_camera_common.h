#ifndef MULTI_EXPOXURE_CAMERA_COMMON_H
#define MULTI_EXPOXURE_CAMERA_COMMON_H


#define CAM_MAX_SEND_PACKET_DATA_SIZE 120
#define CAM_SEND_PACKET_HEADER_SIZE 8
#define CAM_RECV_PACKET_DATA_SIZE 966U

#define CAM_DEAD_BEEF_VALUE  0xdea1



typedef struct CameraSendPacket
{
    unsigned short dead_beef;
    unsigned short packet_id;
    unsigned short target_id;
    unsigned short data_size;
    unsigned char data[CAM_MAX_SEND_PACKET_DATA_SIZE];
} CameraSendPacket;


typedef struct CameraRecvPacketHeader
{
    unsigned int sdram_addr;
    unsigned int img_cnt;
    unsigned int block_cnt;

    struct {
        unsigned int dummy0 : 1;
        unsigned int is_last_packet : 1;
        unsigned int img_cfg_type : 3;
        unsigned int double_img : 1;
        unsigned int not_video : 1;
        unsigned int not_epilog : 1;
        unsigned int dummy1 : 24;
    } controlbits;

} CameraRecvPacketHeader;


typedef struct CameraImageConfig
{
    unsigned short config_index;
    unsigned short exposure_type;
    unsigned short exposure_value;
    unsigned short exposure_max_limit;
    unsigned short flash_1_type;
    unsigned short flash_1_wire_id;
    unsigned short flash_1_abs_value;
    unsigned short flash_1_rel_value;
    unsigned short flash_2_type;
    unsigned short flash_2_wire_id;
    unsigned short flash_2_abs_value;
    unsigned short flash_2_rel_value;
    unsigned short resolution_type;
    unsigned short trigger_edge_polarity;
    unsigned short trigger_wire_id;
    unsigned short trigger_delay_cnt;
    unsigned short trigger_type;
    bool config_enabled;
    bool eth_enabled;
    unsigned short next_config;

    CameraImageConfig()
    {
        config_index = 0;
        exposure_type = 1;        // relative exposure value
        exposure_value = 100;     // 100%
        exposure_max_limit = 0;   // without effect
        flash_1_type = 0;         // absolute flash value
        flash_1_wire_id = 0;
        flash_1_abs_value = 0;
        flash_1_rel_value = 0;
        flash_2_type = 0;         // absolute flash value
        flash_2_wire_id = 0;
        flash_2_abs_value = 0;
        flash_2_rel_value = 0;
        resolution_type = 1;      // full-frame
        trigger_edge_polarity = 0;  // falling
        trigger_wire_id = 0;
        trigger_delay_cnt = 0;    // now
        trigger_type = 1;         // full len
        config_enabled = true;
        eth_enabled = true;
        next_config = 0;          // default is first (0) confing
    }
} CameraImageConfig;


// Target ids
#define CAM_TARGET_ID_SYSTEM        0
#define CAM_TARGET_ID_GRABBER       1
#define CAM_TARGET_ID_PARAMS        2
#define CAM_TARGET_ID_IRIS_MACHINE  3
#define CAM_TARGET_ID_AD_CONFIG     4
#define CAM_TARGET_ID_HOST_PORT     5
#define CAM_TARGET_ID_SEQ_CONFIG    6
#define CAM_TARGET_ID_LOOP          7
#define CAM_TARGET_ID_LIDAR         8
#define CAM_TARGET_ID_RADAR         9


// Commands for system of camera
// In the >>data<< section, there is one CAM_INT, saying what to do; possible values:
#define CAM_SYS_CMD_RESET            0
#define CAM_SYS_CMD_HEY_YOU          1   // where are you packet Create->MCU; TODO divny popis
                                         // followed by 1 word: 0=must reply;
                                         // other value= set this value to camera
#define CAM_SYS_CMD_MODE_CONT        2   // no more data
#define CAM_SYS_CMD_MODE_TRIG        3   // no more data
#define CAM_SYS_CMD_PELT_ENABLE      4   // no more data
#define CAM_SYS_CMD_PELT_DISABLE     5   // no more data
#define CAM_SYS_CMD_HEAT_ENABLE      6   // no more data
#define CAM_SYS_CMD_HEAT_DISABLE     7   // no more data
#define CAM_SYS_CMD_POWER_OFF        8   // no more data
#define CAM_SYS_CMD_HEAT_TEMP_RANGE  9   // followed by 2x 16-bit signed int in this order:
                                           // heating start temp., heating stop temp.
                                           // values in hundredths of °C


// Commands for grabber of camera
#define CAM_GRAB_CMD_SET_ACTIVE_WINDOW  1   // this word is followed with 4 more words
                                            // in this order: XStart, XEnd, YStart, YEnd
#define CAM_GRAB_CMD_GRABBER_START      2   // this word is followed  with 32 Bytes
                                            // - encrypted date time
#define CAM_GRAB_CMD_GRABBER_STOP       3   // no more data
#define CAM_GRAB_CMD_FULL_SIZE_IMG      4   // this word is followed with 1 32-bit dword: the image-ID


// Comands for set of parameters
#define CAM_PRM_SET_GAIN            1     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_OFFSET          2     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_EXPOSURE        3     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_EXPOSURE_HIGH   8     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_APERTURE        4     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_ZOOM            5     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_ZOOM_DRIVE     11     // this word is followed with 3 more words in this order:
                                          // drive: 0=servo; 1=motor, ServoMin, ServoMax
#define CAM_PRM_SET_FOCUS           6     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_FOCUS_DIFF     10     // this word is followed with 2 more words in this order:
                                          // number of steps, direction: 0=to infinity; 1=to zero
#define CAM_PRM_SET_FLASH_TIME      7     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_FLASH_FOREVER   9     // no more data
#define CAM_PRM_SET_SS_ZOOM_MM     12     // this word is followed with 1 more word: the Value in milimeters
#define CAM_PRM_SET_SS_ZOOM_MOTOR  13     // this word is followed with 3 more words in this order:
                                          //    direction, speed, time
#define CAM_PRM_SET_IR_CCD_POS     14     // this word is followed with 1 more word: position of IR filter (0/1)
#define CAM_PRM_SET_DIGITAL_OFFSET 15     // this word is followed with 1 more word: the Value
#define CAM_PRM_SET_SS_FOCUS_MOTOR 16     // this word is followed with 3 more words in this order:
                                       //    direction, speed, time


// Comands for set of Iris Machine
#define CAM_IRIS_MACHINE_SET_REGULATOR_PAR   1   // this word is followed with 5 more words in this order:
                                                 //   Ppar, Spar, Dpar, RefBrightness, RegSMaxVal
#define CAM_IRIS_MACHINE_REGULATOR_START     2   // no more data
#define CAM_IRIS_MACHINE_REGULATOR_STOP      3   // no more data
#define CAM_IRIS_MACHINE_REGULATOR_MAX_EXPO  4   // this word is followed with 1 more word: the Value




// in the >>data<< section, there is one word (C621WORD), saying what to do; possible values:
#define CAM_SQC_CMD_REQUEST_SEQUENCE    1     // this word is followed with 1 word N
                                       // - the number of following couples
                                       // one couple:
                                       //     1 32b dword (2 words: lower, higher) - ImageID (absolute)
                                       //     1 word - Configuration index to use (1, 2, ...)
#define CAM_SQC_CMD_SET_IMAGE_CONFIG    2     // this word is followed with 1 word: ID of config to be set
                                       // and a series of the following TAG/VALUE couples:
#define CAM_SQC_CMD_SET_END             0
#define CAM_SQC_CMD_SET_TRIGGER         1    // followed by 2 values: trigger ID (0-7), delay (0=now, in frames)
#define CAM_SQC_CMD_SET_EXPO_FIXED      2    // followed by 1 value: duration in us (0-65535)
#define CAM_SQC_CMD_SET_EXPO_RELATIVE   3    // followed by 2 values: duration in perc (0-2000), max expo (0-65535, 0=no limit)
#define CAM_SQC_CMD_SET_LIGHT_DEFAULT   4    // followed by 1 value: light ID (0-1)
#define CAM_SQC_CMD_SET_LIGHT_FIXED     5    // followed by 3 values: light ID (0-1), duration in us (0-8000), wire ID (0-7)
#define CAM_SQC_CMD_SET_LIGHT_RELATIVE  6    // followed by 3 values: light ID (0-1), duration in perc (0-100), wire ID (0-7)
#define CAM_SQC_CMD_SET_RESOLUTION      7    // followed by 1 value: 0-halfframe, 1-fullframe
#define CAM_SQC_CMD_SET_EXPO_USER_CFG   8    // followed by 3 values: type (2-X), value, max expo (0-65535, 0=no limit)
#define CAM_SQC_CMD_SET_LIGHT_USER_CFG  9    // followed by 5 values: light ID (0-1), type (3-X), abs value, rel value, wire ID (0-7)
#define CAM_SQC_CMD_SET_ETH_OUT_ENABLE 10    // followed by 1 value: 0-disabled, other-enabled
#define CAM_SQC_CMD_SET_CONFIG_ENABLE  11    // followed by 1 value: 0-disabled, other-enabled
#define CAM_SQC_CMD_SET_TRIGGER_EDGE   12    // followed by 1 value: active edge (0=falling, 1=rising)
#define CAM_SQC_CMD_SET_TRIGGER_TYPE   13    // followed by 1 value:   0 - reserva
                                        //                        1 = full len
                                        //                        2   = 2*2 = 4 ms
                                        //                        3   = 2*3 = 6 ms)
#define CAM_SQC_CMD_SET_NEXT_CONFIG    14    // followed by 1 value: ID-number of config of next image

#endif // MULTI_EXPOXURE_CAMERA_COMMON_H
