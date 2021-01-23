#ifndef ANCHORED_RECT_H
#define ANCHORED_RECT_H

#include "render_object.h"

enum SAUCER_BORDER {
    LEFT_BORDER     ,
    RIGHT_BORDER    ,
    TOP_BORDER      ,
    BOTTOM_BORDER
};

class AnchoredRect : public RenderObject {
    
    REGISTER_AS_INHERITED_COMPONENT(AnchoredRect,RenderObject);
    
    private:
        unsigned char   anchored_borders[4];
        Vector2         rect_pos;
        Vector2         rect_size;
        bool            starts_on_viewport;
        bool            ignore_mouse;
        bool            use_scene_node_transform;
        
    public:
        AnchoredRect();
        ~AnchoredRect();

        std::vector<RenderData>  generate_render_data() override ;

        bool                is_border_anchored( int border , int parent_border ) const ;
        void                set_anchored_border( int border , int parent_border , bool new_val );
        Vector2             get_rect_pos() const ;
        void                set_rect_pos( Vector2 new_val );
        Vector2             get_rect_size() const ;
        void                set_rect_size( Vector2 new_val );
        bool                get_starts_on_viewport() const ;
        void                set_starts_on_viewport( bool new_val );
        bool                get_ignore_mouse() const;
        void                set_ignore_mouse(bool new_val);
        bool                get_use_scene_node_transform() const ;
        void                set_use_scene_node_transform( bool new_val );
        void                grow( int border , float amount );
        AnchoredRect*       get_parent_rect() const;
        Vector2             get_global_rect_pos() const;
        Transform           get_parent_global_transform() const;

        static void         bind_methods();
        YamlNode            to_yaml_node() const override ;
        void                from_yaml_node( YamlNode ) override ;

    protected:
        virtual void        cb_mouse_entered( );
        virtual void        cb_mouse_exiting( );
        virtual void        cb_mouse_button( Input::InputEventMouseButton& ev );
        virtual void        cb_key( Input::InputEventKey& ev );
        virtual void        cb_char( Input::InputEventChar& ev );

        const std::vector<AnchoredRect*>    get_children_rects() const;
        
};

#endif