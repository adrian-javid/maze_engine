#ifndef Application_SoundTable_hpp
#define Application_SoundTable_hpp

#include <utility>
#include <array>
#include <limits>
#include "simple_directmedia_layer.hpp"

namespace App::SoundTable {
	inline constexpr std::size_t size{7u};
	static_assert(size <= std::numeric_limits<int>::max());

	struct DataView {
		void const *data;
		std::size_t size;
		[[nodiscard]] explicit DataView(
			decltype(data) const paramData, decltype(size) const paramSize
		):
			data{paramData}, size{paramSize}
		{}
	};

	void put(std::size_t const identifier, DataView const dataView);

	void freeAllChunks();

	void play(std::size_t const identifier);
}

#endif
