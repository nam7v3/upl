#ifndef CUTIE_H
#define CUTIE_H

#include <assert.h>
#include <ctype.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char         c8;
typedef char const * cstr;
typedef float        f32;
typedef int16_t      i16;
typedef int32_t      i32;
typedef int64_t      i64;
typedef int8_t       i8;
typedef long double  f64;
typedef ptrdiff_t    isize;
typedef size_t       usize;
typedef uint16_t     u16;
typedef uint32_t     u32;
typedef uint64_t     u64;
typedef uint8_t      u8;
typedef void *       ptr;

// SOME USEFUL MACROS

// get the count of a literal array
#define count_of(a) (isize)(sizeof(a) / sizeof(a[0]))

// Get the container of a member ptr
#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })
// Align of value
#define alignup(x, a) alignmask(x, (typeof(x))(a) - 1)
#define alignmask(x, mask) (((x) + (mask)) & ~(mask))

// Pointer to a value. Useful for htable insertion
#define lit(type, val) &(type) { val }

// Swap to lvalue
#define swap(a, b)                                                             \
  ({                                                                           \
    typeassert(a, b);                                                          \
    typeof(a) __tmp = a;                                                       \
    a = b;                                                                     \
    b = __tmp;                                                                 \
  })

// Some mathematic macros
#define clamp(x, a, b)                                                         \
  ({                                                                           \
    typeof(x) __x = x;                                                         \
    typeof(a) __a = a;                                                         \
    typeof(b) __b = b;                                                         \
    __x<__a ? __a : __x> __b ? __b : __x;                                      \
  })

#define min(a, b)                                                              \
  ({                                                                           \
    typeof(a) __a = a;                                                         \
    typeof(b) __b = b;                                                         \
    __a < __b ? __a : __b;                                                     \
  })

#define max(a, b)                                                              \
  ({                                                                           \
    typeof(a) __a = a;                                                         \
    typeof(b) __b = b;                                                         \
    __a > __b ? __a : __b;                                                     \
  })

// Logging functions
#define INFO(fmt, ...) fprintf(stderr, "INFO: " fmt "\n", ##__VA_ARGS__)
#define ERROR(fmt, ...) fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__)
#define WARNING(fmt, ...) fprintf(stderr, "WARNING: " fmt "\n", ##__VA_ARGS__)
#define FATAL(fmt, ...) fprintf(stderr, "FATAL: " fmt "\n", ##__VA_ARGS__), exit(EXIT_FAILURE)

