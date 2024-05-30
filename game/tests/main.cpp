#include <gtest/gtest.h>
#include "tst_game.h"
#include "../game.h"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
