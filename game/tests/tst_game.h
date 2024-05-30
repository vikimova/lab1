#include <gtest/gtest.h>

#include "../game.h"

TEST(playerIsDead, arg) {
	Player p(0, 0);
    ASSERT_EQ(p.is_dead(), false);
}
