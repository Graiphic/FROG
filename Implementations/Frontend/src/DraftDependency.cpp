#include "frog_native_engine/DraftDependency.hpp"
#include "frog_native_engine/Sha256.hpp"

namespace frog::native_engine {
DraftDependencyResult resolveDraftDependency(const DraftDependencyRequest &request,
    const DraftDependencyResolver &resolver) {
    if(!resolver)return {DraftDependencyState::Missing,{}, {}, {}, {}, {},
        "No immutable dependency resolver was supplied."};
    auto result=resolver(request);
    if(result.state!=DraftDependencyState::Present)return result;
    const auto fail=[&](std::string code,std::string message) {
        result.state=DraftDependencyState::Unsupported;
        result.message=std::move(message);
        result.diagnosticCode=std::move(code);
        result.bytes.reset();
        return result;
    };
    if(result.canonicalUri.empty()||!result.bytes)
        return fail("dependency.invalid_snapshot","A present dependency requires a canonical URI and immutable content.");
    const auto digest=hashing::sha256Hex(*result.bytes);
    if((!result.sha256.empty()&&result.sha256!=digest)||
       (!request.expectedSha256.empty()&&request.expectedSha256!=digest))
        return fail("dependency.hash_mismatch","The dependency bytes do not match their declared SHA-256.");
    if(!request.expectedIdentity.empty()&&result.identity!=request.expectedIdentity)
        return fail("dependency.identity_mismatch","The dependency identity does not match its declaration.");
    if(!request.expectedVersion.empty()&&result.version!=request.expectedVersion)
        return fail("dependency.version_mismatch","The dependency version does not match its declaration.");
    result.sha256=digest;
    return result;
}
}
