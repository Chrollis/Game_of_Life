#pragma once
#include "util.h"

class FpsController {
private:
	int interval = 83;
	int fps_target = 12;
	clock_t begin = 0;
	clock_t end = 0;
	int actual_fps = 0;
public:
	FpsController() = default;
	~FpsController() = default;

	void start();
	void delay()const;
	void set(const int fps);
	void draw();

	friend class Game;
};

void FpsController::start() {
	begin = clock();
}
void FpsController::delay()const {
	Sleep((DWORD)(max(0, interval + clock() - begin)));
}
void FpsController::set(const int fps) {
	interval = 500 / fps;
	fps_target = fps;
}
void FpsController::draw() {
	static int counter = 0;
	if (counter == 0) {
		actual_fps = 1000 * fps_target / (clock() - end);
		end = clock();
	}
	counter = (++counter) % fps_target;
	ostringstream oss;
	oss << "Fps = " << actual_fps << " : " << fps_target;
	settextcolor(0xFFFFFF);
	settextstyle(20, 0, L"Times New Roman", 0, 0, 0, 1, 0, 0);
	setbkmode(TRANSPARENT);
	outtextxy(3, graph_side_length - 23, string_to_lpcwstr(oss.str()));
}