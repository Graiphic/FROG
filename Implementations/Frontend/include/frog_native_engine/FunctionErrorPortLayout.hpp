#pragma once

#include <algorithm>
#include <cmath>

namespace frog::native_engine {

// Presentation only: semantic port identities, order and type contracts stay
// untouched. Each layout owner supplies its own coordinate representation.
template<class Ports,class Id,class IsOutput,class Row>
void alignFunctionErrorPortRows(Ports &ports,Id id,IsOutput isOutput,Row row,
    double minimumSeparation=1e-9) {
    const auto input=std::find_if(ports.begin(),ports.end(),[&](const auto &p){
        return !isOutput(p) && id(p)=="error_in";
    });
    const auto output=std::find_if(ports.begin(),ports.end(),[&](const auto &p){
        return isOutput(p) && id(p)=="error_out";
    });
    if(input==ports.end() || output==ports.end()) return;
    const double target=row(*input),previous=row(*output);
    if(std::abs(target-previous)<1e-9) return;
    // Exchange the previous error row with a conflicting output. The
    // coordinate owner may reserve a real spacing margin: distinct fractions
    // alone can still project to the same selectable pixel at a small zoom.
    for(auto &port:ports)
        if(&port!=&*output && isOutput(port) && std::abs(row(port)-target)<(std::max)(1e-9,minimumSeparation))
            row(port)=previous;
    row(*output)=target;
}

} // namespace frog::native_engine
