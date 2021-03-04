// The Wasmer C/C++ header file compatible with the [`wasm-c-api`]
// standard API, as `wasm.h` (included here).
//
// This file is automatically generated by `lib/c-api/build.rs` of the
// [`wasmer-c-api`] Rust crate.
//
// # Stability
//
// The [`wasm-c-api`] standard API is a _living_ standard. There is no
// commitment for stability yet. We (Wasmer) will try our best to keep
// backward compatibility as much as possible. Nonetheless, some
// necessary API aren't yet standardized, and as such, we provide a
// custom API, e.g. `wasi_*` types and functions.
//
// The documentation makes it clear whether a function is unstable.
// 
// When a type or a function will be deprecated, it will be marked as
// such with the appropriated compiler warning, and will be removed at
// the next release round.
//
// # Documentation
//
// At the time of writing, the [`wasm-c-api`] standard has no
// documentation. This file also does not include inline
// documentation. However, we have made (and we continue to make) an
// important effort to document everything. [See the documentation
// online][documentation]. Please refer to this page for the real
// canonical documentation. It also contains numerous examples.
//
// To generate the documentation locally, run `cargo doc --open` from
// within the [`wasmer-c-api`] Rust crate.
//
// [`wasm-c-api`]: https://github.com/WebAssembly/wasm-c-api
// [`wasmer-c-api`]: https://github.com/wasmerio/wasmer/tree/master/lib/c-api
// [documentation]: https://wasmerio.github.io/wasmer/crates/wasmer_c_api/

#if !defined(WASMER_WASM_H_PRELUDE)

#define WASMER_WASM_H_PRELUDE

// Define the `ARCH_X86_X64` constant.
#if defined(MSVC) && defined(_M_AMD64)
#  define ARCH_X86_64
#elif (defined(GCC) || defined(__GNUC__) || defined(__clang__)) && defined(__x86_64__)
#  define ARCH_X86_64
#endif

// Compatibility with non-Clang compilers.
#if !defined(__has_attribute)
#  define __has_attribute(x) 0
#endif

// Compatibility with non-Clang compilers.
#if !defined(__has_declspec_attribute)
#  define __has_declspec_attribute(x) 0
#endif

// Define the `DEPRECATED` macro.
#if defined(GCC) || defined(__GNUC__) || __has_attribute(deprecated)
#  define DEPRECATED(message) __attribute__((deprecated(message)))
#elif defined(MSVC) || __has_declspec_attribute(deprecated)
#  define DEPRECATED(message) __declspec(deprecated(message))
#endif

// The `jit` feature has been enabled for this build.
#define WASMER_JIT_ENABLED

// The `compiler` feature has been enabled for this build.
#define WASMER_COMPILER_ENABLED

// The `wasi` feature has been enabled for this build.
#define WASMER_WASI_ENABLED

// This file corresponds to the following Wasmer version.
#define WASMER_VERSION "1.0.2"
#define WASMER_VERSION_MAJOR 1
#define WASMER_VERSION_MINOR 0
#define WASMER_VERSION_PATCH 2
#define WASMER_VERSION_PRE ""

#endif // WASMER_WASM_H_PRELUDE


//
// OK, here we go. The code below is automatically generated.
//


#ifndef WASMER_WASM_H
#define WASMER_WASM_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "wasm.h"

#if defined(WASMER_WASI_ENABLED)
typedef enum wasi_version_t {
#if defined(WASMER_WASI_ENABLED)
  INVALID_VERSION = -1,
#endif
#if defined(WASMER_WASI_ENABLED)
  LATEST = 0,
#endif
#if defined(WASMER_WASI_ENABLED)
  SNAPSHOT0 = 1,
#endif
#if defined(WASMER_WASI_ENABLED)
  SNAPSHOT1 = 2,
#endif
} wasi_version_t;
#endif

#if defined(WASMER_COMPILER_ENABLED)
typedef enum wasmer_compiler_t {
  CRANELIFT = 0,
  LLVM = 1,
  SINGLEPASS = 2,
} wasmer_compiler_t;
#endif

