#include "Pattern.h"

#include <cairo.h>

Nan::Persistent<v8::FunctionTemplate> Pattern::constructor;

void
Pattern::Initialize(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
  Nan::HandleScope scope;

  // Constructor
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(Pattern::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("Pattern").ToLocalChecked());

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("Pattern").ToLocalChecked());

  // Prototype
  Nan::Set(target, Nan::New("Pattern").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(Pattern::New) {
  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Do not call `Pattern()`: use `Pattern.create_XXX()` methods");
  }

  if (info.Length() == 1 && info[0]->IsExternal()) {
    // There's a security problem if somebody calls `new Pattern(someEvilValue)`
    // but programmers will have a very hard time calling that by accident, and
    // if they're using eval() to call it on purpose they have bigger problems.
    v8::Local<v8::External> ext = info[0].As<v8::External>();
    cairo_pattern_t* cPattern = static_cast<cairo_pattern_t*>(ext->Value());
    Pattern* pattern = new Pattern(cPattern);

    pattern->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  }

  return Nan::ThrowTypeError("Do not use `new Pattern()`: use `Pattern.create_XXX()` methods");
}

#define THROW_IF_ERROR(pattern) \
  do { \
    if (unlikely(cairo_pattern_status(pattern) != CAIRO_STATUS_SUCCESS)) { \
      return Nan::ThrowError(cairo_status_to_string(cairo_pattern_status(pattern))); \
    } \
  } while (0)

#define RETURN_NEW_INSTANCE(pattern) \
  do { \
    v8::Local<v8::Value> argv[1] = { Nan::New<v8::External>(pattern); }; \
    info.GetReturnValue().Set(Nan::NewInstance(Nan::New(constructor()), 1, argv)); \
  } while (0)

