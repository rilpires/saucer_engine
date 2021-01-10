#ifndef LABEL_RECT_H
#define LABEL_RECT_H

#include "anchored_rect.h"
#include "resources/font.h"

class LabelRect : public AnchoredRect {
    
    REGISTER_AS_INHERITED_COMPONENT(LabelRect,AnchoredRect);

    private:
        std::string         text;
        FontResource*       font;
        int                 font_size;
        int                 line_gap;

    public:
        LabelRect();

        std::vector<RenderData>  generate_render_data() const ;

        std::string         get_text() const;
        void                set_text( std::string s );
        FontResource*       get_font() const;
        void                set_font( FontResource* f );
        int                 get_line_gap() const;
        void                set_line_gap(int new_val);
        
        static void         bind_methods();

};

#endif