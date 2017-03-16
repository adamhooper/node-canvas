#ifndef __NODE_PATTERN_H__
#define __NODE_PATTERN_H__

#include <v8.h>
#include <nan.h>

typedef struct _cairo_pattern cairo_pattern_t;

class Pattern: public Nan::ObjectWrap {
  public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static void Initialize(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);

    static NAN_METHOD(New);
    static NAN_METHOD(create_rgb);
    static NAN_METHOD(create_rgba);
    static NAN_METHOD(create_for_surface);
    static NAN_METHOD(create_linear);
    static NAN_METHOD(create_radial);
    // mesh: not yet implemented
    //static NAN_METHOD(create_mesh);

    static NAN_METHOD(add_color_stop_rgb);
    static NAN_METHOD(add_color_stop_rgba);
    static NAN_METHOD(get_color_stop_count);
    static NAN_METHOD(get_color_stop_rgba);
    static NAN_METHOD(get_rgba);
    static NAN_METHOD(get_surface);
    static NAN_METHOD(get_linear_points);
    static NAN_METHOD(get_radial_circles);
    // mesh: not yet implemented
    //static NAN_METHOD(mesh_begin_patch);
    //static NAN_METHOD(mesh_end_patch);
    //static NAN_METHOD(mesh_move_to);
    //static NAN_METHOD(mesh_line_to);
    //static NAN_METHOD(mesh_curve_to);
    //static NAN_METHOD(mesh_set_control_point);
    //static NAN_METHOD(mesh_set_corner_color_rgb);
    //static NAN_METHOD(mesh_set_corner_color_rgba);
    //static NAN_METHOD(mesh_get_patch_count);
    // paths: not yet implemented
    //static NAN_METHOD(mesh_get_path);
    //static NAN_METHOD(mesh_get_control_point);
    //static NAN_METHOD(mesh_get_corner_color_rgba);
    static NAN_METHOD(set_extend);
    static NAN_METHOD(get_extend);
    static NAN_METHOD(set_filter);
    static NAN_METHOD(get_filter);
    // matrix: not yet implemented
    //static NAN_METHOD(set_matrix);
    //static NAN_METHOD(get_matrix);
    static NAN_METHOD(get_type);

  private:
    Pattern(cairo_pattern_t* pattern): c(pattern);
    ~Pattern();

    cairo_pattern_t* c;

    // ImageSurface's dtor shouldn't be called until all Patterns that refer to
    // it are gone. That means we need to hold a reference.
    Nan::Persistent<v8::Object> surface;
};

#endif
