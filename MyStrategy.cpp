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

void CheckField(std::vector<std::vector<bool>> & field, std::pair<double, double> pos)
{
	field[(int)(pos.second / 72.0)][(int)(pos.first / 72.0)] = true;
}

int GetStartMove(std::vector<std::vector<bool>> & field, std::pair<double, double> pos)
{
	int X = (int)(pos.first / 72.0);
	int Y = (int)(pos.second / 72.0);

	switch (X)
	{
	case 0:
		switch (Y)
		{
		case 0:
			if (field[1][X])
			{
				if (field[Y][X + 1])
				{
					return 0; // wait
				}
				else
				{
					field[Y][X + 1] = true;
					return 3; // right
				}
			}
			else
			{
				field[1][X] = true;
				return 4; // down
			}
			break;
		case 1:
			return 0; // nothing
			break;
		case 2:
			if (field[1][X])
			{
				if (field[Y][X + 1])
				{
					return 0; // wait
				}
				else
				{
					field[Y][X + 1] = true;
					return 3; // right
				}
			}
			else
			{
				field[1][X] = true;
				return 2; // up
			}
			break;
		}
		break;
	case 1:
		switch (Y)
		{
		case 0:
			if (field[1][X])
			{
				if (field[1][X - 1])
				{
					if (field[Y][X + 1])
					{
						return 0; // wait
					}
					else
					{
						field[Y][X + 1] = true;
						return 3; // right
					}
				}
				else
				{
					if (field[Y][X - 1])
					{
						if (field[Y][X + 1])
						{
							return 0; // wait
						}
						else
						{
							field[Y][X + 1] = true;
							return 3; // right
						}
					}
					else
					{
						field[Y][X - 1] = true;
						return 1; // left
					}
				}
			}
			else
			{
				field[1][X] = true;
				return 4; // down
			}
			break;
		case 1:
			return 0; // nothing
			break;
		case 2:
			if (field[1][X])
			{
				if (field[1][X + 1])
				{
					if (field[Y][X - 1])
					{
						return 0; // wait
					}
					else
					{
						field[Y][X - 1] = true;
						return 1; // left
					}
				}
				else
				{
					if (field[Y][X + 1])
					{
						if (field[Y][X - 1])
						{
							return 0; // wait
						}
						else
						{
							field[Y][X - 1] = true;
							return 1; // left
						}
					}
					else
					{
						field[Y][X + 1] = true;
						return 3; // right
					}
				}
			}
			else
			{
				field[1][X] = true;
				return 2; // up
			}
			break;
		}
		break;
	case 2:
		switch (Y)
		{
		case 0:
			if (field[1][X])
			{
				if (field[Y][X - 1])
				{
					return 0; // wait
				}
				else
				{
					field[Y][X - 1] = true;
					return 1; // left
				}
			}
			else
			{
				field[1][X] = true;
				return 4; // down
			}
			break;
		case 1:
			return 0; // nothing
			break;
		case 2:
			if (field[1][X])
			{
				if (field[Y][X - 1])
				{
					return 0; // wait
				}
				else
				{
					field[Y][X - 1] = true;
					return 1; // left
				}
			}
			else
			{
				field[1][X] = true;
				return 2; // up
			}
			break;
		}
		break;
	}
	return 0;
}

void DoStartMove(model::Game const& game, std::vector<CMove> & moves, model::VehicleType type, int dir)
{
	switch (dir)
	{
	case 0:
		return;
	case 1:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(-74.0);
			move_move.setY(0.0);
			moves.push_back(move_move);
		}
		return;
	case 2:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(0.0);
			move_move.setY(-74.0);
			moves.push_back(move_move);
		}
		return;
	case 3:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(74.0);
			move_move.setY(0.0);
			moves.push_back(move_move);
		}
		return;
	case 4:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(0.0);
			move_move.setY(74.0);
			moves.push_back(move_move);
		}
		return;
	}
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

	// strategy

	static int mode = 0;

	if (mode == 0 && current_move == (int)moves.size() && last_move_time + moves[current_move - 1].m_wait <= world.getTickIndex())
	{
		std::vector<std::vector<bool>> field_g = {
			{ false, false, false },
			{ false, false, false },
			{ false, false, false }
		};

		std::vector<std::vector<bool>> field_a = {
			{ false, false, false },
			{ false, false, false },
			{ false, false, false }
		};

		auto arrv_pos = GetCenter(me.getId(), vehicles, model::VehicleType::ARRV);
		auto ifv_pos = GetCenter(me.getId(), vehicles, model::VehicleType::IFV);
		auto tank_pos = GetCenter(me.getId(), vehicles, model::VehicleType::TANK);

		auto fighter_pos = GetCenter(me.getId(), vehicles, model::VehicleType::FIGHTER);
		auto helicopter_pos = GetCenter(me.getId(), vehicles, model::VehicleType::HELICOPTER);
		
		CheckField(field_g, arrv_pos);
		CheckField(field_g, ifv_pos);
		CheckField(field_g, tank_pos);

		CheckField(field_a, fighter_pos);
		CheckField(field_a, helicopter_pos);

		if (field_g[1][0] && field_g[1][1] && field_g[1][2] && ((int)field_a[1][0] + (int)field_a[1][1] + (int)field_a[1][2]) == 2)
		{
			mode = 1;
		}
		else
		{
			auto moves_size = moves.size();

			DoStartMove(game, moves, model::VehicleType::ARRV, GetStartMove(field_g, arrv_pos));
			DoStartMove(game, moves, model::VehicleType::IFV, GetStartMove(field_g, ifv_pos));
			DoStartMove(game, moves, model::VehicleType::TANK, GetStartMove(field_g, tank_pos));

			DoStartMove(game, moves, model::VehicleType::FIGHTER, GetStartMove(field_a, fighter_pos));
			DoStartMove(game, moves, model::VehicleType::HELICOPTER, GetStartMove(field_a, helicopter_pos));

			if (moves.size() != moves_size)
				moves.back().m_wait = (int)(74.0 / (game.getTankSpeed() * 0.6) + 0.5);
		}
	}

	if (mode == 1 && current_move == (int)moves.size() && last_move_time + moves[current_move - 1].m_wait <= world.getTickIndex())
	{
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(74.0 + 7.0);
			sel_move.setBottom(game.getWorldHeight());
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(14.0);
			move_move.setY(0.0);
			moves.push_back(move_move);
		}
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(74.0 * 2.0 + 7.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(-14.0);
			move_move.setY(0.0);
			move_move.m_wait = (int)(14.0 / (game.getTankSpeed() * 0.6) + 0.5);
			moves.push_back(move_move);
		}
		mode = 2;
	}

	// get move

	if (last_move_time + moves[current_move - 1].m_wait <= world.getTickIndex())
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
}

MyStrategy::MyStrategy()
{
}