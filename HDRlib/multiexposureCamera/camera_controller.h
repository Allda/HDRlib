#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#define WIEW_WAIT_KEY_TIME 30

#include <string>
#include "multi_exposure_camera_thrd.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ldrimage.h"
#include <deque>
//#include "load_configuration.h"
//#include "multi_exposure_camera.h"

class LoadConfiguration;


class CameraController
{
public:
    enum CameraAction { SHOW, RECORD, RECORD_AND_SHOW, UNDEFINED, GETIMAGES };

    CameraController( void );
    ~CameraController( void );

    bool show( void );
    bool capture( bool is_view = false );
    bool doConfigAction( void );
    std::vector<LDRImage *> getImages();

    bool connectToCamera( void );
    bool loadConfiguration( const char * p_configuration_file );
    bool configureCamera( void );


private:
    MultiExposureCameraThrd * mp_camera;
    LoadConfiguration * mp_configuration;
    std::vector<LDRImage *> ldrImageList;
};


#endif // CAMERA_CONTROLLER_H
