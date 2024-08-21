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
		Vector2 const &start,
		StorageT &storage,
		std::function<bool(Vector2 const &)> const processKey
	);

	class MazeSearchIterator;

	template <typename StorageT>
	class AbstractSearchIterator;
}

template <typename StorageT>
auto MazeEngine::abstractSearch(
	Maze const &maze,
	Vector2 const &start,
	StorageT &storage,
	std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {

	Vector2::HashMap<Vector2> history;

	storage.push(start);
	history.insert({start, start});

	while (not storage.empty()) {
		Vector2 const key = [&storage]() constexpr -> Vector2 const &{
			if constexpr (std::is_same_v<StorageT, std::stack<Vector2>>)
				return storage.top();
			else if constexpr (std::is_same_v<StorageT, std::queue<Vector2>>)
				return storage.front();
			else /* other, such as priority queue */
				return storage.top();
		}();
		storage.pop();

		if (processKey != nullptr and processKey(key)) break;

		maze.forEachNeighbor(key, [&history, &key, &storage](Vector2 const &neighbor) {
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
		virtual MazeSearchIterator &operator++() = 0;
		[[nodiscard]] virtual Vector2 const &operator*() const = 0;
		[[nodiscard]] virtual bool isEnd() const = 0;
		[[nodiscard]] virtual Vector2::HashMap<Vector2> const &getHistory() const = 0;
		virtual ~MazeSearchIterator() = default;
};

template <typename StorageT>
class MazeEngine::AbstractSearchIterator : public MazeSearchIterator {
	private:
		Maze const *maze;

	public:
		Vector2 key;

	private:
		StorageT storage;
		Vector2::HashMap<Vector2> history;

		FORCE_INLINE void gatherNeighbors() {
			maze->forEachNeighbor(key, [this](Vector2 const &neighbor) {
				if (this->history.find(neighbor) == this->history.end()) {
					this->history.insert({neighbor, this->key});
					this->storage.push(neighbor);
				}
			});
		}

	public:
		explicit AbstractSearchIterator(
			Maze const &mazeReference,
			Vector2 start,
			StorageT storageValue
		):
			maze(&mazeReference), key(std::move(start)), storage(std::move(storageValue)), history()
		{
			history.insert({key, key});
			gatherNeighbors();
		}

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

		AbstractSearchIterator &operator++() override {
			assert(not storage.empty());

			key = popFrom(storage);
			gatherNeighbors();

			return *this;
		}

		[[nodiscard]] Vector2 const &operator*() const override { return key; }

		[[nodiscard]] Vector2::HashMap<Vector2> const &getHistory() const override { return history; }

		[[nodiscard]] StorageT const &getStorage() const { return storage; }

		[[nodiscard]] bool isEnd() const override { return storage.empty(); };
};

#endif
