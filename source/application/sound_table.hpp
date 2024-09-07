#ifndef Application_SoundTable_hpp
#define Application_SoundTable_hpp

#include <utility>
#include <array>
#include <limits>
#include "simple_directmedia_layer.hpp"

namespace App { class SoundTable; }

class App::SoundTable {

	private:

		static constexpr std::size_t size{6u};
		static_assert(size <= std::numeric_limits<int>::max());
		std::array<Mix_Chunk *, size> table{};

	public:

		[[nodiscard]] constexpr SoundTable() = default;

		[[nodiscard]] constexpr SoundTable(SoundTable const &) = delete;
		[[nodiscard]] constexpr SoundTable & operator=(SoundTable const &) = delete;
		[[nodiscard]] constexpr SoundTable(SoundTable &&) = delete;
		[[nodiscard]] constexpr SoundTable & operator=(SoundTable &&) = delete;

		class DataView {
			private:
				void const *data;
				std::size_t size;

			public:
				[[nodiscard]] explicit constexpr DataView(
					decltype(data) const data_, decltype(size) const size_
				):
					data{data_}, size{size_}
				{}

				[[nodiscard]] constexpr void const * getData() const { return data; }

				[[nodiscard]] constexpr std::size_t getSize() const { return size; }
		};

		void put(std::size_t const identifier, DataView const dataView);

		void freeAllChunks();

		void play(std::size_t const identifier) const;

};

#endif
