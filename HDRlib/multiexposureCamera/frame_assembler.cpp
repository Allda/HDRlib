#include "frame_assembler.h"

#include <stdlib.h>

#include <iostream>




FrameAssembler::FrameAssembler( int width, int height, int data_type,
                                int number_expected_packets )
    : m_img_height( height ),
      m_img_width( width ),
      m_previous_image_id( -1 ),  // první obraz má id 0
      m_current_image_id( -1 )
{
    m_number_expected_packets = number_expected_packets;
    m_number_received_packets = 0;

    mp_complete_frame = new Frame( width, height, data_type );
    mp_recv_frame = new Frame( width, height, data_type );
}


FrameAssembler::~FrameAssembler( void )
{
    delete mp_complete_frame;
    delete mp_recv_frame;
}


Frame * FrameAssembler::releaseFrame( void )
{
    Frame * p_frame;

    p_frame = mp_complete_frame->cloneFrame();

    memset( mp_complete_frame->ptrData(), 255, mp_complete_frame->size() );

    //std::cout << "m_number_received_packets: " << m_number_received_packets << std::endl;
    m_number_received_packets = 0;

    return p_frame;
}


int FrameAssembler::processPacket( CameraRecvPacketHeader * p_packet_header,
                                   int data_size, unsigned char * p_packet_data )
{
    //std::cout << p_packet_header->controlbits.img_cfg_type << std::endl;

    //if ( p_packet_header->controlbits.img_cfg_type != )
    //  return false;


    // Pokud se jedná o paket z předchozího obrázku (někde se opozdil)
    // tak se zahazuje
    if ( (int) p_packet_header->img_cnt == m_previous_image_id )
    {
        return OLD_FRAME;
    }


    // Pokud se jedná o první packet z nového obrázku, tak se nastaví
    // aktuální image id
    if ( m_current_image_id == -1 )
    {
        m_current_image_id = p_packet_header->img_cnt;
    }
    else
    {
        // Nejednáli se ani o aktuální ani o předchozí obrázek,
        // tak zde máme nový obrázek bez ukončení předchozího.
        // Toto zjištění vracíme.
        if ( (int) p_packet_header->img_cnt != m_current_image_id )
        {
            m_previous_image_id = m_current_image_id;
            m_current_image_id = p_packet_header->img_cnt;

            // Přehození snímku pro příjem za hotový snímek
            Frame * p_frame;
            p_frame = mp_complete_frame;
            mp_complete_frame = mp_recv_frame;
            mp_recv_frame = p_frame;

            mp_complete_frame->setCamImgID( m_previous_image_id );

            return NEW_FRAME;
        }
    }


    if ( p_packet_header->controlbits.not_epilog )
        return processImageData( p_packet_header, data_size, p_packet_data );
    else
        return processEpilogData( p_packet_header, data_size, p_packet_data );

}


int FrameAssembler::processImageData( CameraRecvPacketHeader * p_packet_header,
                                      int data_size, unsigned char * p_packet_data )
{
    // Asi nenastane, jen pro jistotu.
    if ( (int) p_packet_header->block_cnt >= m_number_expected_packets )
    {
        std::cerr << "ERROR: Too many packets per image" << std::endl;
        return INTERNAL_ERROR;
    }

    mp_recv_frame->insertData( p_packet_header->block_cnt * CAM_RECV_PACKET_DATA_SIZE,
                               p_packet_data, data_size );

    m_number_received_packets++;

    return FRAME_PROCESSED;
}


int FrameAssembler::processEpilogData( CameraRecvPacketHeader * ,
                                       int , unsigned char *  )
{
    return EPILOG_DATA;
}


