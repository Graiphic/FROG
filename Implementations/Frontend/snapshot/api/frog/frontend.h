/*
 * SPDX-License-Identifier: Apache-2.0
 * FROG Frontend API 3 -- experimental local implementation 0.1.0-local.
 * API 1 was a P01 declaration-only contract with no delivered binary.
 */
#ifndef FROG_FRONTEND_H_INCLUDED
#define FROG_FRONTEND_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#ifndef FROG_FRONTEND_API
#define FROG_FRONTEND_API
#endif
#if defined(_WIN32)
#define FROG_FRONTEND_CALL __cdecl
#else
#define FROG_FRONTEND_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FROG_FRONTEND_API_VERSION UINT32_C(3)

/* Caller and callee must agree on API version and sizeof each public structure.
 * API 3 rejects other sizes, nonzero reserved fields and unknown codes. It does
 * not reinterpret a future structure prefix as a current request. */
typedef struct frog_frontend_context frog_frontend_context;
typedef struct frog_frontend_result frog_frontend_result;

/* UTF-8, length-delimited, not necessarily NUL-terminated. NULL is permitted
 * only with size == 0. Embedded NUL is invalid for text/JSON/identities.
 * No borrowed view transfers allocation ownership. */
typedef struct frog_frontend_utf8 {
    const char *data;
    size_t size;
} frog_frontend_utf8;

typedef uint32_t frog_frontend_status;
enum {
    FROG_FRONTEND_OK = 0,
    FROG_FRONTEND_INVALID_ARGUMENT = 1,
    FROG_FRONTEND_UNSUPPORTED_API = 2,
    FROG_FRONTEND_BUSY = 3,
    FROG_FRONTEND_OUT_OF_MEMORY = 4,
    FROG_FRONTEND_INTERNAL_ERROR = 5
};

/* A successful C call can return a rejected program. Only SUCCESS admits the
 * requested operation; validation success is not native execution capability. */
typedef uint32_t frog_frontend_outcome;
enum {
    FROG_FRONTEND_SUCCESS = 0,
    FROG_FRONTEND_REJECTED = 1,
    FROG_FRONTEND_UNSUPPORTED = 2,
    FROG_FRONTEND_CANCELLED = 3,
    FROG_FRONTEND_LIMIT_EXCEEDED = 4
};

typedef uint32_t frog_frontend_operation;
enum {
    FROG_FRONTEND_VALIDATE = 1,
    FROG_FRONTEND_DERIVE = 2,
    FROG_FRONTEND_DESCRIBE = 3
};

/* Zero requests the documented API-3 default. Nonzero values are caller
 * ceilings, never permission to exceed a supported semantic profile's cap.
 * See frontend-api.md for units, defaults and counting rules. */
typedef struct frog_frontend_limits {
    uint64_t max_source_bytes;
    uint64_t max_dependency_bytes;
    uint64_t max_total_dependency_bytes;
    uint64_t max_result_bytes;
    uint64_t max_nodes;
    uint64_t max_connections;
    uint64_t max_nesting_depth;
    uint64_t max_dependencies;
    uint64_t max_diagnostics;
} frog_frontend_limits;

typedef struct frog_frontend_config {
    uint32_t struct_size;
    uint32_t reserved;
    frog_frontend_limits limits;
} frog_frontend_config;

/* Return 0 to continue, 1 to cancel; any other value is a callback failure.
 * Called synchronously on the operation's calling thread. */
typedef uint32_t (FROG_FRONTEND_CALL *frog_frontend_cancel_fn)(void *user);

typedef uint32_t frog_frontend_dependency_kind;
enum {
    FROG_FRONTEND_DEPENDENCY_SOURCE = 1,
    FROG_FRONTEND_DEPENDENCY_TYPE_DEFINITION = 2,
    FROG_FRONTEND_DEPENDENCY_LIBRARY_CONTRACT = 3,
    FROG_FRONTEND_DEPENDENCY_PROFILE_CONTRACT = 4
};

typedef struct frog_frontend_dependency_request {
    uint32_t struct_size;
    frog_frontend_dependency_kind kind;
    frog_frontend_utf8 requesting_source_uri;
    frog_frontend_utf8 source_pointer;  /* JSON Pointer in requesting source. */
    frog_frontend_utf8 reference;       /* Exact authored reference, not CWD. */
    frog_frontend_utf8 expected_identity; /* Empty only if not authored. */
    frog_frontend_utf8 expected_version;  /* Empty only if not authored. */
    frog_frontend_utf8 expected_sha256;   /* Empty or 64 lowercase hex digits. */
} frog_frontend_dependency_request;

typedef uint32_t frog_frontend_resolve_status;
enum {
    FROG_FRONTEND_RESOLVED = 0,
    FROG_FRONTEND_NOT_FOUND = 1,
    FROG_FRONTEND_ACCESS_DENIED = 2,
    FROG_FRONTEND_RESOLVE_CANCELLED = 3,
    FROG_FRONTEND_RESOLVE_ERROR = 4
};

