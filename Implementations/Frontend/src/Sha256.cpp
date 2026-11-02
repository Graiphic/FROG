#include "frog_native_engine/Sha256.hpp"
#include <bit>
#include <vector>
namespace frog::native_engine::hashing {
namespace {
using Bytes = std::span<const std::uint8_t>;
// Small, platform-independent hash implementations keep metadata inspection
// distributable without an SDK, a managed runtime, or an external executable.
std::vector<std::uint8_t> hashPadding(Bytes bytes) {
    std::vector<std::uint8_t> padded(bytes.begin(), bytes.end()); padded.push_back(0x80);
    while (padded.size() % 64 != 56) padded.push_back(0);
    const auto bits = std::uint64_t(bytes.size()) * 8;
    for (int i = 7; i >= 0; --i) padded.push_back(static_cast<std::uint8_t>(bits >> (8 * i)));
    return padded;
}
template<std::size_t N> std::array<std::uint8_t, N * 4> hashOutput(const std::array<std::uint32_t, N> &words) {
    std::array<std::uint8_t, N * 4> result{};
    for (std::size_t i = 0; i < N; ++i) for (unsigned j = 0; j < 4; ++j)
        result[i * 4 + j] = static_cast<std::uint8_t>(words[i] >> (24 - 8 * j));
    return result;
}
std::uint32_t bigWord(Bytes data, std::size_t offset) {
    return (std::uint32_t(data[offset]) << 24) | (std::uint32_t(data[offset + 1]) << 16) |
        (std::uint32_t(data[offset + 2]) << 8) | data[offset + 3];
}
}
std::array<std::uint8_t, 32> sha256(Bytes data) {
    constexpr std::array<std::uint32_t, 64> constants{
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::array<std::uint32_t, 8> state{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    const auto padded = hashPadding(data);
    for (std::size_t offset = 0; offset < padded.size(); offset += 64) {
        std::array<std::uint32_t, 64> w{};
        for (unsigned i = 0; i < 16; ++i) w[i] = bigWord(padded, offset + i * 4);
        for (unsigned i = 16; i < 64; ++i) {
            const auto s0 = std::rotr(w[i-15],7) ^ std::rotr(w[i-15],18) ^ (w[i-15] >> 3);
            const auto s1 = std::rotr(w[i-2],17) ^ std::rotr(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        auto [a,b,c,d,e,f,g,h] = state;
        for (unsigned i = 0; i < 64; ++i) {
            const auto s1 = std::rotr(e,6) ^ std::rotr(e,11) ^ std::rotr(e,25);
            const auto t1 = h + s1 + ((e & f) ^ (~e & g)) + constants[i] + w[i];
            const auto s0 = std::rotr(a,2) ^ std::rotr(a,13) ^ std::rotr(a,22);
            const auto t2 = s0 + ((a & b) ^ (a & c) ^ (b & c));
            h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
        }
        const std::array round{a,b,c,d,e,f,g,h};
        for (unsigned i = 0; i < 8; ++i) state[i] += round[i];
    }
    return hashOutput(state);
}
std::string sha256Hex(std::string_view bytes) {
    const auto digest=sha256({reinterpret_cast<const std::uint8_t *>(bytes.data()),bytes.size()});
    constexpr char digits[]="0123456789abcdef";
    std::string result;result.reserve(64);
    for(const auto byte:digest){result+=digits[byte>>4];result+=digits[byte&15];}
    return result;
}
}
