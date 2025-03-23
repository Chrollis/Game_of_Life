#pragma once
#include <graphics.h>
#include <Windows.h>
#include <iostream>
#include <Eigen/Eigen>

class fpsController {
public:
	fpsController() = default;
	~fpsController() = default;
	void start();
	void delay();
	void set(const int fps);
	void draw();
private:
	int interval = 83;
	int fpsTarget = 12;
	DWORD startTime = 0;
	DWORD deltaTime = 0;
	DWORD endTime = 0;
	int fpsActual = 0;
};

void fpsController::start() {
	startTime = clock();
}
void fpsController::delay() {
	deltaTime = clock() - startTime;
	if (deltaTime < (unsigned)interval) {
		Sleep(interval - deltaTime);
	}
}
void fpsController::set(const int fps) {
	interval = 1000 / fps;
	fpsTarget = 1000 / interval;
}
void fpsController::draw() {
	static int counter = 0;
	if (counter == 0) {
		fpsActual = 1000 * fpsTarget / (clock() - endTime);
		endTime = clock();
	}
	counter = (++counter) % fpsTarget;
	wchar_t str[32];
	swprintf_s(str, L"Fps = %d : %d", fpsActual, fpsTarget);
	settextcolor(0xFFFFFF);
	setbkmode(TRANSPARENT);
	settextstyle(20, 0, L"Times New Roman");
	outtextxy(5, 25, str);
}


constexpr int graphW = 1280;
constexpr int graphH = 720;
constexpr int blockW = 160;
constexpr int blockH = 90;

class block {
public:
	block() = default;
	~block() = default;

	void draw(int x, int y, int scale)const;
public:
	bool alive = false;
	COLORREF color = 0xFFFFFF;
};

void block::draw(int x, int y, int scale)const {
	if (alive) {
		setfillcolor(color);
		fillrectangle(x, y, x + scale, y + scale);
	}
	setlinecolor(0x202020);
	rectangle(x, y, x + scale, y + scale);
}
bool check(Eigen::Matrix<block, 3, 3> input) {
	int num = 0;
	for (int i = 0; i < 9; i++) {
		if (i != 4) {
			num += (int)input(i / 3, i % 3).alive;
		}
	}
	bool flag = (!input(1, 1).alive && num == 3) || (input(1, 1).alive && (num < 2 || num>3));
	return (bool)(input(1, 1).alive ^ flag);
}

class Game {
public:
	Game() = default;
	~Game() = default;

	void progress(fpsController& fpsC);
	void draw();
	void move();
	void upgrade();
public:
	int fps = 12;
	int scale = 40;

	struct point {
		int x;
		int y;
	}camera = { scale,scale }, arrow = { 0,0 };
	int cameraSpeed = 1;
	int cameraAccelerate = 3;
	int cameraDeaccelerate = -1;

	bool running = false;
	ExMessage msg = { 0 };
	block temp[8];
	Eigen::Matrix<block, blockW, blockH> currentBlocks = Eigen::Matrix<block, blockW, blockH>(blockW, blockH);
	Eigen::Matrix<block, blockW, blockH> temporaryBlocks = Eigen::Matrix<block, blockW, blockH>(blockW, blockH);

	enum direction {
		left,
		up,
		right,
		down
	};
	bool isMove[4] = { false,false,false,false };
	bool isShift = false;
	bool isControl = false;
};

