#ifndef __NODE_CONTEXT_H__
#define __NODE_CONTEXT_H__

#include <v8.h>
#include <node_version.h>
#include <nan.h>

class ImageSurface;

#include <ft2build.h>
#include <cairo-ft.h>
#include FT_FREETYPE_H

class Context: public Nan::ObjectWrap {
  public:
    Context(ImageSurface* imageSurface);
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static void Initialize(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);

    static NAN_METHOD(save);
    static NAN_METHOD(restore);

    // groups: not yet implemented
    //static NAN_METHOD(push_group);
    //static NAN_METHOD(push_group_with_content);
    //static NAN_METHOD(pop_group);
    //static NAN_METHOD(pop_group_to_source);
    //static NAN_METHOD(get_group_target);

    static NAN_METHOD(set_source_rgb);
    static NAN_METHOD(set_source_rgba);
    static NAN_METHOD(set_source);
    static NAN_METHOD(set_source_surface);
    static NAN_METHOD(get_source); // TK cairo_pattern_reference()
    static NAN_METHOD(set_antialias);
    static NAN_METHOD(get_antialias);
    static NAN_METHOD(set_dash);
    static NAN_METHOD(get_dash_count);
    static NAN_METHOD(get_dash);
    static NAN_METHOD(set_fill_rule);
    static NAN_METHOD(get_fill_rule);
    static NAN_METHOD(set_line_cap);
    static NAN_METHOD(get_line_cap);
    static NAN_METHOD(set_line_join);
    static NAN_METHOD(get_line_join);
    static NAN_METHOD(set_line_width);
    static NAN_METHOD(get_line_width);
    static NAN_METHOD(set_miter_limit);
    static NAN_METHOD(get_miter_limit);
    static NAN_METHOD(set_operator);
    static NAN_METHOD(get_operator);
    static NAN_METHOD(set_tolerance);
    static NAN_METHOD(get_tolerance);
    static NAN_METHOD(clip);
    static NAN_METHOD(clip_preserve);
    static NAN_METHOD(clip_extents);
    static NAN_METHOD(in_clip);
    static NAN_METHOD(reset_clip);

    // rectangle_lists: not yet implemented
    static NAN_METHOD(rectangle_list_destroy);
    static NAN_METHOD(copy_clip_rectangle_list);

    static NAN_METHOD(fill);
    static NAN_METHOD(fill_preserve);
    static NAN_METHOD(fill_extents);
    static NAN_METHOD(in_fill);
    static NAN_METHOD(mask);
    static NAN_METHOD(mask_surface);
    static NAN_METHOD(paint);
    static NAN_METHOD(paint_with_alpha);
    static NAN_METHOD(stroke);
    static NAN_METHOD(stroke_preserve);
    static NAN_METHOD(stroke_extents);
    static NAN_METHOD(in_stroke);
    static NAN_METHOD(copy_page);

    // pages: not implemented (because we only have ImageSurface)
    //static NAN_METHOD(copy_page);
    //static NAN_METHOD(show_page);

    // Paths

    // cairo_path_t: not yet implemented
    //static NAN_METHOD(copy_path);
    //static NAN_METHOD(copy_path_flat);
    //static NAN_METHOD(append_path);

    static NAN_METHOD(has_current_point);
    static NAN_METHOD(get_current_point);
    static NAN_METHOD(new_path);
    static NAN_METHOD(new_sub_path);
    static NAN_METHOD(close_path);
    static NAN_METHOD(arc);
    static NAN_METHOD(arc_negative);
    static NAN_METHOD(curve_to);
    static NAN_METHOD(line_to);
    static NAN_METHOD(move_to);
    static NAN_METHOD(rectangle);

    // glyphs: not yet implemented
    //static NAN_METHOD(glyph_path);
    //static NAN_METHOD(text_path); is BAD: don't implement it.

    static NAN_METHOD(rel_curve_to);
    static NAN_METHOD(rel_line_to);
    static NAN_METHOD(rel_move_to);
    static NAN_METHOD(path_extents);

    // Transformations
    static NAN_METHOD(translate);
    static NAN_METHOD(scale);
    static NAN_METHOD(rotate);
    static NAN_METHOD(transform);
    static NAN_METHOD(set_matrix);
    static NAN_METHOD(get_matrix);
    static NAN_METHOD(identity_matrix);
    static NAN_METHOD(user_to_device);
    static NAN_METHOD(user_to_device_distance);
    static NAN_METHOD(device_to_user);
    static NAN_METHOD(device_to_user_distance);

    // Text
    //static NAN_METHOD(select_font_face); is TOY API: do not implement
    static NAN_METHOD(set_font_size);
    static NAN_METHOD(set_font_matrix);
    static NAN_METHOD(get_font_matrix);
    static NAN_METHOD(set_font_options);
    static NAN_METHOD(get_font_options);
    static NAN_METHOD(set_font_face);
    static NAN_METHOD(get_font_face);
    static NAN_METHOD(set_scaled_font);
    static NAN_METHOD(get_scaled_font);
    //static NAN_METHOD(show_text); is TOY API: do not implement
    static NAN_METHOD(show_glyphs);
    //static NAN_METHOD(show_text_glyphs); is just show_glyphs() for surfaces _other_ than cairo_image_surface_t
    static NAN_METHOD(font_extents);
    //static NAN_METHOD(text_extents); is TOY API: do not implement
    static NAN_METHOD(glyph_extents);
    //static NAN_METHOD(toy_font_face_create); is TOY API: do not implement
    //static NAN_METHOD(toy_font_face_get_family); is TOY API: do not implement
    //static NAN_METHOD(toy_font_face_get_slant); is TOY API: do not implement
    //static NAN_METHOD(toy_font_face_get_weight); is TOY API: do not implement

  private:
    ~Context();

    /**
     * The cairo_t* we're wrapping.
     */
    cairo_t* c;
};

#endif
