#pragma once

#include "frog_native_engine/ArrayCapacity.hpp"
#include "frog_native_engine/SharedBuffer.hpp"

#include <bit>
#include <charconv>
#include <cstring>
#include <iterator>
#include <map>
#include <optional>

namespace frog::native_engine {

// Exact authoring cells. Ordinary numeric cells use fixed-width pages.
// Unallocated pages represent the canonical zero. Noncanonical spellings
// (including incomplete edits) are retained verbatim on their own page.
class DiagramArrayValues {
    static constexpr std::size_t cellsPerPage = 4096;
    enum class Representation : std::uint8_t { Text, I8, U8, I16, U16, I32, U32, I64, U64, F32, F64 };
    struct Page {
        std::vector<std::uint8_t> bytes;
        std::vector<std::uint16_t> shorts;
        std::vector<std::uint32_t> narrow;
        std::vector<std::uint64_t> wide;
        std::map<std::size_t, std::string> text;
    };
    struct Storage {
        Representation representation;
        std::size_t count;
        std::vector<std::shared_ptr<Page>> pages;
    };

public:
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using reference = std::string;
        using pointer = void;

        const_iterator() = default;
        reference operator*() const { return (*owner_)[index_]; }
        const_iterator &operator++() { ++index_; return *this; }
        const_iterator operator++(int) { auto before = *this; ++*this; return before; }
        friend bool operator==(const const_iterator &, const const_iterator &) = default;

    private:
        friend class DiagramArrayValues;
        const_iterator(const DiagramArrayValues *owner, std::size_t index) : owner_(owner), index_(index) {}
        const DiagramArrayValues *owner_ = nullptr;
        std::size_t index_ = 0;
    };

    DiagramArrayValues() = default;
    DiagramArrayValues(std::vector<std::string> values) : text_(std::move(values)) {}
    DiagramArrayValues(std::initializer_list<std::string> values) : text_(values) {}
    DiagramArrayValues &operator=(std::vector<std::string> values) {
        text_ = std::move(values);
        numeric_.reset();
        return *this;
    }
    DiagramArrayValues &operator=(std::initializer_list<std::string> values) {
        return *this = std::vector<std::string>(values);
    }
    void assign(std::size_t count, const std::string &value) {
        *this = std::vector<std::string>(count, value);
    }
    void clear() noexcept { text_.clear(); numeric_.reset(); }
    void swap(DiagramArrayValues &other) noexcept {
        text_.swap(other.text_);
        numeric_.swap(other.numeric_);
    }
    [[nodiscard]] std::size_t size() const noexcept { return numeric_ ? numeric_->count : text_.size(); }
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    [[nodiscard]] std::size_t capacity() const noexcept { return numeric_ ? size() : text_.capacity(); }
    [[nodiscard]] std::string operator[](std::size_t index) const {
        if (!numeric_) return text_[index];
        const auto &page = numeric_->pages[index / cellsPerPage];
        if (!page) return "0";
        const auto offset = index % cellsPerPage;
        if (const auto spelling = page->text.find(offset); spelling != page->text.end())
            return spelling->second;
        return format(numeric_->representation, pageWord(*page, offset));
    }
    [[nodiscard]] std::string front() const { return (*this)[0]; }
    [[nodiscard]] std::string back() const { return (*this)[size() - 1]; }
    [[nodiscard]] const_iterator begin() const noexcept { return {this, 0}; }
    [[nodiscard]] const_iterator end() const noexcept { return {this, size()}; }
    [[nodiscard]] std::vector<std::string> values() const {
        return numeric_ ? std::vector<std::string>(begin(), end()) : text_.values();
    }
    template<class Edit> void update(Edit &&edit) {
        auto prepared = values();
        std::forward<Edit>(edit)(prepared);
        *this = std::move(prepared);
    }
    void replaceAt(std::size_t index, std::string value) {
        if (index >= size()) throw std::out_of_range("Array cell index is outside the value.");
        if (numeric_) writeNumeric(index, std::move(value));
        else text_.replaceAt(index, std::move(value));
    }

