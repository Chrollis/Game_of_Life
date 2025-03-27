#pragma once
#include "util.h"

class Cell {
private:
	bool alive : 1;
	bool signal : 1;
public:
	Cell() {
		alive = signal = 0;
	}
	~Cell() = default;

	bool proceed(const Eigen::Matrix<Cell, 3, 3>& input);
	void draw(int x, int y)const;

	friend class Game;
};

void Cell::draw(int x, int y)const {
	if (alive) {
		setfillcolor(0xFFFFFF);
		fillrectangle(x, y, x + scale, y + scale);
	}
	setlinecolor(0x202020);
	rectangle(x, y, x + scale, y + scale);
}
bool Cell::proceed(const Eigen::Matrix<Cell, 3, 3>& input) {
	int num = 0;
	for (int i = 0; i < 9; i++) {
		if (i != 4) {
			num += (int)input(i / 3, i % 3).alive;
		}
	}
	if (!alive && num == 3) {
		alive = 1;
		return 1;
	}
	else if (alive && (num < 2 || num>3)) {
		alive = 0;
		return 1;
	}
	signal = 0;
	return 0;
}