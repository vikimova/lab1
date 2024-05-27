#include <iostream>
#include <random>
#include <tuple>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <chrono>
#include <thread>


const char WALL = '.';
const char EXIT = '#';
const char FLOOR = ' ';
const char ENEMY = '@';
const char PLAYER = '%';


int randint(int from_, int to_) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(from_, to_);
	return distr(gen);
}

bool roll_dice(int expect, int total) {
    int roll = randint(1, total);
    return roll == expect;
}

std::tuple<int, int> get_pads(int w, int h) {
	struct winsize win;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	int pad_w, pad_h;
	int tw = win.ws_col;
	int th = win.ws_row;
	int bw = w;
	int bh = h;

    if (tw > bw) {
        pad_w = (tw-bw)/2;
	} else {
        pad_w = 0;
	}

    if (th > bh) {
        pad_h = (th-bh)/2;
	} else {
        pad_h = 0;
	}

	std::cout << pad_w << "_" << pad_h << std::endl;

	return std::make_tuple(pad_w, pad_h);
}


void print_title(char *title, int pad_x, int pad_y) {
	std::cout << "\033[2J";
	for (int i=0;i<pad_y;i++) {
		std::cout << std::endl;
	}
	for (int i=0;i<pad_x;i++) {
		std::cout << " ";
	}
	std::cout << title;
	std::cout << std::endl;
	for (int i=0;i<pad_y;i++) {
		std::cout << std::endl;
	}
}


class Game {
public:
	int player_health = 100;
	int board_width;
	int board_height;

	char **dungeon;
	std::tuple<int, int> player_pos;
	std::tuple<int, int> exit_pos;
	std::vector<std::tuple<int, int>> enemies;

	Game () {
		board_width = randint(15, 20);
		board_height = randint(30, 40);

		dungeon = new char*[board_height];

		for (int i=0;i<board_height;i++) {
			dungeon[i] = new char[board_width];
		}

		for (int i=0;i<board_height;i++) {
			for (int j=0;j<board_width;j++) {
				dungeon[i][j] = WALL;
			}
		}

		generate_dungeon();
	};
	~Game () {};

