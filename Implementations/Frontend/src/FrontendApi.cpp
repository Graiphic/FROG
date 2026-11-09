// SPDX-License-Identifier: Apache-2.0
// C ownership/concurrency boundary only. All language work belongs to the
// common operation implementation; no source model is copied into this layer.
#include "FrontendOperation.hpp"

#include <atomic>
#include <new>
#include <string>
#include <string_view>
#include <utility>

struct frog_frontend_context {
    explicit frog_frontend_context(const frog_frontend_config &value)
        : config(value) {}

    const frog_frontend_config config;
    std::atomic_flag busy = ATOMIC_FLAG_INIT;
};

struct frog_frontend_result {
    frog_frontend_result(frog_frontend_operation operationValue,
                         frog::frontend::OperationResult &&value)
        : operation(operationValue), documents(std::move(value)) {}

    const frog_frontend_operation operation;
    const frog::frontend::OperationResult documents;
};

namespace {

class OperationLease final {
public:
    explicit OperationLease(frog_frontend_context &context) noexcept
        : context_(context) {}
    ~OperationLease() { context_.busy.clear(std::memory_order_release); }
    OperationLease(const OperationLease &) = delete;
    OperationLease &operator=(const OperationLease &) = delete;

private:
    frog_frontend_context &context_;
};

frog_frontend_status failureStatus(frog_frontend_status status) noexcept
{
    switch (status) {
    case FROG_FRONTEND_INVALID_ARGUMENT:
    case FROG_FRONTEND_UNSUPPORTED_API:
    case FROG_FRONTEND_BUSY:
    case FROG_FRONTEND_OUT_OF_MEMORY:
    case FROG_FRONTEND_INTERNAL_ERROR:
        return status;
    default:
        return FROG_FRONTEND_INTERNAL_ERROR;
    }
}

frog_frontend_status operate(frog_frontend_context *context,
                            const frog_frontend_request *request,
                            frog_frontend_result **outResult,
                            frog_frontend_operation operation) noexcept
{
    if (!outResult) return FROG_FRONTEND_INVALID_ARGUMENT;
    *outResult = nullptr;
    if (!context) return FROG_FRONTEND_INVALID_ARGUMENT;
    if (context->busy.test_and_set(std::memory_order_acquire))
        return FROG_FRONTEND_BUSY;
    const OperationLease lease(*context);
    try {
        auto documents = frog::frontend::runOperation(operation, context->config,
                                                       request);
        if (documents.outcome > FROG_FRONTEND_LIMIT_EXCEEDED)
            return FROG_FRONTEND_INTERNAL_ERROR;
        *outResult = new frog_frontend_result(operation, std::move(documents));
        return FROG_FRONTEND_OK;
    } catch (const frog::frontend::ApiFailure &failure) {
        return failureStatus(failure.status);
    } catch (const std::bad_alloc &) {
        return FROG_FRONTEND_OUT_OF_MEMORY;
    } catch (...) {
        return FROG_FRONTEND_INTERNAL_ERROR;
    }
}

frog_frontend_utf8 viewOf(const std::string &text) noexcept
{
    return text.empty() ? frog_frontend_utf8{nullptr, 0}
                        : frog_frontend_utf8{text.data(), text.size()};
}

} // namespace

extern "C" {

FROG_FRONTEND_API uint32_t FROG_FRONTEND_CALL
frog_frontend_api_version(void)
{
    return FROG_FRONTEND_API_VERSION;
}

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_context_create(uint32_t requestedApiVersion,
                             const frog_frontend_config *config,
                             frog_frontend_context **outContext)
{
    if (!outContext) return FROG_FRONTEND_INVALID_ARGUMENT;
    *outContext = nullptr;
    if (requestedApiVersion != FROG_FRONTEND_API_VERSION)
        return FROG_FRONTEND_UNSUPPORTED_API;
    if (!config || config->struct_size != sizeof(*config) || config->reserved != 0)
        return FROG_FRONTEND_INVALID_ARGUMENT;
    try {
        *outContext = new frog_frontend_context(*config);
        return FROG_FRONTEND_OK;
    } catch (const std::bad_alloc &) {
        return FROG_FRONTEND_OUT_OF_MEMORY;
    } catch (...) {
        return FROG_FRONTEND_INTERNAL_ERROR;
    }
}

FROG_FRONTEND_API void FROG_FRONTEND_CALL
frog_frontend_context_destroy(frog_frontend_context *context)
{
    delete context;
}

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_validate(frog_frontend_context *context,
                       const frog_frontend_request *request,
                       frog_frontend_result **outResult)
{
    return operate(context, request, outResult, FROG_FRONTEND_VALIDATE);
}

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_derive(frog_frontend_context *context,
                     const frog_frontend_request *request,
                     frog_frontend_result **outResult)
{
    return operate(context, request, outResult, FROG_FRONTEND_DERIVE);
}

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_describe(frog_frontend_context *context,
                       frog_frontend_result **outResult)
{
    return operate(context, nullptr, outResult, FROG_FRONTEND_DESCRIBE);
}

FROG_FRONTEND_API frog_frontend_status FROG_FRONTEND_CALL
frog_frontend_result_get(const frog_frontend_result *result,
                         frog_frontend_result_view *view)
{
    if (!result || !view || view->struct_size != sizeof(*view) ||
        view->reserved != 0) return FROG_FRONTEND_INVALID_ARGUMENT;

    frog_frontend_result_view prepared{};
    prepared.struct_size = sizeof(prepared);
    prepared.operation = result->operation;
    prepared.outcome = result->documents.outcome;
    prepared.fir_json = viewOf(result->documents.firJson);
    prepared.diagnostics_json = viewOf(result->documents.diagnosticsJson);
    prepared.source_maps_json = viewOf(result->documents.sourceMapsJson);
    prepared.provenance_json = viewOf(result->documents.provenanceJson);
    prepared.descriptor_json = viewOf(result->documents.descriptorJson);
    prepared.resolved_source_json = viewOf(result->documents.resolvedSourceJson);
    *view = prepared;
    return FROG_FRONTEND_OK;
}

FROG_FRONTEND_API void FROG_FRONTEND_CALL
frog_frontend_result_destroy(frog_frontend_result *result)
{
    delete result;
}

} // extern "C"
