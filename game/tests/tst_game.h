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

	Player p(2, 2);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('A');

	ASSERT_EQ(map_[1][2], '%');
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

	Player p(2, 2);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('A');

	ASSERT_EQ(g.dungeon[1][2], '.');
	ASSERT_EQ(g.dungeon[2][2], '%');
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

	Player p(2, 1);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;
	g.exit_door = {2, 0};

	g.handle_event('A');

	ASSERT_EQ(g.dungeon[0][2], '%');
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

	Player p(2, 2);

	Game g{};
	g.dungeon = map_;
	g.board_width = 5;
	g.board_height = 5;
	g.player = p;

	g.handle_event('g');

	ASSERT_EQ(g.dungeon[1][2], '.');
	ASSERT_EQ(g.dungeon[2][2], '%');
}
