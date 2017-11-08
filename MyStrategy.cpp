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


std::pair<int, int> GetSquare(int pid, std::vector<VehicleRealInfo> const& vehicles, model::VehicleType type)
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
			result.first += v.x;
			result.second += v.y;
		}
	}
	if (count == 0)
		return { 0, 0 };
	return { (int)((result.first / (double)count) / 64.0), (int)((result.second / (double)count) / 64.0) };
}

void MyStrategy::move(model::Player const& me, model::World const& world, model::Game const& game, model::Move & move)
{
	static std::vector<VehicleRealInfo> vehicles;
	static bool rush = false;

	if (world.getTickIndex() == 0)
	{
		for (auto const& v : world.getNewVehicles())
			vehicles.push_back({ v.getId(), v.getPlayerId(), v.getType(), v.getX(), v.getY(), v.getDurability() });
	}

	for (auto const& u : world.getVehicleUpdates())
		for (auto & v : vehicles)
			if (u.getId() == v.id)
			{
				v.x = u.getX();
				v.y = u.getY();
				v.durability = u.getDurability();
			}

	if (world.getTickIndex() == 10001 && me.getScore() == 0)
		rush = true;

	if (rush)
	{
		if (world.getTickIndex() == 10001)
		{
			move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
			move.setRight(game.getWorldWidth());
			move.setBottom(game.getWorldHeight());
		}
		else if (world.getTickIndex() == 10002)
		{
			move.setAction(model::ActionType::ACTION_MOVE);
			move.setX(game.getWorldWidth());
			move.setY(game.getWorldHeight());
			move.setMaxSpeed(game.getTankSpeed());
		}
		return;
	}

	if ((world.getTickIndex() % 30) == 0)
	{
		move.setAction(model::ActionType::ACTION_CLEAR_AND_SELECT);
		move.setRight(game.getWorldWidth());
		move.setBottom(game.getWorldHeight());
		switch (((world.getTickIndex() / 30) % 5))
		{
		case 0: move.setVehicleType(model::VehicleType::VEHICLE_IFV); break;
		case 1: move.setVehicleType(model::VehicleType::VEHICLE_TANK); break;
		case 2: move.setVehicleType(model::VehicleType::VEHICLE_ARRV); break;
		case 3: move.setVehicleType(model::VehicleType::VEHICLE_HELICOPTER); break;
		case 4: move.setVehicleType(model::VehicleType::VEHICLE_FIGHTER); break;
		}
	}

	if ((world.getTickIndex() % 30) == 1)
	{
		move.setAction(model::ActionType::ACTION_ROTATE);
		move.setX(64.0);
		move.setY(64.0);
		if ((world.getTickIndex() / 30) % 2 == 0)
			move.setAngle(PI);
		else
			move.setAngle(-PI);
	}

	if ((world.getTickIndex() % 30) == 29)
	{
		move.setAction(model::ActionType::ACTION_MOVE);
		std::pair<int, int> tmp;
		switch (((world.getTickIndex() / 30) % 5))
		{
		case 0:
			tmp = GetSquare(me.getId(), vehicles, model::VehicleType::VEHICLE_IFV);
			move.setX((0 - tmp.first) * 64.0);
			move.setY((0 - tmp.second) * 64.0);
			break;
		case 1:
			tmp = GetSquare(me.getId(), vehicles, model::VehicleType::VEHICLE_TANK);
			move.setX((0 - tmp.first) * 64.0);
			move.setY((0 - tmp.second) * 64.0);
			break;
		case 2: 
			tmp = GetSquare(me.getId(), vehicles, model::VehicleType::VEHICLE_ARRV);
			move.setX((0 - tmp.first) * 64.0);
			move.setY((0 - tmp.second) * 64.0);
			break;
		case 3: 
			tmp = GetSquare(me.getId(), vehicles, model::VehicleType::VEHICLE_HELICOPTER);
			move.setX((0 - tmp.first) * 64.0);
			move.setY((0 - tmp.second) * 64.0);
			break;
		case 4:
			tmp = GetSquare(me.getId(), vehicles, model::VehicleType::VEHICLE_FIGHTER);
			move.setX((0 - tmp.first) * 64.0);
			move.setY((0 - tmp.second) * 64.0);
			break;
		}
	}
}

MyStrategy::MyStrategy()
{
}
