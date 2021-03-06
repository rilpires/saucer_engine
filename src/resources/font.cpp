#include "resources/font.h"
#include "render_engine.h"
#include "engine.h"
#include "lua_engine.h"
#include <math.h>


FT_Library FontResource::ft;

#define FT_CALL(x)\
if(x) saucer_err("FreeType error: " , #x );

FontResource::FontResource( const std::vector<uint8_t>& mem_data ) {
    raw_data = mem_data;
    if(FT_New_Memory_Face( ft , &(raw_data[0]) , raw_data.size() , 0 , &face ))
        saucer_err("Couldn't load FontResource.");
    
    int font_pixel_width = 24;
    int font_pixel_height = 24;
    
    FT_CALL( FT_Set_Pixel_Sizes( face , font_pixel_width , font_pixel_height ) );

    size_t max_char_bitmap_width = 0;
    size_t max_char_bitmap_height = 0;
    int num_rows = ceil(pow((float)(face->num_glyphs) , 0.5));
    int num_cols = ceil(((float)face->num_glyphs)/num_rows);
    for( int i = 0 ; i < face->num_glyphs ; i++ ){
        FT_CALL( FT_Load_Glyph( face , i , 0 ) );
        auto& glyph = face->glyph;
        FT_CALL( FT_Render_Glyph( glyph , FT_RENDER_MODE_NORMAL ) );
        if( glyph->bitmap.width > max_char_bitmap_width )
            max_char_bitmap_width = glyph->bitmap.width;
        if( glyph->bitmap.rows > max_char_bitmap_height )
            max_char_bitmap_height = glyph->bitmap.rows;
    }

    max_pixels_height = max_char_bitmap_height;
    Vector2 max_char_bitmap_size( max_char_bitmap_width,max_char_bitmap_height);
    int data_size = num_rows*num_cols*max_char_bitmap_width*max_char_bitmap_height;
    unsigned char* data = new unsigned char[data_size];
    memset( data , 0 , data_size );

    for( int i = 0 ; i < face->num_glyphs ; i++ ){
        FT_CALL( FT_Load_Glyph( face , i , 0 ) );
        auto& glyph = face->glyph;
        FT_CALL( FT_Render_Glyph( glyph , FT_RENDER_MODE_NORMAL ) );
        int col = i % num_cols;
        int row = i / num_cols;
        for( size_t dx = 0 ; dx < glyph->bitmap.width ; dx++ )
        for( size_t dy = 0 ; dy < glyph->bitmap.rows  ; dy++ )
        {
            int pixel_col = col*max_char_bitmap_width + dx;
            int pixel_row = row*max_char_bitmap_height + dy;
            data[pixel_col + pixel_row*num_cols*max_char_bitmap_width] = glyph->bitmap.buffer[dx + dy*glyph->bitmap.width];
        }
        
        FontResource::CharData char_data;
        char_data.pixel_size      = Vector2(glyph->bitmap.width , glyph->bitmap.rows );
        char_data.top_left_uv     = ( Vector2(col,row) )/ Vector2( num_cols , num_rows );
        char_data.bottom_right_uv = ( Vector2(col,row) + char_data.pixel_size/max_char_bitmap_size ) / Vector2( num_cols , num_rows );
        char_data.bearing         = Vector2(glyph->metrics.horiBearingX , glyph->metrics.horiBearingY) / 64.0;
        char_data.pixels_advance = glyph->advance.x / 64.0;
        
        char_datas.insert( std::pair<uint64_t,FontResource::CharData>( i , char_data ) );
    }
    // Overriding some newline character's attributes
    uint64_t newline_glyph_index = FT_Get_Char_Index( face , '\n' );
    char_datas[newline_glyph_index].pixel_size.x = 0;
    char_datas[newline_glyph_index].pixels_advance = 0;

    GL_CALL( glActiveTexture( GL_TEXTURE15 ) );
    GL_CALL( glGenTextures(1,&tex_id) );
    GL_CALL( glBindTexture(GL_TEXTURE_2D,tex_id) );
    GL_CALL( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) );  

    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ) );
    
    GL_CALL( glTexImage2D(GL_TEXTURE_2D,0,GL_R8,num_cols*max_char_bitmap_width,num_rows*max_char_bitmap_height,0,GL_RED,GL_UNSIGNED_BYTE,data) );
    glActiveTexture( GL_TEXTURE0 );
    delete[] data;
}
FontResource::~FontResource() {
    FT_CALL( FT_Done_Face(face) );   
}
const FontResource::CharData  FontResource::get_char_data( uint64_t unicode ) const {
    uint64_t glyph_index = FT_Get_Char_Index( face , unicode );
    auto it = char_datas.find( glyph_index );
    if( it == char_datas.end() )
        saucer_err("Unicode " , unicode , " doesn't exists in this font (" , path , ")" );
    return it->second;
}
TextureId                   FontResource::get_texture_id() const{
    return tex_id;
}
int         FontResource::get_max_pixels_height() const{
    return max_pixels_height;
}
void    FontResource::initialize(){
    if( FT_Init_FreeType(&ft) ){
        saucer_err( "Couldn't initialize FreeType library" );
    }
}
void FontResource::bind_methods() {
    initialize();

}
