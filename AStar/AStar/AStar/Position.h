#pragma once

class POSITION {
public:
	int y;
	int x;

	bool operator==(const POSITION& rhs) const {
		return x == rhs.x && y == rhs.y;
	}

	bool operator!=(const POSITION& rhs) const {
		return x != rhs.x || y != rhs.y;
	}
};

// std::hash Æ¯¼öÈ­
namespace std {
	template <>
	struct hash<POSITION> {
		size_t operator()(const POSITION& pos) const {
			return hash<int>()(pos.x) ^ (hash<int>()(pos.y) << 1);
		}
	};
}