    // Called with the author's scalar type and a checked linear size. The
    // editor publishes the new shape only after this operation succeeds.
    [[nodiscard]] bool resizeNumeric(std::size_t count, std::string_view type) {
        const auto representation = storageFor(type);
        if (representation == Representation::Text) return false;
        if (count > kMaximumArrayElements || count < size())
            throw std::length_error("Invalid numeric Array growth.");
        if (!count) return true;
        if (numeric_ && numeric_->representation == representation && count == size()) return true;
        DiagramArrayValues prepared;
        if (numeric_ && numeric_->representation == representation)
            prepared.numeric_ = std::make_shared<Storage>(*numeric_);
        else
            prepared.numeric_ = std::make_shared<Storage>(Storage{representation, 0, {}});
        prepared.numeric_->pages.resize(count / cellsPerPage + (count % cellsPerPage != 0));
        prepared.numeric_->count = count;
        if (!numeric_ || numeric_->representation != representation)
            for (std::size_t index = 0; index < size(); ++index)
                prepared.writeNumeric(index, (*this)[index]);
        swap(prepared);
        return true;
    }
    [[nodiscard]] bool hasNumericPages() const noexcept { return static_cast<bool>(numeric_); }

    // Storage codecs consume the immutable pages directly. A missing page is
    // an implicit run of zero words. Text exceptions remain separate.
    [[nodiscard]] static unsigned numericWordBytes(std::string_view scalarType) noexcept {
        return wordBytes(storageFor(scalarType));
    }
    [[nodiscard]] static std::optional<std::uint64_t> numericWordFromToken(
        std::string_view scalarType, std::string_view token,bool canonical=false) {
        return parse(storageFor(scalarType), token, canonical);
    }
    [[nodiscard]] bool hasNumericWords(std::string_view scalarType) const noexcept {
        return numeric_ && storageFor(scalarType) == numeric_->representation;
    }
    // Checked callers retain the numeric page owner and use a known in-range index.
    [[nodiscard]] std::uint64_t numericWordAt(std::size_t index) const noexcept {
        const auto &page=numeric_->pages[index/cellsPerPage];
        if(!page)return 0;
        return pageWord(*page,index%cellsPerPage);
    }
    template<class Visit> void visitNumericWords(Visit &&visit) const {
        for (std::size_t i = 0; i < numeric_->pages.size(); ++i) {
            const auto &page = numeric_->pages[i];
            const auto count = (std::min)(cellsPerPage,size()-i*cellsPerPage);
            if (!page) visit(count, std::span<const std::uint8_t>{});
            else if (!page->bytes.empty()) visit(count, std::span<const std::uint8_t>(page->bytes));
            else if (!page->shorts.empty()) visit(count, std::span<const std::uint16_t>(page->shorts));
            else if (!page->narrow.empty()) visit(count, std::span<const std::uint32_t>(page->narrow));
            else visit(count, std::span<const std::uint64_t>(page->wide));
        }
    }
    template<class Visit> void visitNumericSpellings(Visit &&visit) const {
        for (std::size_t i = 0; i < numeric_->pages.size(); ++i)
            if (const auto &page = numeric_->pages[i])
                for (const auto &[offset,text] : page->text)
                    if (i*cellsPerPage+offset<size()) visit(i*cellsPerPage+offset,std::string_view(text));
    }
    // A new compact payload is decoded into an unpublished set of pages.
    // Validate exact word width first; preserve demand allocation for zeros.
    bool loadNumericWords(std::size_t start,std::span<const std::uint8_t> bytes) {
        if (!numeric_ || numeric_.use_count()!=1) return false;
        const auto width = wordBytes(numeric_->representation);
        if (bytes.size()%width || start>size() || bytes.size()/width>size()-start) return false;
        for (std::size_t cursor = 0; cursor < bytes.size();) {
            const auto first = start+cursor/width;
            auto &page = numeric_->pages[first/cellsPerPage];
            const auto offset = first%cellsPerPage;
            const auto count = (std::min)(cellsPerPage-offset,(bytes.size()-cursor)/width);
            const auto section = bytes.subspan(cursor,count*width);
            if (!page && std::all_of(section.begin(),section.end(),[](auto byte){return byte==0;})) {
                cursor += section.size();
                continue;
            }
            if (page && page.use_count()!=1) return false;
            if (!page) page = makePage(width);
            if constexpr (std::endian::native == std::endian::little) {
                std::memcpy(pageWordAddress(*page,offset),section.data(),section.size());
                cursor += section.size();
            } else {
                for (std::size_t i=0;i<count;++i) {
                    std::uint64_t bits = 0;
                    for (unsigned byte=0;byte<width;++byte) bits |= std::uint64_t(bytes[cursor++])<<(8*byte);
                    setPageWord(*page,offset+i,bits);
                }
            }
        }
        return true;
    }
    bool loadNumericSpelling(std::size_t index, std::string text) {
        if (!numeric_ || index >= size() || parse(numeric_->representation,text)) return false;
        const auto &page = numeric_->pages[index/cellsPerPage];
        if (page) {
            const auto offset = index%cellsPerPage;
            if (page->text.contains(offset) || pageWord(*page,offset))
                return false;
        }
        writeNumeric(index,std::move(text));
        return true;
    }

