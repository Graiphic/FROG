// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/graph/XYGraph.hpp"
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>
#include "frog_native_engine/graph/XYGraphDataHelpers.hpp"
#include <chrono>
namespace frog::native_engine::xy {
namespace data_detail {
constexpr double ln10=2.302585092994045684;
Coordinate logOrigin(const AxisRange&r){return r.origin.positive()?r.origin:Coordinate(1.0);}

Coordinate unprojected(const Axis&a,const AxisRange&r,double v)noexcept{if(a.mapping==Mapping::Logarithmic){auto o=logOrigin(r);const double delta=o.approximate()*std::expm1(v*ln10);if(std::isfinite(delta))return shifted(o,delta);return Coordinate(std::exp(std::log(o.approximate())+v*ln10));}return shifted(r.origin,v*r.scale);}

std::optional<double> timeUnit(std::string_view unit){if(unit=="s")return 1;if(unit=="ms")return .001;if(unit=="us")return 1e-6;if(unit=="ns")return 1e-9;if(unit=="min")return 60;if(unit=="h")return 3600;if(unit=="d")return 86400;return {};}

std::optional<std::int64_t> timeOrigin(std::string_view text){using namespace std::chrono;if(text=="FROG epoch")text="1904-01-01T00:00:00Z";if(text.size()!=20||text[4]!='-'||text[7]!='-'||text[10]!='T'||text[13]!=':'||text[16]!=':'||text[19]!='Z')return {};const auto part=[&](std::size_t first,std::size_t n)->int{int value=0;auto r=std::from_chars(text.data()+first,text.data()+first+n,value);return r.ec==std::errc{}&&r.ptr==text.data()+first+n?value:-1;};const int yr=part(0,4),mon=part(5,2),dy=part(8,2),hr=part(11,2),min=part(14,2),sec=part(17,2);const year_month_day date{year{yr},month{unsigned(mon)},day{unsigned(dy)}};if(yr<1||yr>9999||!date.ok()||hr<0||hr>23||min<0||min>59||sec<0||sec>59)return {};return duration_cast<seconds>(sys_days(date).time_since_epoch()).count()+hr*3600+min*60+sec;}

std::optional<int> timeZone(std::string_view text){if(text=="UTC"||text=="Z")return 0;if(text=="local"||text=="Local")return 100000;if(text.size()!=9||!text.starts_with("UTC")||(text[3]!='+'&&text[3]!='-')||text[6]!=':')return {};int hr=0,min=0;auto h=std::from_chars(text.data()+4,text.data()+6,hr),m=std::from_chars(text.data()+7,text.data()+9,min);if(h.ec!=std::errc{}||h.ptr!=text.data()+6||m.ec!=std::errc{}||m.ptr!=text.data()+9||hr>23||min>59)return {};return (hr*3600+min*60)*(text[3]=='-'?-1:1);}
bool utf8(std::string_view text){
    for(std::size_t i=0;i<text.size();){
        const auto lead=static_cast<unsigned char>(text[i++]);
        if(lead<0x80)continue;
        unsigned n=0,code=0,minimum=0;
        if(lead>=0xC2&&lead<=0xDF){n=1;code=lead&31;minimum=0x80;}
        else if(lead>=0xE0&&lead<=0xEF){n=2;code=lead&15;minimum=0x800;}
        else if(lead>=0xF0&&lead<=0xF4){n=3;code=lead&7;minimum=0x10000;}
        else return false;
        if(n>text.size()-i)return false;
        for(unsigned j=0;j<n;++j){const auto c=static_cast<unsigned char>(text[i++]);if((c&0xC0)!=0x80)return false;code=(code<<6)|(c&63);}
        if(code<minimum||code>0x10FFFF||(code>=0xD800&&code<=0xDFFF))return false;
    }
    return true;
}

bool finiteRect(Rect r){return std::isfinite(r.x)&&std::isfinite(r.y)&&std::isfinite(r.width)&&std::isfinite(r.height)&&r.width>0&&r.height>0&&std::isfinite(r.x+r.width)&&std::isfinite(r.y+r.height);}

}
using namespace data_detail;
std::string validateAxisTextFormat(const Axis&a){if(a.format!=NumberFormat::Duration&&a.format!=NumberFormat::DateTime)return {};if(!timeUnit(a.timeUnit))return "XY time unit must be s, ms, us, ns, min, h or d.";if(a.format==NumberFormat::DateTime&&(!timeOrigin(a.timeOrigin)||!timeZone(a.timeZone)))return "XY date/time needs an ISO UTC origin and UTC, local or fixed UTC+/-hh:mm zone.";return {};}

bool validRange(const Axis&a,const AxisRange&r)noexcept{if(!r.origin.finite()||!std::isfinite(r.scale)||r.scale<=0||!std::isfinite(r.offsets.minimum)||!std::isfinite(r.offsets.maximum)||r.offsets.minimum>=r.offsets.maximum||!std::isfinite(r.offsets.maximum-r.offsets.minimum))return false;const auto lo=unprojected(a,r,r.offsets.minimum),hi=unprojected(a,r,r.offsets.maximum);return lo.finite()&&hi.finite()&&std::isfinite(lo.approximate())&&std::isfinite(hi.approximate())&&(a.mapping!=Mapping::Logarithmic||(lo.positive()&&hi.positive()));}
std::string validateDecorations(const Design&d){
    if(d.decorations.size()>maximumDecorations)return "XY supports at most 16 decorations.";
    std::set<std::string>ids;std::size_t total=0;
    for(const auto&v:d.decorations){
        if(v.id.empty()||v.id.size()>256||!utf8(v.id)||!ids.insert(v.id).second)return "XY decoration IDs require unique bounded UTF-8 identities.";
        if(v.xAxis.size()>256||v.yAxis.size()>256||v.disabledReason.size()>2048||!utf8(v.xAxis)||!utf8(v.yAxis)||!utf8(v.disabledReason))return "XY decoration text exceeds its UTF-8 metadata budget.";
        if(unsigned(v.layer)>2||!finiteRect(v.pixelRect)||!v.firstX.finite()||!v.firstY.finite()||!v.secondX.finite()||!v.secondY.finite())return "XY decoration has invalid geometry or layer.";
        const auto*x=findAxis(d,v.xAxis),*y=findAxis(d,v.yAxis);
        if(!x||!y||!x->horizontal||y->horizontal)return "XY decoration scale associations are unavailable.";
        if(!v.image||v.image->width==0||v.image->height==0||v.image->width>4096||v.image->height>4096)return "XY decoration requires a valid native Picture Display image.";
        const auto pixels=std::size_t(v.image->width)*v.image->height;
        if(pixels>maximumDecorationPixels-total)return "XY decorations exceed the shared 1,000,000-pixel image budget.";
        if(!v.image->pixels.empty()&&v.image->pixels.size()!=pixels)return "XY decoration native pixel dimensions and buffer length disagree.";
        total+=pixels;
    }
    return {};
}
bool Decoration::operator==(const Decoration&o)const{
    const bool sameImage=image==o.image||(image&&o.image&&image->width==o.image->width&&image->height==o.image->height&&image->pixels==o.image->pixels);
    return id==o.id&&layer==o.layer&&visible==o.visible&&dataCoordinates==o.dataCoordinates&&xAxis==o.xAxis&&yAxis==o.yAxis&&pixelRect.x==o.pixelRect.x&&pixelRect.y==o.pixelRect.y&&pixelRect.width==o.pixelRect.width&&pixelRect.height==o.pixelRect.height&&firstX==o.firstX&&firstY==o.firstY&&secondX==o.secondX&&secondY==o.secondY&&disabledReason==o.disabledReason&&sameImage;
}
}
