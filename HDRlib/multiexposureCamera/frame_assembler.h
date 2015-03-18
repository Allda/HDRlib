#ifndef FRAME_ASSEMBLER_H
#define FRAME_ASSEMBLER_H

#include "multi_exposure_camera_common.h"
#include "frame.h"

#include <string.h>


class FrameAssembler
{
public:
    enum { FRAME_PROCESSED, FRAME_FINISHED, NEW_FRAME, OLD_FRAME, EPILOG_DATA, INTERNAL_ERROR };

    FrameAssembler( int width, int height, int data_type,
                    int number_expected_packets );
    ~FrameAssembler( void );

    Frame * releaseFrame( void );

    int processPacket( CameraRecvPacketHeader * p_packet_header,
                       int data_size, unsigned char * p_packet_data );




private:
    int processImageData( CameraRecvPacketHeader * p_packet_header,
                          int data_size, unsigned char * p_packet_data );
    int processEpilogData( CameraRecvPacketHeader * p_packet_header,
                           int data_size, unsigned char * p_packet_data );

    int m_img_height;
    int m_img_width;
    int m_img_size;
    int m_number_expected_packets;
    int m_number_received_packets;

    int m_previous_image_id;
    int m_current_image_id;

    Frame * mp_complete_frame;
    Frame * mp_recv_frame;
};

#endif // FRAME_ASSEMBLER_HPP
