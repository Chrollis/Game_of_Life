#pragma once
#include "cell.h"
#include "fps_controller.h"

class Game {
public:
	Game() = default;
	~Game() = default;

	void enter();
	void proceed(clock_t delta);
	void input(const ExMessage& msg);
	void draw();
	void move();
	void quit();
protected:
	void unload_block(int x,int y);
	void load_block(int x, int y);
	void centre_move(int x, int y);
public:
	FpsController fps_controller;
private:
	struct Camera {
		POINT coor;
		int speed;
	}camera = { {0,0},1 };
	bool running;
	bool is_shift = 0;
	bool is_control = 0;
	bool is_moving[4] = { 0,0,0,0 };
	clock_t compute_interval = 300;
	Eigen::Matrix<Cell, block_size * 5, block_size * 5> current_cells;
	POINT centre_block = { 0,0 };
	POINT cursor = { 0,0 };
};

void Game::enter() {
	current_cells = Eigen::Matrix<Cell, block_size * 5, block_size * 5>(block_size * 5, block_size * 5);
	CreateDirectoryA(".\\cache", nullptr);
}
void Game::proceed(clock_t delta) {
	static clock_t timer_ = 0;
	static clock_t timer = 0;
	if (running) {
		if (timer_ >= compute_interval) {
			Eigen::Matrix<Cell, block_size * 5, block_size * 5> copy_cells = current_cells;
			for (int col = 1; col < block_size * 5; col++) {
				for (int row = 1; row < block_size * 5; row++) {
					if (current_cells(row, col).signal) {
						if (current_cells(row, col).proceed(copy_cells.block<3, 3>(row - 1, col - 1))) {
							for (int j = 0; j < 5;j++) {
								for (int i = 0; i < 5; i++) {
									if (row - 2 + i >= 0 && row - 2 + i < 5 * block_size) {
										if (col - 2 + j >= 0 && col - 2 + j < 5 * block_size) {
											current_cells(row - 2 + i, col - 2 + j).signal = 1;
										}
									}
								}
							}
						}
					}
				}
			}
			timer_ = 0;
		}
		timer_ += delta;
	}
	if (timer >= 120) {
		move();
		timer = 0;
	}
	timer += delta;
}
void Game::input(const ExMessage& msg) {
	switch (msg.message) {
	case WM_KEYDOWN:
		switch (msg.vkcode) {
		case 'A':
			is_moving[0] = 1;
			break;
		case 'W':
			is_moving[1] = 1;
			break;
		case 'D':
			is_moving[2] = 1;
			break;
		case 'S':
			is_moving[3] = 1;
			break;
		case 'Q':
			if (compute_interval > 75) {
				compute_interval /= 2;
			}
			break;
		case 'E':
			if (compute_interval < 2400) {
				compute_interval *= 2;
			}
			break;
		case VK_SPACE:
			running = !running;
			break;
		case VK_SHIFT:
			is_shift = 1;
			break;
		case VK_CONTROL:
			is_control = 1;
			break;
		case VK_ESCAPE:
			quit();
			exit(0);
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (msg.vkcode) {
		case 'A':
			is_moving[0] = 0;
			break;
		case 'W':
			is_moving[1] = 0;
			break;
		case 'D':
			is_moving[2] = 0;
			break;
		case 'S':
			is_moving[3] = 0;
			break;
		case VK_SHIFT:
			is_shift = 0;
			break;
		case VK_CONTROL:
			is_control = 0;
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		current_cells((camera.coor.x + msg.x) / scale + block_size * 2,
			(camera.coor.y + msg.y) / scale + block_size * 2).alive = 1;
		current_cells((camera.coor.x + msg.x + block_size * 2) / scale,
			(camera.coor.y + msg.y) / scale + block_size * 2).signal = 1;
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				if ((camera.coor.x + msg.x) / scale + block_size * 2 - 1 + i >= 0 && (camera.coor.x + msg.x) / scale + block_size * 2 - 1 + i < 5 * block_size) {
					if ((camera.coor.y + msg.y) / scale + block_size * 2 - 1 + j >= 0 && (camera.coor.y + msg.y) / scale + block_size * 2 - 1 + j < 5 * block_size) {
						current_cells((camera.coor.x + msg.x) / scale + block_size * 2 - 1 + i, (camera.coor.y + msg.y) / scale + block_size * 2 - 1 + j).signal = 1;
					}
				}
			}
		}
		break;
	case WM_RBUTTONDOWN:
		current_cells((camera.coor.x + msg.x) / scale + block_size * 2,
			(camera.coor.y + msg.y) / scale + block_size * 2).alive = 0;
		current_cells((camera.coor.x + msg.x) / scale + block_size * 2,
			(camera.coor.y + msg.y) / scale + block_size * 2).signal = 0;
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				if ((camera.coor.x + msg.x) / scale + block_size * 2 - 1 + i >= 0 && (camera.coor.x + msg.x) / scale + block_size * 2 - 1 + i < 5 * block_size) {
					if ((camera.coor.y + msg.y) / scale + block_size * 2 - 1 + j >= 0 && (camera.coor.y + msg.y) / scale + block_size * 2 - 1 + j < 5 * block_size) {
						current_cells((camera.coor.x + msg.x) / scale + block_size * 2 - 1 + i, (camera.coor.y + msg.y) / scale + block_size * 2 - 1 + j).signal = 1;
					}
				}
			}
		}
		break;
	case WM_MOUSEMOVE:
		cursor.x = msg.x;
		cursor.y = msg.y;
		break;
	default:
		break;
	}
}
void Game::centre_move(int x, int y) {
	if (abs(x) + abs(y) == 2) {
		for (int i = -2; i < 2; i++) {
			unload_block(-x * 2, -y * i);
		}
		for (int i = 2; i >= -2; i--) {
			unload_block(-x * i, -y * 2);
		}
		Eigen::Matrix<Cell, 4 * block_size, 4 * block_size> copy_cells = current_cells.block<4 * block_size, 4 * block_size>(block_size / 2 * (1 + y), block_size / 2 * (1 + x));
		for (int j = 0; j < 4 * block_size; j++) {
			for (int i = 0; i < 4 * block_size; i++) {
				current_cells(i + block_size / 2 * (1 - y), j + block_size / 2 * (1 - x)) = copy_cells(i, j);
			}
		}
		for (int i = -2; i < 2; i++) {
			load_block(x * 2, y * i);
		}
		for (int i = 2; i >= -2; i--) {
			load_block(x * i, y * 2);
		}
	}
	else {
		if (!x && y) {
			for (int i = -2; i <= 2; i++) {
				unload_block(i, -y * 2);
			}
			Eigen::Matrix<Cell, 4 * block_size, 5 * block_size> copy_cells = current_cells.block<4 * block_size, 5 * block_size>(block_size / 2 * (1 + y), 0);
			for (int j = 0; j < 5 * block_size; j++) {
				for (int i = 0; i < 4 * block_size; i++) {
					current_cells(i + block_size / 2 * (1 - y), j) = copy_cells(i, j);
				}
			}
			for (int i = -2; i <= 2; i++) {
				load_block(i, y * 2);
			}
		}
		else if(!y && x){
			for (int i = -2; i <= 2; i++) {
				unload_block(-x * 2, i);
			}
			Eigen::Matrix<Cell, 5 * block_size, 4 * block_size> copy_cells = current_cells.block<5 * block_size, 4 * block_size>(0, block_size / 2 * (1 + x));
			for (int j = 0; j < 4 * block_size; j++) {
				for (int i = 0; i < 5 * block_size; i++) {
					current_cells(i, j + block_size / 2 * (1 + x)) = copy_cells(i, j);
				}
			}
			for (int i = -2; i <= 2; i++) {
				load_block(x * 2, i);
			}
		}
	}
	centre_block.x += x;
	centre_block.y += y;
	camera.coor.x -= x * block_size * scale;
	camera.coor.y -= y * block_size * scale;
}
void Game::unload_block(int x, int y) {
	bool temp = running;
	running = 0;
	ofstream ofs(".\\cache\\block[" + to_string(x) + ", " + to_string(y) + "].bin", ios::binary);
	if (!ofs.is_open()) {
		return;
	}
	else {
		Eigen::Matrix<Cell, block_size, block_size> block = current_cells.block<block_size, block_size>(block_size * (2 + y), block_size * (2 + x));
		for (int j = 0; j < block_size; j++) {
			for (int i = 0; i < block_size; i++) {
				ofs << block(i, j).alive << endl;
			}
		}
		ofs.close();
	}
	running = temp;
}
void Game::load_block(int x, int y) {
	bool temp = running;
	running = 0;
	ifstream ifs(".\\cache\\block[" + to_string(x) + ", " + to_string(y) + "].bin", ios::binary);
	int t = 0;
	if (!ifs.is_open()) {
		for (int j = 0; j < block_size; j++) {
			for (int i = 0; i < block_size; i++) {
				current_cells(block_size * (2 + y) + i, block_size * (2 + x) + j).alive = 0;
			}
		}
		return;
	}
	else {
		for (int j = 0; j < block_size; j++) {
			for (int i = 0; i < block_size; i++) {
				ifs >> t;
				current_cells(block_size * (2 + y) + i, block_size * (2 + x) + j).alive = (bool)t;
			}
		}
		ifs.close();
	}
	running = temp;
}
void Game::draw() {
	int m = graph_side_length / scale;
	for (int j = -1; j <= m; j++) {
		for (int i = -1; i <= m; i++) {
			current_cells(camera.coor.x / scale + i + block_size * 2, camera.coor.y / scale + j + block_size * 2).draw(
				camera.coor.x / scale * scale + i * scale - camera.coor.x,
				camera.coor.y / scale * scale + j * scale - camera.coor.y
			);
		}
	}
	ostringstream oss;
	oss << "Camera(" << camera.coor.x << ", " << camera.coor.y << ")";
	settextcolor(0xFFFFFF);
	settextstyle(20, 0, L"Times New Roman", 0, 0, 0, 1, 0, 0);
	setbkmode(TRANSPARENT);
	outtextxy(3, 3, string_to_lpcwstr(oss.str()));
	if (is_control) {
		oss.str("");
		oss << "Block(" << (camera.coor.x + cursor.x) / scale + block_size * centre_block.x
			<< ", " << (camera.coor.y + cursor.y) / scale + block_size * centre_block.y << ")";
		outtextxy(3, graph_side_length - 43, string_to_lpcwstr(oss.str()));
	}
}
void Game::move() {
	int dirX, dirY;
	double dirL;
	dirX = is_moving[2] - is_moving[0];
	dirY = is_moving[3] - is_moving[1];
	dirL = sqrt(dirX * dirX + dirY * dirY);

	if (is_shift && camera.speed < 18) {
		camera.speed += 2;
	}
	if (!is_shift && camera.speed > 2) {
		camera.speed -= 2;
	}
	dirX = (int)(camera.speed * dirX * dirL);
	dirY = (int)(camera.speed * dirY * dirL);
	if (abs(camera.coor.x + dirX) <= block_size * scale) {
		camera.coor.x += dirX;
	}
	if (abs(camera.coor.y + dirY) <= block_size * scale) {
		camera.coor.y += dirY;
	}
	if (abs(camera.coor.x + camera.coor.y) + abs(camera.coor.x - camera.coor.y) >= 2 * (block_size - 2) * scale) {
		int x = (int)(camera.coor.x * (1 + 2 * (double)block_size / scale) / (block_size * scale));
		int y = (int)(camera.coor.y * (1 + 2 * (double)block_size / scale) / (block_size * scale));
		centre_move(x, y);
	}
}
void Game::quit() {
	clear_directory(L".\\cache");
}