#ifndef Application_SoundTable_hpp
#define Application_SoundTable_hpp

#include <utility>
#include <array>
#include <limits>
#include "simple_directmedia_layer.hpp"
#include "uniform_int_distribution.hpp"
#include "audio_data.hpp"

namespace App { class SoundTable; }

class App::SoundTable {

	public:
		using Data = std::array<Mix_Chunk *, std::size_t{6u * 2u}>;
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

		template <typename SoundInstrument>
		std::enable_if_t<
			std::is_same_v<SoundInstrument, AudioData::Piano> or
			std::is_same_v<SoundInstrument, AudioData::Synthesizer>,
		void> populateSelfFromSoundInstrument() {
			static constexpr std::size_t sectionOffset{6u};
			static_assert(sectionOffset == std::tuple_size_v<Data> / 2u);
			static constexpr std::size_t byteCount{
				std::tuple_size_v<AudioData::SimpleSound> * sizeof(AudioData::SimpleSound::value_type)
			};

			this->put(0u                , AudioDataView(SoundInstrument::Low ::    first.data(), byteCount));
			this->put(1u                , AudioDataView(SoundInstrument::Low ::    third.data(), byteCount));
			this->put(2u                , AudioDataView(SoundInstrument::Low ::    fifth.data(), byteCount));
			this->put(3u                , AudioDataView(SoundInstrument::Low ::highFirst.data(), byteCount));
			this->put(4u                , AudioDataView(SoundInstrument::Low ::highThird.data(), byteCount));
			this->put(5u                , AudioDataView(SoundInstrument::Low ::highFifth.data(), byteCount));
			this->put(0u + sectionOffset, AudioDataView(SoundInstrument::High::    first.data(), byteCount));
			this->put(1u + sectionOffset, AudioDataView(SoundInstrument::High::    third.data(), byteCount));
			this->put(2u + sectionOffset, AudioDataView(SoundInstrument::High::    fifth.data(), byteCount));
			this->put(3u + sectionOffset, AudioDataView(SoundInstrument::High::highFirst.data(), byteCount));
			this->put(4u + sectionOffset, AudioDataView(SoundInstrument::High::highThird.data(), byteCount));
			this->put(5u + sectionOffset, AudioDataView(SoundInstrument::High::highFifth.data(), byteCount));
		}
};

#endif
