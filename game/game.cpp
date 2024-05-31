#include <iostream>
#include <random>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
//#include <cstring>
#include <chrono>
#include <thread>
#include <ncurses.h>

#include "game.h"



const char WALL_S = '.';
const char EXIT_S = '#';
const char FLOOR_S = ' ';
const char ENEMY_S = '@';
const char PLAYER_S = '%';


int randint(int from_, int to_) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(from_, to_);
	return distr(gen);
}

int gfp(std::tuple<int, int> tup, int indx) {
	if (indx == 0) {
		return std::get<0>(tup);
	}
	return std::get<1>(tup);
}

bool roll_dice(int expect, int total) {
	// подбрасывание кубика
    int roll = randint(1, total);
    return roll == expect;
}

std::tuple<int, int> get_pads(int w, int h) {
	struct winsize win;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	int pad_w, pad_h;
	int tw = win.ws_col;  // получаем ширину терминала
	int th = win.ws_row;  // получаем высоту терминала
	int bw = w; // ширина блока текста
	int bh = h; // высота блока текста

    if (tw > bw) {
		// если блок текста влезает терминал - делаем рамки по разнице между размерами текста и окна
        pad_w = (tw-bw)/2;
	} else {
        pad_w = 0;
	}

    if (th > bh) {
        pad_h = (th-bh)/2;
	} else {
        pad_h = 0;
	}

	return std::make_tuple(pad_w, pad_h);
}


void print_title(char *title) {
	// вывод текста

	// получение длина
	int title_len = sizeof(title)/sizeof(char);

	// получение отступов
	std::tuple<int, int> pads = get_pads(title_len, 1);

	int pad_x = std::get<0>(pads);
	int pad_y = std::get<1>(pads);

	// вывод отступов
	std::cout << "\033[2J";
	//clear();
	for (int i=0;i<pad_y;i++) {
		std::cout << std::endl;
	}

	// вывод отступов
	for (int i=0;i<pad_x;i++) {
		std::cout << " ";
	}

	std::cout << title << std::endl;

	// вывод отступов
	for (int i=0;i<pad_y;i++) {
		std::cout << std::endl;
	}

	//sleep_mil(10);
}


void sleep_mil(int millis) {
	std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}


Entity::Entity() {
	pos_x = 0;
	pos_y = 0;
}
Entity::Entity(int px, int py) {
	pos_x = px;
	pos_y = py;
}
Entity::~Entity() {}

void Entity::update_position(int new_x, int new_y, char **map, int map_height, int map_width) {
	if ((new_x >= map_width) || (new_x <= 0)) {
		return;
	}
	if ((new_y >= map_height) || (new_y <= 0)) {
		return;
	}

	this->pos_x = new_x;
	this->pos_y = new_y;
}

bool Entity::is_entity_by_symbol(char symbol) {
	if (symbol == Entity::symbol) {
		return true;
	}
	return false;
}


std::tuple<int, int> Enemy::generate_random_position() {
	int new_x = this->pos_x + randint(-1, 1);
	int new_y = this->pos_y + randint(-1, 1);
	// случайная позиция = любая случайная следующая ячейка вокруг текущей
	// . . .
	// . x .
	// . . .
	return std::make_tuple(new_x, new_y);
}

void Enemy::update_position(int new_x, int new_y, char **map, int map_height, int map_width) {
	// проверка на попадание на границы поля
	if ((new_x >= map_width) || (new_x <= 0)) {
		return;
	}
	// проверка на попадание на границы поля
	if ((new_y >= map_height) || (new_y <= 0)) {
		return;
	}

	char element = map[new_y][new_x];

	if (element != WALL_S) {
		this->pos_x = new_x;
		this->pos_y = new_y;
	}
}


Player::Player() {
	pos_x = 0;
	pos_y = 0;
}

Player::Player(int px, int py) {
	pos_x = px;
	pos_y = py;
}

bool Player::is_dead() {
	return this->health == 0;
}

