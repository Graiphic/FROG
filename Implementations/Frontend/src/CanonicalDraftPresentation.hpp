#pragma once
#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/FrogIconPresentation.hpp"
#include "frog_native_engine/CanonicalStringNative.hpp"
#include "frog_native_engine/DiagramCommentAttachment.hpp"
#include "frog_native_engine/DiagramCommentTextRuns.hpp"
#include "frog_native_engine/DocumentTextDefaults.hpp"
#include <algorithm>
#include <initializer_list>

// Private presentation is retained byte-for-byte in authored-source provenance.
// It does not manufacture executable nodes or connector evidence.
namespace frog::native_engine::draft_presentation {
using namespace canonical_json;
inline constexpr std::string_view provenance =
    "/ide/recoverability/graiphic_studio_migration/preserved_authoring_source";
template<class Reject> void fields(const Json &value,
    std::initializer_list<std::string_view> names,const std::string &path,Reject reject) {
    if(value.type!=Json::Type::Object)reject(path,"Presentation requires an explicit object.");
    for(const auto &[key,_]:value.object)
        if(std::find(names.begin(),names.end(),key)==names.end())
            reject(pointer(path,key),"An unknown presentation field cannot conceal executable state.");
}
template<class Reject> std::string text(const Json *value,const std::string &path,
    std::size_t limit,bool allowEmpty,Reject reject) {
    if(!value||value->type!=Json::Type::String||(!allowEmpty&&value->string.empty())||
       value->string.size()>limit||value->string.find('\0')!=std::string::npos||
       !canonicalStringUtf8Valid(value->string))reject(path,"Presentation text must be bounded UTF-8.");
    return value->string;
}
template<class Reject> double number(const Json *value,const std::string &path,Reject reject) {
    if(!value||!typedScalar(*value,"f64"))reject(path,"Presentation geometry must be finite.");
    return std::stod(value->string);
}
template<class Reject> void icon(const Json &value,Reject reject) {
    fields(value,{"svg"},"/icon",reject);
    const auto *svg=value.member("svg");
    if(!svg||svg->type!=Json::Type::String)
        reject("/icon/svg","Icon SVG requires a string.");
    if(const auto error=frogIconPresentationError(svg->string);!error.empty())
        reject("/icon/svg",std::string(error));
}
template<class Reject> void literalText(const Json &node,std::string_view type,const std::string &path,Reject reject) {
    const auto metadata=[&](const char *name,bool pathOptions) {
        const auto *raw=node.member(name);if(!raw)return;
        const auto at=path+"/"+name;
        if(raw->type!=Json::Type::Object)reject(at,"Literal presentation metadata must be an explicit object.","migration.record_presentation_invalid");
        for(const auto &[key,v]:raw->object) {
            const auto member=at+"/"+key;
            const bool boolean=pathOptions?(key=="folder_visible"||key=="browse_button_visible"||key=="scrollbar_visible"||key=="size_to_text"):
                (key=="display_style_visible"||key=="limit_to_single_line"||key=="wrap_at_word_breaks"||key=="update_while_typing"||key=="vertical_scrollbar_visible"||key=="horizontal_scrollbar_visible"||key=="size_to_text");
            if(boolean) {if(v.type!=Json::Type::Boolean)reject(member,"The writer's literal presentation flag must be Boolean.","migration.record_presentation_invalid");continue;}
            const bool text=pathOptions?(key=="overflow_mode"||key=="browse_prompt"||key=="browse_pattern_label"||key=="browse_pattern"||key=="browse_selection_mode"||key=="browse_existence_mode"||key=="browse_start_path"):
                (key=="display_style"||key=="text_alignment");
            if(!text)reject(member,"Unknown literal presentation fields are not silently erased during semantic migration.","migration.unsupported");
            if(v.type!=Json::Type::String)reject(member,"The writer's literal presentation text must be a string.","migration.record_presentation_invalid");
            const auto &x=v.string;
            if((key=="display_style"&&x!="normal"&&x!="backslash_codes"&&x!="password"&&x!="hexadecimal")||
               (key=="text_alignment"&&x!="left"&&x!="center"&&x!="right")||
               (key=="overflow_mode"&&x!="abbreviate"&&x!="clip")||
               (key=="browse_selection_mode"&&x!="files"&&x!="folders"&&x!="files_or_folders")||
               (key=="browse_existence_mode"&&x!="existing"&&x!="new"&&x!="new_or_existing"))
                reject(member,"Literal presentation enum is outside the actual writer/reader contract.","migration.record_presentation_invalid");
        }
    };
    metadata("string_constant",false);if(type=="path")metadata("path_constant",true);
    // Presentation/editing options do not reinterpret literal text. The exact
    // original Source retains them; the immutable Record carries only data.
}

template<class Reject> void wireLabel(const Json &value,const std::string &path,Reject reject) {
    fields(value,{"text","normalized_position","offset_x","offset_y"},path,reject);
    text(value.member("text"),path+"/text",4096,true,reject);
    // Omitted geometry uses the document reader's presentation defaults.
    for(const auto key:{"normalized_position","offset_x","offset_y"})
        if(const auto *coordinate=value.member(key)) {
            const auto n=number(coordinate,path+"/"+key,reject);
            if(std::string_view(key)=="normalized_position"&&(n<0||n>1))
                reject(path+"/"+key,"A wire label must retain its normalized anchor.");
        }
}

template<class Reject> void comment(const Json &value,const Json &scope,
    const std::string &path,Reject reject) {
    fields(value,{"id","kind","comment_auto_size","comment_attachment","comment_text_runs","text",
        "vertical_arrangement","font","hidden","locked","group","layout"},path,reject);
    const auto id=text(value.member("id"),path+"/id",1024,false,reject);
    if(text(value.member("kind"),path+"/kind",32,false,reject)!="comment")
        reject(path,"A presentation comment must have its actual comment kind.");
    const auto content=text(value.member("text"),path+"/text",1'048'576,true,reject);
    if(const auto *runs=value.member("comment_text_runs")) {
        const auto at=path+"/comment_text_runs";
        if(runs->type!=Json::Type::Array)reject(at,"Comment formatting must be an array of text ranges.");
        const auto boundaries=comment_text_internal::scalarBoundaries(content);
        const auto textLength=boundaries.back();
        if(runs->array.size()>textLength)reject(at,"Comment formatting exceeds the authored text length.");
        std::size_t previousEnd=0;
        for(std::size_t i=0;i<runs->array.size();++i) {
            const auto entry=at+"/"+std::to_string(i);const auto &run=runs->array[i];
            fields(run,{"start_utf16","length_utf16","format"},entry,reject);
            const auto index=[&](const char *key) {
                const auto *n=run.member(key);
                if(!n||!typedScalar(*n,"u64"))reject(entry+"/"+key,"A comment range requires a nonnegative integer.");
                const auto result=std::stoull(n->string);
                if(result>textLength)reject(entry+"/"+key,"A comment range exceeds the authored text.");
                return static_cast<std::size_t>(result);
            };
            const auto start=index("start_utf16"),length=index("length_utf16");
            if(!length||start<previousEnd||length>textLength-start||
                !std::binary_search(boundaries.begin(),boundaries.end(),start)||
                !std::binary_search(boundaries.begin(),boundaries.end(),start+length))
                reject(entry,"Comment ranges must be ordered, non-overlapping and preserve Unicode characters.");
            previousEnd=start+length;
            const auto *format=run.member("format");
            if(!format)reject(entry+"/format","A comment range must retain its format record.");
            const auto fmt=entry+"/format";
            fields(*format,{"family","style","point_size","text_rgb","use_theme_text_color"},fmt,reject);
            const auto family=text(format->member("family"),fmt+"/family",256,true,reject);
            if(!family.empty()&&!validDocumentTextFontFamily(family))reject(fmt+"/family","Invalid authored font family.");
            text(format->member("style"),fmt+"/style",128,false,reject);
            if(!validDocumentTextPointSize(number(format->member("point_size"),fmt+"/point_size",reject)))
                reject(fmt+"/point_size","Comment typography requires its actual 6..144pt size.");
            if(const auto *flag=format->member("use_theme_text_color");!flag||flag->type!=Json::Type::Boolean)
                reject(fmt+"/use_theme_text_color","The authored theme color flag must be Boolean.");
            if(const auto *color=format->member("text_rgb");color&&
                (!typedScalar(*color,"u32")||std::stoull(color->string)>0xffffff))
                reject(fmt+"/text_rgb","A presentation color must be an RGB value.");
        }
    }
    if(const auto *group=value.member("group"))text(group,path+"/group",1024,true,reject);
    for(const auto key:{"hidden","locked","comment_auto_size"})
        if(const auto *flag=value.member(key);flag&&flag->type!=Json::Type::Boolean)
            reject(path+"/"+key,"A presentation flag must be Boolean.");
    const auto arrangement=text(value.member("vertical_arrangement"),path+"/vertical_arrangement",32,false,reject);
    if(arrangement!="none"&&arrangement!="stacked"&&arrangement!="clockwise"&&arrangement!="counterclockwise")
        reject(path+"/vertical_arrangement","The comment arrangement is not an authored presentation mode.");
    const auto *layout=value.member("layout");
    if(!layout)reject(path+"/layout","A comment must retain its actual authored frame.");
    fields(*layout,{"x","y","width","height"},path+"/layout",reject);
    for(const auto key:{"x","y","width","height"}) {
        const auto n=number(layout->member(key),path+"/layout/"+key,reject);
        if((std::string_view(key)=="width"||std::string_view(key)=="height")&&n<=0)
            reject(path+"/layout/"+key,"Comment dimensions must be positive.");
    }
    const auto *font=value.member("font");
    if(!font)reject(path+"/font","A comment must retain its authored font record.");
    fields(*font,{"family","style","use_default_size","geometry_initialized","use_theme_text_color","point_size","text_rgb","background_rgb"},path+"/font",reject);
    const auto family=text(font->member("family"),path+"/font/family",256,true,reject);
    if(!family.empty()&&!validDocumentTextFontFamily(family))reject(path+"/font/family","Invalid authored font family.");
    text(font->member("style"),path+"/font/style",128,false,reject);
    if(!validDocumentTextPointSize(number(font->member("point_size"),path+"/font/point_size",reject)))
        reject(path+"/font/point_size","Comment typography requires its actual 6..144pt size.");
    for(const auto key:{"use_default_size","geometry_initialized"})
        if(const auto *flag=font->member(key);!flag||flag->type!=Json::Type::Boolean)
            reject(path+"/font/"+key,"The authored font flag must be Boolean.");
    // New comments explicitly record theme inheritance. Older documents omit
    // this presentation-only flag; neither form changes executable state.
    if(const auto *flag=font->member("use_theme_text_color");flag&&flag->type!=Json::Type::Boolean)
        reject(path+"/font/use_theme_text_color","The authored theme color flag must be Boolean.");
    for(const auto key:{"text_rgb","background_rgb"})if(const auto *color=font->member(key)) {
        if(!typedScalar(*color,"u32")||std::stoull(color->string)>0xffffff)
            reject(path+"/font/"+key,"A presentation color must be an RGB value.");
    }
    if(const auto *attachment=value.member("comment_attachment")) {
        fields(*attachment,{"kind","target_id","node_x","node_y","wire_position"},path+"/comment_attachment",reject);
        const auto kind=diagramCommentTargetKind(text(attachment->member("kind"),path+"/comment_attachment/kind",32,false,reject));
        if(!kind)reject(path+"/comment_attachment/kind","Unknown comment attachment kind.");
        text(attachment->member("target_id"),path+"/comment_attachment/target_id",1024,false,reject);
        for(const auto key:{"node_x","node_y","wire_position"}) {
            const auto n=number(attachment->member(key),path+"/comment_attachment/"+key,reject);
            if(n<0||n>1)reject(path+"/comment_attachment/"+key,"An attachment must retain its normalized anchor.");
        }
        // Attachment identities are document-wide editor metadata: a free
        // comment may point into another structure region. The document reader
        // resolves them against the complete model and detaches missing targets.
        // Neither a cross-region nor a stale annotation creates an executable
        // dependency. Keep validating its schema, never require a dataflow edge
        // or a same-scope target merely to run the unchanged program.
    }
    // Comments deliberately have no ports. Reject wiring explicitly instead
    // of allowing presentation to masquerade as a value-producing node.
    if(const auto *edges=scope.member("edges");edges&&edges->type==Json::Type::Array)
        for(const auto &edge:edges->array)for(const auto side:{"from","to"})
            if(const auto *endpoint=edge.member(side))if(const auto *node=endpoint->member("node");
                node&&node->type==Json::Type::String&&node->string==id)
                reject(path,"A presentation comment cannot participate in executable wiring.");
}
}
