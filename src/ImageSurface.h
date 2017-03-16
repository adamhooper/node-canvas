#ifndef __NODE_IMAGE_SURFACE_H__
#define __NODE_IMAGE_SURFACE_H__

#include <v8.h>
#include <node_version.h>
#include <nan.h>

#include <cairo.h>

class ImageSurface: public Nan::ObjectWrap {
  public:
    /**
     * The cairo_surface_t* we're wrapping.
     *
     * Let's call this "package-private".
     */
    cairo_surface_t* c;

    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static void Initialize(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);
    static NAN_METHOD(New);
    static NAN_METHOD(flush);
    static NAN_METHOD(mark_dirty);
    static NAN_METHOD(mark_dirty_rectangle);
    static NAN_METHOD(get_stride);
    static NAN_METHOD(get_width);
    static NAN_METHOD(get_height);
    static NAN_METHOD(get_format);
    static NAN_METHOD(get_data);

    ImageSurface(cairo_format_t format, int width, int height);

  private:
    /**
     * A JS-friendly view to the cairo surface's data.
     *
     * This makes get_data() simple. More importantly, it lets us track the
     * return value of get_data(), since every call returns the same value. In
     * our dtor we Neuter() the ArrayBuffer: callers won't access memory after
     * cairo free()s it.
     */
    Nan::Persistent<v8::ArrayBuffer> arrayBuffer;

    ~ImageSurface();
};

#endif
