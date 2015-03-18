#include "load_configuration.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

LoadConfiguration::LoadConfiguration()
{
    m_camera_action = CameraController::CameraAction::SHOW;

    m_local_ip = "192.168.6.2";
    m_camera_ip = "192.168.6.1";
    m_port = 10101;

    m_is_multi_exposure_mode = false;

    m_camera_gain = 20;
    m_camera_offset = 24;
    m_camera_aperture = 500;
    m_camera_exposures[0] = 8000;
    m_camera_exposures[1] = 16000;
    m_camera_exposures[2] = 32000;
    m_camera_exposures[3] = 64000;

    m_exposure_sequence = { 0, 1, 2, 3 };

    m_is_half_frame = false;

    m_camera_active_window = { 286, 1566, 9, 1024 };

    m_iris_psd_params = { 20, 10, 0, 134, 200 };

    m_iris_auto_expo_limit = 20000;
    m_is_psd_auto_mode = false;
    m_is_camera_heating = false;
    m_is_camera_anti_fog = false;
    m_camera_flash_time = 0;
}

bool LoadConfiguration::load( const char * p_filename )
{
    std::string line, key, value;

    std::ifstream config_file( p_filename );

    if ( ! config_file.is_open() )
    {
        std::cerr << "Unable to open configuration file: \"" << p_filename
                  << "\n" << std::endl;

        return false;
    }

    // Procházení jednotlivých řádků konfiguračního souboru.
    // Řádky začínající znakem # jsou komentáře a zahazují se.
    while ( std::getline( config_file, line ) )
    {
        if ( line.empty() )
            continue;

        if ( line[0] == '#' )
            continue;

        std::istringstream is_line( line );
        std::getline( is_line, key, '=' );

        // Odstranění bílých znaků
        key.erase( std::remove_if( key.begin(), key.end(), ::isspace ), key.end() );

        // Vymazat předchozí hodnotu aby bylo možné kontrolovat novou hodnotu
        value.clear();


        if ( key.compare( "ACTION" ) == 0 )
        {
            is_line >> value;
            if ( convertAction( value ) != CameraController::CameraAction::UNDEFINED )
                m_camera_action = convertAction( value );
            else
                std::cerr << "Warning:: Wrong value for ACTION in the "
                             "configuration file. Using default value." << std::endl;
        }
        else if ( key.compare( "VIEW_RATION" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value, true ) )
                m_view_ratio = stringToFloat( value );
            else
                std::cerr << "Warning: Wrong value for VIEW_RATIO in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "LOCAL_IP" ) == 0 )
        {
            is_line >> m_local_ip;
        }
        else if ( key.compare( "CAMERA_IP" ) == 0 )
        {
            is_line >> m_camera_ip;
        }
        else if ( key.compare( "PORT" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_port = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for PORT in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "MULTI_EXPOSURE_MODE" ) == 0 )
        {
            is_line >> value;
            if ( isBool( value ) )
                m_is_multi_exposure_mode = stringToBool( value );
            else
                std::cerr << "Warning: Wrong value for MULTI_EXPOSURE_MODE in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "NUMBER_OF_CAPTURED_FRAMES" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_number_captured_frames = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for NUMBER_OF_CAPTURED_FRAMES in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_GAIN" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_gain = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_GAIN in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_OFFSET" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_offset = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_OFFSET in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_MULTI_EXPOSURE_0" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_exposures[0] = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_MULTI_EXPOSURE_0 in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_MULTI_EXPOSURE_1" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_exposures[1] = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_MULTI_EXPOSURE_1 in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_MULTI_EXPOSURE_2" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_exposures[2] = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_MULTI_EXPOSURE_2 in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_MULTI_EXPOSURE_3" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_exposures[3] = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_MULTI_EXPOSURE_3 in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_APERTURE" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_aperture = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_APERTURE in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "MULTI_EXPOSURE_SEQUENCE" ) == 0 )
        {
            std::vector<unsigned short> values;

            while( is_line >> value )
            {
                if ( isNumber( value ) )
                    values.push_back( (unsigned short) stringToInt( value ) );
                else
                {
                    // Pokud je chyba, tak se načtené hodnoty vymažou
                    values.clear();
                    break;
                }
            }

            // Pokud se hodnoty načetly správně, tak se přepíší výchozí hodnoty
            if ( ! values.empty() )
                m_exposure_sequence  = values;
            else
                std::cerr << "Warning: Wrong value for MULTI_EXPOSURE_SEQUENCE "
                             "in the configuration file." << std::endl;
        }
        else if ( key.compare( "HALF_FRAME" ) == 0 )
        {
            is_line >> value;
            if ( isBool( value ) )
                m_is_half_frame = stringToBool( value );
            else
                std::cerr << "Warning: Wrong value for HALF_FRAME in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_ACTIVE_WINDOW" ) == 0 )
        {
            std::vector<int> values;

            while( is_line >> value )
            {
                if ( isNumber( value ) )
                    values.push_back( stringToInt( value ) );
                else
                {
                    // Pokud je chyba, tak se načtené hodnoty vymažou
                    values.clear();
                    break;
                }
            }

            // Pokud se hodnoty načetly správně, tak se přepíší výchozí hodnoty
            if ( values.size() == 4 )
                m_camera_active_window  = values;
            else
                std::cerr << "Warning: Wrong value for CAMERA_ACTIVE_WINDOW "
                             "in the configuration file." << std::endl;
        }
        else if ( key.compare( "IRIS_PSD_PARAMS" ) == 0 )
        {
            std::vector<int> values;

            while( is_line >> value )
            {
                if ( isNumber( value ) )
                    values.push_back( stringToInt( value ) );
                else
                {
                    // Pokud je chyba, tak se načtené hodnoty vymažou
                    values.clear();
                    break;
                }
            }

            // Pokud se hodnoty načetly správně, tak se přepíší výchozí hodnoty
            if ( values.size() == 5 )
                m_iris_psd_params  = values;
            else
                std::cerr << "Warning: Wrong value for IRIS_PSD_PARAMS "
                             "in the configuration file." << std::endl;

        }
        else if ( key.compare( "IRIS_AUTO_EXPO_LIMIT" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_iris_auto_expo_limit = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for IRIS_AUTO_EXPO_LIMIT in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "IRIS_PSD_AUTO_MODE" ) == 0 )
        {
            is_line >> value;
            if ( isBool( value ) )
                m_is_psd_auto_mode = stringToBool( value );
            else
                std::cerr << "Warning: Wrong value for IRIS_PSD_AUTO_MODE in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_HEATING" ) == 0 )
        {
            is_line >> value;
            if ( isBool( value ) )
                m_is_camera_heating = stringToBool( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_HEATING in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_ANTI_FOG" ) == 0 )
        {
            is_line >> value;
            if ( isBool( value ) )
                m_is_camera_anti_fog = stringToBool( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_ANTI_FOG in the "
                             "configuration file." << std::endl;
        }
        else if ( key.compare( "CAMERA_FLASH_TIME" ) == 0 )
        {
            is_line >> value;
            if ( isNumber( value ) )
                m_camera_flash_time = stringToInt( value );
            else
                std::cerr << "Warning: Wrong value for CAMERA_FLASH_TIME in the "
                             "configuration file." << std::endl;
        }
        else
        {
            std::cout << "Undefined parameter in the configuration file: \""
                      << key << "\"" << std::endl;
        }

    }

    config_file.close();

    return true;
}

CameraController::CameraAction LoadConfiguration::convertAction( std::string & str )
{
    if ( str.compare( "SHOW" ) == 0 )
        return CameraController::CameraAction::SHOW;
    else if ( str.compare( "RECORD" ) == 0 )
        return CameraController::CameraAction::RECORD;
    else if ( str.compare( "RECORD_AND_SHOW" ) == 0 )
        return CameraController::CameraAction::RECORD_AND_SHOW;
    else
        return CameraController::CameraAction::UNDEFINED;
}


bool LoadConfiguration::isNumber( std::string & str, bool is_decimal )
{
    if ( ! is_decimal )
    {
        std::string::const_iterator it = str.begin();
        while( it != str.end() && std::isdigit( *it ) )
            it++;

        return ! str.empty() && it == str.end();
    }
    else
    {
        std::string tmp;

        tmp = str;

        std::size_t found = tmp.find( "." );
        if ( found != std::string::npos )
            tmp.erase( found, sizeof( char ) );

        std::string::const_iterator it = tmp.begin();
        while( it != tmp.end() && std::isdigit( *it ) )
            it++;

        return ! tmp.empty() && it == tmp.end();
    }
}

bool LoadConfiguration::isBool( std::string & str )
{
    if ( str.compare( "true" ) == 0 )
        return true;
    else if ( str.compare( "false" ) == 0 )
        return true;
    else if ( str.compare( "True" ) == 0 )
        return true;
    else if ( str.compare( "False" ) == 0 )
        return true;
    else
        return false;
}


int LoadConfiguration::stringToInt( std::string & str )
{
    int result;

    std::istringstream buffer( str );
    buffer >> result;

    return result;
}

float LoadConfiguration::stringToFloat( std::string & str )
{
    float result;

    std::istringstream buffer( str );
    buffer >> result;

    return result;
}


int LoadConfiguration::stringToBool( std::string & str )
{
    if ( str.compare( "true" ) == 0 )
        return true;
    else if ( str.compare( "false" ) == 0 )
        return false;
    else if ( str.compare( "True" ) == 0 )
        return true;
    else if ( str.compare( "False" ) == 0 )
        return false;
    else
    {
        std::cout << "Warning: undefined bool value: \""
                  << str << "\"" << std::endl;
        return false;
    }
}

int LoadConfiguration::getExposureValue(int index){
    return m_camera_exposures[index];
}
