#ifndef LOAD_CONFIGURATION_H
#define LOAD_CONFIGURATION_H

#include <string>
#include <vector>

#include "camera_controller.h"


class LoadConfiguration
{
public:
    LoadConfiguration();

    bool load( const char * p_filename );

    CameraController::CameraAction cameraAction( void ) const;
    float viewRatio( void ) const;

    const std::string * localIP( void ) const;
    const std::string * cameraIP( void ) const;
    int port( void ) const;

    bool isMultiExposureMode( void ) const;

    int numberCapturedFrames( void ) const;

    int cameraGain( void ) const;
    int cameraOffset( void ) const;
    const int * cameraExposures( void ) const;
    int cameraAperture( void ) const;
    const std::vector<unsigned short> * exposureSequence( void ) const;
    bool isHalfFrame( void ) const;

    const std::vector<int> * cameraActiveWindow( void ) const;
    const std::vector<int> * irisPSDParams( void ) const;
    int irisAutoExpoLimit( void ) const;
    bool isPSDAutoMode( void ) const;
    bool isCameraHeating( void ) const;
    bool isCameraAntiFog( void ) const;
    int cameraFlashTime( void ) const;
    int getExposureValue(int index);

private:
    CameraController::CameraAction convertAction( std::string & str );
    bool isNumber( std::string & str, bool is_decimal = false );
    bool isBool( std::string & str );
    int stringToInt( std::string & str );
    float stringToFloat( std::string & str );
    int stringToBool( std::string & str );

    CameraController::CameraAction m_camera_action;
    float m_view_ratio;

    std::string m_local_ip;
    std::string m_camera_ip;
    int m_port;

    bool m_is_multi_exposure_mode;

    int m_number_captured_frames;

    int m_camera_gain;
    int m_camera_offset;
    int m_camera_exposures[4];
    int m_camera_aperture;

    std::vector<unsigned short> m_exposure_sequence;

    bool m_is_half_frame;

    std::vector<int> m_camera_active_window;
    std::vector<int> m_iris_psd_params;
    int m_iris_auto_expo_limit;
    bool m_is_psd_auto_mode;
    bool m_is_camera_heating;
    bool m_is_camera_anti_fog;
    int m_camera_flash_time;
};


inline CameraController::CameraAction LoadConfiguration::cameraAction( void ) const
{
    return m_camera_action;
}

inline float LoadConfiguration::viewRatio( void ) const
{
    return m_view_ratio;
}

inline const std::string * LoadConfiguration::localIP( void ) const
{
    return &m_local_ip;
}

inline const std::string * LoadConfiguration::cameraIP( void ) const
{
    return &m_camera_ip;
}

inline int LoadConfiguration::port( void ) const
{
    return m_port;
}

inline bool LoadConfiguration::isMultiExposureMode( void ) const
{
    return m_is_multi_exposure_mode;
}

inline int LoadConfiguration::numberCapturedFrames( void ) const
{
    return m_number_captured_frames;
}

inline int LoadConfiguration::cameraGain( void ) const
{
    return m_camera_gain;
}

inline int LoadConfiguration::cameraOffset( void ) const
{
    return m_camera_offset;
}

inline const int * LoadConfiguration::cameraExposures( void ) const
{
    return m_camera_exposures;
}

inline int LoadConfiguration::cameraAperture( void ) const
{
    return m_camera_aperture;
}

inline const std::vector<unsigned short> * LoadConfiguration::exposureSequence( void ) const
{
    return &m_exposure_sequence;
}

inline bool LoadConfiguration::isHalfFrame( void ) const
{
    return m_is_half_frame;
}

inline const std::vector<int> * LoadConfiguration::cameraActiveWindow( void ) const
{
    return &m_camera_active_window;
}

inline const std::vector<int> * LoadConfiguration::irisPSDParams( void ) const
{
    return &m_iris_psd_params;
}

inline int LoadConfiguration::irisAutoExpoLimit( void ) const
{
    return m_iris_auto_expo_limit;
}

inline bool LoadConfiguration::isPSDAutoMode( void ) const
{
    return m_is_psd_auto_mode;
}

inline bool LoadConfiguration::isCameraHeating( void ) const
{
    return m_is_camera_heating;
}

inline bool LoadConfiguration::isCameraAntiFog( void ) const
{
    return m_is_camera_anti_fog;
}

inline int LoadConfiguration::cameraFlashTime( void ) const
{
    return m_camera_flash_time;
}


#endif // LOAD_CONFIGURATION_H
