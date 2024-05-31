#include <tuple>
#include <vector>

// функция получения случйного целого в диапазоне
int randint(int from_, int to_);

// get from pair - функция получения элемента двумерного кортежа
int gfp(std::tuple<int, int> tup, int indx);

// функция иммитации подбрасывания кубика - нужна для генерации врагов с определенной вероятностью в случайные позиции
bool roll_dice(int expect, int total);

// функция получения отступов для блока текста в терминале
std::tuple<int, int> get_pads(int w, int h);

// функция вывода заголовка в терминал
void print_title(char *title);

// функция sleep в миллисекундах
void sleep_mil(int millis);

#ifndef ENTITY
#define ENTITY

// класс сущности
class Entity {
public:
	explicit Entity();
	explicit Entity(int px, int py);
	~Entity();

	static const char symbol = '?';
	// символ сущности на игровом поле

	int pos_x;
	int pos_y;

	void update_position(int new_x, int new_y, char **map, int map_height, int map_width);

	static bool is_entity_by_symbol(char symbol);
	
};

#endif // !ENTITY


#ifndef WALL
#define WALL

class Wall : public Entity {
public:
	static const char symbol = '.';

	using Entity::Entity;
};
#endif // !WALL

#ifndef DOOR
#define DOOR
class Door : public Entity {
public:
	static const char symbol = '#';

	using Entity::Entity;
};
#endif // !DOOR


#ifndef ENEMY
#define ENEMY
class Enemy : public Entity {
public:
	static const char symbol = '@';

	using Entity::Entity;


	std::tuple<int, int> generate_random_position();

	void update_position(int new_x, int new_y, char **map, int map_height, int map_width);
};
#endif // !ENEMY

#ifndef PLAYER
#define PLAYER

class Player : public Entity {
public:
	static const char symbol = '%';

	int health = 100;

	Player();

	Player(int px, int py);

	bool is_dead();

	void reduce_health(int delta = 5);

	void update_position(int new_x, int new_y, char **map, int map_height, int map_width);
};
#endif // !PLAYER


#ifndef GAME
#define GAME
class Game {
public:
	int board_width;
	int board_height;

	char **dungeon;

	std::vector<std::tuple<int, int>> enemies;
	std::tuple<int, int> exit_door;

	Player player;

	Game();
	~Game();

	void print_dungeon();

	void handle_event(char pressed_c);

	std::tuple<int, int> get_player_move_by_key(char c);

	void shuffle_enemies();

private:
	std::tuple<int, int> get_rand_direction(int current_x, int current_y);

	int fill_dungeon();
};
#endif // !GAME

#ifndef SCREEN
#define SCREEN
class Screen {
public:
	Game game;

	Screen(Game g);

	void print_game();
};
#endif // !SCREEN

int run_game_loop();