void Player::reduce_health(int delta) {
	this->health -= delta;
}

void Player::update_position(int new_x, int new_y, char **map, int map_height, int map_width) {
	int x, y;

	char tile = map[new_y][new_x];

	if ((new_x >= map_width) || (new_x <= 0)) {
		if (tile == EXIT_S) {
			endwin();
			print_title((char *)"You win!");
			exit(0);
		} else {
			return;
		}
	}
	if ((new_y >= map_height) || (new_y <= 0)) {
		if (tile == EXIT_S) {
			endwin();
			print_title((char *)"You win!");
			exit(0);
		} else {
			return;
		}
	}

	
	if (tile == WALL_S) {
		return;
	} else if (tile == ENEMY_S) {
		x = this->pos_x;
		y = this->pos_y;
		map[y][x] = Enemy::symbol;

		reduce_health();

		if (is_dead() == true) {
			endwin();
			print_title((char *)"You lost!");
			exit(0);
		}
	} else {
		x = this->pos_x;
		y = this->pos_y;
		map[y][x] = FLOOR_S;
	}

	this->pos_x = new_x;
	this->pos_y = new_y;

	map[this->pos_y][this->pos_x] = PLAYER_S;

}


Game::Game() {
	board_width = randint(15, 20);
	board_height = randint(30, 40);
	clock = 0;

	dungeon = new char*[board_height];

	for (int i=0;i<board_height;i++) {
		dungeon[i] = new char[board_width];
	}

	// создаем матрицу поля из стен
	for (int i=0;i<board_height;i++) {
		for (int j=0;j<board_width;j++) {
			dungeon[i][j] = WALL_S;
		}
	}

	// заполняем поле сущностями
	fill_dungeon();
}
Game::~Game() {}

void Game::print_dungeon() {
	std::tuple<int,int> pads = get_pads(this->board_width*2, this->board_height);

	int pad_x = gfp(pads, 0);
	int pad_y = gfp(pads, 1);

	erase();

	for (int i=0;i<pad_y;i++) {
		printw("\n");
	}
	
	for (int i=0;i<this->board_height;i++) {
		for (int k=0; k < pad_x; k++) {
			printw(" ");
		}
		for (int j=0;j<this->board_width;j++) {
			printw(" %c", this->dungeon[i][j]);
		}
		printw("\n");
	}

	printw("\n");
	for (int k=0; k < pad_x; k++) {
		printw(" ");
	}

	printw("Health: %d\n", this->player.health);
	for (int i=0;i<pad_y;i++) {
		printw("\n");
	}
}

void Game::handle_event(char pressed_c) {
	print_dungeon();

	if (pressed_c == 'q') {
		endwin();
		exit(0);
	}

	std::tuple<int, int> new_pos = get_player_move_by_key(pressed_c);
	
	int nx = gfp(new_pos, 0);
	int ny = gfp(new_pos, 1);

	this->player.update_position(
		nx, ny,
		this->dungeon,
		this->board_height, this->board_width
	);

	this->clock += 1;
	if (this->clock == 600) {
		shuffle_enemies();
		this->clock = 0;
	}
}

std::tuple<int, int> Game::get_player_move_by_key(char c) {
	if (c == 'A') {
		return std::make_tuple(this->player.pos_x, this->player.pos_y-1);
	} else if (c == 'B') {
		return std::make_tuple(this->player.pos_x, this->player.pos_y+1);
	} else if (c == 'D') {
		return std::make_tuple(this->player.pos_x-1, this->player.pos_y);
	} else if (c == 'C') {
		return std::make_tuple(this->player.pos_x+1, this->player.pos_y);
	} else {
		return std::make_tuple(this->player.pos_x, this->player.pos_y);
	}
}

