#ifndef CAMERA_VIEWER_H
#define CAMERA_VIEWER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "multi_exposure_camera_thrd.h"


/**
 * @brief The CameraViewer class slouží pro nahled obrazu zachycovaného z kamery
 *
 * Nahled lze ukončit klávesou Esc.
 *
 * Při zobrazování multi-expozičních snímků je nutné inicializovat
 * multi-expoziční sekvenci prostřednictvím metoda initSequence( ... ).
 *
 * Obraz z kamery je zobrazován v plném rozlišení zachycených snímků. Pro změnu
 * velikosti nahledu slouží metoda setViewSizeRatio(...). Tímto poměrem se násobí
 * originální velikost snímku.
 */

class CameraViewer
{
public:
    CameraViewer( void );
    ~CameraViewer( void );

    void setCamera( MultiExposureCameraThrd * p_camera_thrd );
    void viewCamera( bool is_multiexposure, bool is_half_frame );
    void initSequence( const std::vector<unsigned short> * p_configs,
                       int repetatition = 5 );
    void setViewSizeRatio( float ratio );

private:
    MultiExposureCameraThrd * mp_camera_thrd;

    unsigned int * mp_sequence_images_id;
    std::vector<std::vector<unsigned short> *> m_sequence_configs;
    unsigned short m_sequence_length;
    unsigned int m_current_sequence;

    float m_view_size_ratio;
};

#endif // CAMERA_VIEWER_HPP
