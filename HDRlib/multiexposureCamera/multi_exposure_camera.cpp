#include "multi_exposure_camera.h"

#include <iostream>

#ifdef unix
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>


MultiExposureCamera::MultiExposureCamera( void )
    : mp_frame( NULL ),
      m_camera_socket( -1 ),
      m_last_send_packet_id( 0 ),
      m_is_camera_running( false )
{

    mp_frame_assembler = new FrameAssembler( 1280, 1015, Frame::DATA_8UC1,
                                             1280*1016 / CAM_RECV_PACKET_DATA_SIZE );


    m_number_received_packets = 0;
    m_img_id = 0;
    m_rec_error_limit = 10;
}


MultiExposureCamera::~MultiExposureCamera( void )
{
    if ( m_is_camera_running )
        stopCameraGrabber();

    if ( m_camera_socket != -1 )
    {
#ifdef unix
        close( m_camera_socket );
#elif win32
        closesocket( m_camera_socket );
#endif
    }

    delete mp_frame_assembler;

    if ( mp_frame != NULL )
    {
        delete mp_frame;
    }
}

#ifdef unix
bool MultiExposureCamera::initConnection( const char * p_camera_ip,
                                          const short camera_port,
                                          const char * p_local_ip,
                                          bool )
{
    if ( ! p_camera_ip )
    {
        logErrorMessage( "IP address of HDR camera is not defined." );
        return false;
    }

    if ( ! p_local_ip )
    {
        logErrorMessage( "IP address of HDR camera is not defined." );
        return false;
    }

    m_camera_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( m_camera_socket == -1 )
    {
        logErrorMessage( "Failed to create socket" );
        return false;
    }


    m_local_sockaddr.sin_family = AF_INET;
    m_local_sockaddr.sin_port = htons( camera_port );
    m_local_sockaddr.sin_addr.s_addr = inet_addr( p_local_ip );

    m_camera_sockaddr.sin_family = AF_INET;
    m_camera_sockaddr.sin_port = htons( camera_port );
    m_camera_sockaddr.sin_addr.s_addr = inet_addr( p_camera_ip );

    return true;
}
#endif


#ifdef win32
bool MultiExposureCamera::initConnection( const char * p_camera_ip,
                                          const short camera_port,
                                          const char * p_local_ip,
                                          bool )
{
    WSADATA wsa_data;

    if ( ! p_camera_ip )
    {
        logErrorMessage( "IP address of HDR camera is not defined." );
        return false;
    }

    if ( ! p_local_ip )
    {
        logErrorMessage( "IP address of HDR camera is not defined." );
        return false;
    }

    if( WSAStartup( MAKEWORD(2, 2), &wsa_data ) != NO_ERROR )
    {
        logErrorMessage( "Socket Initialization: Error with WSAStartup" );
        WSACleanup();
        return false;
    }

    m_camera_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( m_camera_socket == INVALID_SOCKET )
    {
        logErrorMessage( "Failed to create socket" );
        WSACleanup();
        return false;
    }


    m_local_sockaddr.sin_family = AF_INET;
    m_local_sockaddr.sin_port = htons( camera_port );
    m_local_sockaddr.sin_addr.s_addr = inet_addr( p_local_ip );

    m_camera_sockaddr.sin_family = AF_INET;
    m_camera_sockaddr.sin_port = htons( camera_port );
    m_camera_sockaddr.sin_addr.s_addr = inet_addr( p_camera_ip );

    return true;
}
#endif


#ifdef unix
bool MultiExposureCamera::bindToCamera( void )
{
    if ( bind( m_camera_socket, (sockaddr *) &m_local_sockaddr,
               sizeof(m_local_sockaddr) ) == -1 )
    {
        logErrorMessage( "Failed to bind HDR camera" );

        return false;
    }

    timeval time_val;
    time_val.tv_sec = 0;
    time_val.tv_usec = 500000;
    setsockopt( m_camera_socket, SOL_SOCKET, SO_RCVTIMEO, &time_val, sizeof( time_val ) );

    // TODO udelat vypis a tak
    int opt_val;
    socklen_t opt_len;
    opt_len = sizeof( opt_val );

    opt_val = 300000;
    setsockopt( m_camera_socket, SOL_SOCKET, SO_RCVBUF, &opt_val, opt_len );


    getsockopt( m_camera_socket, SOL_SOCKET, SO_RCVBUF, &opt_val, &opt_len );
    std::cout << "SO_RCVBUF: " << opt_val << std::endl;

    return true;

}
#endif


