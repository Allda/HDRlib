#include "camera_controller.h"

#include "camera_viewer.h"
#include "load_configuration.h"

#include "multi_exposure_camera.h"

#include <unistd.h>

#include <deque>
#include "ldrimage.h"
// OpenCV pro zobrazování nahledu při nahrávání.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


CameraController::CameraController( void )
    : mp_camera( NULL ),
      mp_configuration( NULL )
{
    mp_configuration = new LoadConfiguration;
    mp_camera = new MultiExposureCameraThrd;
    //mp_camera->initConnection("192.168.6.1",10101,"192.168.6.2");
}


CameraController::~CameraController( void )
{
    if ( mp_camera != NULL )
        delete mp_camera;

    if ( mp_configuration != NULL )
        delete mp_configuration;
}


bool CameraController::show( void )
{
    CameraViewer viewer;

    viewer.setCamera( mp_camera );
    viewer.setViewSizeRatio( mp_configuration->viewRatio() );

    if ( mp_configuration->isMultiExposureMode() )
        viewer.initSequence( mp_configuration->exposureSequence(), 12 );

    // Spuštění graberu na kameře
    if ( ! mp_camera->startCameraGrabber() )
        return false;

    // Spuštění grabování snímků z kamery v pc
    mp_camera->startGrabbing();


    // Čekání na naběhnutí kamery
#ifdef unix
            sleep( 1 );
#elif win32
            Sleep( 1000 );
#endif

    viewer.viewCamera( mp_configuration->isMultiExposureMode(),
                       mp_configuration->isHalfFrame() );

    mp_camera->stopGrabbing();

    if ( ! mp_camera->stopCameraGrabber() )
        return false;

    return true;
}

bool CameraController::capture( bool is_view )
{
    Frame * p_frame;
    unsigned int img_id;

    unsigned int number_frames;

    unsigned int * p_sequence_image_id;
    unsigned short * p_sequence_config;
    unsigned short sequence_length;

    float view_size_ratio;
    view_size_ratio = mp_configuration->viewRatio();


    // Aktivní multi-expoziční režim - Vytvoření dat pro zasílání požadavku
    // na multi-expoziční sekvenci z konfigurace.
    if ( mp_configuration->isMultiExposureMode() )
    {
        int length;
        length = mp_configuration->exposureSequence()->size();

        sequence_length = (18 / length) * length;

        p_sequence_config = new unsigned short[sequence_length];
        p_sequence_image_id = new unsigned int[sequence_length];

        for ( int i = 0; i < sequence_length / length; i++ )
        {
            memcpy( p_sequence_config + i*length,
                    mp_configuration->exposureSequence()->data(),
                    length * sizeof( unsigned short ) );
        }

        for ( int i = 0; i < sequence_length; i++ )
            p_sequence_image_id[i] = 10 + i;
    }


    // Spuštění graberu na kameře
    if ( ! mp_camera->startCameraGrabber() )
        return false;

    // Spuštění grabování snímků z kamery v pc
    mp_camera->startGrabbing();


    // Čekání na naběhnutí kamery
#ifdef unix
            sleep( 1 );
#elif win32
            Sleep( 1000 );
#endif


    // Aktivní multi-expoziční režim - Vyslání prvního požadavku na sekvenci
    // multi-expozičních snímků
    if ( mp_configuration->isMultiExposureMode() )
    {
        mp_camera->requestSequnce( sequence_length,
                                   p_sequence_image_id,
                                   p_sequence_config );
    }

    number_frames = mp_configuration->numberCapturedFrames();


    // Vytvoření okna pro zachytávání cv::waitKey() pokud se bude nahrávání
    // ukončovat klávesou. Toto okno se i použije, pokud je aktivní nahled.
    if ( number_frames == 0 )
        cv::namedWindow( "Capturing" );


    while( true )
    {
        p_frame = mp_camera->releaseFrame();

        // Pokud není dostupný žádný snímek tak se krátce čeká
        if ( p_frame == NULL )
        {
#ifdef unix
            usleep( 50000 );
#elif win32
            Sleep( 50 );
#endif
            continue;
        }

        img_id = p_frame->camImgID();

        if ( mp_configuration->isHalfFrame() )
            p_frame->cutHalfFrame();

        p_frame->saveFrame();

        // Zobrazování nahledu, pokud je aktivní
        if ( is_view )
        {
            cv::Mat orig_frame( p_frame->height(), p_frame->width(),
                                CV_8UC1, p_frame->ptrData() );

            cv::Mat view_frame;
            if ( view_size_ratio != 1.0f )
            {
                cv::Size view_size( p_frame->width() * view_size_ratio,
                                    p_frame->height() * view_size_ratio );

                cv::resize( orig_frame, view_frame, view_size );
            }
            else
                view_frame = orig_frame;

            cv::imshow( "Capturing", view_frame );
        }

        // Uklizení aktuálního snímku
        delete p_frame;

        // Kontrala ukončení prostřednictvím počtu snímků.
        if ( number_frames > 0 )
            if ( img_id > number_frames )
                break;

        // Ukončení nahrávání prostřednictvím cv::waitKey(). Ukončení se provádí
        // klávesou Esc.
        if ( number_frames == 0 )
            if ( (255 & cv::waitKey( WIEW_WAIT_KEY_TIME )) == 27 ) // Esc
                break;


        // Aktivní multi-expoziční režim - Posílání požadavku na sekvenci
        // multi-expozičních snímků. Posílá se v předstihu jedne celé sekvence.
        if ( mp_configuration->isMultiExposureMode() )
        {
            if ( (img_id-10) % sequence_length == 0 )
            {
                for ( int i = 0; i < sequence_length; i++ )
                    p_sequence_image_id[i] += sequence_length;

                mp_camera->requestSequnce( sequence_length,
                                           p_sequence_image_id,
                                           p_sequence_config );
            }
        }

        // Poslání HeyYouPacketu pro udržení kamery v činném režimu
        if ( img_id % 100 == 0 )
            mp_camera->sendHeyYouPacket();
    }

    // Aktivní multi-expoziční režim - Uklizení paměti
    if ( mp_configuration->isMultiExposureMode() )
    {
        delete p_sequence_config;
        delete p_sequence_image_id;
    }

    mp_camera->stopGrabbing();

    if ( ! mp_camera->stopCameraGrabber() )
        return false;

    return true;
}