typedef enum wasmer_engine_t {
  JIT = 0,
  NATIVE = 1,
  OBJECT_FILE = 2,
} wasmer_engine_t;

#if defined(WASMER_WASI_ENABLED)
typedef struct wasi_config_t wasi_config_t;
#endif

#if defined(WASMER_WASI_ENABLED)
typedef struct wasi_env_t wasi_env_t;
#endif

typedef struct wasmer_cpu_features_t wasmer_cpu_features_t;

typedef struct wasmer_features_t wasmer_features_t;

#if defined(WASMER_WASI_ENABLED)
typedef struct wasmer_named_extern_t wasmer_named_extern_t;
#endif

typedef struct wasmer_target_t wasmer_target_t;

typedef struct wasmer_triple_t wasmer_triple_t;

#if defined(WASMER_WASI_ENABLED)
typedef struct wasmer_named_extern_vec_t {
  uintptr_t size;
  struct wasmer_named_extern_t **data;
} wasmer_named_extern_vec_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(WASMER_WASI_ENABLED)
void wasi_config_arg(struct wasi_config_t *config, const char *arg);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasi_config_capture_stderr(struct wasi_config_t *config);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasi_config_capture_stdout(struct wasi_config_t *config);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasi_config_env(struct wasi_config_t *config, const char *key, const char *value);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasi_config_inherit_stderr(struct wasi_config_t *config);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasi_config_inherit_stdin(struct wasi_config_t *config);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasi_config_inherit_stdout(struct wasi_config_t *config);
#endif

#if defined(WASMER_WASI_ENABLED)
bool wasi_config_mapdir(struct wasi_config_t *config, const char *alias, const char *dir);
#endif

#if defined(WASMER_WASI_ENABLED)
struct wasi_config_t *wasi_config_new(const char *program_name);
#endif

#if defined(WASMER_WASI_ENABLED)
bool wasi_config_preopen_dir(struct wasi_config_t *config, const char *dir);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasi_env_delete(struct wasi_env_t *_state);
#endif

#if defined(WASMER_WASI_ENABLED)
struct wasi_env_t *wasi_env_new(struct wasi_config_t *config);
#endif

#if defined(WASMER_WASI_ENABLED)
intptr_t wasi_env_read_stderr(struct wasi_env_t *env, char *buffer, uintptr_t buffer_len);
#endif

#if defined(WASMER_WASI_ENABLED)
intptr_t wasi_env_read_stdout(struct wasi_env_t *env, char *buffer, uintptr_t buffer_len);
#endif

#if defined(WASMER_WASI_ENABLED)
DEPRECATED("This function is no longer necessary. You may safely remove all calls to it and everything will continue to work.")
bool wasi_env_set_instance(struct wasi_env_t *_env,
                           const wasm_instance_t *_instance);
#endif

#if defined(WASMER_WASI_ENABLED)
DEPRECATED("This function is no longer necessary. You may safely remove all calls to it and everything will continue to work.")
void wasi_env_set_memory(struct wasi_env_t *_env,
                         const wasm_memory_t *_memory);
#endif

#if defined(WASMER_WASI_ENABLED)
bool wasi_get_imports(const wasm_store_t *store,
                      const wasm_module_t *module,
                      const struct wasi_env_t *wasi_env,
                      wasm_extern_vec_t *imports);
#endif

#if defined(WASMER_WASI_ENABLED)
wasm_func_t *wasi_get_start_function(wasm_instance_t *instance);
#endif

#if defined(WASMER_WASI_ENABLED)
bool wasi_get_unordered_imports(const wasm_store_t *store,
                                const wasm_module_t *module,
                                const struct wasi_env_t *wasi_env,
                                struct wasmer_named_extern_vec_t *imports);
#endif

#if defined(WASMER_WASI_ENABLED)
enum wasi_version_t wasi_get_wasi_version(const wasm_module_t *module);
#endif

