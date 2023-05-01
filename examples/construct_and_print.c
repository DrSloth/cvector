#include <stdio.h>
#include <stdint.h>

#include "../cvec.h"

DeclVec(uint32_t);
ImplVec(uint32_t);

int main() {
  Vec(uint32_t) v = vec_new_with_cap(uint32_t, 10);
  for(uint32_t i = 0; i < vec_capacity(&v); i += 1) {
    vec_push(uint32_t, &v, i);
  }

  VEC_FOREACH(uint32_t, &v, i, { printf("Number: %d\n", *i); });
}