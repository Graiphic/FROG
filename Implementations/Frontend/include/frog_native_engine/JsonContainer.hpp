#pragma once
#include <memory>
#include <initializer_list>
#include <utility>

namespace frog::native_engine::source {
// A JSON scalar owns no empty map/vector allocations or container headers.
// Tree copies share immutable branches; every mutable access detaches that
// container before exposing references. Writers never mutate another snapshot.
template<class Container> class JsonContainer {
    std::shared_ptr<Container> owner_;
    bool mutableAliases_=false;
    const Container &read() const noexcept {static const Container empty;return owner_?*owner_:empty;}
    static Container &emptyRange() noexcept {static Container empty;return empty;}
    Container &write() {
        if(!owner_)owner_=std::make_shared<Container>();
        else if(owner_.use_count()!=1)owner_=std::make_shared<Container>(*owner_);
        mutableAliases_=true;
        return *owner_;
    }
public:
    using value_type=typename Container::value_type;
    using iterator=typename Container::iterator;
    using const_iterator=typename Container::const_iterator;
    JsonContainer()=default;
    JsonContainer(const JsonContainer &other):owner_(other.owner_&&other.mutableAliases_?std::make_shared<Container>(*other.owner_):other.owner_){}
    JsonContainer(JsonContainer &&) noexcept=default;
    JsonContainer &operator=(const JsonContainer &other){if(this!=&other){JsonContainer copy(other);swap(copy);}return *this;}
    JsonContainer &operator=(JsonContainer &&) noexcept=default;
    JsonContainer(Container value):owner_(std::make_shared<Container>(std::move(value))),mutableAliases_(true){}
    JsonContainer(std::initializer_list<value_type> values):JsonContainer(Container(values)){}
    JsonContainer &operator=(Container value){owner_=std::make_shared<Container>(std::move(value));mutableAliases_=true;return *this;}
    JsonContainer &operator=(std::initializer_list<value_type> values){return *this=Container(values);}
    operator const Container &() const noexcept{return read();}
    operator Container &(){return write();}
    bool empty()const noexcept{return read().empty();}
    auto size()const noexcept{return read().size();}
    bool shared()const noexcept{return owner_.use_count()>1;}
    // Parser-only boundary: all temporary mutable references have expired.
    void sealParsedContainer() noexcept {mutableAliases_=false;}
    const void *storageIdentity()const noexcept{return owner_.get();}
    void clear()noexcept{owner_.reset();mutableAliases_=false;}
    void swap(JsonContainer &other)noexcept{owner_.swap(other.owner_);std::swap(mutableAliases_,other.mutableAliases_);}
    auto begin()const noexcept{return read().begin();} auto end()const noexcept{return read().end();}
    auto cbegin()const noexcept{return read().cbegin();} auto cend()const noexcept{return read().cend();}
    auto begin(){return empty()?emptyRange().begin():write().begin();} auto end(){return empty()?emptyRange().end():write().end();}
    auto rbegin()const noexcept{return read().rbegin();} auto rend()const noexcept{return read().rend();}
    auto rbegin(){return empty()?emptyRange().rbegin():write().rbegin();} auto rend(){return empty()?emptyRange().rend():write().rend();}
    template<class Key> decltype(auto) operator[](Key &&key){return write()[std::forward<Key>(key)];}
    template<class Key> decltype(auto) operator[](Key &&key)const{return read()[std::forward<Key>(key)];}
    template<class Key> decltype(auto) at(const Key &key){return write().at(key);}
    template<class Key> decltype(auto) at(const Key &key)const{return read().at(key);}
    template<class Key> auto find(const Key &key){return empty()?emptyRange().end():write().find(key);}
    template<class Key> auto find(const Key &key)const{return read().find(key);}
    template<class Key> bool contains(const Key &key)const{return read().contains(key);}
    template<class... Args> auto emplace(Args&&... args){return write().emplace(std::forward<Args>(args)...);}
    template<class... Args> auto try_emplace(Args&&... args){return write().try_emplace(std::forward<Args>(args)...);}
    template<class... Args> decltype(auto) emplace_back(Args&&... args){return write().emplace_back(std::forward<Args>(args)...);}
    template<class... Args> auto insert(Args&&... args){return write().insert(std::forward<Args>(args)...);}
    auto insert(const value_type &value){return write().insert(value);}
    auto insert(value_type &&value){return write().insert(std::move(value));}
    auto insert(std::initializer_list<value_type> values){return write().insert(values);}
    auto insert(const_iterator position,const value_type &value){return write().insert(position,value);}
    auto insert(const_iterator position,std::initializer_list<value_type> values){return write().insert(position,values);}
    template<class... Args> auto insert_or_assign(Args&&... args){return write().insert_or_assign(std::forward<Args>(args)...);}
    template<class... Args> auto erase(Args&&... args){return write().erase(std::forward<Args>(args)...);}
    template<class T> void push_back(T&& value){write().push_back(std::forward<T>(value));}
    void push_back(const value_type &value){write().push_back(value);}
    void push_back(value_type &&value){write().push_back(std::move(value));}
    void pop_back(){write().pop_back();}
    void reserve(std::size_t n){if(n)write().reserve(n);}
    void resize(std::size_t n){write().resize(n);}
    auto capacity()const noexcept{return read().capacity();}
    decltype(auto) front(){return write().front();} decltype(auto) back(){return write().back();}
    decltype(auto) front()const{return read().front();} decltype(auto) back()const{return read().back();}
    auto data(){return write().data();} auto data()const noexcept{return read().data();}
    friend bool operator==(const JsonContainer &a,const JsonContainer &b){return a.owner_==b.owner_||a.read()==b.read();}
};
}
