#include <gtest/gtest.h>

#include "../game.h"


TEST(playerIsDeadCheck, isAlivePositive) {
	Player p(0, 0);
    ASSERT_EQ(p.is_dead(), false);
}

TEST(playerIsDeadCheck, isDeadPositive) {
	Player p(0, 0);
	p.reduce_health(100);
    ASSERT_EQ(p.is_dead(), true);
}

TEST(playerMoves, NothingOnTile) {
	char **map_ = new char*[5];

	for (int i=0;i<5;i++) {
		map_[i] = new char[5];
	}

	for (int i=0;i<5;i++) {
		for (int j=0;j<5;j++) {
			map_[i][j] = '.';
		}
	}

	map_[0][2] = '#';
	map_[1][2] = ' ';
	map_[2][2] = '%';

	// делаем карту 5 на 5
	// . . # . . - дверь
	// . .   . . - пол
	// . . % . . - игрок
	// . . . . .
	// . . . . .

	Player p(2, 2);

	Game g{};
	// меняем сгенерированные значения на наши
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('A');

	ASSERT_EQ(map_[1][2], '%');
	// проверяем на перемещение
	// . . # . . - дверь
	// . . % . . - пол
	// . .   . . - игрок
	// . . . . .
	// . . . . .
}

TEST(playerMoves, WallOnTile) {
	char **map_ = new char*[5];

	for (int i=0;i<5;i++) {
		map_[i] = new char[5];
	}

	for (int i=0;i<5;i++) {
		for (int j=0;j<5;j++) {
			map_[i][j] = '.';
		}
	}

	map_[0][2] = '#';
	map_[1][2] = '.';
	map_[2][2] = '%';

	// делаем карту 5 на 5
	// . . # . . - дверь
	// . . . . . - стена
	// . . % . . - игрок
	// . . . . .
	// . . . . .

	Player p(2, 2);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('A');

	ASSERT_EQ(g.dungeon[1][2], '.');
	ASSERT_EQ(g.dungeon[2][2], '%');
	// проверяем на не перемещение игрока
	// . . # . . - дверь
	// . . . . . - стена
	// . . % . . - игрок
	// . . . . .
	// . . . . .
}

TEST(playerMoves, EnemyOnTile) {
	char **map_ = new char*[5];

	for (int i=0;i<5;i++) {
		map_[i] = new char[5];
	}

	for (int i=0;i<5;i++) {
		for (int j=0;j<5;j++) {
			map_[i][j] = '.';
		}
	}

	map_[0][2] = '#';
	map_[1][2] = '@';
	map_[2][2] = '%';

	// делаем карту 5 на 5
	// . . # . . - дверь
	// . . @ . . - враг
	// . . % . . - игрок
	// . . . . .
	// . . . . .

	Player p(2, 2);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('A');

	ASSERT_EQ(g.dungeon[1][2], '%');
	ASSERT_EQ(g.dungeon[2][2], '@');
	ASSERT_EQ(g.player.health, 95);
	// проверяем что игрок поменялся с врагом местами и получил урон
	// . . # . . - дверь
	// . . % . . - игрок
	// . . @ . . - враг
	// . . . . .
	// . . . . .

}


TEST(playerMoves, DoorOnTile) {
	char **map_ = new char*[5];

	for (int i=0;i<5;i++) {
		map_[i] = new char[5];
	}

	for (int i=0;i<5;i++) {
		for (int j=0;j<5;j++) {
			map_[i][j] = '.';
		}
	}

	map_[0][2] = '#';
	map_[1][2] = '%';
	map_[2][2] = ' ';

	// делаем карту 5 на 5
	// . . # . . - дверь
	// . . % . . - игрок
	// . .   . . 
	// . . . . .
	// . . . . .

	Player p(2, 1);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;
	g.exit_door = {2, 0};

	g.handle_event('A');

	ASSERT_EQ(g.dungeon[0][2], '%');
	// проверяем что игрок вышел
	// . . % . . - игрок
	// . .   . .
	// . .   . . 
	// . . . . .
	// . . . . .

}


TEST(playerMoves, NotAnArrowKey) {
	char **map_ = new char*[5];

	for (int i=0;i<5;i++) {
		map_[i] = new char[5];
	}

	for (int i=0;i<5;i++) {
		for (int j=0;j<5;j++) {
			map_[i][j] = '.';
		}
	}

	map_[0][2] = '#';
	map_[1][2] = '.';
	map_[2][2] = '%';
	
	// делаем карту 5 на 5
	// . . # . . - дверь
	// . . . . . - стена
	// . . % . . - игрок
	// . . . . .
	// . . . . .

	Player p(2, 2);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('g');

	ASSERT_EQ(g.dungeon[1][2], '.');
	ASSERT_EQ(g.dungeon[2][2], '%');
	// проверяем на игнорирование случайной клавиши
	// . . # . . - дверь
	// . . . . . - стена
	// . . % . . - игрок
	// . . . . .
	// . . . . .
}


TEST(playerMoves, InvalidSymbol) {
	char **map_ = new char*[5];

	for (int i=0;i<5;i++) {
		map_[i] = new char[5];
	}

	for (int i=0;i<5;i++) {
		for (int j=0;j<5;j++) {
			map_[i][j] = '.';
		}
	}

	map_[0][2] = '?';
	map_[1][2] = '?';
	map_[2][2] = '%';
	
	// делаем карту 5 на 5
	// . . ? . . - случайный символ
	// . . ? . . - случайный символ
	// . . % . . - игрок
	// . . . . .
	// . . . . .

	Player p(2, 2);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('A');
	ASSERT_EQ(g.dungeon[1][2], '?');
	ASSERT_EQ(g.dungeon[2][2], '%');
}