	void print_dungeon(int pad_x, int pad_y) {
		for (int i=0;i<pad_y;i++) {
			std::cout << std::endl;
		}
		for (int i=0;i<this->board_height;i++) {
			for (int k=0; k < pad_x; k++) {
				std::cout << " ";
			}
			for (int j=0;j<this->board_width;j++) {
				std::cout << " " << this->dungeon[i][j];
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		for (int k=0; k < pad_x; k++) {
			std::cout << " ";
		}
		std::cout << "Health: " << this->player_health << std::endl;
		for (int i=0;i<pad_y;i++) {
			std::cout << std::endl;
		}
	}

	void handle_event(char pressed_c) {
		if (pressed_c == 'q') {
			exit(0);
		}

		int x = std::get<0>(this->player_pos);
		int y = std::get<1>(this->player_pos);

		update_player_pos(pressed_c, x, y);

		int nx = std::get<0>(this->player_pos);
		int ny = std::get<1>(this->player_pos);

        if (this->dungeon[ny][nx] == WALL) {
			nx = x;
			ny = y;
            this->player_pos = {nx, ny};
            this->dungeon[y][x] = FLOOR;
		} else if (this->dungeon[ny][nx] == ENEMY) {
            this->player_health -= 5;
            this->dungeon[y][x] = ENEMY;
		} else {
            this->dungeon[y][x] = FLOOR;
		}

        this->dungeon[ny][nx] = PLAYER;

        if (this->player_health<= 0) {
			std::tuple<int, int> pads = get_pads(9, 1);
			print_title("You lost!", std::get<0>(pads), std::get<1>(pads));
            exit(0);
		}
	}

	void update_player_pos(char c, int x, int y) {
		int nx = x;
		int ny = y;

		if (c == 'A') {
			ny = y-1;
			if (ny == 0) {
				if (this->dungeon[ny][nx] == EXIT) {
					std::tuple<int, int> pads = get_pads(8, 1);
					print_title("You win!", std::get<0>(pads), std::get<1>(pads));
					exit(0);
				} else {
					ny = y;
				}
			}
			if (ny < 0) {
				ny = y;
			}
		}
		if (c == 'B') {
			ny = y+1;
			if (ny == this->board_height-1) {
				if (this->dungeon[ny][nx] == EXIT) {
					std::tuple<int, int> pads = get_pads(8, 1);
					print_title("You win!", std::get<0>(pads), std::get<1>(pads));
					exit(0);
				} else {
					ny = y;
				}
			}
			if (ny > this->board_height) {
				ny = y;
			}
		}
		if (c == 'D') {
			nx = x-1;
			if (nx == 0) {
				if (this->dungeon[ny][nx] == EXIT) {
					std::tuple<int, int> pads = get_pads(8, 1);
					print_title("You win!", std::get<0>(pads), std::get<1>(pads));
					exit(0);
				} else {
					ny = y;
				}
			}
			if (nx < 0) {
				nx = x;
			}
		}
		if (c == 'C') {
			nx = x+1;
			if (nx == this->board_width) {
				if (this->dungeon[ny][nx] == EXIT) {
					exit(0);
				}
				else {
					nx = x;
				}
			}
			if (nx > this->board_width) {
				nx = x;
			}
		}

		this->player_pos = {nx, ny};
	}

	void shuffle_enemies() {
		for (int i=0;i<this->enemies.size(); i++) {
			int ex = std::get<0>(this->enemies[i]);
			int ey = std::get<1>(this->enemies[i]);
			int nex = ex+randint(-1, 1);
			int ney = ey+randint(-1, 1);

			if (nex <= 0 or nex >= this->board_width) {
				nex = ex;
			}
			if (ney <= 0 or ney >= this->board_height) {
				ney = ey;
			}
			if (this->dungeon[ney][nex] == WALL) {
				nex = ex;
				ney = ey;
			}
			this->enemies[i] = {nex, ney};
			this->dungeon[ney][nex] = ENEMY;
			this->dungeon[ey][ex] = FLOOR;
		}
	}

private:
	std::tuple<int, int> get_rand_direction(int current_x, int current_y) {
        int delta_x = randint(-1, 1);
        int delta_y = randint(-1, 1);

        int new_x = current_x + delta_x;
        int new_y = current_y + delta_y;

        if (new_x >= this->board_width-1 || new_x <= 0) {
            new_x = current_x;
		}
        if (new_y >= this->board_height-1 || new_y <= 0) {
            new_y = current_y;
		}

		return std::make_tuple(new_x, new_y);
	};

	int generate_dungeon() {
		int max_floors = this->board_width*this->board_height/2.5;
		int num_of_floors = 1;

		int x = randint(1, this->board_width-2);
		int y = randint(1, this->board_height-2);

		int old_x = x;
		int old_y = y;

		std::vector<std::tuple<int, int>> enemies;

		while (num_of_floors < max_floors) {
			std::tuple<int, int> coords = get_rand_direction(x, y);
			x = std::get<0>(coords);
			y = std::get<1>(coords);
			if (this->dungeon[y][x] == WALL) {
				this->dungeon[y][x] = FLOOR;
				if (roll_dice(3, 15) == true) {
					this->dungeon[y][x] = ENEMY;
					enemies.push_back({x, y});
				}
				num_of_floors += 1;
			}
		}

		std::tuple<int, int> exit_pos;

		for (int yy = 0; yy < this->board_height; yy++) {
			int f = 0;
			for (int xx=0; xx<this->board_width; xx++) {
				if (yy != 0 && yy != this->board_height-1) {
					if (xx != 0 && x != this->board_width-1) {
						continue;
					}
				}
				char neighbour;
				if (yy == 0) {
					neighbour = this->dungeon[yy+1][xx];
				} else if (yy == this->board_height-1) {
					neighbour = this->dungeon[yy-1][xx];
				} else if (xx == 0) {
					neighbour = this->dungeon[yy][xx+1];
				} else {
					neighbour = this->dungeon[yy][xx-1];
				}
				if (neighbour == FLOOR) {
					this->dungeon[yy][xx] = FLOOR;
					exit_pos = {xx, yy};
					f = 1;
					break;
				}
			}
			if (f == 1) {
				break;
			}
		}

		x = std::get<0>(exit_pos);
		y = std::get<1>(exit_pos);
		this->dungeon[y][x] = EXIT;
		this->dungeon[old_y][old_x] = PLAYER;

		std::tuple<int, int> player_pos(old_x, old_y);

		this->exit_pos = exit_pos;
		this->player_pos = player_pos;
		this->enemies = enemies;

		return 0;
	};
};

void print_game(Game game) {
	std::tuple<int, int> pads = get_pads(game.board_width*2, game.board_height);

	std::cout << "\033[2J";

	game.print_dungeon(std::get<0>(pads), std::get<1>(pads));
}

int run_game_loop() {
	static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);

	char c;
	Game game;
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		print_game(game);
		c = getchar();
		game.handle_event(c);
		game.shuffle_enemies();
	}
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	return 0;
}


int main() {
	run_game_loop();
	return 0;
}
