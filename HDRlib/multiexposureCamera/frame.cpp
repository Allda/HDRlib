#include "frame.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>



Frame::Frame(int width, int height , int type )
    : m_width( width ),
      m_height( height ),
      m_type( type ),
      m_cam_img_id( 0 )
{
    switch ( m_type )
    {
    case DATA_8UC1:
        m_size = m_width * m_height;
        break;

    case DATA_8UC3:
        m_size = m_width * m_height * 3;
        break;

    case DATA_16UC1:
        m_size = m_width * m_height * 2;
        break;

    case DATA_16UC3:
        m_size = m_width * m_height * 6;
        break;

    default:
        // Neočekávají se žádné jiné hodnoty
        break;
    }

    mp_data = (unsigned char *) malloc( m_size * sizeof( char ) );
    memset( mp_data, 0, m_size * sizeof( char ) );
}


Frame::~Frame()
{
    free( mp_data );
}


Frame * Frame::cloneFrame()
{
    Frame * p_frame;

    p_frame = new Frame( m_width, m_height, m_type );
    p_frame->setCamImgID( m_cam_img_id );

    memcpy( p_frame->ptrData(), mp_data, m_size );

    return p_frame;
}


bool Frame::insertData( int shift, const unsigned char * p_data, int size )
{
    // Kontrolo jestli vkládaná data nebudou mimo rozsah
    // alokované paměti.
    if ( shift < 0 )
        return false;

    if ( shift > m_size )
        return false;

    if ( shift + size > m_size )
        return false;

    memcpy( mp_data + shift, p_data, size );

    return true;
}


bool Frame::insertRGBValue( int shift, char red, char green, char blue )
{
    if ( shift < 0 )
        return false;

    if ( shift > m_size )
        return false;

    if ( m_type != DATA_8UC3 )
        return false;

    unsigned char * p_data;
    p_data = mp_data + 3*shift;

    p_data[0] = red;
    p_data[1] = green;
    p_data[2] = blue;

    return true;
}


void Frame::cutHalfFrame( void )
{
    m_height /= 2;

    switch ( m_type )
    {
    case DATA_8UC1:
        m_size = m_width * m_height;
        break;

    case DATA_8UC3:
        m_size = m_width * m_height * 3;
        break;

    case DATA_16UC1:
        m_size = m_width * m_height * 2;
        break;

    case DATA_16UC3:
        m_size = m_width * m_height * 6;
        break;

    default:
        // Neočekávají se žádné jiné hodnoty
        break;
    }


    mp_data = (unsigned char *) realloc( mp_data, m_size * sizeof( char ) );
}


bool Frame::saveFrame()
{
    FILE * p_file;
    char buffer[32];

    switch ( m_type )
    {
    case DATA_8UC1:
    case DATA_16UC1:
        sprintf( buffer, "record/image %3d.pgm", m_cam_img_id );
        break;

    case DATA_8UC3:
    case DATA_16UC3:
        sprintf( buffer, "record/image %3d.ppm", m_cam_img_id );
        break;

    default:
        // Neočekávají se žádné jiné hodnoty
        break;
    }


    p_file = fopen( buffer, "wb" );
    if ( p_file == NULL )
    {
        fprintf( stderr, "Failed to create image file" );
        return false;
    }

    switch ( m_type )
    {
    case DATA_8UC1:
    case DATA_16UC1:
        sprintf( buffer, "P5\n" );
        break;

    case DATA_8UC3:
    case DATA_16UC3:
        sprintf( buffer, "P6\n" );
        break;

    default:
        // Neočekávají se žádné jiné hodnoty
        break;
    }
    fwrite( buffer, sizeof( char ), strlen( buffer ), p_file );

    sprintf( buffer, "%d\n%d\n", m_width, m_height );
    fwrite( buffer, sizeof( char ), strlen( buffer ), p_file );


    switch ( m_type )
    {
    case DATA_8UC1:
    case DATA_8UC3:
        sprintf( buffer, "255\n" );
        break;

    case DATA_16UC1:
    case DATA_16UC3:
        sprintf( buffer, "65535\n" );
        break;

    default:
        // Neočekávají se žádné jiné hodnoty
        break;
    }
    fwrite( buffer, sizeof( char ), strlen( buffer ), p_file );

    fwrite( mp_data, sizeof( char ), m_size, p_file );

    fclose( p_file );
    return true;
}