void Game::progress(fpsController& fpsC) {
	while (peekmessage(&msg)) {
		switch (msg.message) {
		case WM_KEYDOWN:
			switch (msg.vkcode) {
			case 'a':
			case 'A':
				isMove[left] = true;
				break;
			case 'w':
			case 'W':
				isMove[up] = true;
				break;
			case 'd':
			case 'D':
				isMove[right] = true;
				break;
			case 's':
			case 'S':
				isMove[down] = true;
				break;
			case 'q':
			case 'Q':
				if (scale > 20) {
					scale /= 2;
				}
				break;
			case 'e':
			case 'E':
				if (scale < 80) {
					scale *= 2;
				}
				break;
			case 'r':
			case 'R':
				if (fps > 6) {
					fps /= 2;
					fpsC.set(fps);
				}
				break;
			case 'f':
			case 'F':
				if (fps < 96) {
					fps *= 2;
					fpsC.set(fps);
				}
				break;
			case VK_SPACE:
				running = !running;
				break;
			case VK_SHIFT:
				isShift = true;
				break;
			case VK_CONTROL:
				isControl = true;
				break;
			default:
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.vkcode) {
			case 'a':
			case 'A':
				isMove[left] = false;
				break;
			case 'w':
			case 'W':
				isMove[up] = false;
				break;
			case 'd':
			case 'D':
				isMove[right] = false;
				break;
			case 's':
			case 'S':
				isMove[down] = false;
				break;
			case VK_SHIFT:
				isShift = false;
				break;
			case VK_CONTROL:
				isControl = false;
				break;
			default:
				break;
			}
			break;
		case WM_LBUTTONDOWN:
			currentBlocks((static_cast<Eigen::Index>(camera.x) + msg.x) / scale, (static_cast<Eigen::Index>(camera.y) + msg.y) / scale).alive = true;
			break;
		case WM_RBUTTONDOWN:
			currentBlocks((static_cast<Eigen::Index>(camera.x) + msg.x) / scale, (static_cast<Eigen::Index>(camera.y) + msg.y) / scale).alive = false;
			break;
		case WM_LBUTTONDBLCLK:
			while (true) {
				msg = getmessage();
				if (msg.message == WM_MOUSEMOVE) {
					currentBlocks((static_cast<Eigen::Index>(camera.x) + msg.x) / scale, (static_cast<Eigen::Index>(camera.y) + msg.y) / scale).alive = true;
					currentBlocks((static_cast<Eigen::Index>(camera.x) + msg.x) / scale, (static_cast<Eigen::Index>(camera.y) + msg.y) / scale).
						draw((camera.x + msg.x) / scale * scale - camera.x, (camera.y + msg.y) / scale * scale - camera.y, scale);
					FlushBatchDraw();
				}
				else if (msg.message == WM_LBUTTONUP) {
					break;
				}
			}
			break;
		case WM_RBUTTONDBLCLK:
			while (true) {
				msg = getmessage();
				if (msg.message == WM_MOUSEMOVE) {
					currentBlocks((static_cast<Eigen::Index>(camera.x) + msg.x) / scale, (static_cast<Eigen::Index>(camera.y) + msg.y) / scale).alive = false;
					currentBlocks((static_cast<Eigen::Index>(camera.x) + msg.x) / scale, (static_cast<Eigen::Index>(camera.y) + msg.y) / scale).
						draw((camera.x + msg.x) / scale * scale - camera.x, (camera.y + msg.y) / scale * scale - camera.y, scale);
					FlushBatchDraw();
				}
				else if (msg.message == WM_RBUTTONUP) {
					break;
				}
			}
			break;
		case WM_MOUSEMOVE:
			arrow.x = msg.x;
			arrow.y = msg.y;
			break;
		default:
			break;
		}
	}
}
void Game::draw() {
	for (int i = 0; i <= graphW / scale; i++) {
		for (int j = 0; j <= graphH / scale; j++) {
			currentBlocks(camera.x / scale + static_cast<Eigen::Index>(i), camera.y / scale + static_cast<Eigen::Index>(j)).
				draw(camera.x / scale * scale + i * scale - camera.x, camera.y / scale * scale + j * scale - camera.y, scale);
		}
	}
	wchar_t str[32];
	settextcolor(0xFFFFFF);
	setbkmode(1);
	settextstyle(20, 0, L"Times New Roman");
	swprintf_s(str, L"Camera = (%d, %d)", camera.x, camera.y);
	outtextxy(5, 5, str);
	if (isControl) {
		settextcolor(0x909090);
		swprintf_s(str, L"Block = (%d, %d)", (camera.x + arrow.x) / scale, (camera.y + arrow.y) / scale);
		outtextxy(arrow.x - 2, arrow.y - 22, str);
	}
}
void Game::move() {
	int dirX, dirY;
	double dirL;
	dirX = isMove[right] - isMove[left];
	dirY = isMove[down] - isMove[up];
	dirL = sqrt(dirX * dirX + dirY * dirY);

	if (cameraSpeed > 20) {
		cameraSpeed = 20;
	}
	else if (cameraSpeed < 1) {
		cameraSpeed = 1;
	}
	else {
		if (isShift) {
			cameraSpeed += cameraAccelerate;
		}
		else {
			cameraSpeed += cameraDeaccelerate;
		}
	}

	camera.x += (int)(cameraSpeed * dirX * dirL);
	camera.y += (int)(cameraSpeed * dirY * dirL);

	if (camera.x < scale) {
		camera.x = scale;
	}
	if (camera.y < scale) {
		camera.y = scale;
	}
	if (camera.x > (blockW - 1) * scale - graphW) {
		camera.x = (blockW - 1) * scale - graphW;
	}
	if (camera.y > (blockH - 1) * scale - graphH) {
		camera.y = (blockH - 1) * scale - graphH;
	}
}
void Game::upgrade() {
	for (int i = 0; i < blockW - 2; i++) {
		for (int j = 0; j < blockH - 2; j++) {
			temporaryBlocks(static_cast<Eigen::Index>(i) + 1, static_cast<Eigen::Index>(j) + 1).alive = check(currentBlocks.block<3, 3>(i, j));
		}
	}
	currentBlocks = temporaryBlocks;
}