bool CameraController::doConfigAction( void )
{
    bool is_ok;
    switch ( mp_configuration->cameraAction() )
    {
    case CameraAction::SHOW:
        is_ok = show();
        break;

    case CameraAction::RECORD:
        is_ok = capture( false );
        break;

    case CameraAction::RECORD_AND_SHOW:
        is_ok = capture( true );
        break;

    // Undefined hodnota v konfiguraci nenastane.
    default:
        is_ok = false;
        break;
    }

    return is_ok;
}

std::vector<LDRImage *> CameraController::getImages()
{
    Frame * p_frame;
    unsigned int img_id;
    bool startCapturing = false;

    std::map<int, int> exposureIDHistory;

    int index = 0;

    unsigned int * p_sequence_image_id;
    unsigned short * p_sequence_config;
    unsigned short sequence_length;

    // Aktivní multi-expoziční režim - Vytvoření dat pro zasílání požadavku
    // na multi-expoziční sekvenci z konfigurace.
    if ( mp_configuration->isMultiExposureMode() )
    {
        int length;
        length = mp_configuration->exposureSequence()->size();

        sequence_length = (18 / length) * length;

        p_sequence_config = new unsigned short[sequence_length];
        p_sequence_image_id = new unsigned int[sequence_length];

        for ( int i = 0; i < sequence_length / length; i++ )
        {
            memcpy( p_sequence_config + i*length,
                    mp_configuration->exposureSequence()->data(),
                    length * sizeof( unsigned short ) );
        }

        for ( int i = 0; i < sequence_length; i++ ){
            p_sequence_image_id[i] = 10 + i;
            exposureIDHistory[p_sequence_image_id[i]] = p_sequence_config[i];
            std::cout << p_sequence_image_id[i]<< " " << p_sequence_config[i] << std::endl;
        }


    }



    // Spuštění graberu na kameře
    if ( ! mp_camera->startCameraGrabber() )
        throw exception();

    // Spuštění grabování snímků z kamery v pc
    mp_camera->startGrabbing();


    // Čekání na naběhnutí kamery
#ifdef unix
            sleep( 1 );
#elif win32
            Sleep( 1000 );
#endif


    // Aktivní multi-expoziční režim - Vyslání prvního požadavku na sekvenci
    // multi-expozičních snímků
    if ( mp_configuration->isMultiExposureMode() )
    {
        mp_camera->requestSequnce( sequence_length,
                                   p_sequence_image_id,
                                   p_sequence_config );
    }

    /*cv::namedWindow( "Capturing", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
    cv::namedWindow( "First", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );
    cv::namedWindow( "Second", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );
    cv::namedWindow( "Third", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );*/
    LDRImage * ldrImage;

    while( true )
    {
        p_frame = mp_camera->releaseFrame();

        // Pokud není dostupný žádný snímek tak se krátce čeká
        if ( p_frame == NULL )
        {
#ifdef unix
            usleep( 50000 );
#elif win32
            Sleep( 50 );
#endif
            continue;
        }

        img_id = p_frame->camImgID();
        // skip first 11 frames
        if(img_id > 11){
            cv::Mat orig_frame( p_frame->height(), p_frame->width(), CV_8UC1, p_frame->ptrData() );


            //cv::imshow( "Capturing", orig_frame );
            std::cout << img_id << std::endl;

            // catch first frame from sequence
            if((img_id-1)%3 == 0 ){
                std::cout <<"First:" << img_id << std::endl;
                //cv::imshow( "First", orig_frame );
                startCapturing = true;
                index = 1;
                double exposureValue = mp_configuration->getExposureValue(exposureIDHistory[img_id]);
                ldrImage = new LDRImage(orig_frame,exposureValue*0.000001);
                //ldrImage->showImage();
                ldrImageList.push_back(ldrImage);
                /*LDRImage * ldrI = ldrImageList.at(ldrImageList.size()-1);
                ldrI->showImage();*/
            }

            if((img_id-1)%3==1){
                if(index == 1){
                    cout << "Second " << img_id << std::endl;
                    //cv::imshow( "Second", orig_frame );
                    index++;
                    double exposureValue = mp_configuration->getExposureValue(exposureIDHistory[img_id]);
                    ldrImage = new LDRImage(orig_frame,exposureValue*0.000001);
                    //ldrImage->showImage();
                    ldrImageList.push_back(ldrImage);
                    /*LDRImage * ldrI = ldrImageList.at(ldrImageList.size()-1);
                    ldrI->showImage();*/
                }
            }
            if((img_id-1)%3==2){
                if(index == 2){
                    cout << "Second " << img_id << std::endl;
                    //cv::imshow( "Third", orig_frame );
                    index++;
                    double exposureValue = mp_configuration->getExposureValue(exposureIDHistory[img_id]);
                    ldrImage = new LDRImage(orig_frame,exposureValue*0.000001);
                    //ldrImage->showImage();
                    ldrImageList.push_back(ldrImage);
                    std::cout << "Size: " << ldrImageList.size() << std::endl;
                    /*LDRImage * ldrI = ldrImageList.at(ldrImageList.size()-1);
                    ldrI->showImage();*/
                    /*for(unsigned i = 0; i < ldrImageList.size();i++){
                        LDRImage * ldrI = ldrImageList.at(i);
                        ldrI->showImage();
                        std::cout << ldrImage->getExposureTime() << std::endl;
                    }*/
                    break;
                }


            }
            /*if(index == 3){
                for(int i = 0; i < ldrImageList.size();i++){
                    LDRImage * img = ldrImageList.at(i);
                    img->showImage();
                }
                break;
            }*/
        }






        // Uklizení aktuálního snímku
        //delete p_frame;

        if ( (255 & cv::waitKey( WIEW_WAIT_KEY_TIME )) == 27 ) // Esc
            break;


        // Aktivní multi-expoziční režim - Posílání požadavku na sekvenci
        // multi-expozičních snímků. Posílá se v předstihu jedne celé sekvence.
        if ( mp_configuration->isMultiExposureMode() )
        {
            if ( (img_id-10) % sequence_length == 0 )
            {
                for ( int i = 0; i < sequence_length; i++ ){
                    p_sequence_image_id[i] += sequence_length;
                    exposureIDHistory[p_sequence_image_id[i]] = p_sequence_config[i];
                }
                mp_camera->requestSequnce( sequence_length,
                                           p_sequence_image_id,
                                           p_sequence_config );
            }
        }

        // Poslání HeyYouPacketu pro udržení kamery v činném režimu
        if ( img_id % 100 == 0 )
            mp_camera->sendHeyYouPacket();
    }


    // Aktivní multi-expoziční režim - Uklizení paměti
    if ( mp_configuration->isMultiExposureMode() )
    {
        delete p_sequence_config;
        delete p_sequence_image_id;
    }

    mp_camera->stopGrabbing();

    if ( ! mp_camera->stopCameraGrabber() )
        throw exception();

    return ldrImageList;
}

