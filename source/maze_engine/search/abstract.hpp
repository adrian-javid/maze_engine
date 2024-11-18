#ifndef MazeEngine_Search_Abstract_hpp
#define MazeEngine_Search_Abstract_hpp

#include <optional>
#include <type_traits>
#include <stack>
#include <queue>

#include "maze_engine/vector2.hpp"
#include "application/macros.hpp"

namespace MazeEngine {
	template <typename StorageT>
	[[deprecated]]
	Vector2::HashMap<Vector2> abstractSearch(
		Maze const &maze,
		Vector2 const start,
		StorageT &storage,
		std::function<bool(Vector2 const)> const processKey
	);

	class MazeSearchIterator;

	template <typename StorageT>
	class AbstractSearchIterator;
}

template <typename StorageT>
auto MazeEngine::abstractSearch(
	Maze const &maze,
	Vector2 const start,
	StorageT &storage,
	std::function<bool(Vector2 const)> const processKey
) -> Vector2::HashMap<Vector2> {

	Vector2::HashMap<Vector2> history;

	storage.push(start);
	history.insert({start, start});

	while (not storage.empty()) {
		Vector2 const key([&storage]() constexpr -> Vector2 {
			if constexpr (std::is_same_v<StorageT, std::stack<Vector2>>)
				return storage.top();
			else if constexpr (std::is_same_v<StorageT, std::queue<Vector2>>)
				return storage.front();
			else /* other, such as priority queue */
				return storage.top();
		}());
		storage.pop();

		if (processKey != nullptr and processKey(key)) break;

		maze.forEachNeighbor(key, [&history, key, &storage](Vector2 const neighbor) {
			if (history.find(neighbor) == history.end()) {
				history.insert({neighbor, key});
				storage.push(neighbor);
			}
		});
	}

	return history;
}

class MazeEngine::MazeSearchIterator {
	public:
		virtual void advance() = 0;
		[[nodiscard]] virtual Vector2 getVector() const = 0;
		[[nodiscard]] virtual bool isDone() const = 0;
		[[nodiscard]] virtual Vector2::HashMap<Vector2> const & getHistory() const = 0;
		[[nodiscard]] explicit MazeSearchIterator() = default;
		[[nodiscard]] explicit MazeSearchIterator(MazeSearchIterator const &) = default;
		virtual ~MazeSearchIterator() = default;
};

template <typename StorageT>
class MazeEngine::AbstractSearchIterator : public MazeSearchIterator {
	private:
		Maze const *maze;
		Vector2 key;
		StorageT storage;
		Vector2::HashMap<Vector2> history;

		FORCE_INLINE void gatherNeighbors() {
			maze->forEachNeighbor(key, [this](Vector2 const neighbor) {
				if (this->history.find(neighbor) == this->history.end()) {
					this->history.insert({neighbor, this->key});
					this->storage.push(neighbor);
				}
			});
		}

	public:

		/*
			The way this constructor is implemented,
			the first vertex will be processed twice.

			That's okay for the purposes of this program,
			even though that is not how I prefer for it to work.
		*/
		[[nodiscard]]
		explicit AbstractSearchIterator(
			Maze const &mazeReference,
			Vector2 start,
			StorageT storageValue
		):
			maze(&mazeReference), key(std::move(start)), storage(std::move(storageValue)), history{{start, start}}
		{
			storage.push(key);
		}

		[[nodiscard]]
		static Vector2 popFrom(StorageT &storage) {
			Vector2 const key([&storage]() constexpr -> Vector2 {
				if constexpr (std::is_same_v<StorageT, std::stack<Vector2>>)
					return storage.top();
				else if constexpr (std::is_same_v<StorageT, std::queue<Vector2>>)
					return storage.front();
				else /* other, such as priority queue */
					return storage.top();
			}());
			storage.pop();
			return key;
		}

		void advance() override {
			assert(not storage.empty());
			if (storage.empty()) return;

			key = popFrom(storage);
			gatherNeighbors();
		}

		[[nodiscard]] Vector2 getVector() const override { return key; }

		[[nodiscard]] Vector2::HashMap<Vector2> const &getHistory() const override { return history; }

		[[nodiscard]] StorageT const &getStorage() const { return storage; }

		[[nodiscard]] bool isDone() const override { return storage.empty(); };
};

#endif
