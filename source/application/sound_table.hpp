#ifndef Application_SoundTable_hpp
#define Application_SoundTable_hpp

#include <utility>
#include <array>
#include <limits>
#include "simple_directmedia_layer.hpp"
#include "uniform_int_distribution.hpp"

namespace App { class SoundTable; }

class App::SoundTable {

	public:
		using Data = std::array<Mix_Chunk *, 6u>;
		static_assert(std::tuple_size_v<Data> <= std::numeric_limits<int>::max());


	private:
		Data table{};

	public:

		[[nodiscard]] constexpr SoundTable() = default;

		[[nodiscard]] constexpr SoundTable(SoundTable const &) = delete;
		[[nodiscard]] constexpr SoundTable & operator=(SoundTable const &) = delete;
		[[nodiscard]] constexpr SoundTable(SoundTable &&) = delete;
		[[nodiscard]] constexpr SoundTable & operator=(SoundTable &&) = delete;

		class AudioDataView {
			private:
				void const *data;
				std::size_t size;

			public:
				[[nodiscard]] explicit constexpr AudioDataView(
					decltype(data) const paramData, decltype(size) const paramSize
				):
					data{paramData}, size{paramSize}
				{}

				[[nodiscard]] constexpr void const * getData() const { return data; }

				[[nodiscard]] constexpr std::size_t getSize() const { return size; }
		};

		void put(std::size_t const identifier, AudioDataView const dataView);

		void freeAllChunks();

		void play(std::size_t const identifier) const;

		[[nodiscard]]
		constexpr Data const & getData() const { return table; }

		[[nodiscard]]
		static UniformIntDistribution<Data::size_type> makeRandomSoundPicker(unsigned int const seed) {
			static_assert(std::tuple_size_v<Data> > 0u, "Assert can subtract 1 from size.");
			return UniformIntDistribution<Data::size_type>(seed, {0u, std::tuple_size_v<Data> - 1u});
		}

};

#endif