bool CameraController::connectToCamera( void )
{
    if ( mp_camera->initConnection( mp_configuration->cameraIP()->c_str(),
                                    (short) mp_configuration->port(),
                                    mp_configuration->localIP()->c_str() ) )
    {
        std::cout << "Init connestion has been successful" << std::endl;
    }
    else
    {
        std::cerr << "Failed to init connection" << std::endl;
        return false;
    }


    if ( mp_camera->bindToCamera() )
    {
        std::cout << "Bind to camera has been successful" << std::endl;
    }
    else
    {
        std::cerr << "Failed to bind to camera" << std::endl;
        return false;
    }

    return true;
}


bool CameraController::loadConfiguration( const char * p_configuration_file )
{
    bool result;
    result = mp_configuration->load( p_configuration_file );

    return result;
}


bool CameraController::configureCamera( void )
{
    mp_camera->sendHeyYouPacket( true );

    mp_camera->setCameraActiveWindow( mp_configuration->cameraActiveWindow()->at(0),
                                      mp_configuration->cameraActiveWindow()->at(1),
                                      mp_configuration->cameraActiveWindow()->at(2),
                                      mp_configuration->cameraActiveWindow()->at(3) );
    mp_camera->setCameraGain( mp_configuration->cameraGain() );  // 145
    mp_camera->setCameraOffset( mp_configuration->cameraOffset() );
    //mp_camera->setCameraFocus( 29272 );  // ???
    mp_camera->setCameraExposure( mp_configuration->cameraExposures()[0] );  // mikro sekundy
    mp_camera->setCameraAperture( mp_configuration->cameraAperture() ); // 6500
    mp_camera->setIrisPSDParams( mp_configuration->irisPSDParams()->at(0),
                                 mp_configuration->irisPSDParams()->at(1),
                                 mp_configuration->irisPSDParams()->at(2),
                                 mp_configuration->irisPSDParams()->at(3),
                                 mp_configuration->irisPSDParams()->at(4) );
    mp_camera->setIrisAutoExpoLimit( mp_configuration->irisAutoExpoLimit() );
    mp_camera->setIrisPSDAutomode( mp_configuration->isPSDAutoMode() );
    mp_camera->setCameraHeating( mp_configuration->isCameraHeating() );
    mp_camera->setCameraAntiFog( mp_configuration->isCameraAntiFog() );
    mp_camera->setCameraFlashTime( mp_configuration->cameraFlashTime() );

    // exposure_type = 0-> exposure_value absolutní hodnota v mikro sekundách
    // exposure_type = 1 -> exposure_value relativní hodnota v % vůči nastavení výše
    CameraImageConfig img_config;
    img_config.config_index = 0;
    img_config.exposure_type = 1; // neměnit, shoduje se s nastavením výše
    img_config.exposure_value = 100; // neměnit, shoduje se s nastavením výše
    img_config.resolution_type = mp_configuration->isHalfFrame() ? 0 : 1; // 0 - half frame
    mp_camera->setCameraImgConfig( img_config );

    img_config.config_index = 1;
    img_config.exposure_type = 0;
    img_config.exposure_value = mp_configuration->getExposureValue(1);
    mp_camera->setCameraImgConfig( img_config );

    img_config.config_index = 2;
    img_config.exposure_type = 0;
    img_config.exposure_value = mp_configuration->getExposureValue(2);
    mp_camera->setCameraImgConfig( img_config );

    img_config.config_index = 3;
    img_config.exposure_type = 0;
    img_config.exposure_value = mp_configuration->getExposureValue(3);
    mp_camera->setCameraImgConfig( img_config );

    return true;
}

