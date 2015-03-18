#ifndef MULTI_EXPOSURE_CAMERA_THRD_H
#define MULTI_EXPOSURE_CAMERA_THRD_H

#define FRAMES_BUFFER_LENGTH 20

#include <thread>
#include <atomic>
#include <mutex>

#include "multi_exposure_camera.h"
#include "frame.h"


class MultiExposureCameraThrd : public MultiExposureCamera
{
public:
    MultiExposureCameraThrd( void );
    virtual ~MultiExposureCameraThrd( void );

    void startGrabbing( void );
    void stopGrabbing( void );

    virtual Frame * releaseFrame( void );
    virtual bool grabFrame( bool must_complete = false );

    bool isRunning( void ) const;


private:
    void runGrabbing( void );
    void pushFrameToBuffer( Frame * p_frame );
    Frame * pullFrameFromBuffer( void );


    Frame * m_frames_buffer[FRAMES_BUFFER_LENGTH];
    int m_head_frames_buffer;
    int m_tail_frames_buffer;

    std::thread m_thread;
    std::atomic<bool> m_is_running;
    std::atomic<bool> m_stop_cmd;
    std::atomic<bool> m_quit_cmd;
    std::mutex m_buffer_mutex;
};

#endif // MULTI_EXPOSURE_CAMERA_THRD_HPP
