#include "multi_exposure_camera_thrd.h"

#include <unistd.h>


MultiExposureCameraThrd::MultiExposureCameraThrd()
    : MultiExposureCamera(),
      m_head_frames_buffer( 0 ),
      m_tail_frames_buffer( -1 ),
      m_thread(),
      m_is_running( false ),
      m_stop_cmd( false ),
      m_quit_cmd( false )

{
    for ( int i = 0; i < FRAMES_BUFFER_LENGTH; i++ )
        m_frames_buffer[i] = NULL;
}


MultiExposureCameraThrd::~MultiExposureCameraThrd( void )
{
    m_quit_cmd.store( true );

    if ( m_thread.joinable() )
        m_thread.join();

    for ( int i = 0; i < FRAMES_BUFFER_LENGTH; i++ )
    {
        if ( m_frames_buffer[i] != NULL )
            delete m_frames_buffer[i];
    }
}


void MultiExposureCameraThrd::startGrabbing( void )
{
    if ( m_stop_cmd )
        m_stop_cmd.store( false );

    if ( m_is_running )
        return;

    m_thread = std::thread( &MultiExposureCameraThrd::runGrabbing, this );
    m_is_running.store( true );
}


void MultiExposureCameraThrd::stopGrabbing( void )
{
    m_stop_cmd.store( true );
}


Frame *MultiExposureCameraThrd::releaseFrame( void )
{
    return pullFrameFromBuffer();

    if ( m_is_running )
        return pullFrameFromBuffer();
    else
        return MultiExposureCamera::releaseFrame();
}


// Pokud běží grabování snímku, tak funkce vratí false a nic se neprovede
// jinak se volá běžné grabování snímku
bool MultiExposureCameraThrd::grabFrame( bool must_complete )
{
    if ( m_is_running )
        return false;

    return MultiExposureCamera::grabFrame( must_complete );
}


bool MultiExposureCameraThrd::isRunning( void ) const
{
    return m_is_running.load();
}


void MultiExposureCameraThrd::runGrabbing()
{
    Frame * p_frame;

    while ( ! m_quit_cmd )
    {
        if ( m_stop_cmd )
        {
#ifdef unix
            usleep( 1000 );
#elif win32
            Sleep( 2 );
#endif
            continue;
        }
        else
        {
            MultiExposureCamera::grabFrame();
            p_frame = MultiExposureCamera::releaseFrame();

            pushFrameToBuffer( p_frame );
        }
    }

    m_is_running.store( false );
}

void MultiExposureCameraThrd::pushFrameToBuffer( Frame *p_frame )
{
    //std::cout << "MultiExposureCameraThrd::pushFrameToBuffer" << std::endl;
    //std::cout << m_head_frames_buffer << "  " << m_tail_frames_buffer << std::endl;

    std::lock_guard<std::mutex> lock( m_buffer_mutex );
    m_frames_buffer[m_head_frames_buffer] = p_frame;

    // Pokud byl buffer prázdný, tak se nastaví index výstupu na nově vložený
    // snímek.
    if ( m_tail_frames_buffer < 0 )
        m_tail_frames_buffer = m_head_frames_buffer;

    m_head_frames_buffer = (m_head_frames_buffer+1) % FRAMES_BUFFER_LENGTH;

    // Kontrola jestli není buffer plný, pokud ano, tak se posune index odkud se
    // odebírá frame a původní se přepíše.
    if ( m_head_frames_buffer == m_tail_frames_buffer )
    {
        delete m_frames_buffer[m_tail_frames_buffer];
        m_frames_buffer[m_tail_frames_buffer] = NULL;

        m_tail_frames_buffer = (m_tail_frames_buffer+1) % FRAMES_BUFFER_LENGTH;
    }

}

Frame *MultiExposureCameraThrd::pullFrameFromBuffer( void )
{
    //std::cout << "MultiExposureCameraThrd::pullFrameToBuffer" << std::endl;
    //std::cout << m_head_frames_buffer << "  " << m_tail_frames_buffer << std::endl;

    Frame * p_frame;

    std::lock_guard<std::mutex> lock( m_buffer_mutex );

    if ( m_tail_frames_buffer < 0 )
        return NULL;

    p_frame = m_frames_buffer[m_tail_frames_buffer];
    m_frames_buffer[m_tail_frames_buffer] = NULL;

    m_tail_frames_buffer = (m_tail_frames_buffer+1) % FRAMES_BUFFER_LENGTH;

    // Kontrola jestli není fronta prázdNá pokud ano, tak se nastaví pozice
    // na -1
    if ( m_tail_frames_buffer == m_head_frames_buffer )
        m_tail_frames_buffer = -1;

    return p_frame;
}
