#ifndef MULTI_EXPOXURE_CAMERA_H
#define MULTI_EXPOXURE_CAMERA_H

#include "multi_exposure_camera_common.h"
#include "frame_assembler.h"
#include "frame.h"

#define RECEIVE_BUFFER_SIZE 20000


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



#ifdef win32
#include <winsock2.h>
#endif


#include <stddef.h>
#include <stdio.h>

class MultiExposureCamera
{
    enum { FRAME_COMPLETE, VIDEO_DATA_PROCESSED, NON_VIDEO_DATA_PROCESSED, ERROR_POOR_FRAME, INTERNAL_ERROR };

public:
    MultiExposureCamera( void );
    virtual ~MultiExposureCamera( void );

    bool initConnection( const char * p_camera_ip,
                         const short int camera_port,
                         const char * p_local_ip,
                         bool is_jumbo = false );
    bool bindToCamera( void );

    virtual bool grabFrame( bool must_complete = false );
    virtual Frame * releaseFrame( void );

    bool sendHeyYouPacket( bool is_required_response = false );

    bool startCameraGrabber( void );
    bool stopCameraGrabber( void );

    bool setCameraActiveWindow( unsigned short start_x, unsigned short end_x,
                                unsigned short start_y, unsigned short end_y );
    bool setCameraGain( unsigned short gain );
    bool setCameraOffset( unsigned short offset );
    bool setCameraExposure( unsigned short exposure );
    bool setCameraAperture( unsigned short aperture );
    bool setIrisPSDParams( unsigned short p_par, unsigned short s_par,
                           unsigned short d_par, unsigned short brightness,
                           unsigned short reg_s_max_value );
    bool setIrisPSDAutomode( bool automode_enabled );
    bool setIrisAutoExpoLimit( unsigned short value );
    bool setCameraHeating( bool heating_enabled );
    bool setCameraAntiFog( bool anti_fog_enabled );
    bool setCameraFocus( unsigned short focus );
    bool setCameraFlashTime( unsigned short time );
    bool requestSequnce( unsigned short count_sequence,
                         unsigned int * p_image_id, unsigned short * p_config );
    bool setCameraImgConfig( const CameraImageConfig & img_config );



private:
    bool receivePacket( void );
    int processReceivedPacket( void );
    int processVideoPacket( CameraRecvPacketHeader * p_packet_header );
    int processNonVideoPacket( void );

    bool sendCommandPacket( unsigned short target_id,
                            unsigned short length,
                            unsigned char *p_buffer );



    void logErrorMessage( const char * p_error_message ,
                          const char * p_function_name = NULL );

    FrameAssembler * mp_frame_assembler;
    Frame * mp_frame;

    int m_camera_socket;
    unsigned char m_receive_buffer[RECEIVE_BUFFER_SIZE];
    int m_recv_packet_size;

    unsigned short m_send_buffer[CAM_MAX_SEND_PACKET_DATA_SIZE / sizeof(unsigned short)];

    unsigned short m_last_send_packet_id;

    bool m_is_camera_running;

    sockaddr_in m_camera_sockaddr;
    sockaddr_in m_local_sockaddr;

    int m_rec_error_limit;

    // ????
    int m_number_received_packets;
    unsigned int m_img_id;

};

#endif // MULTI_EXPOXURE_CAMERA_HPP
