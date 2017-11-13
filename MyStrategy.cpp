#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>

class CMove : public model::Move
{
public:
	int m_wait;
	CMove() : model::Move(), m_wait(0) {}
};

std::pair<double, double> GetCenter(int pid, std::vector<model::Vehicle> const& vehicles, model::VehicleType type)
{
	int count = 0;
	std::pair<double, double> result;

	for (auto const& v : vehicles)
	{
		if (v.getPlayerId() != pid)
			continue;
		if (v.getDurability() == 0)
			continue;
		if (v.getType() == type)
		{
			count++;
			result.first += v.getX() * v.getX();
			result.second += v.getY() * v.getY();
		}
	}
	if (count == 0)
		return { 0.0, 0.0 };
	return { sqrt(result.first / (double)count), sqrt(result.second / (double)count) };
}

void MyStrategy::move(model::Player const& me, model::World const& world, model::Game const& game, model::Move & new_move)
{
	static std::vector<model::Vehicle> vehicles;
	static std::vector<CMove> moves = { CMove() };
	static int current_move = 1;
	static int last_move_time = 0;

	// update vehicles list

	for (auto const& v : world.getNewVehicles())
		vehicles.push_back(v);

	for (auto const& u : world.getVehicleUpdates())
		for (auto & v : vehicles)
			if (u.getId() == v.getId())
				v = model::Vehicle(v, u);

	// get move

	if (last_move_time + moves[current_move - 1].m_wait >= world.getTickIndex())
	{
		if ((int)moves.size() > current_move && me.getRemainingActionCooldownTicks() == 0)
		{
			auto const& mv = moves[current_move];
			new_move.setAction(mv.getAction());
			new_move.setAngle(mv.getAngle());
			new_move.setBottom(mv.getBottom());
			new_move.setFacilityId(mv.getFacilityId());
			new_move.setGroup(mv.getGroup());
			new_move.setLeft(mv.getLeft());
			new_move.setMaxAngularSpeed(mv.getMaxAngularSpeed());
			new_move.setMaxSpeed(mv.getMaxSpeed());
			new_move.setRight(mv.getRight());
			new_move.setTop(mv.getTop());
			new_move.setVehicleType(mv.getVehicleType());
			new_move.setX(mv.getX());
			new_move.setY(mv.getY());
			
			current_move++;
			last_move_time = world.getTickIndex();
		}
	}

	CMove move;

	// begin of strategy



	// end of strategy

	// save move

	if (move.getAction() != model::ActionType::ACTION_NONE)
		moves.push_back(move);
}

MyStrategy::MyStrategy()
{
}