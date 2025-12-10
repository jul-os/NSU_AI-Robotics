#include <gtest/gtest.h>
#include "GameField.hpp"
#include "Robot.hpp"
#include "RobotFactory.hpp"
#include "Position.hpp"

class GameFieldTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        field = std::make_unique<GameField>(5, 5);
    }

    std::unique_ptr<GameField> field;
};

TEST_F(GameFieldTest, Initialization)
{
    EXPECT_EQ(field->GetWidth(), 5);
    EXPECT_EQ(field->GetHeight(), 5);
    EXPECT_EQ(field->GetCoinCount(), 0);
}

TEST_F(GameFieldTest, PlaceCoins)
{
    field->PlaceCoinsRandomly(3);
    EXPECT_EQ(field->GetCoinCount(), 3);
}

TEST_F(GameFieldTest, InvalidPosition)
{
    Position invalid{-1, -1};
    EXPECT_FALSE(field->IsValidPosition(invalid));
}

TEST_F(GameFieldTest, CoinCollection)
{
    Position pos{1, 1};
    field->PlaceCoinsRandomly(1);

    // Симулируем сбор монеты
    if (field->HasCoin(pos))
    {
        field->RemoveCoin(pos);
    }

    EXPECT_FALSE(field->HasCoin(pos));
}

// Тесты для роботов
class RobotTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        field = std::make_unique<GameField>(5, 5);
        field->PlaceCoinsRandomly(5);
    }

    std::unique_ptr<GameField> field;
    std::vector<Robot *> allRobots; // Заглушка для GameState
};

TEST_F(RobotTest, RandomRobotCreation)
{
    auto robot = RobotFactory::CreateRobot("RandomRobot");
    EXPECT_EQ(robot->GetName(), "RandomRobot");
    EXPECT_EQ(robot->GetCoinsCollected(), 0);
}

TEST_F(RobotTest, GreedyRobotStrategy)
{
    auto robot = RobotFactory::CreateRobot("GreedyRobot");
    Position startPos{0, 0};
    robot->SetPosition(startPos);

    // Создаем mock GameState
    std::vector<Robot *> robots = {robot.get()};
    GameState state(*field, robots, *robot, 1, 10);

    Direction move = robot->DecideMove(state);
    EXPECT_NE(move, Direction::STAY); // GreedyRobot должен пытаться двигаться к монетам
}

TEST_F(RobotTest, RobotMovement)
{
    auto robot = RobotFactory::CreateRobot("RandomRobot");
    Position startPos{2, 2};
    robot->SetPosition(startPos);

    std::vector<Robot *> robots = {robot.get()};
    GameState state(*field, robots, *robot, 1, 10);

    // Тестируем перемещение
    robot->Move(Direction::RIGHT, state);
    Position newPos = robot->GetPosition();

    EXPECT_EQ(newPos.x, 3);
    EXPECT_EQ(newPos.y, 2);
}

// Тесты для фабрики

TEST(RobotFactoryTest, CreateUnknownStrategy)
{
    EXPECT_THROW(RobotFactory::CreateRobot("UnknownRobot"), std::invalid_argument);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}