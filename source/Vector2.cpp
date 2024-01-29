#include "Vector2.hpp"

using csm4880::Vector2;


size_t Vector2::Hash::operator()(Vector2 const &vector) const noexcept {
    static std::hash<size_t> constexpr hash;
    size_t hashRow = hash(vector.row);
    size_t hashCol = hash(vector.col);
    /*
        This is a copy of `boost::hash_combine`.
        https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
    */
    return hashRow ^ (hashCol + 0x9e3779b9 + (hashRow << 6) + (hashRow >> 2));
}
