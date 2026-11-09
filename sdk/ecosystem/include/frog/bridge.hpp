#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Source API convenience only; the third-party binary contract is the bounded
// public UTF-8 IPC protocol, never this C++ ABI or a Runtime/Studio header.
namespace frog::bridge {
struct Limits {
    std::uint32_t maximumMessageBytes=1048576;
    std::uint32_t requestTimeoutMs=30000;
    std::uint32_t maximumProgressMessages=256;
    std::uint32_t maximumStderrBytes=65536;
};
struct Identity {
    std::string requestId,sessionId;
    std::uint64_t generation=0;
    bool operator==(const Identity &)const=default;
};
struct Diagnostic {
    std::string code,stage="admit",severity="error",message;
    std::string sourceUri,sourcePointer,objectId,portId,regionId;
};
struct Progress {std::string stage;std::uint64_t completed=0,total=0;};
struct Message {
    std::string protocolVersion="0.1",kind="request",method="describe",status;
    Identity identity;
    std::string payloadJson="{}";
    std::vector<Diagnostic> diagnostics;
    std::optional<Progress> progress;
};
struct Error {std::string code,message,pointer;};
struct Format {std::string family,version;bool operator==(const Format &)const=default;};
struct Profile {std::string id,version;bool operator==(const Profile &)const=default;};
struct Target {
    std::string id,hardwareId,backendId;
    std::vector<std::string> profiles;
};
struct Description {
    std::string schemaVersion="0.2",bridgeId,protocolVersion="0.1";
    std::string softwareVersion,engineId;
    std::vector<Format> firFormats;
    std::vector<Profile> profiles;
    std::vector<std::string> operations,methods,typeFamilies,services,commands;
    // false means discovery is not a complete operation whitelist. Unknown
    // operations still require the same mandatory complete-graph validation;
    // their absence is neither availability nor incompatibility evidence.
    bool operationsExhaustive=true;
    std::vector<Target> targets;
    // Versioned constraints are descriptive; full-graph validate is mandatory.
    std::string constraintsJson="{}";
    Limits limits;
};
struct GraphRequirements {
    Format fir;
    std::vector<Profile> profiles;
    std::vector<std::string> operations;
    std::string sourceUri,documentId,firSha256,frontendFingerprint,contextSha256;
    std::string engineId,targetId,profileId;
    std::string provenanceJson="{}";
};
bool decodeMessage(std::string_view,Message &,Error &,const Limits & = {});
std::string encodeMessage(const Message &);
// Envelope compatibility is distinct from admission of a method's payload.
bool validatePayload(const Message &,Error &);
bool decodeDescription(std::string_view,Description &,Error &);
std::string encodeDescription(const Description &);
// Explicitly adapts admitted API3 provenance (0.3), not historical context0.1.
// Keeps full provenance; independently negotiates FIR family/version/profiles.
bool graphRequirements(std::string_view provenanceJson,std::string_view firJson,
    GraphRequirements &,Error &);
bool negotiate(const GraphRequirements &,const Description &,Error &);
// Stateful envelope admission. An obsolete tuple is discarded, not retargeted.
class Exchange {
public:
    explicit Exchange(Identity,std::string method,Limits = {});
    bool accept(const Message &,Error &);
    void cancel() noexcept;
    bool finished()const noexcept;
    bool cancelled()const noexcept;
    std::uint32_t discarded()const noexcept;
private:
    Identity identity_;std::string method_;Limits limits_;
    std::uint32_t progress_=0,discarded_=0;
    bool finished_=false,cancelled_=false;
};
}
