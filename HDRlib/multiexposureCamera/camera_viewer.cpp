#include "camera_viewer.h"

#ifdef unix
#include <unistd.h>
#elif win32
#endif

#include <iostream>


CameraViewer::CameraViewer( void )
    : mp_camera_thrd( NULL ),
      mp_sequence_images_id( NULL ),
      m_current_sequence( 0 ),
      m_view_size_ratio( 1.0 )
{
}

CameraViewer::~CameraViewer( void )
{
    for ( unsigned int i = 0; i < m_sequence_configs.size(); i++ )
        m_sequence_configs.at(i)->clear();

    m_sequence_configs.clear();

    if ( mp_sequence_images_id != NULL )
        delete mp_sequence_images_id;
}

void CameraViewer::setCamera( MultiExposureCameraThrd * p_camera_thrd )
{
    mp_camera_thrd = p_camera_thrd;
}


void CameraViewer::viewCamera( bool is_multiexposure , bool is_half_frame )
{
    int wait_key;
    Frame * p_frame;
    unsigned int frame_id;

    if ( mp_camera_thrd == NULL )
        return;

    if ( ! mp_camera_thrd->isRunning() )
        mp_camera_thrd->startGrabbing();


    if ( is_multiexposure )
    {
        if ( mp_sequence_images_id == NULL )
        {
            std::cerr << "Error: you have to init multi exposure sequence" << std::endl;
            return;
        }
        else
            mp_camera_thrd->requestSequnce(
                        m_sequence_length,
                        mp_sequence_images_id,
                        m_sequence_configs.at( m_current_sequence )->data() );
    }

    while( true )
    {
        p_frame = mp_camera_thrd->releaseFrame();
        if ( p_frame == NULL )
        {
#ifdef unix
            usleep( 50000 );
#elif win32
            Sleep( 50 );
#endif
            continue;
        }

        frame_id = p_frame->camImgID();

        if ( is_multiexposure )
        {
            if ( (frame_id-10) % m_sequence_length == 0 )
            {
                m_current_sequence++;
                if ( m_current_sequence >= m_sequence_configs.size() )
                    m_current_sequence = 0;

                for ( int i = 0; i < m_sequence_length; i++ )
                    mp_sequence_images_id[i] += m_sequence_length;

                mp_camera_thrd->requestSequnce(
                            m_sequence_length,
                            mp_sequence_images_id,
                            m_sequence_configs.at( m_current_sequence )->data() );
            }
        }

        if ( is_half_frame )
            p_frame->cutHalfFrame();


        // Zatím takhle časem možná předělat
        cv::Mat orig_frame( p_frame->height(), p_frame->width(),
                            CV_8UC1, p_frame->ptrData() );

        cv::Mat view_frame;
        if ( m_view_size_ratio != 1.0f )
        {
            cv::Size view_size( p_frame->width() * m_view_size_ratio,
                                p_frame->height() * m_view_size_ratio );

            cv::resize( orig_frame, view_frame, view_size );
        }
        else
            view_frame = orig_frame;

        cv::imshow( "View Camera", view_frame );

        // Poslání HeyYouPacketu pro udržení kamery v činném režimu
        if ( p_frame->camImgID() % 100 == 0 )
            mp_camera_thrd->sendHeyYouPacket();

        wait_key = 255 & cv::waitKey( 50 );

        delete p_frame;

        if ( wait_key == 27 ) // Esc
        {
            break;
        }
    }

    //cvDestroyWindow( "View Camera" );

    mp_camera_thrd->stopGrabbing();
}


void CameraViewer::initSequence( const std::vector<unsigned short> * p_configs ,
                                 int repetition )
{
    if ( repetition < 3 )
    {
        std::cerr << "Warning: Minimum number of repetition during multi-exposition "
                  << "showing is 3, repetition value is set to 3." << std::endl;

        repetition = 3;
    }
    else if ( repetition > 16 )
    {
        std::cerr << "Warning: Maximum number of repetition during multi-exposition "
                  << "showing is 16, repetition value is set to 16." << std::endl;

        repetition = 16;
    }

    for ( unsigned int i = 0; i < p_configs->size(); i++ )
    {
        m_sequence_configs.push_back( new std::vector<unsigned short> );

        for ( int j = 0; j < repetition; j++ )
            m_sequence_configs.at(i)->push_back( p_configs->at(i) );
    }


    mp_sequence_images_id = new unsigned int[repetition];
    for ( int i = 0; i < repetition; i++ )
        mp_sequence_images_id[i] = 10+i;

    m_sequence_length = repetition;
}


void CameraViewer::setViewSizeRatio( float ratio )
{
    m_view_size_ratio = ratio;
}