    // The repeated implicit default needs only one text measurement.
    template<class Visit> void visitStoredValues(Visit &&visit) const {
        if (!numeric_) {
            for (const auto &value : text_) visit(value);
            return;
        }
        visit(std::string("0"));
        for (std::size_t page = 0; page < numeric_->pages.size(); ++page) {
            if (!numeric_->pages[page]) continue;
            const auto first = page * cellsPerPage;
            const auto last = (std::min)(size(), first + cellsPerPage);
            for (auto index = first; index < last; ++index) {
                auto value = (*this)[index];
                if (value != "0") visit(value);
            }
        }
    }
    template<class Visit> void visitAllocations(Visit &&visit) const {
        if (!numeric_) {
            if (text_.storageIdentity()) visit(text_.storageIdentity(), text_.retainedBytes());
            return;
        }
        visit(numeric_.get(), sizeof(Storage) + 32 + numeric_->pages.capacity() * sizeof(std::shared_ptr<Page>));
        for (const auto &page : numeric_->pages) {
            if (!page) continue;
            std::size_t bytes = sizeof(Page) + 32 + page->bytes.capacity() +
                page->shorts.capacity() * sizeof(std::uint16_t) +
                page->narrow.capacity() * sizeof(std::uint32_t) + page->wide.capacity() * sizeof(std::uint64_t);
            for (const auto &entry : page->text) bytes += sizeof(entry) + 32 + entry.second.capacity() + 1;
            visit(page.get(), bytes);
        }
    }
    [[nodiscard]] const void *storageIdentity() const noexcept {
        return numeric_ ? static_cast<const void *>(numeric_.get()) : text_.storageIdentity();
    }
    [[nodiscard]] std::size_t retainedBytes() const noexcept {
        std::size_t bytes = 0;
        visitAllocations([&](const void *, std::size_t size) { bytes += size; });
        return bytes;
    }
    friend bool operator==(const DiagramArrayValues &a, const DiagramArrayValues &b) {
        return a.storageIdentity() == b.storageIdentity() ||
            (a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin()));
    }
    friend bool operator==(const DiagramArrayValues &a, const std::vector<std::string> &b) {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    }