/* Host-owned snapshot, including all strings. The frontend checks declared
 * identities/versions, computes SHA-256 over the exact bytes and compares any
 * requested digest. Supplied metadata never overrides the document's meaning. */
typedef struct frog_frontend_resolved_dependency {
    uint32_t struct_size;
    uint32_t reserved;
    frog_frontend_utf8 source_uri; /* Explicit stable identity; no implicit base. */
    frog_frontend_utf8 identity;
    frog_frontend_utf8 version;
    frog_frontend_utf8 bytes;
    frog_frontend_utf8 error_message; /* Optional host diagnostic on refusal. */
    void *lease; /* Opaque host token; may be NULL. */
} frog_frontend_resolved_dependency;

typedef frog_frontend_resolve_status
(FROG_FRONTEND_CALL *frog_frontend_resolve_fn)(
    void *user,
    const frog_frontend_dependency_request *request,
    frog_frontend_resolved_dependency *resolved);

/* Exactly once after each normally returned resolve callback, for every
 * status, including rejected metadata and cancellation. No view is used after
 * release returns. Called before the enclosing operation returns. */
typedef void (FROG_FRONTEND_CALL *frog_frontend_release_dependency_fn)(
    void *user,
    const frog_frontend_resolved_dependency *resolved);

typedef struct frog_frontend_resolver {
    void *user;
    frog_frontend_resolve_fn resolve;
    frog_frontend_release_dependency_fn release;
} frog_frontend_resolver;

typedef struct frog_frontend_request {
    uint32_t struct_size;
    uint32_t reserved;
    frog_frontend_utf8 source_bytes;          /* Exact immutable authored input. */
    frog_frontend_utf8 source_uri;            /* Required, explicit, UTF-8. */
    frog_frontend_utf8 source_document_id;    /* Required technical identity. */
    frog_frontend_utf8 source_format_id;      /* Explicit format, never guessed. */
    frog_frontend_utf8 source_format_version; /* Must match its actual envelope. */
    frog_frontend_utf8 fir_format_version;    /* Exact target; no "latest". */
    frog_frontend_resolver resolver;         /* Both callbacks set, or both NULL. */
    frog_frontend_cancel_fn is_cancelled;    /* Optional; cooperative. */
    void *cancel_user;
    /* Explicit JSON object for the declared semantic compilation context.
     * Empty means {}, without inherited OS, environment or Studio settings.
     * This API-2 addition is not an API-1-compatible structure extension. */
    frog_frontend_utf8 semantic_context_json;
    /* Required independent caller-owned FIR document identity. API 3 never
     * substitutes this for the original source_document_id in provenance. */
    frog_frontend_utf8 fir_document_id;
} frog_frontend_request;

/* Views are read-only, valid until result_destroy, and independent of context
 * lifetime. Each nonempty JSON field is a separate complete UTF-8 document.
 * No native layout, UI state or private Runtime ProgramArtifact is exposed. */
typedef struct frog_frontend_result_view {
    uint32_t struct_size;
    frog_frontend_operation operation;
    frog_frontend_outcome outcome;
    uint32_t reserved;
    frog_frontend_utf8 fir_json;         /* Only successful DERIVE. */
    frog_frontend_utf8 diagnostics_json; /* Every result, including empty list. */
    frog_frontend_utf8 source_maps_json; /* Attributed source/IR identities. */
    frog_frontend_utf8 provenance_json;  /* Exact source/dependency/version facts. */
    frog_frontend_utf8 descriptor_json;  /* Only successful DESCRIBE. */
    /* Successful validate/derive only, if resolution changed the source.
     * Exact complete projected Source JSON used to seal the FIR, not a patch.
     * Empty for unchanged source, refusals and DESCRIBE. Same result lifetime. */
    frog_frontend_utf8 resolved_source_json;
} frog_frontend_result_view;

/* out_context/out_result are set to NULL before processing. On non-OK status
 * there is no result to consume; on OK the non-NULL result must be destroyed.
 * No C++ exception may cross this boundary in either direction. */
FROG_FRONTEND_API uint32_t FROG_FRONTEND_CALL
frog_frontend_api_version(void);

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_context_create(
    uint32_t requested_api_version,
    const frog_frontend_config *config,
    frog_frontend_context **out_context);

FROG_FRONTEND_API void FROG_FRONTEND_CALL
frog_frontend_context_destroy(frog_frontend_context *context);

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_validate(
    frog_frontend_context *context,
    const frog_frontend_request *request,
    frog_frontend_result **out_result);

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_derive(
    frog_frontend_context *context,
    const frog_frontend_request *request,
    frog_frontend_result **out_result);

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_describe(
    frog_frontend_context *context,
    frog_frontend_result **out_result);

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_result_get(
    const frog_frontend_result *result,
    frog_frontend_result_view *view);

FROG_FRONTEND_API void FROG_FRONTEND_CALL
frog_frontend_result_destroy(frog_frontend_result *result);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* FROG_FRONTEND_H_INCLUDED */

