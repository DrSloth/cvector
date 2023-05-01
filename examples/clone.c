#include <stdio.h>
#include <stdint.h>

#include "../cvec.h"

UseVec(uint32_t);

int main() {
  Vec(uint32_t) v = vec_new_with_cap(uint32_t, 10);
  for(uint32_t i = 0; i < vec_capacity(&v); i += 1) {
    vec_push(uint32_t, &v, i);
  }
  Vec(uint32_t) vclone = vec_clone(uint32_t, &v);
  vec_push(uint32_t, &vclone, 420);

  VEC_FOREACH(uint32_t, &v, i, { printf("Original: %d\n", *i); });
  VEC_FOREACH(uint32_t, &vclone, i, { printf("Clone: %d\n", *i); });

  printf("v cap: %ld len: %ld\n", vec_capacity(&v), vec_len(&v));
  printf("vclone cap: %ld len: %ld\n", vec_capacity(&vclone), vec_len(&vclone));
}