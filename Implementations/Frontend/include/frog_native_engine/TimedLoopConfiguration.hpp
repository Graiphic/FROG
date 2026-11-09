#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <charconv>
#include <stdexcept>
#include <algorithm>

namespace frog::native_engine::timed_loop {
inline constexpr std::string_view profile="frog.studio.timed-loop@1";
using Json=source::JsonValue;
inline Json object(){Json v;v.type=Json::Type::Object;return v;}
inline Json text(std::string v){Json j;j.type=Json::Type::String;j.string=std::move(v);return j;}
inline Json integer(std::int64_t v){Json j;j.type=Json::Type::Number;j.string=std::to_string(v);j.number=double(v);return j;}
inline const Json &member(const Json &j,std::string_view key){const auto *v=j.member(key);if(j.type!=Json::Type::Object||!v)throw std::runtime_error("Timed Loop: missing "+std::string(key));return *v;}
inline std::string string(const Json &j){if(j.type!=Json::Type::String)throw std::runtime_error("Timed Loop: expected text");return j.string;}
inline std::int64_t number(const Json &j){
 if(j.type!=Json::Type::Number)throw std::runtime_error("Timed Loop: expected exact signed 64-bit integer");
 const auto lexeme=numeric_value::canonicalWideIntegerLexeme(j.string,"i64");
 if(!lexeme)throw std::runtime_error("Timed Loop: expected exact signed 64-bit integer");
 std::int64_t value=0;const auto r=std::from_chars(lexeme->data(),lexeme->data()+lexeme->size(),value);
 if(r.ec!=std::errc{}||r.ptr!=lexeme->data()+lexeme->size())throw std::runtime_error("Timed Loop: signed tick value is out of range");return value;
}
inline void keys(const Json &j,std::initializer_list<std::string_view> allowed){if(j.type!=Json::Type::Object)throw std::runtime_error("Timed Loop: expected object");for(const auto &[k,v]:j.object)if(std::find(allowed.begin(),allowed.end(),k)==allowed.end())throw std::runtime_error("Timed Loop: unknown field "+k);}
inline Json scheduleJson(const DiagramTimedLoopSchedule &s){auto j=object();auto policy=object();policy.object={{"missed_periods",text(s.missedPeriods)},{"phase",text(s.phasePolicy)}};
 j.object={{"timing_source",text(s.timingSource)},{"period",integer(s.period)},{"offset",integer(s.offset)},{"deadline",integer(s.deadline)},{"priority",integer(s.priority)},{"processor",integer(s.processor)},{"timeout",integer(s.timeout)},{"late_iteration_policy",std::move(policy)}};return j;}
inline DiagramTimedLoopSchedule readSchedule(const Json &j){keys(j,{"timing_source","period","offset","deadline","priority","processor","timeout","late_iteration_policy"});DiagramTimedLoopSchedule s;
 s.timingSource=string(member(j,"timing_source"));s.period=number(member(j,"period"));s.offset=number(member(j,"offset"));s.deadline=number(member(j,"deadline"));s.priority=number(member(j,"priority"));s.processor=number(member(j,"processor"));s.timeout=number(member(j,"timeout"));const auto &p=member(j,"late_iteration_policy");keys(p,{"missed_periods","phase"});s.missedPeriods=string(member(p,"missed_periods"));s.phasePolicy=string(member(p,"phase"));return s;}
inline bool deadlineMode(std::string_view m){return m=="period"||m=="none"||m=="custom";}
inline bool nextField(std::string_view id){return id=="period"||id=="offset"||id=="deadline"||id=="timeout"||id=="priority"||id=="processor"||id=="mode";}
inline Json optionsJson(const DiagramTimedLoopOptions &o){auto j=object();Json enabled;enabled.type=Json::Type::Array;for(const auto &id:o.nextEnabled)enabled.array.push_back(text(id));
 j.object={{"profile",text(std::string(profile))},{"deadline_mode",text(o.deadlineMode)},{"next_deadline_mode",text(o.nextDeadlineMode)},{"next_enabled",std::move(enabled)},{"next",scheduleJson(o.next)}};return j;}
inline DiagramTimedLoopOptions readOptions(const Json &j){keys(j,{"profile","deadline_mode","next_deadline_mode","next_enabled","next"});if(string(member(j,"profile"))!=profile)throw std::runtime_error("Timed Loop: unsupported options profile");DiagramTimedLoopOptions o;o.deadlineMode=string(member(j,"deadline_mode"));o.nextDeadlineMode=string(member(j,"next_deadline_mode"));if(!deadlineMode(o.deadlineMode)||!deadlineMode(o.nextDeadlineMode))throw std::runtime_error("Timed Loop: unknown deadline mode");
 const auto &a=member(j,"next_enabled");if(a.type!=Json::Type::Array)throw std::runtime_error("Timed Loop: expected next field list");for(const auto &v:a.array){auto id=string(v);if(!nextField(id)||std::find(o.nextEnabled.begin(),o.nextEnabled.end(),id)!=o.nextEnabled.end())throw std::runtime_error("Timed Loop: unknown or repeated next field");o.nextEnabled.push_back(std::move(id));}o.next=readSchedule(member(j,"next"));return o;}
inline std::string validate(const DiagramTimedLoopSchedule &s,std::string_view deadline){
 if(s.timingSource.empty())return "Timing Source: choose a source.";
 if(s.period<=0)return "Period: must be greater than zero.";
 if(s.offset<0)return "Offset: must be non-negative.";
 if(!deadlineMode(deadline)||(deadline=="custom"&&s.deadline<=0))return "Deadline: choose a positive custom duration.";
 if(s.timeout < -1)return "Timeout: use Infinite or a non-negative duration.";
 if(s.priority<1||s.priority>65535)return "Priority: expected an integer from 1 to 65535.";
 if(s.processor < -1)return "Processor: use Automatic or a non-negative index.";
 if(s.missedPeriods!="discard"&&s.missedPeriods!="execute")return "Late policy: choose Discard or Catch up.";
 if(s.phasePolicy!="maintain"&&s.phasePolicy!="reset")return "Late policy: choose Maintain or Reset phase.";
 return {};}
} // namespace frog::native_engine::timed_loop