#ifdef win32
bool MultiExposureCamera::bindToCamera( void )
{
    if ( bind( m_camera_socket, (sockaddr *) &m_local_sockaddr,
               sizeof(m_local_sockaddr) ) == SOCKET_ERROR )
    {
        logErrorMessage( "Failed to bind HDR camera" );
        WSACleanup();
        return false;
    }

    DWORD time_val;
    time_val = 500000;
    setsockopt( m_camera_socket, SOL_SOCKET, SO_RCVTIMEO,
                (char *) &time_val, sizeof( time_val ) );

    // TODO udelat vypis a tak
    int opt_val;
    int opt_len;
    opt_val = 300000;
    opt_len = sizeof( opt_val );

    setsockopt( m_camera_socket, SOL_SOCKET, SO_RCVBUF,
                (char *) &opt_val, opt_len );


    getsockopt( m_camera_socket, SOL_SOCKET, SO_RCVBUF,
                (char *) &opt_val, &opt_len  );
    std::cout << "SO_RCVBUF: " << opt_val << std::endl;

    return true;

}
#endif

bool MultiExposureCamera::grabFrame( bool must_complete )
{
    int recv_error_count = 0;
    if(must_complete)
        recv_error_count = 0;

    while ( true )
    {
        // Příjem paketu. Pokud se překročí počet chyb v přijetí paketu, tak se
        // grabování ukončí (většinou chyba ve spojení).
        if ( ! receivePacket() )
        {
            recv_error_count++;

            if ( recv_error_count > m_rec_error_limit )
            {
                logErrorMessage( "Too many receiving errror during grab image" );
                return false;
            }
            else
                continue;
        }

        switch( processReceivedPacket() )
        {
        case NON_VIDEO_DATA_PROCESSED:
        case VIDEO_DATA_PROCESSED:
            break;

        case FRAME_COMPLETE:
            if ( mp_frame != NULL )
                delete mp_frame;

            mp_frame = mp_frame_assembler->releaseFrame();

            // Mozna do budoucnu dodelat, aby se zpracoval i výstup
            // po tomto zpracování
            processReceivedPacket();

            return true;

        case INTERNAL_ERROR:
            break;

        default:
            logErrorMessage( "Undefine error",
                             "MultiExposureCamera::grabFrame()" );
            return false;
        }
    }
    return true;
}


Frame * MultiExposureCamera::releaseFrame( void )
{
    // Jakmile se uvolní snímke, tak se ukazatel na něj nastaví na NULL
    // což je příznak, že nyní nemáme žádný dostupný snímek
    Frame * p_frame;
    p_frame = mp_frame;
    mp_frame = NULL;

    return p_frame;
}


bool MultiExposureCamera::receivePacket( void )
{
#ifdef unix
    m_recv_packet_size = recv( m_camera_socket, &m_receive_buffer,
                               RECEIVE_BUFFER_SIZE, 0 );
#elif win32
    m_recv_packet_size = recv( m_camera_socket, (char *) &m_receive_buffer,
                               RECEIVE_BUFFER_SIZE, 0 );
#endif

    if ( m_recv_packet_size == -1 )
    {
        logErrorMessage( "Failed to receive any data." );
        return false;
    }

    return true;
}


int MultiExposureCamera::processReceivedPacket( void )
{
    int result;
    CameraRecvPacketHeader * p_packet_header;

    // Nastavení ukazatele na počátek hlavičky paketu.
    // Hlavička paketu se nachází až za daty.
    p_packet_header = (CameraRecvPacketHeader *)
            ( m_receive_buffer + m_recv_packet_size - sizeof( CameraRecvPacketHeader ) );

    if ( p_packet_header->controlbits.not_video )
    {
        // Zpracování neobrazových dat. Pro naše účely není zapotřebí.
        return NON_VIDEO_DATA_PROCESSED;
    }
    else
    {
        /* DEBUG
        std::cout << p_packet_header->block_cnt << "  ";
        if ( p_packet_header->block_cnt % 20 == 0 )
            std::cout << "\n";
        */
        result = mp_frame_assembler->processPacket( p_packet_header,
                                                    m_recv_packet_size - sizeof( CameraRecvPacketHeader ),
                                                    m_receive_buffer );

        switch ( result )
        {
        case FrameAssembler::FRAME_PROCESSED:
        case FrameAssembler::OLD_FRAME:
            return VIDEO_DATA_PROCESSED;

        case FrameAssembler::EPILOG_DATA:
            return NON_VIDEO_DATA_PROCESSED;

        case FrameAssembler::NEW_FRAME:
            return FRAME_COMPLETE;

        default:
            logErrorMessage( "Undefine error",
                             "MultiExposureCamera::processReceivedPacket()" );
            return INTERNAL_ERROR;
        }
    }
}


