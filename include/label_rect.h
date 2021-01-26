#ifndef LABEL_RECT_H
#define LABEL_RECT_H

#include "anchored_rect.h"
#include "resources/font.h"

enum LABEL_ALIGN {
    HORIZONTAL_ALIGN_LEFT   = 0b000001 ,
    HORIZONTAL_ALIGN_CENTER = 0b000010 ,
    HORIZONTAL_ALIGN_RIGHT  = 0b000100 ,
    VERTICAL_ALIGN_TOP      = 0b001000 ,
    VERTICAL_ALIGN_CENTER   = 0b010000 ,
    VERTICAL_ALIGN_BOTTOM   = 0b100000 ,
};

class LabelRect : public AnchoredRect {
    
    REGISTER_AS_INHERITED_COMPONENT(LabelRect,AnchoredRect);

    private:
        std::string         text;
        FontResource*       font;
        int                 font_size;
        int                 line_gap;
        unsigned char       align_flags;
        bool                editable;
        bool                enter_is_newline;

    public:
        LabelRect();
        ~LabelRect();

        std::vector<RenderData>  generate_render_data() override;
        void                update_vertex_data();

        std::string         get_text() const;
        void                set_text( std::string s );
        FontResource*       get_font() const;
        void                set_font( FontResource* f );
        int                 get_font_size() const;
        void                set_font_size(int new_val);
        int                 get_line_gap() const;
        void                set_line_gap(int new_val);
        void                set_align_flags( int new_val );
        bool                get_editable() const;
        void                set_editable( bool new_val );
        bool                get_enter_is_newline() const;
        void                set_enter_is_newline( bool new_val );

    protected:
        void        cb_key( Input::InputEventKey& ev ) override ;
        void        cb_mouse_button( Input::InputEventMouseButton& ev ) override ;
        void        cb_char( Input::InputEventChar& ev ) override ;

    public:
        static void         bind_methods();
        void                push_editor_items();
        YamlNode            to_yaml_node() const override ;
        void                from_yaml_node( YamlNode ) override ;




};

#endif