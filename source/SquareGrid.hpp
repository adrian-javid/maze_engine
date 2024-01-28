#ifndef SquareGrid_hpp
#define SquareGrid_hpp

#include <string>
#include <bitset>
#include <sstream> 

namespace csm4880 { template<size_t, size_t> class SquareGrid; }

template<size_t rowCount, size_t columnCount> class csm4880::SquareGrid {

  public:

    constexpr static size_t getRowCount() { return rowCount; }
    constexpr static size_t getColumnCount() { return columnCount; }
    constexpr static size_t getSize() { return rowCount * columnCount; }

    using BitSet = std::bitset<getSize()>;

  public:

    BitSet flatData;

    constexpr explicit SquareGrid(): flatData() {}

    bool at(size_t const row, size_t const column) const {
      return flatData[row * columnCount + column];
    }

    typename BitSet::reference at(size_t const row, size_t const column) {
      return flatData[row * columnCount + column];
    }

    std::string toString(char const trueSymbol='#', char const falseSymbol='.') const {
      std::stringstream buffer;

      for (size_t row{0}; row < rowCount; ++row) {
        for (size_t column{0}; column < columnCount; ++column)
          buffer << ' ' << (at(row, column) ? trueSymbol : falseSymbol);
        buffer << ' ' << '\n';
      }

      return buffer.str();
    }

    friend std::ostream& operator<<(std::ostream &outputStream, SquareGrid const &squareGrid) {
      outputStream << squareGrid.toString(); return outputStream;
    }

};

#endif