bool MultiExposureCamera::sendCommandPacket( unsigned short target_id,
                                          unsigned short length,
                                          unsigned char *p_buffer )
{
    CameraSendPacket packet;
    int send_size, data_size;

    // Výpočet velikosti dat v závislosti na datovém typu
    // Pro jistotu kontrola jestli velikost dat nepřesahuje
    // velikost bufferu v paketu. Nemělo by nastat.
    data_size = length * sizeof( unsigned short );
    if ( data_size > CAM_MAX_SEND_PACKET_DATA_SIZE )
    {
        std::cerr << "Warning: Size of sent data is too big - "
                  << data_size << "B. The data are truncated at 120B." << std::endl;

        data_size = CAM_MAX_SEND_PACKET_DATA_SIZE;
    }


    // Naplnění hlavičky paketu
    packet.dead_beef = CAM_DEAD_BEEF_VALUE;
    packet.packet_id = ++m_last_send_packet_id;
    packet.target_id = target_id;
    packet.data_size = data_size;


    // Kopírování dat do paketu
    memcpy( packet.data, p_buffer, data_size );

    send_size = sendto( m_camera_socket,
                        (char *) &packet,
                        CAM_SEND_PACKET_HEADER_SIZE + data_size,
                        0,
                        (sockaddr *) &m_camera_sockaddr,
                        sizeof( m_camera_sockaddr ) );
    if ( send_size == -1 )
    {
        logErrorMessage( "Failed to send packet" );

        // Zpracovani dat MCU v kamere
#ifdef unix
        usleep( 20000 );
#elif win32
        Sleep( 2 );
#endif

        return false;
    }
    else
    {
        // Zpracovani dat MCU v kamere
#ifdef unix
        usleep( 20000 );
#elif win32
        Sleep( 2 );
#endif

        return true;
    }

}


bool MultiExposureCamera::sendHeyYouPacket( bool is_required_response )
{
    // sendHeyYouPacket si vytváří vlastní buffer, aby nedocházelo k přepsání
    unsigned short buffer[CAM_MAX_SEND_PACKET_DATA_SIZE / sizeof(unsigned short)];

    buffer[0] = CAM_SYS_CMD_HEY_YOU;
    buffer[1] = is_required_response ? 0 : 1;

    return sendCommandPacket( CAM_TARGET_ID_SYSTEM, 2,
                              (unsigned char *) buffer );
}


bool MultiExposureCamera::startCameraGrabber( void )
{
    m_send_buffer[0] = CAM_GRAB_CMD_GRABBER_START;

    sendHeyYouPacket( false );

    if ( sendCommandPacket( CAM_TARGET_ID_GRABBER, 1,
                            (unsigned char *) m_send_buffer ) )
    {
        m_is_camera_running = true;
        sendHeyYouPacket( true );

        return true;
    }
    else
        return false;
}


bool MultiExposureCamera::stopCameraGrabber( void )
{
    m_send_buffer[0] = CAM_GRAB_CMD_GRABBER_STOP;

    if ( sendCommandPacket( CAM_TARGET_ID_GRABBER, 1,
                            (unsigned char *) m_send_buffer ) )
    {
        m_is_camera_running = false;
        return true;
    }
    else
        return false;
}


bool MultiExposureCamera::setCameraActiveWindow( unsigned short start_x, unsigned short end_x,
                                              unsigned short start_y, unsigned short end_y )
{
    m_send_buffer[0] = CAM_GRAB_CMD_SET_ACTIVE_WINDOW;
    m_send_buffer[1] = start_x;
    m_send_buffer[2] = end_x;
    m_send_buffer[3] = start_y;
    m_send_buffer[4] = end_y;

    return sendCommandPacket( CAM_TARGET_ID_GRABBER, 5,
                              (unsigned char *) m_send_buffer );
}