void Game::shuffle_enemies() {
	for (int i=0;i<this->enemies.size(); i++) {
		// текущая позиция врага
		int ex = gfp(this->enemies[i], 0);
		int ey = gfp(this->enemies[i], 1);

		Enemy e(ex, ey);

		std::tuple<int, int> new_enemy_pos = e.generate_random_position();

		e.update_position(
			gfp(new_enemy_pos, 0),
			gfp(new_enemy_pos, 1),
			this->dungeon,
			this->board_height,
			this->board_width
		);


		// если позиция помнялась - перемещаем врага, обновляем прошлую его позицию на пол
		if (ex != e.pos_x || ey != e.pos_y) {
			this->enemies[i] = {e.pos_x, e.pos_y};
			this->dungeon[e.pos_y][e.pos_x] = ENEMY_S;
			this->dungeon[ey][ex] = FLOOR_S;
		}
	}
}

std::tuple<int, int> Game::get_rand_direction(int current_x, int current_y) {
	int delta_x = randint(-1, 1);
	int delta_y = randint(-1, 1);

	int new_x = current_x + delta_x;
	int new_y = current_y + delta_y;

	// если не попадаем на границу - меняем коодинату
	if (new_x >= this->board_width-1 || new_x <= 0) {
		new_x = current_x;
	}
	if (new_y >= this->board_height-1 || new_y <= 0) {
		new_y = current_y;
	}

	return std::make_tuple(new_x, new_y);
};

int Game::fill_dungeon() {
	// максимальное число пустых клеток на поле
	int max_floors = this->board_width*this->board_height/2.5;
	// счетчик выставленных пустых клеток (полов)
	int num_of_floors = 1;

	// начальная точка генерации поля
	int x = randint(1, this->board_width-2);
	int y = randint(1, this->board_height-2);

	int old_x = x;
	int old_y = y;

	std::vector<std::tuple<int, int>> enemies;

	// пока счетчик не равен максимальному числу полов
	//  генерируем случайную позицию
	//  если на позиции стена - заменяем на пол
	while (num_of_floors < max_floors) {
		std::tuple<int, int> coords = get_rand_direction(x, y);

		x = gfp(coords, 0);
		y = gfp(coords, 1);

		if (this->dungeon[y][x] == WALL_S) {
			this->dungeon[y][x] = FLOOR_S;

			// если на 15-ти-граннике выпала тройка - ставим врага в точку и добавляем в список
			if (roll_dice(3, 15) == true) {
				this->dungeon[y][x] = ENEMY_S;
				enemies.push_back({x, y});
			}

			num_of_floors += 1;
		}
	}

	std::tuple<int, int> exit_pos;

	// проходимся по границам карты
	// если в соседней внутренней ячейке пол - ставим дверь и завершаем циклы
	for (int yy = 0; yy < this->board_height; yy++) {
		int f = 0;
		for (int xx=0; xx<this->board_width; xx++) {
			if (yy != 0 && yy != this->board_height-1) {
				// . . . .
				// . x x . <-- мы тут => пропускаем, т.к. не границы
				// . . . . 
				if (xx != 0 && x != this->board_width-1) {
					continue;
				}
			}
			// соседняя не граничная ячейка
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
			if (neighbour == FLOOR_S) {
				this->dungeon[yy][xx] = FLOOR_S;
				exit_pos = {xx, yy};
				f = 1;
				break;
			}
		}
		if (f == 1) {
			break;
		}
	}

	x = gfp(exit_pos, 0);
	y = gfp(exit_pos, 1);

	this->dungeon[y][x] = EXIT_S;
	this->dungeon[old_y][old_x] = PLAYER_S;

	std::tuple<int, int> player_pos(old_x, old_y);

	this->exit_door = exit_pos;
	this->player = Player(gfp(player_pos, 0), gfp(player_pos, 1));
	this->enemies = enemies;

	return 0;
};

Screen::Screen(Game g) {
	game = g;
}

void Screen::print_game() {
	std::cout << "\033[2J";
	this->game.print_dungeon();
}


int run_game_loop() {
	// предварительные настройки ncurses для печати и захвата клавиш
	initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

	Game game;
	Screen screen(game);

	while (true) {
		game.handle_event(getch());

	}
	return 0;
}
