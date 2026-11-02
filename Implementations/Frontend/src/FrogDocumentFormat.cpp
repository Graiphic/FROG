#include "frog_native_engine/FrogDocumentFormat.hpp"

#include <cmath>
#include <charconv>
#include <limits>
#include <string>

namespace frog::native_engine {

bool validFrogStudioFileVersion(std::string_view version) noexcept {
    if(version.empty()||version.size()>64)return false;unsigned count=0;
    for(std::size_t pos=0;pos<version.size();) {
        const auto end=version.find('.',pos);const auto token=version.substr(pos,end==version.npos?version.size()-pos:end-pos);
        if(token.empty()||(token.size()>1&&token.front()=='0'))return false;
        std::uint32_t component=0;const auto converted=std::from_chars(token.data(),token.data()+token.size(),component);
        if(converted.ec!=std::errc{}||converted.ptr!=token.data()+token.size())return false;
        ++count;if(end==version.npos)break;pos=end+1;if(pos==version.size())return false;
    }
    return count==4;
}

std::optional<FrogDraftRevisionResolution> resolveFrogDraftRevision(
    FrogDraftRevisionField field,
    std::string *error) {
    if (error != nullptr) {
        error->clear();
    }

    if (field.kind == FrogDraftRevisionFieldKind::Missing) {
        return FrogDraftRevisionResolution{
            kLegacyFrogDraftRevision,
            kLegacyFrogDraftRevision != kCurrentFrogDraftRevision
        };
    }

    if (field.kind != FrogDraftRevisionFieldKind::Number ||
        !std::isfinite(field.number) ||
        field.number < 0.0 ||
        std::trunc(field.number) != field.number ||
        field.number > static_cast<double>((std::numeric_limits<std::uint32_t>::max)())) {
        if (error != nullptr) {
            *error = "FROG draft_revision must be a non-negative integer.";
        }
        return std::nullopt;
    }

    const auto revision = static_cast<std::uint32_t>(field.number);
    if (revision > kMaximumSupportedFrogDraftRevision) {
        if (error != nullptr) {
            *error = "Unsupported FROG draft_revision " + std::to_string(revision) +
                "; maximum supported revision is " + std::to_string(kMaximumSupportedFrogDraftRevision) + ".";
        }
        return std::nullopt;
    }

    return FrogDraftRevisionResolution{
        revision,
        revision < kCurrentFrogDraftRevision
    };
}

} // namespace frog::native_engine
