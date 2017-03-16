//
// ImageSurface.cc
//
// Copyright (c) 2010 LearnBoost <tj@learnboost.com>
//

#include "ImageSurface.h"

#include <assert>
#include <stdlib>
#include <string>
#include <node_buffer.h>
#include <node_version.h>

using namespace v8;
using namespace node;

Nan::Persistent<FunctionTemplate> ImageSurface::constructor;

namespace {
  int cairo_image_surface_n_bytes(cairo_surface_t* surface) {
    int stride = cairo_image_surface_get_stride(surface);
    int height = cairo_image_surface_get_height(surface);
    return stride * height;
  }
}

#define THROW_IF_ERROR(c) \
  do { \
    cairo_status_t status = cairo_surface_status(c); \
    if (status != CAIRO_STATUS_SUCCESS) { \
      return Nan::ThrowError(cairo_status_to_string(status)); \
    } \
  } while (false)

void
ImageSurface::Initialize(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
  Nan::HandleScope scope;

  // Constructor
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(ImageSurface::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("ImageSurface").ToLocalChecked());

  // Prototype
  Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
  Nan::SetPrototypeMethod(ctor, "flush", flush);
  Nan::SetPrototypeMethod(ctor, "mark_dirty", mark_dirty);
  Nan::SetPrototypeMethod(ctor, "mark_dirty_rectangle", mark_dirty_rectangle);
  Nan::SetPrototypeMethod(ctor, "get_stride", get_stride);
  Nan::SetPrototypeMethod(ctor, "get_width", get_width);
  Nan::SetPrototypeMethod(ctor, "get_height", get_height);
  Nan::SetPrototypeMethod(ctor, "get_format", get_format);
  Nan::SetPrototypeMethod(ctor, "get_data", get_data);

  Nan::Set(target, Nan::New("Surface").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(ImageSurface::New) {
  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Class constructors cannot be invoked without 'new'");
  }

  v8::Maybe<int32_t> format_int = Nan::To<int32_t>(info[0]);
  v8::Maybe<int32_t> width = Nan::To<int32_t>(info[1]);
  v8::Maybe<int32_t> height = Nan::To<int32_t>(info[2]);

  if (format_int.IsNothing() || width.IsNothing() || height.IsNothing()) {
    return Nan::ThrowTypeError("Must pass `format`, `width` and `height`: three integers");
  }

  // Convert to cairo_format_t
  cairo_format_t format;
  switch (format_int.FromJust()) {
    case CAIRO_FORMAT_ARGB32:
    case CAIRO_FORMAT_RGB24:
    case CAIRO_FORMAT_A8:
    case CAIRO_FORMAT_A1:
    case CAIRO_FORMAT_RGB16_565:
    case CAIRO_FORMAT_RGB30:
      format = static_cast<cairo_format_t>(format_int.FromJust());
      break;
    default:
      return Nan::ThrowRangeError("Must pass a valid 'format' integer");
  }

  ImageSurface *surface = new ImageSurface(format, width.FromJust(), height.FromJust());
  surface->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

#define GETTER(method, cType, jsType) \
  NAN_METHOD(ImageSurface::##method) { \
    ImageSurface* surface = Nan::ObjectWrap::Unwrap<ImageSurface>(info.This()); \
    ##cType value = cairo_image_surface_##method(surface->c); \
    info.GetReturnValue().Set(Nan::New<jsType>(value)); \
  }

GETTER(get_width, int, Number)
GETTER(get_height, int, Number)
GETTER(get_stride, int, Number)
GETTER(get_format, int, Number)

NAN_METHOD(ImageSurface::get_data) {
  ImageSurface* surface = Nan::ObjectWrap::Unwrap<ImageSurface>(info.This());
  info.GetReturnValue().Set(arrayBuffer);
}

NAN_METHOD(ImageSurface::flush) {
  ImageSurface* surface = Nan::ObjectWrap::Unwrap<ImageSurface>(info.This());
  cairo_surface_flush(surface->c);
  THROW_IF_ERROR(surface->c);
}

NAN_METHOD(ImageSurface::mark_dirty) {
  ImageSurface* surface = Nan::ObjectWrap::Unwrap<ImageSurface>(info.This());
  cairo_surface_mark_dirty(surface->c);
  THROW_IF_ERROR(surface->c);
}

NAN_METHOD(ImageSurface::mark_dirty_rectangle) {
  ImageSurface* surface = Nan::ObjectWrap::Unwrap<ImageSurface>(info.This());

  v8::Maybe<int32_t> x = Nan::To<int32_t>(info[0]);
  v8::Maybe<int32_t> y = Nan::To<int32_t>(info[1]);
  v8::Maybe<int32_t> width = Nan::To<int32_t>(info[2]);
  v8::Maybe<int32_t> height = Nan::To<int32_t>(info[3]);

  if (x.IsNothing() || y.IsNothing() || width.IsNothing() || height.IsNothing()) {
    return Nan::ThrowTypeError("Must pass `x`, `y`, `width` and `height`: four integers");
  }

  cairo_surface_mark_dirty_rectangle(
    surface->c,
    x.FromJust(),
    y.FromJust(),
    width.FromJust(),
    height.FromJust()
  );
  THROW_IF_ERROR(surface->c);
}

ImageSurface::ImageSurface(cairo_format_t format, int width, int height)
: Nan::ObjectWrap() {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  Nan::EscapableHandleScope scope;

  c = cairo_image_surface_create(format, width, height);

  // Check allocation succeeded. (c is always be a valid pointer, but sometimes
  // it points to an empty object.)
  THROW_IF_ERROR(c);

  int nBytes = cairo_image_surface_n_bytes(surface);
  char* data = cairo_image_surface_get_data(surface);
  Local<ArrayBuffer> ab = ArrayBuffer::New(data, nBytes);
  arrayBuffer->Reset(isolate, scope.Escape(ab));

  Nan::AdjustExternalMemory(nBytes);
}

ImageSurface::~ImageSurface() {
  int nBytes = cairo_image_surface_n_bytes(c);
  cairo_surface_destroy(c);
  arrayBuffer->Neuter();
  Nan::AdjustExternalMemory(-nBytes);
}