bool MultiExposureCamera::setCameraGain( unsigned short gain )
{
    m_send_buffer[0] = CAM_PRM_SET_GAIN;
    m_send_buffer[1] = gain | (gain << 8); // Netuším proč takhle, vyčteno z originálních paketu

    return sendCommandPacket( CAM_TARGET_ID_PARAMS, 2,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraOffset( unsigned short offset )
{
    m_send_buffer[0] = CAM_PRM_SET_OFFSET;
    m_send_buffer[1] = offset | (offset << 8); // Netuším proč takhle, vyčteno z originálních paketu

    return sendCommandPacket( CAM_TARGET_ID_PARAMS, 2,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraExposure( unsigned short exposure )
{
    m_send_buffer[0] = CAM_PRM_SET_EXPOSURE;
    m_send_buffer[1] = exposure;

    return sendCommandPacket( CAM_TARGET_ID_PARAMS, 2,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraAperture( unsigned short aperture )
{
    m_send_buffer[0] = CAM_PRM_SET_APERTURE;
    m_send_buffer[1] = aperture;

    return sendCommandPacket( CAM_TARGET_ID_PARAMS, 2,
                              (unsigned char *) m_send_buffer );
}

bool MultiExposureCamera::setIrisPSDParams( unsigned short p_par, unsigned short s_par,
                                         unsigned short d_par, unsigned short brightness,
                                         unsigned short reg_s_max_value )
{
    m_send_buffer[0] = CAM_IRIS_MACHINE_SET_REGULATOR_PAR;
    m_send_buffer[1] = p_par;
    m_send_buffer[2] = s_par;
    m_send_buffer[3] = d_par;
    m_send_buffer[4] = brightness;
    m_send_buffer[5] = reg_s_max_value;

    return sendCommandPacket( CAM_TARGET_ID_IRIS_MACHINE, 6,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setIrisPSDAutomode( bool automode_enabled )
{
    if ( automode_enabled )
        m_send_buffer[0] = CAM_IRIS_MACHINE_REGULATOR_START;
    else
        m_send_buffer[0] = CAM_IRIS_MACHINE_REGULATOR_STOP;

    return sendCommandPacket( CAM_TARGET_ID_IRIS_MACHINE, 1,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setIrisAutoExpoLimit( unsigned short value )
{
    m_send_buffer[0] = CAM_IRIS_MACHINE_REGULATOR_MAX_EXPO;
    m_send_buffer[1] = value;

    return sendCommandPacket( CAM_TARGET_ID_IRIS_MACHINE, 2,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraHeating( bool heating_enabled )
{
    if ( heating_enabled )
        m_send_buffer[0] = CAM_SYS_CMD_HEAT_ENABLE;
    else
        m_send_buffer[0] = CAM_SYS_CMD_HEAT_DISABLE;

    return sendCommandPacket( CAM_TARGET_ID_SYSTEM, 1,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraAntiFog( bool anti_fog_enabled )
{
    if ( anti_fog_enabled )
        m_send_buffer[0] = CAM_SYS_CMD_PELT_ENABLE;
    else
        m_send_buffer[0] = CAM_SYS_CMD_PELT_DISABLE;

    return sendCommandPacket( CAM_TARGET_ID_SYSTEM, 1,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraFocus( unsigned short focus )
{
    m_send_buffer[0] = CAM_PRM_SET_FOCUS;
    m_send_buffer[1] = focus;

    return sendCommandPacket( CAM_TARGET_ID_PARAMS, 2,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraFlashTime( unsigned short time )
{
    m_send_buffer[0] = CAM_PRM_SET_FLASH_TIME;
    m_send_buffer[1] = time;

    return sendCommandPacket( CAM_TARGET_ID_PARAMS, 2,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::requestSequnce( unsigned short count_sequence,
                                       unsigned int * p_image_id, unsigned short * p_config )
{
    m_send_buffer[0] = CAM_SQC_CMD_REQUEST_SEQUENCE;
    m_send_buffer[1] = count_sequence;

    for ( unsigned short i = 0; i < count_sequence; i++ )
    {
        m_send_buffer[i*3 + 2] = p_image_id[i] & 0xFFFF;
        m_send_buffer[i*3 + 2 + 1] = (p_image_id[i] >> 16) & 0xFFFF;
        m_send_buffer[i*3 + 2 + 2] = p_config[i];
    }

    return sendCommandPacket( CAM_TARGET_ID_SEQ_CONFIG, 3*count_sequence + 2,
                              (unsigned char *) m_send_buffer );
}


bool MultiExposureCamera::setCameraImgConfig( const CameraImageConfig & img_config )
{
    unsigned short cnt = 0;
    unsigned short buffer[CAM_MAX_SEND_PACKET_DATA_SIZE / sizeof(unsigned short)];

    buffer[cnt++] = CAM_SQC_CMD_SET_IMAGE_CONFIG;  // command ID
    buffer[cnt++] = img_config.config_index;          // ktera konfigurace

    // nastaveni triggeru
    buffer[cnt++] = CAM_SQC_CMD_SET_TRIGGER;
    buffer[cnt++] = img_config.trigger_wire_id;
    buffer[cnt++] = img_config.trigger_delay_cnt;
    //  ###  aco takhle poslat triggertype

    // nastaveni expozice
    switch ( img_config.exposure_type )
    {
    case 0 : // absolute expo
      buffer[cnt++] = CAM_SQC_CMD_SET_EXPO_FIXED;
      buffer[cnt++] = img_config.exposure_value;
      break;
    case 1 : // relative expo
      buffer[cnt++] = CAM_SQC_CMD_SET_EXPO_RELATIVE;
      buffer[cnt++] = img_config.exposure_value;
      buffer[cnt++] = img_config.exposure_max_limit;
      break;
    default :
      buffer[cnt++] = CAM_SQC_CMD_SET_EXPO_USER_CFG;
      buffer[cnt++] = img_config.exposure_type;
      buffer[cnt++] = img_config.exposure_value;
      buffer[cnt++] = img_config.exposure_max_limit;
      break;
    }

    // svetlo 0
    switch ( img_config.flash_1_type )
    {
    case 0 : // absolute
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_FIXED;
      buffer[cnt++] = 0;
      buffer[cnt++] = img_config.flash_1_abs_value;
      buffer[cnt++] = img_config.flash_1_wire_id;
      break;
    case 1 : // relative
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_RELATIVE;
      buffer[cnt++] = 0;
      buffer[cnt++] = img_config.flash_1_rel_value;
      buffer[cnt++] = img_config.flash_1_wire_id;
      break;
    case 2 : // default
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_DEFAULT;
      buffer[cnt++] = 0;
      break;
    default :
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_USER_CFG;
      buffer[cnt++] = 0;
      buffer[cnt++] = img_config.flash_1_type;
      buffer[cnt++] = img_config.flash_1_abs_value;
      buffer[cnt++] = img_config.flash_1_rel_value;
      buffer[cnt++] = img_config.flash_1_wire_id;
      break;
    }

    // svetlo 1
    switch ( img_config.flash_2_type )
    {
    case 0 : // absolute
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_FIXED;
      buffer[cnt++] = 0;
      buffer[cnt++] = img_config.flash_2_abs_value;
      buffer[cnt++] = img_config.flash_2_wire_id;
      break;
    case 1 : // relative
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_RELATIVE;
      buffer[cnt++] = 0;
      buffer[cnt++] = img_config.flash_2_rel_value;
      buffer[cnt++] = img_config.flash_2_wire_id;
      break;
    case 2 : // default
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_DEFAULT;
      buffer[cnt++] = 0;
      break;
    default :
      buffer[cnt++] = CAM_SQC_CMD_SET_LIGHT_USER_CFG;
      buffer[cnt++] = 0;
      buffer[cnt++] = img_config.flash_2_type;
      buffer[cnt++] = img_config.flash_2_abs_value;
      buffer[cnt++] = img_config.flash_2_rel_value;
      buffer[cnt++] = img_config.flash_2_wire_id;
      break;
    }

    // resolution
    buffer[cnt++] = CAM_SQC_CMD_SET_RESOLUTION;
    buffer[cnt++] = img_config.resolution_type;

    buffer[cnt++] = CAM_SQC_CMD_SET_ETH_OUT_ENABLE;
    buffer[cnt++] = img_config.eth_enabled ? 1 : 0;

    buffer[cnt++] = CAM_SQC_CMD_SET_CONFIG_ENABLE;
    buffer[cnt++] = img_config.config_enabled ? 1 : 0;

    buffer[cnt++] = CAM_SQC_CMD_SET_TRIGGER_EDGE;
    buffer[cnt++] = img_config.trigger_edge_polarity;

    buffer[cnt++] = CAM_SQC_CMD_SET_TRIGGER_TYPE;
    buffer[cnt++] = img_config.trigger_type;

    buffer[cnt++] = CAM_SQC_CMD_SET_NEXT_CONFIG;
    buffer[cnt++] = img_config.next_config;


    // END marker
    buffer[cnt++] = CAM_SQC_CMD_SET_END;

    return sendCommandPacket( CAM_TARGET_ID_SEQ_CONFIG, cnt,
                             (unsigned char *) buffer );
}


void MultiExposureCamera::logErrorMessage( const char * p_error_message,
                                        const char * p_function_name )
{
    if ( ! p_function_name )
    {
        std::cerr << "Error: " << p_error_message << std::endl;
    }
    else
    {
        std::cerr << "Error in " << p_function_name << " : "
                  << p_error_message << std::endl;
    }
}
