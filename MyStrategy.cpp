#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>

struct VehicleRealInfo
{
	long long id;
	long long pid;
	model::VehicleType type;
	double x;
	double y;
	int durability;
};


std::pair<double, double> GetCenter(int pid, std::vector<VehicleRealInfo> const& vehicles, model::VehicleType type)
{
	int count = 0;
	std::pair<double, double> result;

	for (auto const& v : vehicles)
	{
		if (v.pid != pid)
			continue;
		if (v.durability == 0)
			continue;
		if (v.type == type)
		{
			count++;
			result.first += v.x * v.x;
			result.second += v.y * v.y;
		}
	}
	if (count == 0)
		return { 0.0, 0.0 };
	return { sqrt(result.first / (double)count), sqrt(result.second / (double)count) };
}

void MyStrategy::move(model::Player const& me, model::World const& world, model::Game const& game, model::Move & move)
{
	static std::vector<VehicleRealInfo> vehicles;
	static bool rush = false;
	static std::pair<double, double> arrv_pos;

	if (world.getTickIndex() == 0)
	{
		for (auto const& v : world.getNewVehicles())
			vehicles.push_back({ v.getId(), v.getPlayerId(), v.getType(), v.getX(), v.getY(), v.getDurability() });

		arrv_pos = GetCenter(me.getId(), vehicles, model::VehicleType::VEHICLE_ARRV);
	}

	for (auto const& u : world.getVehicleUpdates())
		for (auto & v : vehicles)
			if (u.getId() == v.id)
			{
				v.x = u.getX();
				v.y = u.getY();
				v.durability = u.getDurability();
			}


	if (world.getTickIndex() < 200)
	{
		if (world.getTickIndex() == 0)
		{
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setLeft(0.0);
			move.setRight(0.0);
			move.setRight(game.getWorldWidth());
			move.setBottom(game.getWorldHeight());
		}
		else if (world.getTickIndex() == 1)
		{
			move.setAction(model::ActionType::ACTION_MOVE);
			move.setX(128.0 + 32.0 - arrv_pos.first);
			move.setY(128.0 + 32.0 - arrv_pos.second);
			move.setMaxSpeed(game.getTankSpeed() * 0.6);
		}
		if (world.getTickIndex() == 199)
			arrv_pos = GetCenter(me.getId(), vehicles, model::VehicleType::VEHICLE_ARRV);
		return;
	}

	if (world.getTickIndex() < 300)
	{
		if (world.getTickIndex() == 200)
		{
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setLeft(0.0);
			move.setRight(0.0);
			move.setRight(game.getWorldWidth());
			move.setBottom(game.getWorldHeight());
		}
		else if (world.getTickIndex() == 201)
		{
			move.setAction(model::ActionType::ACTION_ROTATE);
			move.setX(arrv_pos.first);
			move.setY(arrv_pos.second);
			move.setAngle(PI);
		}
		return;
	}

	if (world.getTickIndex() == 10000 && me.getScore() == 0)
		rush = true;

	if (rush)
	{
		if (world.getTickIndex() == 10061)
		{
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setRight(game.getWorldWidth());
			move.setBottom(game.getWorldHeight());
		}
		else if (world.getTickIndex() == 10062)
		{
			move.setAction(model::ActionType::ACTION_MOVE);
			move.setX(game.getWorldWidth() - 64.0 - arrv_pos.first);
			move.setY(game.getWorldHeight() - 64.0 - arrv_pos.second);
			move.setMaxSpeed(game.getTankSpeed() * 0.6);
		}
		return;
	}

	if ((world.getTickIndex() % 20) == 0)
	{
		switch (((world.getTickIndex() / 20 + 4) % 6))
		{
		case 0:
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setLeft(0.0);
			move.setTop(0.0);
			move.setRight(arrv_pos.first);
			move.setBottom(arrv_pos.second);
			break;
		case 1:
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setLeft(0.0);
			move.setTop(arrv_pos.second);
			move.setRight(arrv_pos.first);
			move.setBottom(game.getWorldHeight());
			break;
		case 2:
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setLeft(arrv_pos.first);
			move.setTop(0.0);
			move.setRight(game.getWorldWidth());
			move.setBottom(arrv_pos.second);
			break;
		case 3:
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setLeft(arrv_pos.first);
			move.setTop(arrv_pos.second);
			move.setRight(game.getWorldWidth());
			move.setBottom(game.getWorldHeight());
			break;
		case 4:
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setLeft(0.0);
			move.setTop(0.0);
			move.setRight(game.getWorldWidth());
			move.setBottom(game.getWorldHeight());
			break;
		case 5:
			move.setAction(model::ActionType::ACTION_NONE);
			break;
		}
	}

	/*if ((world.getTickIndex() % 30) == 1)
	{
	move.setAction(model::ActionType::ACTION_ROTATE);
	move.setX(64.0 + 32.0);
	move.setY(64.0 + 32.0);
	if ((world.getTickIndex() / 30) % 2 == 0)
	move.setAngle(PI);
	else
	move.setAngle(-PI);
	}*/

	if ((world.getTickIndex() % 20) == 1)
	{
		switch (((world.getTickIndex() / 20 + 4) % 6))
		{
		case 0:
			move.setAction(model::ActionType::ACTION_MOVE);
			move.setX(64.0);
			move.setY(64.0);
			break;
		case 1:
			move.setAction(model::ActionType::ACTION_MOVE);
			move.setX(64.0);
			move.setY(-64.0);
			break;
		case 2: 
			move.setAction(model::ActionType::ACTION_MOVE);
			move.setX(-64.0);
			move.setY(64.0);
			break;
		case 3: 
			move.setAction(model::ActionType::ACTION_MOVE);
			move.setX(-64.0);
			move.setY(-64.0);
			break;
		case 4:
			move.setAction(model::ActionType::ACTION_ROTATE);
			move.setX(arrv_pos.first);
			move.setY(arrv_pos.second);
			move.setAngle(PI);
			break;
		case 5:
			move.setAction(model::ActionType::ACTION_NONE);
			break;
		}
	}
}

MyStrategy::MyStrategy()
{
}