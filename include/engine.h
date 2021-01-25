#ifndef WINDOW_H

#define WINDOW_H



#include "debug.h"
#include <list>

#include "saucer_object.h"
#include "vector.h"

class Scene;
class AudioEngine;
class RenderEngine;

class Engine : public SaucerObject{
    REGISTER_SAUCER_OBJECT(Engine,SaucerObject);

    friend class Scene;    
    private:
        static RenderEngine*        render_engine;
        static AudioEngine*         audio_engine;
        static Scene*               current_scene;
        static std::list<double>    last_uptimes;
        static double               next_frame_time;
        static YamlNode             config;
    public:
        
        /**
         * @brief This should be the first function ever to be called in a Saucer Engine application
         */
        static void             initialize( YamlNode config );
        
        /**
         * @brief Safely close every resource managed by Saucer Engine
         */
        static void             close();
        
        /**
         * @brief Proceeds with a full frame: input handling, script processing, physics processing and rendering
         */
        static void             update();
        
        /**
         * @brief Get the uptime (time since initialization of the Engine) in seconds.
         * 
         * @return double 
         */
        static double           get_uptime();
        
        /**
         * @brief Get the current calculated FPS (frames per second), accounting the last FPS_FRAMES_TO_ACCOUNT frames
         * 
         * @return double 
         */
        static double           get_fps();

        /**
         * @brief Get the last frame duration in seconds
         * 
         * @return double 
         */
        static double           get_last_frame_duration();
        
        /**
         * @brief Set the current scene object to be updated every frame. Don't set this as nullptr or else you
         * may be stuck forever?
         * 
         * @param scene 
         */
        static void             set_current_scene(Scene* scene);
        
        /**
         * @brief Get the current scene object
         * 
         * @return Scene* 
         */
        static Scene*           get_current_scene();
        
        /**
         * @brief Set the window size. Note that this is only a shortcut for RenderEngine::set_window_size()
         * 
         * @param new_size Vector2
         */
        static void             set_window_size( Vector2 new_size );
        
        /**
         * @brief Get the window size object. Note that this is only a shortcut for RenderEngine::get_window_size()
         * 
         * @return Vector2 
         */
        static Vector2          get_window_size();
        
        /**
         * @brief Get the window size object. Note that this is only a shortcut for RenderEngine::get_viewport_rect().get_size()
         * 
         * @return Vector2 
         */
        static Vector2          get_viewport_size();
        
        /**
         * @brief Set the window position if not fullscreen. (0,0) is the top-left corner of the monitor.
         * Note that this is only a shortcut for RenderEngine::set_window_pos()
         * 
         * @param new_pos 
         */
        static void             set_window_pos( Vector2 new_pos );
        
        /**
         * @brief Get the window position if not fullscreen.
         *  Note that this is only a shortcut for RenderEngine::get_window_pos()
         * 
         * @return Vector2 
         */
        static Vector2          get_window_pos();
        
        /**
         * @brief Set the application to be run on a fullscreen.  Note that this is only a shortcut for RenderEngine::set_fullscreen()
         * 
         * @param fs 
         */
        static void             set_fullscreen( bool fs );
        
        /**
         * @brief Returns true if application is fullscreen.
         * 
         * @return true 
         * @return false 
         */
        static bool             is_fullscreen();
        
        /**
         * @brief Get the title appearing at header of the window.
         * 
         * @return std::string 
         */
        static std::string      get_window_title();
        
        /**
         * @brief Set a new window title (header of the window)
         * 
         * @param new_title 
         */
        static void             set_window_title( std::string new_title );
        
        /**
         * @return RenderEngine* 
         */
        static RenderEngine*    get_render_engine();
        /**
         * @return AudioEngine* 
         */
        static AudioEngine*     get_audio_engine();
        
        /**
         * @brief When this returns true, it means that the application will close when current frame is done processing
         */
        static bool             should_close();

        static YamlNode&        get_config();

        static void         bind_methods();

        #ifdef SAUCER_EDITOR
        static bool is_editor();
        #else
        static constexpr bool is_editor(){ return false;};
        #endif

};

#endif