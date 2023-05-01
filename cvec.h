#ifndef VEC_DECL_H
#define VEC_DECL_H

#include <stdint.h>
#include <stdlib.h>

#define CONCAT_(A, B) A##B
#define CONCAT(A, B) CONCAT_(A, B)
#define DOC(TEXT)

#define VPtr(T) __vec##T##__ptr

#define VDefPtr(T) typedef VPtr(T) T *;

/// A vector type storing T. Writing any of the fields is undefined behavior.
/// It is sadly not possible to use pointer types with this, you can use the
/// VDefPtr macro for convinience to achieve that and then use the VPtr macro to
/// use the ptr type.
#define Vec(T) Vector__##T

#define vec_new_with_cap(T, cap) (___vector_new_with_cap__##T(cap))
#define vec_push(T, self, val) (___vector_push__##T(self, val))
#define vec_clone(T, self) (___vector_clone__##T(self))
#define vec_len(self) ((self)->__len_)
#define vec_get_raw(self) ((self)->__alloc_)
#define vec_capacity(self) ((self)->__cap_)
#define vec_uninit_part(self) (self->__alloc_ + self->__len_)
#define vec_get_unchecked(self, i) ((self)->__alloc_ + i)
#define vec_set_unchecked(self, i, v)                                          \
  do {                                                                         \
    (self)->__alloc_[i] = v;                                                   \
  } while (0);
#define vec_reserve_extra(T, self, extra)                                         \
  do {                                                                         \
    (self)->reserve_extra(extra);                                              \
  } while (0);
/// This macro automatically extends the vector with default to at least be able
/// to set at i. This operation might reallocate. This also returns a pointer to
/// the set element. If the extending behavior is not wanted the value should be
/// set by using the pointer of get.
#define vec_set(T, self, i, val, default_value)                                \
  (___vector_set__##T(vec, i, v, default_value))
#define vec_get(self, i) (___vector_set__##T(self, i))
#define vec_clear(self)                                                        \
  do {                                                                         \
    (self)->__len_ = 0;                                                        \
  } while (0);
#define vec_set_len_unchecked(self, len) ((self)->__len_ = len)

/// Declare Vector data structure that can store instances of T.
/// This would generally be placed inside a Header file
#define DeclVec(T)                                                             \
  typedef struct Vec(T) {                                                      \
    size_t __cap_;                                                             \
    size_t __len_;                                                             \
    T *__alloc_;                                                               \
  }                                                                            \
  Vec(T);                                                                      \
  Vec(T) ___vector_new_with_cap__##T(size_t cap);                              \
  void ___vector_push__##T(Vec(T) * self, T val);                              \
  Vec(T) ___vector_clone__##T(Vec(T) * self);                                  \
  T *___vector_get__##T(Vec(T) * self, size_t i);                              \
  T *___vector_set__##T(Vec(T) * self, size_t i, T val, T default_value);

/// The Block argument is a ... and is expanded with __VA_ARGS__ to allow , in
/// {} inside There is a gcc extension which allows the ({}) sytnax in theory
/// but this should be iso C99.
#define VEC_FOREACH(T, VEC, VAR, ...)                                          \
  do {                                                                         \
    Vec(T) *__vec__ptr__ = VEC;                                                \
    T *__iter_end_ptr__ = __vec__ptr__->__alloc_ + __vec__ptr__->__len_;       \
    for (T *__iter__cur__ptr__ = __vec__ptr__->__alloc_;                       \
         __iter__cur__ptr__ != __iter_end_ptr__; __iter__cur__ptr__++) {       \
      T *VAR = __iter__cur__ptr__;                                             \
      __VA_ARGS__;                                                             \
    }                                                                          \
  } while (0);

/// Implement a Vector that is able to store instances of T.
/// This requires a previously used `DeclVec(T)`
#define ImplVec(T)                                                             \
  Vec(T) ___vector_new_with_cap__##T(size_t cap) {                             \
    T *alloc = malloc(cap * sizeof(*alloc));                                   \
    Vec(T) v = {                                                               \
        .__cap_ = cap,                                                         \
        .__len_ = 0,                                                           \
        .__alloc_ = alloc,                                                     \
    };                                                                         \
                                                                               \
    return v;                                                                  \
  }                                                                            \
  void ___vector_push__##T(Vec(T) * self, T val) {                             \
    size_t next_len = self->__len_ + 1;                                        \
    if (next_len > self->__cap_) {                                             \
      size_t prev_cap = self->__cap_;                                          \
      self->__cap_ *= 2;                                                       \
      self->__alloc_ =                                                         \
          realloc(self->__alloc_, self->__cap_ * sizeof(*self->__alloc_));     \
      if (self->__alloc_ == NULL) {                                            \
        fprintf(stderr, "[ERROR] Realloc of Vector to capacity %ld ran OOM",   \
                self->__cap_);                                                 \
        exit(1);                                                               \
      }                                                                        \
      if (self->__cap_ < prev_cap) {                                           \
        fprintf(stderr, "[ERROR] Realloc of Vector had capacity overflow");    \
        exit(1);                                                               \
      }                                                                        \
    }                                                                          \
                                                                               \
    self->__alloc_[self->__len_] = val;                                        \
    self->__len_ = next_len;                                                   \
  }                                                                            \
                                                                               \
  Vec(T) ___vector_clone__##T(Vec(T) * self) {                                 \
    Vec(T) clone = vec_new_with_cap(T, self->__len_);                          \
    clone.__len_ = self->__len_;                                               \
    int i = 0;                                                                 \
    VEC_FOREACH(T, &clone, cur_mut, *cur_mut = self->__alloc_[i++]);           \
    return clone;                                                              \
  }                                                                            \
  T *___vector_get__##T(Vec(T) * self, size_t i) {                             \
    if (i >= self->__len_) {                                                   \
      return NULL;                                                             \
    } else {                                                                   \
      return self->__alloc_ + i;                                               \
    }                                                                          \
  }                                                                            \
  T *___vector_set__##T(Vec(T) * self, size_t i, T val, T default_value) {     \
    DOC(TODO this should be optimised to be just a single reserve call)        \
    if (i >= self->__len_) {                                                   \
      DOC(This adds i - self->__len_ +                                         \
          1 values to the vector without potential over / underflow)           \
      for (size_t j = self->__len_; j <= i; j++) {                             \
        DOC(TODO make default value optional)                                  \
        vec_push(T, self, default_value);                                      \
      }                                                                        \
    }                                                                          \
    self->__alloc_[i] = val;                                                   \
    return self->__alloc_ + i;                                                 \
  }

/// Include and directly declare as well as implement vector type able to store
/// instances of T. To avoid redundancy the Vector declarations and
/// implementations should be seperated into source and header files. This is of
/// course not necessary if you know you won't declare the same Vec two times.
#define UseVec(T) DeclVec(T) ImplVec(T)

#endif // VEC_DECL_H
