#ifndef SquareGrid_hpp
#define SquareGrid_hpp

#include <string>
#include <bitset>
#include <sstream> 

namespace csm4880 {
  template<size_t, size_t> class SquareGrid;
}

template<size_t rowCount, size_t columnCount> class csm4880::SquareGrid {

  public:

    std::bitset<rowCount * columnCount> flatData;

    constexpr explicit SquareGrid(): flatData() {}

    constexpr size_t getRowCount() const { return rowCount; }
    constexpr size_t getColumnCount() const { return columnCount; }

    bool at(size_t const row, size_t const column) const {
      return flatData[row * columnCount + column];
    }

    std::string toString(char const trueSymbol='#', char const falseSymbol='.') const {
      std::stringstream buffer;

      for (size_t row; row < rowCount; ++row)
        for (size_t column; column < columnCount; ++column)
          buffer << (at(row, column) ? trueSymbol : falseSymbol);

      return buffer.str();
    }

    friend std::ostream& operator<<(std::ostream &outputStream, SquareGrid const &squareGrid) {
      outputStream << squareGrid; return outputStream;
    }
};

#endif