NAN_METHOD(Pattern::create_rgb) {
  v8::Maybe<double> r = Nan::To<double>(info[0]);
  v8::Maybe<double> g = Nan::To<double>(info[1]);
  v8::Maybe<double> b = Nan::To<double>(info[2]);

  if (unlikely(r.IsNothing() || g.IsNothing() || b.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `r`, `g` and `b`: three Numbers");
  }

  cairo_pattern_t* pattern = cairo_pattern_create_rgb(r.Value(), g.Value(), b.Value());
  THROW_IF_ERROR(pattern);
  RETURN_NEW_INSTANCE(pattern);
}

NAN_METHOD(Pattern::create_rgba) {
  v8::Maybe<double> r = Nan::To<double>(info[0]);
  v8::Maybe<double> g = Nan::To<double>(info[1]);
  v8::Maybe<double> b = Nan::To<double>(info[2]);
  v8::Maybe<double> a = Nan::To<double>(info[3]);

  if (unlikely(r.IsNothing() || g.IsNothing() || b.IsNothing() || a.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `r`, `g`, `b` and `a`: four Numbers");
  }

  cairo_pattern_t* pattern = cairo_pattern_create_rgba(r.Value(), g.Value(), b.Value(), a.Value());
  THROW_IF_ERROR(pattern);
  RETURN_NEW_INSTANCE(pattern);
}

NAN_METHOD(Pattern::create_linear) {
  v8::Maybe<double> x0 = Nan::To<double>(info[0]);
  v8::Maybe<double> y0 = Nan::To<double>(info[1]);
  v8::Maybe<double> x1 = Nan::To<double>(info[2]);
  v8::Maybe<double> y1 = Nan::To<double>(info[3]);

  if (unlikely(x0.IsNothing() || y0.IsNothing() || x1.IsNothing() || y1.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `x0`, `y0`, `x1` and `y1`: four Numbers");
  }

  cairo_pattern_t* pattern = cairo_pattern_create_linear(x0.Value(), y0.Value(), x1.Value(), y1.Value());
  THROW_IF_ERROR(pattern);
  RETURN_NEW_INSTANCE(pattern);
}

NAN_METHOD(Pattern::create_radial) {
  v8::Maybe<double> cx0 = Nan::To<double>(info[0]);
  v8::Maybe<double> cy0 = Nan::To<double>(info[1]);
  v8::Maybe<double> r0 = Nan::To<double>(info[2]);
  v8::Maybe<double> cx1 = Nan::To<double>(info[3]);
  v8::Maybe<double> cy1 = Nan::To<double>(info[4]);
  v8::Maybe<double> r1 = Nan::To<double>(info[5]);

  if (unlikely(cx0.IsNothing() || cy0.IsNothing() || r0.IsNothing() || cx1.IsNothing() || cy1.IsNothing() || r1.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `cx0`, `cy0`, `r0`, `cx1`, `cy1` and `r1`: six Numbers");
  }

  cairo_pattern_t* pattern = cairo_pattern_create_linear(
    cx0.Value(), cy0.Value(), r0.Value(),
    cx1.Value(), cy1.Value(), r1.Value()
  );
  THROW_IF_ERROR(pattern);
  RETURN_NEW_INSTANCE(pattern);
}

NAN_METHOD(Pattern::create_for_surface) {
  Nan::MaybeLocal<v8::Object> maybe1 = Nan::To<v8::Object>(info[0]);
  if (unlikely(maybe1.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `surface`, an ImageSurface");
  }

  ImageSurface* surface = Nan::ObjectWrap::Unwrap<ImageSurface>(maybe1.ToLocalChecked());

  cairo_pattern_t* pattern = cairo_pattern_create_for_surface(surface->c);
  THROW_IF_ERROR(pattern);
  RETURN_NEW_INSTANCE(pattern);

  // cairo_pattern_t* holds a reference to the cairo_surface_t*, but that
  // isn't enough: we need a reference to our ImageSurface so v8 won't
  // garbage-collect it and invoke its dtor. (The dtor tells v8 about memory
  // usage.)
  Pattern* retPattern = Nan::ObjectWrap::Unwrap<Pattern>(info.GetReturnValue());
  retPattern->surface.Reset(maybe1.ToLocalChecked());
}

NAN_METHOD(add_color_stop_rgb) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());

  v8::Maybe<double> offset = Nan::To<double>(info[0]);
  v8::Maybe<double> r = Nan::To<double>(info[1]);
  v8::Maybe<double> g = Nan::To<double>(info[2]);
  v8::Maybe<double> b = Nan::To<double>(info[3]);

  if (unlikely(offset.IsNothing() || r.IsNothing() || g.IsNothing() || b.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `offset`, `r`, `g` and `b`: four Numbers");
  }

  cairo_pattern_add_color_stop_rgb(pattern->c, offset, r, g, b);
  THROW_IF_ERROR(pattern->c);
}

NAN_METHOD(add_color_stop_rgba) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());

  v8::Maybe<double> offset = Nan::To<double>(info[0]);
  v8::Maybe<double> r = Nan::To<double>(info[1]);
  v8::Maybe<double> g = Nan::To<double>(info[2]);
  v8::Maybe<double> b = Nan::To<double>(info[3]);
  v8::Maybe<double> a = Nan::To<double>(info[4]);

  if (unlikely(offset.IsNothing() || r.IsNothing() || g.IsNothing() || b.IsNothing() || a.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `offset`, `r`, `g`, `b` and `a`: five Numbers");
  }

  cairo_pattern_add_color_stop_rgba(pattern->c, offset, r, g, b, a);
  THROW_IF_ERROR(pattern->c);
}

NAN_METHOD(get_color_stop_count) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());

  int count;
  cairo_status_t status = cairo_pattern_get_color_stop_count(pattern->c, &count);
  if (status != CAIRO_STATUS_SUCCESS) {
    return Nan::ThrowError(cairo_status_to_string(status));
  }

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Int32> ret = Nan::New<v8::Int32>(static_cast<int32_t>(count));
  info.GetReturnValue().Set(scope.Escape(ret));
}

NAN_METHOD(get_color_stop_rgba) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());
  v8::Maybe<int32_t> index = Nan::To<int32_t>(info[0]);
  if (unlikely(index.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `index`, an integer");
  }

  double offset, r, g, b, a;
  cairo_status_t status = cairo_pattern_get_color_stop_rgba(pattern->c, index, &offset, &r, &g, &b, &a);
  if (status != CAIRO_STATUS_SUCCESS) {
    return Nan::ThrowError(cairo_status_to_string(status));
  }

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Object> ret = Nan::New<v8::Object>();
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("offset").ToLocalChecked(), Nan::New(offset));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("r").ToLocalChecked(), Nan::New(r));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("g").ToLocalChecked(), Nan::New(g));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("b").ToLocalChecked(), Nan::New(b));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("a").ToLocalChecked(), Nan::New(a));
  info.GetReturnValue().Set(scope.Escape(ret));
}

NAN_METHOD(get_rgba) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());

  double r, g, b, a;
  cairo_status_t status = cairo_pattern_get_rgba(pattern->c, &r, &g, &b, &a);
  if (status != CAIRO_STATUS_SUCCESS) {
    return Nan::ThrowError(cairo_status_to_string(status));
  }

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Object> ret = Nan::New<v8::Object>();
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("r").ToLocalChecked(), Nan::New(r));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("g").ToLocalChecked(), Nan::New(g));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("b").ToLocalChecked(), Nan::New(b));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("a").ToLocalChecked(), Nan::New(a));
  info.GetReturnValue().Set(scope.Escape(ret));
}

