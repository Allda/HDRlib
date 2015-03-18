#ifndef FRAME_HPP
#define FRAME_HPP

#include <iostream>

class Frame
{
public:
    enum { DATA_8UC1, DATA_16UC1, DATA_8UC3, DATA_16UC3 };

    Frame( int width, int height, int type );
    ~Frame();

    Frame * cloneFrame( void );

    bool insertData( int shift, const unsigned char * p_data,
                     int size );
    bool insertRGBValue( int shift, char red, char green, char blue );

    int width( void ) const { return m_width; }
    int height( void ) const { return m_height; }
    int type( void ) const { return m_type; }
    int size( void ) const { return m_size; }
    unsigned int camImgID( void ) const { return m_cam_img_id; }

    void setCamImgID( unsigned int cam_img_id ) { m_cam_img_id = cam_img_id; }

    void cutHalfFrame( void );

    bool saveFrame( void );

    unsigned char * ptrData( void ) const { return mp_data; }

private:
    int m_width;
    int m_height;
    int m_type;
    int m_size;
    unsigned int m_cam_img_id;

    unsigned char * mp_data;
};

#endif // FRAME_HPP