private:
    static Representation storageFor(std::string_view type) noexcept {
        if (type == "i8") return Representation::I8;
        if (type == "u8") return Representation::U8;
        if (type == "i16") return Representation::I16;
        if (type == "u16") return Representation::U16;
        if (type == "i32") return Representation::I32;
        if (type == "u32") return Representation::U32;
        if (type == "i64") return Representation::I64;
        if (type == "u64") return Representation::U64;
        if (type == "f32") return Representation::F32;
        if (type == "f64") return Representation::F64;
        return Representation::Text;
    }
    static unsigned wordBytes(Representation type) noexcept {
        switch (type) {
        case Representation::I8: case Representation::U8: return 1;
        case Representation::I16: case Representation::U16: return 2;
        case Representation::I32: case Representation::U32: case Representation::F32: return 4;
        case Representation::I64: case Representation::U64: case Representation::F64: return 8;
        default: return 0;
        }
    }
    static std::shared_ptr<Page> makePage(unsigned width) {
        auto page=std::make_shared<Page>();
        if (width==1) page->bytes.resize(cellsPerPage);
        else if (width==2) page->shorts.resize(cellsPerPage);
        else if (width==4) page->narrow.resize(cellsPerPage);
        else page->wide.resize(cellsPerPage);
        return page;
    }
    static std::uint64_t pageWord(const Page &page,std::size_t offset) noexcept {
        if (!page.bytes.empty()) return page.bytes[offset];
        if (!page.shorts.empty()) return page.shorts[offset];
        if (!page.narrow.empty()) return page.narrow[offset];
        return page.wide[offset];
    }
    static void *pageWordAddress(Page &page,std::size_t offset) noexcept {
        if (!page.bytes.empty()) return page.bytes.data()+offset;
        if (!page.shorts.empty()) return page.shorts.data()+offset;
        if (!page.narrow.empty()) return page.narrow.data()+offset;
        return page.wide.data()+offset;
    }
    static void setPageWord(Page &page,std::size_t offset,std::uint64_t bits) noexcept {
        if (!page.bytes.empty()) page.bytes[offset]=static_cast<std::uint8_t>(bits);
        else if (!page.shorts.empty()) page.shorts[offset]=static_cast<std::uint16_t>(bits);
        else if (!page.narrow.empty()) page.narrow[offset]=static_cast<std::uint32_t>(bits);
        else page.wide[offset]=bits;
    }
    template<class T> static std::optional<std::uint64_t> parseExact(std::string_view text,bool canonical) {
        if (text.empty()) return {};
        T value{};
        const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
        if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size()) return {};
        if (canonical) {
            char buffer[64];
            const auto written = std::to_chars(buffer, buffer + sizeof(buffer), value);
            if (written.ec != std::errc{} || text != std::string_view(buffer, written.ptr)) return {};
        }
        if constexpr (sizeof(T) == 1) return std::bit_cast<std::uint8_t>(value);
        else if constexpr (sizeof(T) == 2) return std::bit_cast<std::uint16_t>(value);
        else if constexpr (sizeof(T) == 4) return std::bit_cast<std::uint32_t>(value);
        else return std::bit_cast<std::uint64_t>(value);
    }
    static std::optional<std::uint64_t> parse(Representation type, std::string_view value,bool canonical=true) {
        switch (type) {
        case Representation::I8: return parseExact<std::int8_t>(value,canonical);
        case Representation::U8: return parseExact<std::uint8_t>(value,canonical);
        case Representation::I16: return parseExact<std::int16_t>(value,canonical);
        case Representation::U16: return parseExact<std::uint16_t>(value,canonical);
        case Representation::I32: return parseExact<std::int32_t>(value,canonical);
        case Representation::U32: return parseExact<std::uint32_t>(value,canonical);
        case Representation::I64: return parseExact<std::int64_t>(value,canonical);
        case Representation::U64: return parseExact<std::uint64_t>(value,canonical);
        case Representation::F32: return parseExact<float>(value,canonical);
        case Representation::F64: return parseExact<double>(value,canonical);
        default: return {};
        }
    }
    template<class T> static std::string formatBits(T value) {
        char buffer[64];
        const auto written = std::to_chars(buffer, buffer + sizeof(buffer), value);
        return {buffer, written.ptr};
    }
    static std::string format(Representation type, std::uint64_t bits) {
        if (bits == 0) return "0";
        switch (type) {
        case Representation::I8: return formatBits(std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(bits)));
        case Representation::U8: return formatBits(static_cast<std::uint8_t>(bits));
        case Representation::I16: return formatBits(std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(bits)));
        case Representation::U16: return formatBits(static_cast<std::uint16_t>(bits));
        case Representation::I32: return formatBits(std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(bits)));
        case Representation::U32: return formatBits(static_cast<std::uint32_t>(bits));
        case Representation::I64: return formatBits(std::bit_cast<std::int64_t>(bits));
        case Representation::U64: return formatBits(bits);
        case Representation::F32: return formatBits(std::bit_cast<float>(static_cast<std::uint32_t>(bits)));
        case Representation::F64: return formatBits(std::bit_cast<double>(bits));
        default: throw std::logic_error("Invalid numeric Array representation.");
        }
    }
    void writeNumeric(std::size_t index, std::string value) {
        const auto pageIndex = index / cellsPerPage;
        const auto offset = index % cellsPerPage;
        const auto bits = value == "0" ? std::optional<std::uint64_t>{0} : parse(numeric_->representation, value);
        if (bits == 0 && !numeric_->pages[pageIndex]) return;
        auto owner = numeric_.use_count() == 1 ? numeric_ : std::make_shared<Storage>(*numeric_);
        auto &current = owner->pages[pageIndex];
        auto page = current ? (current.use_count() == 1 ? current : std::make_shared<Page>(*current)) :
            makePage(wordBytes(owner->representation));
        if (bits) page->text.erase(offset);
        else page->text.insert_or_assign(offset, std::move(value));
        setPageWord(*page,offset,bits.value_or(0));
        current = std::move(page);
        numeric_ = std::move(owner);
    }

    SharedBuffer<std::string> text_;
    std::shared_ptr<Storage> numeric_;
};
} // namespace frog::native_engine