NAN_METHOD(get_surface) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());
  info.GetReturnValue().Set(pattern->surface);
}

NAN_METHOD(get_linear_points) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());

  double x0, y0, x1, y1;
  cairo_status_t status = cairo_pattern_get_rgba(pattern->c, &x0, &y0, &x1, &y1);
  if (status != CAIRO_STATUS_SUCCESS) {
    return Nan::ThrowError(cairo_status_to_string(status));
  }

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Object> ret = Nan::New<v8::Object>();
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("x0").ToLocalChecked(), Nan::New(x0));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("y0").ToLocalChecked(), Nan::New(y0));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("x1").ToLocalChecked(), Nan::New(x1));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("y1").ToLocalChecked(), Nan::New(y1));
  info.GetReturnValue().Set(scope.Escape(ret));
}

NAN_METHOD(get_radial_circles) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());

  double x0, y0, r0, x1, y1, r1;
  cairo_status_t status = cairo_pattern_get_rgba(pattern->c, &x0, &y0, &r0, &x1, &y1, &r1);
  if (status != CAIRO_STATUS_SUCCESS) {
    return Nan::ThrowError(cairo_status_to_string(status));
  }

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Object> ret = Nan::New<v8::Object>();
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("x0").ToLocalChecked(), Nan::New(x0));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("y0").ToLocalChecked(), Nan::New(y0));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("r0").ToLocalChecked(), Nan::New(r0));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("x1").ToLocalChecked(), Nan::New(x1));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("y1").ToLocalChecked(), Nan::New(y1));
  ret->CreateDataProperty(Nan::GetCurrentContext(), Nan::New("r1").ToLocalChecked(), Nan::New(r1));
  info.GetReturnValue().Set(scope.Escape(ret));
}

NAN_METHOD(set_extend) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());
  v8::Maybe<int32_t> extend = Nan::To<int32_t>(info[0]);
  if (unlikely(extend.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `extend`, an integer");
  }

  switch (extend) {
    case CAIRO_EXTEND_NONE:
    case CAIRO_EXTEND_REPEAT:
    case CAIRO_EXTEND_REFLECT:
    case CAIRO_EXTEND_PAD:
      cairo_pattern_set_extend(pattern->c, static_cast<cairo_extend_t>(extend));
    default:
      return Nan::ThrowValueError("Invalid `extend` valie");
  }
}

NAN_METHOD(get_extend) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());
  cairo_extend_t extend = cairo_pattern_get_extend(pattern->c);

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Int32> ret = Nan::New<v8::Int32>(static_cast<int32_t>(extend));
  info.GetReturnValue().Set(scope.Escape(ret));
}

NAN_METHOD(set_filter) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());
  v8::Maybe<int32_t> filter = Nan::To<int32_t>(info[0]);
  if (unlikely(filter.IsNothing())) {
    return Nan::ThrowTypeError("Must pass `filter`, an integer");
  }

  switch (filter) {
    case CAIRO_FILTER_FAST:
    case CAIRO_FILTER_GOOD:
    case CAIRO_FILTER_BEST:
    case CAIRO_FILTER_NEAREST:
    case CAIRO_FILTER_BILINEAR:
    case CAIRO_FILTER_GAUSSIAN:
      cairo_pattern_set_filter(pattern->c, static_cast<cairo_filter_t>(filter));
    default:
      return Nan::ThrowValueError("Invalid `filter` valie");
  }
}

NAN_METHOD(get_filter) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());
  cairo_filter_t filter = cairo_pattern_get_filter(pattern->c);

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Int32> ret = Nan::New<v8::Int32>(static_cast<int32_t>(filter));
  info.GetReturnValue().Set(scope.Escape(ret));
}

NAN_METHOD(get_type) {
  Pattern* pattern = Nan::ObjectWrap::Unwrap<Pattern>(info.This());
  cairo_pattern_type_t filter = cairo_pattern_get_type(pattern->c);

  Nan::EscapableHandleScope scope;
  v8::Local<v8::Int32> ret = Nan::New<v8::Int32>(static_cast<int32_t>(type));
  info.GetReturnValue().Set(scope.Escape(ret));
}

Pattern::Pattern(cairo_pattern_t* pattern)
  : Nan::ObjectWrap(), c(pattern)
{
  // This is only called internally: our own create() calls pass along the sole
  // reference to pattern
}

Pattern::~Pattern() {
  cairo_pattern_destroy(_pattern);

  // destruction will also free this->surface, if set, which will free its
  // memory if nothing else holds a reference to it.
}