// Check in compiletime if X and Y is the same type. Use in some generic macros
#define typeassert(X, Y)                                                       \
  static_assert(_Generic((X),                                                  \
                    typeof(X): _Generic((Y), typeof(X): true, default: false), \
                    default: false),                                           \
                #X " and " #Y "is not the same type")

// GENERIC ARRAY TYPE
#define arr_def(name, type)                                                    \
  typedef struct {                                                             \
    type *data;                                                                \
    u32 len;                                                                   \
  } name

arr_def(c8arr_t,    c8);
arr_def(cstrarr_t,  cstr);
arr_def(f32arr_t,   f32);
arr_def(f64arr_t,   f64);
arr_def(i16arr_t,   i16);
arr_def(i32arr_t,   i32);
arr_def(i64arr_t,   i64);
arr_def(i8arr_t,    i8);
arr_def(isizearr_t, isize);
arr_def(ptrarr_t,   isize);
arr_def(u16arr_t,   u16);
arr_def(u32arr_t,   u32);
arr_def(u64arr_t,   u64);
arr_def(u8arr_t,    u8);
arr_def(usizearr_t, usize);


// Convert a cstring to a c8arr
#define c8arr(cstr) (c8arr){.data = cstr, .len = count_of(cstr) - 1}

// Format for c8arr
#define C8ARRFMT "%.*s"
// Unpack the c8arr for printing
#define C8ARRPRNT(str) (str).len, (str).data

// Loop each `item `of an array, `item` is a pointer to the item.
#define foreach(item, arr)                                                     \
  for (typeof((arr).data) item = (arr).data; item != (arr).data + (arr).len;   \
       item++)

// Loop each index of an array
#define foridx(idx, arr) for (u32 idx = 0; idx < (arr).len; ++idx)

// Check if a arr is empty
#define arrempty(arr) (arr.len == 0)

// Compare each byte to arr of the same arrtype.
#define arrcmp(arr1, arr2)                                                     \
  ({                                                                           \
    typeassert(arr1, arr2);                                               \
    memcmp(arr1.data, arr2.data, arr1.len * sizeof(*arr1.data));          \
  })

// ARENA ALLOCATOR
typedef struct arena arena_t;
struct arena {
  u8 *buffer;
  isize capacity;
  isize offset;
};

// flags to put in arnalloc
enum {
  SOFTFAIL = 1 << 0, // If oom, return a NULL pointer (default is abort)
  NONZERO  = 1 << 1, // Initialized the return memory to 0
};

// new(&ARENA, TYPE, COUNT, FLAGS)
#define new(...) newx(__VA_ARGS__, new4, new3, new2)(__VA_ARGS__)
#define newx(a, b, c, d, e, ...) e
#define new2(a, t) (t *)arnalloc(a, sizeof(t), alignof(t), 0)
#define new3(a, t, n) (t *)arnalloc(a, sizeof(t) * n, alignof(t), 0)
#define new4(a, t, n, f) (t *)arnalloc(a, sizeof(t) * n, alignof(t), f)

// Create an arena with size `capacity`
extern arena_t makearena(usize capacity);

// Destroy the arena
extern void    destroyarena(arena_t *arena);
// Clear the allocator, but not destroy it
#define        cleararena(arena) (arena)->offset = 0;

// Free `nbyte` memory
extern void    arnpopb(arena_t *arena, usize nbyte);
// Allocate memory
extern void *  arnalloc(arena_t *a, isize size, isize align, u32 flags);
// Clear all memory from `ptr` on
extern void    arnfree(arena_t *arena, void *ptr);


// SOME ARENA UTILITIES
 // DARRAY
#define darray_def(name, type)                                                 \
  typedef struct {                                                             \
    type *data;                                                                \
    isize len;                                                                 \
    isize cap;                                                                 \
  } name

darray_def(c8da_t,    c8);
darray_def(cstrda_t,  cstr);
darray_def(f32da_t,   f32);
darray_def(f64da_t,   f64);
darray_def(i16da_t,   i16);
darray_def(i32da_t,   i32);
darray_def(i64da_t,   i64);
darray_def(i8da_t,    i8);
darray_def(isizeda_t, isize);
darray_def(ptrda_t,   isize);
darray_def(u16da_t,   u16);
darray_def(u32da_t,   u32);
darray_def(u64da_t,   u64);
darray_def(u8da_t,    u8);
darray_def(usizeda_t, usize);

// Append
#define append(s, a)                                                           \
  ({                                                                           \
    typeof(s) s_ = (s);                                                        \
    typeof(a) a_ = (a);                                                        \
    if (s_->len >= s_->cap) {                                                  \
      dagrow(s_, sizeof(*s_->data), alignof(typeof(*s_->data)), a_);           \
    }                                                                          \
    s_->data + s_->len++;                                                      \
  })

#define unsortedremove(s, idx)                                                 \
  ({                                                                           \
    assert(idx < (s)->len);                                                    \
    (s)->len--;                                                                \
    (s)->data[idx] = (s)->data[(s)->len];                                      \
  })

#define sortedremove(s, idx)                                                   \
  ({                                                                           \
    assert(idx < (s)->len);                                                    \
    memmove((s)->data + idx, (s)->data + idx + 1,                              \
            ((s)->len - idx - 1) * sizeof(*(s)->data));                        \
    (s)->len--;                                                                \
  })

void dagrow(void *array, isize item_size, isize align, arena_t *a);

// SOME ARRAY UTILITIES
//
#define arrcopy(arr, arena)                                                    \
  (typeof(arr)) {                                                              \
    .data = __arrcopy(arena, (arr).data, (arr).len, sizeof(*(arr).data),       \
                      alignof(typeof(*(arr).data))),                           \
    .len = arr.len                                                             \
  }

void *__arrcopy(arena_t *arena, void *data, usize len, usize itemsize,
                usize align);

extern c8*     astrdup(arena_t *a, cstr s);
extern c8*     astrndup(arena_t *a, cstr s, isize len);
extern u8arr_t areadallfile(arena_t *a, cstr path);
extern u8arr_t areadfile(arena_t *a, FILE* f, isize size);

// Generic hash trie
typedef u64 (*hthashfn_t)(u8 *key, usize size);
typedef bool (*htequalfn_t)(u8 *a, u8 *b, usize size);

// Default hash function
extern bool htequal(u8 *a, u8 *b, usize size);
// Default compare function
extern u64 hthash(u8 *key, usize keysize);

#define makehtable(...)                                                        \
  makehtablex(, ##__VA_ARGS__, makehtable2, makehtable1,                       \
              makehtable0)(__VA_ARGS__)
#define makehtablex(a, b, c, d...) d
#define makehtable0() makehtable2(hthash, htequal)
#define makehtable1(hashfn) makehtable2(hashfn, htequal)
#define makehtable2(hashfn, equalfn)                                           \
  {                                                                            \
      .node = NULL,                                                            \
      .hash = hashfn,                                                          \
      .equal = equalfn,                                                        \
  }

#define htable_def(name, keytype, valtype)                                     \
  typedef struct name name;                                                    \
  struct name {                                                                \
    struct {                                                                   \
      void *slots[4];                                                          \
      keytype key;                                                             \
      valtype val;                                                             \
    } *node;                                                                   \
    hthashfn_t hash;                                                           \
    htequalfn_t equal;                                                         \
  };

#define lookup(ht, k)                                                          \
  ({                                                                           \
    typeassert(&(ht)->node->key, k);                                           \
    (typeof(((ht)->node)->val) *)htupsert(                                     \
        (void **)&(ht)->node, sizeof(*(ht)->node), k, sizeof(*k),              \
        offsetof(typeof(*(ht)->node), key),                                    \
        offsetof(typeof(*(ht)->node), val), NULL, (ht)->hash, (ht)->equal);    \
  })

#define upsert(ht, k, arena)                                                   \
  ({                                                                           \
    typeassert(&(ht)->node->key, k);                                           \
    (typeof(((ht)->node)->val) *)htupsert(                                     \
        (void **)&(ht)->node, sizeof(*(ht)->node), k, sizeof(*k),              \
        offsetof(typeof(*(ht)->node), key),                                    \
        offsetof(typeof(*(ht)->node), val), arena, (ht)->hash, (ht)->equal);   \
  })

void *htupsert(void **s, usize stsize, void *key, usize keysize,
               usize keyoffset, usize dataoffset,
               arena_t *arena, hthashfn_t hash, htequalfn_t equal);

// Optional types
#define optional_def(name, T)                                                  \
  typedef struct {                                                             \
    bool ok;                                                                   \
    T data;                                                                    \
  } name

optional_def(cstropt_t,  cstr);
optional_def(u64opt_t,   u64);
optional_def(u32opt_t,   u32);
optional_def(u16opt_t,   u16);
optional_def(u8opt_t,    u8);
optional_def(i64opt_t,   i64);
optional_def(i32opt_t,   i32);
optional_def(i16opt_t,   i16);
optional_def(i8opt_t,    i8);
optional_def(f32opt_t,   f32);
optional_def(f64opt_t,   f64);
optional_def(usizeopt_t, usize);
optional_def(isizeopt_t, isize);
optional_def(ptropt_t,   ptr);

#define optok(opt) (bool)(opt.ok)
#define optget(opt) (typeof(opt.data))(opt.data)
#define optset(opt, d) opt.data = d, opt.ok = true
#define optnull(opt) opt.ok = false

#endif // CUTIE_H
#ifdef CUTIE_IMPLEMENTATION
arena_t makearena(usize capacity) {
  arena_t arena = {
      .buffer = malloc(capacity),
      .capacity = capacity,
  };
  assert(arena.buffer);
  return arena;
}

void destroyarena(arena_t *arena) {
  free(arena->buffer);
  arena->buffer = NULL;
  arena->offset = 0;
  arena->capacity = 0;
}

void arnpopb(arena_t *arena, usize nbyte) {
  arena->offset -= nbyte;
  arena->offset = arena->offset > 0 ? arena->offset : 0;
}

void *arnalloc(arena_t *a, isize size, isize align, u32 flags) {
  isize padding = -(uintptr_t)(a->buffer + a->offset) & (align - 1);
  if (a->offset + padding + size > a->capacity) {
    if (flags & SOFTFAIL)
      return NULL;
    abort();
  }
  void *ptr = a->buffer + a->offset + padding;
  a->offset += padding + size;
  return flags & NONZERO ? ptr : memset(ptr, 0, size);
}

void dagrow(void *array, isize item_size, isize align, arena_t *a) {
  struct {
    void *data;
    isize len;
    isize cap;
  } copy;
  memcpy(&copy, array, sizeof(copy));

  if (!copy.cap) {
    copy.cap = 1;
    copy.data = arnalloc(a, copy.cap * item_size, align, 0);
  } else if (a->buffer + a->offset == copy.data + item_size * copy.cap) {
    // Only allocate one item if darray is the top of arena
    copy.cap += 1;
    arnalloc(a, item_size, align, 0);
  } else {
    copy.cap += 1;
    void *data = arnalloc(a, copy.cap * item_size, align, 0);
    memcpy(data, copy.data, item_size * copy.len);
    copy.data = data;
  }

  memcpy(array, &copy, sizeof(copy));
}

void arnfree(arena_t *a, void *ptr) {
  assert((uintptr_t)a->buffer <= (uintptr_t)ptr &&
         (uintptr_t)ptr <= (uintptr_t)a->buffer + a->offset);
  a->offset = (isize)((u8 *)ptr - a->buffer);
}

c8 *astrdup(arena_t *a, cstr s) {
  isize len = strlen(s) + 1;
  c8 *new_str = new (a, c8, len);
  memcpy(new_str, s, len);
  return new_str;
}

c8 *astrndup(arena_t *a, cstr s, isize len) {
  c8 *new_str = new (a, c8, len);
  memcpy(new_str, s, len);
  return new_str;
}

u8arr_t areadallfile(arena_t *a, cstr path) {
  u8arr_t file = {};
  FILE *f = fopen(path, "rb");
  if (!f)
    return file;
  fseek(f, 0, SEEK_END);
  file.len = ftell(f);
  fseek(f, 0, SEEK_SET);

  file.data = new (a, u8, file.len);
  fread(file.data, 1, file.len, f);
  fclose(f);
  return file;
}

u8arr_t areadfile(arena_t *a, FILE* f, isize size) {
  assert(f);
  u8arr_t file = {};
  file.data = new(a, u8, file.len);
  file.len = fread(file.data, 1, size, f);
  return file;
}

u64 hthash(u8 *key, usize keysize) {
  uint64_t h = 0x100;
  for (ptrdiff_t i = 0; i < keysize; i++) {
    h ^= key[i];
    h *= 1111111111111111111u;
  }
  return h;
}

bool htequal(u8 *a, u8 *b, usize size) { return memcmp(a, b, size) == 0; }

typedef struct htable_header {
  struct htable_header *slots[4];
} htable_header_t;

void *htupsert(void **s, usize stsize, void *key, usize keysize,
               usize keyoffset, usize dataoffset,
               arena_t *arena, hthashfn_t hash, htequalfn_t equal) {
  htable_header_t **ht = (htable_header_t **)(s);
  for (u64 h = hash(key, keysize); (*ht) != NULL; h <<= 2) {
    if (equal(key, ((u8 *)*ht) + keyoffset, keysize)) {
      return ((u8 *)*ht) + dataoffset;
    }
    ht = &((*ht)->slots[(h >> 62)]);
  }
  if (arena) {
    *ht = arnalloc(arena, stsize, sizeof(void *) * 2, 0);
    memcpy(((u8 *)*ht) + keyoffset, key, keysize);
    return ((u8 *)*ht) + dataoffset;
  }
  return NULL;
}

#endif // CUTIE_IMPLEMENTATION
