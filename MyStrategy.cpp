#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <set>
#include <map>

class CMove : public model::Move
{
public:
	bool m_wait_completion;
	CMove() : model::Move(), m_wait_completion(false) {}
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

void CheckField(std::vector<std::vector<bool>> & field, std::pair<int, int> ipos)
{
	field[ipos.first][ipos.second] = true;
}

enum class dir_enum
{
	wait,
	left,
	top,
	right,
	bottom
};

std::set<dir_enum> GetPossibleMoves(std::vector<std::vector<bool>> const& field, std::pair<int, int> ipos)
{
	std::set<dir_enum> result;

	if (ipos == std::make_pair(0, 0))
	{
		if (!field[1][0]) result.insert(dir_enum::right);
		if (!field[0][1]) result.insert(dir_enum::bottom);
	}
	if (ipos == std::make_pair(1, 0))
	{
		if (!field[0][0]) result.insert(dir_enum::left);
		if (!field[1][1]) result.insert(dir_enum::bottom);
		if (!field[2][0]) result.insert(dir_enum::right);
	}
	if (ipos == std::make_pair(2, 0))
	{
		if (!field[1][0]) result.insert(dir_enum::left);
		if (!field[2][1]) result.insert(dir_enum::bottom);
	}
	if (ipos == std::make_pair(0, 1))
	{
		if (!field[0][0]) result.insert(dir_enum::top);
		if (!field[1][1]) result.insert(dir_enum::right);
		if (!field[0][2]) result.insert(dir_enum::bottom);
	}
	if (ipos == std::make_pair(1, 1))
	{
		if (!field[0][1]) result.insert(dir_enum::left);
		if (!field[1][0]) result.insert(dir_enum::top);
		if (!field[2][1]) result.insert(dir_enum::right);
		if (!field[1][2]) result.insert(dir_enum::bottom);
	}
	if (ipos == std::make_pair(2, 1))
	{
		if (!field[1][1]) result.insert(dir_enum::left);
		if (!field[2][0]) result.insert(dir_enum::top);
		if (!field[2][2]) result.insert(dir_enum::bottom);
	}
	if (ipos == std::make_pair(0, 2))
	{
		if (!field[0][1]) result.insert(dir_enum::top);
		if (!field[1][2]) result.insert(dir_enum::right);
	}
	if (ipos == std::make_pair(1, 2))
	{
		if (!field[0][2]) result.insert(dir_enum::left);
		if (!field[1][1]) result.insert(dir_enum::top);
		if (!field[2][2]) result.insert(dir_enum::right);
	}
	if (ipos == std::make_pair(2, 2))
	{
		if (!field[1][2]) result.insert(dir_enum::left);
		if (!field[2][1]) result.insert(dir_enum::top);
	}

	return result;
}

dir_enum GetStartMove0(std::vector<std::vector<bool>> & field, std::pair<int, int> ipos, bool tank, std::map<std::pair<int, int>, dir_enum> & additional_moves)
{
	if (additional_moves.find(ipos) != additional_moves.end())
	{
		auto mv = additional_moves[ipos];
		switch (mv)
		{
		case dir_enum::left: field[ipos.first - 1][ipos.second] = true; break;
		case dir_enum::top: field[ipos.first][ipos.second - 1] = true; break;
		case dir_enum::right: field[ipos.first + 1][ipos.second] = true; break;
		case dir_enum::bottom: field[ipos.first][ipos.second + 1] = true; break;
		}
		return mv;
	}

	auto possible_moves = GetPossibleMoves(field, ipos);

	if (ipos == std::make_pair(0, 0))
	{
		if (possible_moves.find(dir_enum::bottom) != possible_moves.end())
		{
			field[0][1] = true;
			return dir_enum::bottom;
		}
		if (tank)
		{
			auto possible_moves_2 = GetPossibleMoves(field, { 0, 1 });
			if (possible_moves_2.find(dir_enum::right) != possible_moves_2.end())
			{
				additional_moves[{ 0, 1 }] = dir_enum::right;
				return dir_enum::wait;
			}
			additional_moves[{ 1, 1 }] = dir_enum::right;
			field[1][0] = true;
			return dir_enum::right;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(1, 0))
	{
		if (possible_moves.find(dir_enum::bottom) != possible_moves.end())
		{
			field[1][1] = true;
			return dir_enum::bottom;
		}
		if (tank)
		{
			auto possible_moves_2 = GetPossibleMoves(field, { 1, 1 });
			if (possible_moves_2.find(dir_enum::left) != possible_moves_2.end() && !field[0][0] && !field[0][2])
			{
				additional_moves[{ 1, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			if (possible_moves_2.find(dir_enum::right) != possible_moves_2.end() && !field[2][0] && !field[2][2])
			{
				additional_moves[{ 1, 1 }] = dir_enum::right;
				return dir_enum::wait;
			}
			throw 0; // impossible
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 0))
	{
		if (possible_moves.find(dir_enum::bottom) != possible_moves.end())
		{
			field[2][1] = true;
			return dir_enum::bottom;
		}
		if (tank)
		{
			auto possible_moves_2 = GetPossibleMoves(field, { 2, 1 });
			if (possible_moves_2.find(dir_enum::left) != possible_moves_2.end())
			{
				additional_moves[{ 2, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			additional_moves[{ 1, 1 }] = dir_enum::left;
			field[1][0] = true;
			return dir_enum::left;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(0, 1))
	{
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(1, 1))
	{
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 1))
	{
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(0, 2))
	{
		if (possible_moves.find(dir_enum::top) != possible_moves.end())
		{
			field[0][1] = true;
			return dir_enum::top;
		}
		if (tank)
		{
			auto possible_moves_2 = GetPossibleMoves(field, { 0, 1 });
			if (possible_moves_2.find(dir_enum::right) != possible_moves_2.end())
			{
				additional_moves[{ 0, 1 }] = dir_enum::right;
				return dir_enum::wait;
			}
			additional_moves[{ 1, 1 }] = dir_enum::right;
			field[1][0] = true;
			return dir_enum::left;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(1, 2))
	{
		if (possible_moves.find(dir_enum::top) != possible_moves.end())
		{
			field[1][1] = true;
			return dir_enum::top;
		}
		if (tank)
		{
			auto possible_moves_2 = GetPossibleMoves(field, { 1, 1 });
			if (possible_moves_2.find(dir_enum::left) != possible_moves_2.end() && !field[0][0] && !field[0][2])
			{
				additional_moves[{ 1, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			if (possible_moves_2.find(dir_enum::right) != possible_moves_2.end() && !field[2][0] && !field[2][2])
			{
				additional_moves[{ 1, 1 }] = dir_enum::right;
				return dir_enum::wait;
			}
			throw 0; // impossible
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 2))
	{
		if (possible_moves.find(dir_enum::top) != possible_moves.end())
		{
			field[2][1] = true;
			return dir_enum::top;
		}
		if (tank)
		{
			auto possible_moves_2 = GetPossibleMoves(field, { 2, 1 });
			if (possible_moves_2.find(dir_enum::left) != possible_moves_2.end())
			{
				additional_moves[{ 2, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			additional_moves[{ 1, 1 }] = dir_enum::left;
			field[1][2] = true;
			return dir_enum::left;
		}
		return dir_enum::wait;
	}

	throw 0; // impossible
}

dir_enum GetStartMove1(std::vector<std::vector<bool>> & field, std::pair<int, int> ipos, dir_enum dir)
{
	if (dir != dir_enum::wait)
		return dir;

	auto possible_moves = GetPossibleMoves(field, ipos);

	if (ipos == std::make_pair(0, 0))
	{
		if (possible_moves.find(dir_enum::right) != possible_moves.end())
		{
			field[1][0] = true;
			return dir_enum::right;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(1, 0))
	{
		if (!field[0][1] && possible_moves.find(dir_enum::left) != possible_moves.end())
		{
			field[0][0] = true;
			return dir_enum::left;
		}
		if (!field[2][1] && possible_moves.find(dir_enum::right) != possible_moves.end())
		{
			field[2][0] = true;
			return dir_enum::right;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 0))
	{
		if (possible_moves.find(dir_enum::left) != possible_moves.end())
		{
			field[1][0] = true;
			return dir_enum::left;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(0, 1))
	{
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(1, 1))
	{
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 1))
	{
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(0, 2))
	{
		if (possible_moves.find(dir_enum::right) != possible_moves.end())
		{
			field[1][2] = true;
			return dir_enum::right;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(1, 2))
	{
		if (!field[0][1] && possible_moves.find(dir_enum::left) != possible_moves.end())
		{
			field[0][2] = true;
			return dir_enum::left;
		}
		if (!field[2][1] && possible_moves.find(dir_enum::right) != possible_moves.end())
		{
			field[2][2] = true;
			return dir_enum::right;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 2))
	{
		if (possible_moves.find(dir_enum::left) != possible_moves.end())
		{
			field[1][2] = true;
			return dir_enum::left;
		}
		return dir_enum::wait;
	}
	
	throw 0; // impossible
}

void DoStartMove(model::Game const& game, std::vector<CMove> & moves, model::VehicleType type, dir_enum dir)
{
	switch (dir)
	{
	case dir_enum::wait:
		return;
	case dir_enum::left:
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
	case dir_enum::top:
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
	case dir_enum::right:
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
	case dir_enum::bottom:
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

bool CalcNuclear()
{
	return false;
}

void MyStrategy::move(model::Player const& me, model::World const& world, model::Game const& game, model::Move & new_move)
{
	static std::vector<model::Vehicle> vehicles;
	static std::vector<CMove> moves = { CMove() };
	static int current_move = 1;
	static int last_moving_move = 0;

	// update vehicles list

	for (auto const& v : world.getNewVehicles())
		vehicles.push_back(v);

	bool moving = false;

	for (auto const& u : world.getVehicleUpdates())
		for (auto & v : vehicles)
			if (u.getId() == v.getId())
			{
				if (!moving && v.getPlayerId() == me.getId() && u.getDurability() != 0 && (u.getX() != v.getX() || u.getY() != v.getY()))
					moving = true;
				v = model::Vehicle(v, u);
			}

	if (moving)
		last_moving_move = world.getTickIndex();

	bool stopped = false;
	if (last_moving_move + 5 < world.getTickIndex())
		stopped = true;

	// strategy

	static int mode = 0;
	static bool ground_good = false;
	static bool aero_good = false;

	if (mode == 0 && current_move == (int)moves.size() && (!moves[current_move - 1].m_wait_completion || stopped))
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
		
		auto arrv_ipos = std::make_pair((int)(arrv_pos.first / 72.0), (int)(arrv_pos.second / 72.0));
		auto ifv_ipos = std::make_pair((int)(ifv_pos.first / 72.0), (int)(ifv_pos.second / 72.0));
		auto tank_ipos = std::make_pair((int)(tank_pos.first / 72.0), (int)(tank_pos.second / 72.0));
		auto fighter_ipos = std::make_pair((int)(fighter_pos.first / 72.0), (int)(fighter_pos.second / 72.0));
		auto helicopter_ipos = std::make_pair((int)(helicopter_pos.first / 72.0), (int)(helicopter_pos.second / 72.0));

		CheckField(field_g, arrv_ipos);
		CheckField(field_g, ifv_ipos);
		CheckField(field_g, tank_ipos);

		CheckField(field_a, fighter_ipos);
		CheckField(field_a, helicopter_ipos);

		ground_good = ground_good | (field_g[0][1] && field_g[1][1] && field_g[2][1]);
		aero_good = aero_good | (((int)field_a[0][1] + (int)field_a[1][1] + (int)field_a[2][1]) == 2);

		if (ground_good && aero_good)
		{
			mode = 1;
		}
		else
		{
			auto moves_size = moves.size();

			std::map<std::pair<int, int>, dir_enum> empty_additional_moves;
			std::map<std::pair<int, int>, dir_enum> additional_moves;

			auto tank0 = GetStartMove0(field_g, tank_ipos, true, additional_moves);
			auto arrv0 = GetStartMove0(field_g, arrv_ipos, false, additional_moves);
			auto ifv0 = GetStartMove0(field_g, ifv_ipos, false, additional_moves);

			auto fighter0 = GetStartMove0(field_a, fighter_ipos, false, empty_additional_moves);
			auto helicopter0 = GetStartMove0(field_a, helicopter_ipos, false, empty_additional_moves);

			//tank0 = GetStartMove1(field_g, tank_ipos, tank0);
			arrv0 = GetStartMove1(field_g, arrv_ipos, arrv0);
			ifv0 = GetStartMove1(field_g, ifv_ipos, ifv0);

			fighter0 = GetStartMove1(field_a, fighter_ipos, fighter0);
			helicopter0 = GetStartMove1(field_a, helicopter_ipos, helicopter0);

			DoStartMove(game, moves, model::VehicleType::ARRV, arrv0);
			DoStartMove(game, moves, model::VehicleType::IFV, ifv0);
			DoStartMove(game, moves, model::VehicleType::TANK, tank0);

			DoStartMove(game, moves, model::VehicleType::FIGHTER, fighter0);
			DoStartMove(game, moves, model::VehicleType::HELICOPTER, helicopter0);

			if (moves.size() != moves_size)
				moves.back().m_wait_completion = true;
		}
	}

	if (mode == 1 && current_move == (int)moves.size() && (!moves[current_move - 1].m_wait_completion || stopped))
	{
		for (int i = 0; i < 5; i++)
		{
			{
				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setLeft(0.0);
				sel_move.setTop(74.0 + 18.0 - 0.1 + i * 6.0);
				sel_move.setRight(game.getWorldWidth());
				sel_move.setBottom(74.0 + 18.0 + 0.1 + i * 6.0);
				moves.push_back(sel_move);
				CMove move_move;
				move_move.setAction(model::ActionType::MOVE);
				move_move.setX(0.0);
				move_move.setY(-6.0 - 12.0 * (4 - i));
				moves.push_back(move_move);
			}
			{
				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setLeft(0.0);
				sel_move.setTop(74.0 + 18.0 + 54.0 - 0.1 - i * 6.0);
				sel_move.setRight(game.getWorldWidth());
				sel_move.setBottom(74.0 + 18.0 + 54.0 + 0.1 - i * 6.0);
				moves.push_back(sel_move);
				CMove move_move;
				move_move.setAction(model::ActionType::MOVE);
				move_move.setX(0.0);
				move_move.setY(6.0 + 12.0 * (4 - i));
				if (i == 4)
					move_move.m_wait_completion = true;
				moves.push_back(move_move);
			}
		}
		mode = 2;
	}

	if (mode == 2 && current_move == (int)moves.size() && (!moves[current_move - 1].m_wait_completion || stopped))
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
			move_move.setX(0.0);
			move_move.setY(-6.0);
			moves.push_back(move_move);
		}
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(74.0 * 2 + 7.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(0.0);
			move_move.setY(6.0);
			move_move.m_wait_completion = true;
			moves.push_back(move_move);
		}
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
			move_move.setX(74.0);
			move_move.setY(0.0);
			moves.push_back(move_move);
		}
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(74.0 * 2 + 7.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			moves.push_back(sel_move);
			CMove move_move;
			move_move.setAction(model::ActionType::MOVE);
			move_move.setX(-74.0);
			move_move.setY(0.0);
			move_move.m_wait_completion = true;
			moves.push_back(move_move);
		}
		mode = 3;
	}

	if (mode == 3 && current_move == (int)moves.size() && (!moves[current_move - 1].m_wait_completion || stopped))
	{
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			moves.push_back(sel_move);
			CMove scale_move;
			scale_move.setAction(model::ActionType::SCALE);
			scale_move.setX(92.0 + 27.0);
			scale_move.setY(92.0 + 27.0);
			scale_move.setFactor(0.1);
			scale_move.m_wait_completion = true;
			moves.push_back(scale_move);
		}
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			moves.push_back(sel_move);
			CMove rotate_move;
			rotate_move.setAction(model::ActionType::ROTATE);
			rotate_move.setX(92.0 + 27.0);
			rotate_move.setY(92.0 + 27.0);
			rotate_move.setAngle(PI / 4.0);
			rotate_move.setMaxAngularSpeed(PI / 800.0);
			rotate_move.m_wait_completion = true;
			moves.push_back(rotate_move);
		}
		mode = 4;
	}

	static int nuclearTickIndex = -1;
	static double nuclearScaleX = 0.0;
	static double nuclearScaleY = 0.0;
	static int nuclearWaitTicks = 0;

	if (nuclearTickIndex == -1 && nuclearWaitTicks > 0)
		nuclearWaitTicks--;

	if (mode == 4 && current_move == (int)moves.size() && (!moves[current_move - 1].m_wait_completion || stopped))
	{
		if (nuclearTickIndex == -1 && nuclearWaitTicks > 0)
		{
			if (nuclearWaitTicks == 1)
			{
				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setLeft(0.0);
				sel_move.setTop(0.0);
				sel_move.setRight(game.getWorldWidth());
				sel_move.setBottom(game.getWorldHeight());
				moves.push_back(sel_move);
				CMove scale_move;
				scale_move.setAction(model::ActionType::SCALE);
				scale_move.setX(nuclearScaleX);
				scale_move.setY(nuclearScaleY);
				scale_move.setFactor(1.0);
				scale_move.m_wait_completion = true;
				moves.push_back(scale_move);
			}
		}
		else if (nuclearTickIndex != -1)
		{
			if (nuclearTickIndex <= world.getTickIndex())
			{
				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setLeft(0.0);
				sel_move.setTop(0.0);
				sel_move.setRight(game.getWorldWidth());
				sel_move.setBottom(game.getWorldHeight());
				moves.push_back(sel_move);
				CMove scale_move;
				scale_move.setAction(model::ActionType::SCALE);
				scale_move.setX(nuclearScaleX);
				scale_move.setY(nuclearScaleY);
				scale_move.setFactor(0.1);
				moves.push_back(scale_move);
				nuclearTickIndex = -1;
			}
		}
		else if (world.getOpponentPlayer().getNextNuclearStrikeTickIndex() != -1)
		{
			nuclearScaleX = world.getOpponentPlayer().getNextNuclearStrikeX();
			nuclearScaleY = world.getOpponentPlayer().getNextNuclearStrikeY();

			bool detected = false;
			for (auto const& v : vehicles)
			{
				if (v.getPlayerId() != me.getId())
					continue;
				if (v.getDurability() == 0)
					continue;
				if (v.getDistanceTo(nuclearScaleX, nuclearScaleY) < game.getTacticalNuclearStrikeRadius())
				{
					detected = true;
					break;
				}
			}

			if (detected)
			{
				nuclearTickIndex = world.getOpponentPlayer().getNextNuclearStrikeTickIndex();
				nuclearWaitTicks = nuclearTickIndex - world.getTickIndex();

				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setLeft(0.0);
				sel_move.setTop(0.0);
				sel_move.setRight(game.getWorldWidth());
				sel_move.setBottom(game.getWorldHeight());
				moves.push_back(sel_move);
				CMove scale_move;
				scale_move.setAction(model::ActionType::SCALE);
				scale_move.setX(nuclearScaleX);
				scale_move.setY(nuclearScaleY);
				scale_move.setFactor(10.0);
				moves.push_back(scale_move);
			}
		}
		else if (me.getRemainingNuclearStrikeCooldownTicks() == 0 && CalcNuclear())
		{
			// need nuclear strike
		}
		else if (world.getTickIndex() % 60 == 0)
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			moves.push_back(sel_move);
			CMove scale_move;
			scale_move.setAction(model::ActionType::SCALE);
			scale_move.setX(92.0 + 27.0);
			scale_move.setY(92.0 + 27.0);
			scale_move.setFactor(0.1);
			scale_move.m_wait_completion = true;
			moves.push_back(scale_move);
		}
	}

	// get move

	if (!moves[current_move - 1].m_wait_completion || stopped)
	{
		if ((int)moves.size() > current_move && me.getRemainingActionCooldownTicks() == 0)
		{
			auto const& mv = moves[current_move];
			new_move.setAction(mv.getAction());
			new_move.setAngle(mv.getAngle());
			new_move.setBottom(mv.getBottom());
			new_move.setFacilityId(mv.getFacilityId());
			new_move.setFactor(mv.getFactor());
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
		}
	}
}

MyStrategy::MyStrategy()
{
}