#if defined(WASMER_COMPILER_ENABLED)
void wasm_config_set_compiler(wasm_config_t *config, enum wasmer_compiler_t compiler);
#endif

void wasm_config_set_engine(wasm_config_t *config, enum wasmer_engine_t engine);

void wasm_config_set_features(wasm_config_t *config, struct wasmer_features_t *features);

void wasm_config_set_target(wasm_config_t *config, struct wasmer_target_t *target);

bool wasmer_cpu_features_add(struct wasmer_cpu_features_t *cpu_features,
                             const wasm_name_t *feature);

void wasmer_cpu_features_delete(struct wasmer_cpu_features_t *_cpu_features);

struct wasmer_cpu_features_t *wasmer_cpu_features_new(void);

bool wasmer_features_bulk_memory(struct wasmer_features_t *features, bool enable);

void wasmer_features_delete(struct wasmer_features_t *_features);

bool wasmer_features_memory64(struct wasmer_features_t *features, bool enable);

bool wasmer_features_module_linking(struct wasmer_features_t *features, bool enable);

bool wasmer_features_multi_memory(struct wasmer_features_t *features, bool enable);

bool wasmer_features_multi_value(struct wasmer_features_t *features, bool enable);

struct wasmer_features_t *wasmer_features_new(void);

bool wasmer_features_reference_types(struct wasmer_features_t *features, bool enable);

bool wasmer_features_simd(struct wasmer_features_t *features, bool enable);

bool wasmer_features_tail_call(struct wasmer_features_t *features, bool enable);

bool wasmer_features_threads(struct wasmer_features_t *features, bool enable);

bool wasmer_is_compiler_available(enum wasmer_compiler_t compiler);

bool wasmer_is_engine_available(enum wasmer_engine_t engine);

bool wasmer_is_headless(void);

int wasmer_last_error_length(void);

int wasmer_last_error_message(char *buffer, int length);

void wasmer_module_name(const wasm_module_t *module, wasm_name_t *out);

bool wasmer_module_set_name(wasm_module_t *module, const wasm_name_t *name);

#if defined(WASMER_WASI_ENABLED)
const wasm_name_t *wasmer_named_extern_module(const struct wasmer_named_extern_t *named_extern);
#endif

#if defined(WASMER_WASI_ENABLED)
const wasm_name_t *wasmer_named_extern_name(const struct wasmer_named_extern_t *named_extern);
#endif

#if defined(WASMER_WASI_ENABLED)
const wasm_extern_t *wasmer_named_extern_unwrap(const struct wasmer_named_extern_t *named_extern);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasmer_named_extern_vec_copy(struct wasmer_named_extern_vec_t *out_ptr,
                                  const struct wasmer_named_extern_vec_t *in_ptr);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasmer_named_extern_vec_delete(struct wasmer_named_extern_vec_t *ptr);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasmer_named_extern_vec_new(struct wasmer_named_extern_vec_t *out,
                                 uintptr_t length,
                                 struct wasmer_named_extern_t *const *init);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasmer_named_extern_vec_new_empty(struct wasmer_named_extern_vec_t *out);
#endif

#if defined(WASMER_WASI_ENABLED)
void wasmer_named_extern_vec_new_uninitialized(struct wasmer_named_extern_vec_t *out,
                                               uintptr_t length);
#endif

void wasmer_target_delete(struct wasmer_target_t *_target);

struct wasmer_target_t *wasmer_target_new(struct wasmer_triple_t *triple,
                                          struct wasmer_cpu_features_t *cpu_features);

void wasmer_triple_delete(struct wasmer_triple_t *_triple);

struct wasmer_triple_t *wasmer_triple_new(const wasm_name_t *triple);

struct wasmer_triple_t *wasmer_triple_new_from_host(void);

const char *wasmer_version(void);

uint8_t wasmer_version_major(void);

uint8_t wasmer_version_minor(void);

uint8_t wasmer_version_patch(void);

const char *wasmer_version_pre(void);

void wat2wasm(const wasm_byte_vec_t *wat, wasm_byte_vec_t *out);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* WASMER_WASM_H */
