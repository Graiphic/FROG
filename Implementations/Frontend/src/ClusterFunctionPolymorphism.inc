    if(isClusterFunction(id)) {
        const auto ports=clusterFunctionPorts(node);
        const auto current=std::find_if(ports.begin(),ports.end(),[&](const auto &p){return p.id==port;});
        if(current==ports.end()) return invalid("Cluster function terminal is outside the configured rows.");
        if(id=="cluster.bundle" && !wired("cluster")) {
            if(port=="cluster") return known(TypeRef::cluster({},{})); // Optional schema rail.
            const auto requested=clusterFunctionRow(port);
            if(requested) {auto value=input(port);value.complete=value.complete && wired(port);return value;}
            std::vector<std::string> ids;std::vector<TypeRef> types;
            bool complete=true;
            for(unsigned row=0;row<clusterFunctionCount(node);++row) {
                const auto name=clusterFunctionRowId(row);auto value=input(name);
                if(value.resolution.status==ResolutionStatus::Incompatible) return value;
                complete=complete && wired(name) && value.complete && value.resolution.type.known();
                ids.push_back(name);types.push_back(std::move(value.resolution.type));
            }
            auto value=known(TypeRef::cluster(std::move(ids),std::move(types)));
            value.complete=complete;return value;
        }
        if(isClusterRecordFunction(id)) {
            auto cluster=input("cluster");
            if(!cluster.resolution.accepted()) return cluster;
            const auto &schema=cluster.resolution.type;
            if(schema.kind!=TypeKind::Cluster || schema.children.empty()) return invalid("Connect a non-empty Cluster schema.");
            if(id=="cluster.unbundle" && schema.children.size()>32)
                return invalid("Positional Unbundle supports at most 32 complete fields; the schema is never truncated.");
            // The schema input must remain wireable so an invalid selection can be repaired.
            if(port=="cluster") return cluster;
            const bool clusterOutput=isClusterBundleFunction(id) && port=="result";
            const auto requestedRow=clusterFunctionRow(port);
            std::vector<const TypeRef *> fields(clusterFunctionCount(node),nullptr);
            for(unsigned i=0;i<clusterFunctionCount(node);++i) {
                // One unfinished row must not erase other rows' exact types or
                // prevent repairing their wires. The output keeps the input schema;
                // invalid replacements affect completeness and their own terminals.
                if(requestedRow && i!=*requestedRow) continue;
                const auto name=i<node.clusterFieldNames.size()?std::string_view(node.clusterFieldNames[i]):std::string_view{};
                const auto *field=clusterSelectedField(node,schema,i);
                if(!field) {
                    // An unconnected extra positional replacement is inert.
                    // Its own terminal remains invalid; By Name is unchanged.
                    if(clusterOutput && id=="cluster.bundle" && !wired(clusterFunctionRowId(i))) continue;
                    if(clusterOutput) {cluster.complete=false;continue;}
                    return invalid("Cluster field '"+std::string(name)+"' is missing, ambiguous or not selected (row "+std::to_string(i+1)+").");
                }
                fields[i]=field;
                if(isClusterBundleFunction(id) && wired(clusterFunctionRowId(i))) {
                    auto replacement=input(clusterFunctionRowId(i));
                    cluster.complete=cluster.complete && replacement.complete;
                    if(replacement.resolution.accepted()) {
                        const auto checked=connect(replacement.resolution.type,*field,ConnectionPolicy::NumericImplicit);
                        if(!checked.accepted()) {
                            if(clusterOutput) cluster.complete=false;
                            else {
                                auto diagnostic=checked;
                                diagnostic.message="Replacement for Cluster field '"+std::string(name)+"': "+diagnostic.message;
                                return {std::move(diagnostic),false,{}};
                            }
                        }
                    } else {
                        if(clusterOutput) cluster.complete=false;
                        else return replacement;
                    }
                }
                if(id=="cluster.bundle_by_name" && !wired(clusterFunctionRowId(i))) cluster.complete=false;
            }
            if(port=="result") return cluster;
            const auto row=clusterFunctionRow(port);
            return row?result(cluster,*fields[*row]):invalid("Unknown named field terminal.");
        }
        const bool zip=id=="cluster.index_build_array";
        TypeRef common;
        bool complete=true;
        std::vector<TypeRef> fields;
        std::vector<std::string> names;
        for(unsigned i=0;i<clusterFunctionCount(node);++i) {
            auto value=input(clusterFunctionRowId(i));
            complete=complete && wired(clusterFunctionRowId(i)) && value.complete;
            auto type=value.resolution.type;
            if(value.resolution.status==ResolutionStatus::Incompatible) return value;
            if(zip) {
                if(type.known() && (type.kind!=TypeKind::Array || type.rank!=1)) return invalid("Index & Bundle Cluster Array requires 1D Arrays.");
                fields.push_back(type.kind==TypeKind::Array?type.children.front():TypeRef{});
                names.push_back(clusterFunctionRowId(i));
            } else if(type.known()) {
                if(!common.known()) common=type;
                else {
                    const auto joined=commonType(common,type);
                    if(!joined.accepted()) return invalid("Build Cluster Array inputs must have one compatible type and shape.");
                    common=joined.type;
                }
            }
        }
        if(zip) {
            if(!current->output) {
                auto type=fields[*clusterFunctionRow(port)];
                if(!type.known()) type=TypeRef::primitive("f64");
                auto value=known(TypeRef::array(std::move(type)));value.complete=complete;return value;
            }
            auto value=known(TypeRef::array(TypeRef::cluster(std::move(names),std::move(fields))));value.complete=complete;return value;
        }
        if(!common.known()) common=TypeRef::primitive("f64"); // Authoring default, never an implicit runtime value.
        auto value=known(current->output?TypeRef::array(TypeRef::cluster({"element1"},{common})):common);value.complete=complete;return value;
    }
