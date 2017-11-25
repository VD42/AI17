#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <set>
#include <map>
#include <functional>
#include <algorithm>

class CMove : public model::Move
{
public:
	bool m_wait_completion;
	CMove() : model::Move(), m_wait_completion(false) {}
};

std::pair<double, double> GetCenter(long long pid, std::vector<model::Vehicle> const& vehicles, model::VehicleType type)
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
	return { std::sqrt(result.first / (double)count), std::sqrt(result.second / (double)count) };
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
	bottom,

	top_ch,
	top_ach,
	bot_ch,
	bot_ach
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
		return additional_moves[ipos];

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
				if (!field[1][0])
				{
					field[1][0] = true;
					field[1][1] = true;
					return dir_enum::bot_ch;
				}
				field[1][1] = true;
				additional_moves[{ 0, 1 }] = dir_enum::right;
				return dir_enum::wait;
			}
			field[2][1] = true;
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
				if (!field[0][0])
				{
					field[0][0] = true;
					field[0][1] = true;
					return dir_enum::bot_ach;
				}
				field[0][1] = true;
				additional_moves[{ 1, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			if (possible_moves_2.find(dir_enum::right) != possible_moves_2.end() && !field[2][0] && !field[2][2])
			{
				if (!field[2][0])
				{
					field[2][0] = true;
					field[2][1] = true;
					return dir_enum::bot_ch;
				}
				field[2][1] = true;
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
				if (!field[1][0])
				{
					field[1][0] = true;
					field[1][1] = true;
					return dir_enum::bot_ach;
				}
				field[1][1] = true;
				additional_moves[{ 2, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			field[0][1] = true;
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
				if (!field[1][2])
				{
					field[1][2] = true;
					field[1][1] = true;
					return dir_enum::top_ach;
				}
				field[1][1] = true;
				additional_moves[{ 0, 1 }] = dir_enum::right;
				return dir_enum::wait;
			}
			field[2][1] = true;
			additional_moves[{ 1, 1 }] = dir_enum::right;
			field[1][0] = true;
			return dir_enum::right;
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
				if (!field[0][2])
				{
					field[0][2] = true;
					field[0][1] = true;
					return dir_enum::top_ch;
				}
				field[0][1] = true;
				additional_moves[{ 1, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			if (possible_moves_2.find(dir_enum::right) != possible_moves_2.end() && !field[2][0] && !field[2][2])
			{
				if (!field[2][2])
				{
					field[2][2] = true;
					field[2][1] = true;
					return dir_enum::top_ach;
				}
				field[2][1] = true;
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
				if (!field[1][2])
				{
					field[1][2] = true;
					field[1][1] = true;
					return dir_enum::top_ch;
				}
				field[1][1] = true;
				additional_moves[{ 2, 1 }] = dir_enum::left;
				return dir_enum::wait;
			}
			field[0][1] = true;
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
			if (!field[1][1])
			{
				field[1][0] = true;
				field[1][1] = true;
				return dir_enum::bot_ch;
			}
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
			field[0][1] = true;
			return dir_enum::bot_ach;
		}
		if (!field[2][1] && possible_moves.find(dir_enum::right) != possible_moves.end())
		{
			field[2][0] = true;
			field[2][1] = true;
			return dir_enum::bot_ch;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 0))
	{
		if (possible_moves.find(dir_enum::left) != possible_moves.end())
		{
			if (!field[1][1])
			{
				field[1][0] = true;
				field[1][1] = true;
				return dir_enum::bot_ach;
			}
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
			if (!field[1][1])
			{
				field[1][2] = true;
				field[1][1] = true;
				return dir_enum::top_ach;
			}
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
			field[0][1] = true;
			return dir_enum::top_ch;
		}
		if (!field[2][1] && possible_moves.find(dir_enum::right) != possible_moves.end())
		{
			field[2][2] = true;
			field[2][1] = true;
			return dir_enum::top_ach;
		}
		return dir_enum::wait;
	}
	if (ipos == std::make_pair(2, 2))
	{
		if (possible_moves.find(dir_enum::left) != possible_moves.end())
		{
			if (!field[1][1])
			{
				field[1][2] = true;
				field[1][1] = true;
				return dir_enum::top_ch;
			}
			field[1][2] = true;
			return dir_enum::left;
		}
		return dir_enum::wait;
	}
	
	throw 0; // impossible
}

void DoStartMove(model::Game const& game, std::vector<CMove> & moves, model::VehicleType type, std::pair<int, int> pos, dir_enum dir)
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
	case dir_enum::top_ch:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove rotate_move;
			rotate_move.setAction(model::ActionType::ROTATE);
			rotate_move.setX(45.0 + (pos.first) * 74.0);
			rotate_move.setY(45.0 + (pos.second - 1) * 74.0);
			rotate_move.setAngle(PI / 2.0);
			moves.push_back(rotate_move);
		}
		return;
	case dir_enum::top_ach:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove rotate_move;
			rotate_move.setAction(model::ActionType::ROTATE);
			rotate_move.setX(45.0 + (pos.first) * 74.0);
			rotate_move.setY(45.0 + (pos.second - 1) * 74.0);
			rotate_move.setAngle(-PI / 2.0);
			moves.push_back(rotate_move);
		}
		return;
	case dir_enum::bot_ch:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove rotate_move;
			rotate_move.setAction(model::ActionType::ROTATE);
			rotate_move.setX(45.0 + (pos.first) * 74.0);
			rotate_move.setY(45.0 + (pos.second + 1) * 74.0);
			rotate_move.setAngle(PI / 2.0);
			moves.push_back(rotate_move);
		}
		return;
	case dir_enum::bot_ach:
		{
			CMove sel_move;
			sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
			sel_move.setLeft(0.0);
			sel_move.setTop(0.0);
			sel_move.setRight(game.getWorldWidth());
			sel_move.setBottom(game.getWorldHeight());
			sel_move.setVehicleType(type);
			moves.push_back(sel_move);
			CMove rotate_move;
			rotate_move.setAction(model::ActionType::ROTATE);
			rotate_move.setX(45.0 + (pos.first) * 74.0);
			rotate_move.setY(45.0 + (pos.second + 1) * 74.0);
			rotate_move.setAngle(-PI / 2.0);
			moves.push_back(rotate_move);
		}
		return;
	}
}

std::pair<bool, std::pair<double, double>> GetNearestGroupCenter(long long pid, std::vector<model::Vehicle> const& vehicles)
{
	std::vector<std::vector<std::reference_wrapper<model::Vehicle const>>> groups;

	for (auto const& v : vehicles)
	{
		if (v.getPlayerId() == pid)
			continue;
		if (v.getDurability() == 0)
			continue;
		bool found = false;
		for (int i = 0; i < (int)groups.size(); i++)
		{
			if (found)
				break;
			for (int j = 0; j < (int)groups[i].size(); j++)
			{
				if (v.getDistanceTo(groups[i][j]) < 20.0)
				{
					groups[i].push_back(v);
					found = true;
					break;
				}
			}
		}
		if (!found)
		{
			groups.emplace_back();
			groups.back().push_back(v);
		}
	}

	int minGroup = -1;
	double minDistance = 1024.0;
	double minX = 1024.0;
	double minY = 1024.0;

	for (int i = 0; i < (int)groups.size(); i++)
	{
		if (groups[i].size() < 50)
			continue;
		double X = 0.0;
		double Y = 0.0;
		int heals = 0;
		for (int j = 0; j < (int)groups[i].size(); j++)
		{
			X += groups[i][j].get().getX();
			Y += groups[i][j].get().getY();
			if (groups[i][j].get().getType() == model::VehicleType::ARRV)
				heals++;
		}
		if ((double)heals > 0.75 * (double)groups[i].size())
			continue;
		X /= (double)groups[i].size();
		Y /= (double)groups[i].size();
		double distance = std::sqrt((92.0 + 27.0 - X) * (92.0 + 27.0 - X) + (92.0 + 27.0 - Y) * (92.0 + 27.0 - Y));
		if (distance < minDistance)
		{
			minDistance = distance;
			minX = X;
			minY = Y;
			minGroup = i;
		}
	}

	return std::make_pair(minGroup != -1, std::make_pair(minX, minY));
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

			DoStartMove(game, moves, model::VehicleType::TANK, tank_ipos, tank0);
			DoStartMove(game, moves, model::VehicleType::ARRV, arrv_ipos, arrv0);
			DoStartMove(game, moves, model::VehicleType::IFV, ifv_ipos, ifv0);

			DoStartMove(game, moves, model::VehicleType::FIGHTER, fighter_ipos, fighter0);
			DoStartMove(game, moves, model::VehicleType::HELICOPTER, helicopter_ipos, helicopter0);

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

	static double current_angle = PI / 4.0;

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
			CMove assign_move;
			assign_move.setAction(model::ActionType::ASSIGN);
			assign_move.setGroup(1);
			moves.push_back(assign_move);
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
			sel_move.setGroup(1);
			moves.push_back(sel_move);
			CMove rotate_move;
			rotate_move.setAction(model::ActionType::ROTATE);
			rotate_move.setX(92.0 + 27.0);
			rotate_move.setY(92.0 + 27.0);
			rotate_move.setAngle(current_angle);
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

	static bool enable_rotating = false;
	static bool rotating = false;
	static double rotating_angle = 0.0;

	if (mode == 4 && current_move == (int)moves.size() && (!moves[current_move - 1].m_wait_completion || stopped))
	{
		if (!enable_rotating)
			enable_rotating = true;

		if (nuclearTickIndex == -1 && nuclearWaitTicks > 0)
		{
			if (nuclearWaitTicks == 1)
			{
				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setGroup(1);
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
				sel_move.setGroup(1);
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
				sel_move.setGroup(1);
				moves.push_back(sel_move);
				CMove scale_move;
				scale_move.setAction(model::ActionType::SCALE);
				scale_move.setX(nuclearScaleX);
				scale_move.setY(nuclearScaleY);
				scale_move.setFactor(10.0);
				moves.push_back(scale_move);
			}
		}
		else
		{
			static bool bomber_mode = false;
			static bool bomber_mode_fly = false;
			static long long bomber_id = -1;
			static bool bomber_on_way = false;

			if (world.getTickIndex() >= 10000 && me.getScore() < 20 && me.getScore() < world.getOpponentPlayer().getScore())
				bomber_mode = true;

			if (world.getTickIndex() >= 15000 && me.getScore() == 0)
				bomber_mode = true;

			if (bomber_id != -1)
			{
				for (auto const& v : vehicles)
				{
					if (v.getId() != bomber_id)
						continue;
					if (v.getDurability() != 0)
					{
						bool escaping = false;

						double min_distance = 10000.0;
						std::pair<double, double> nearest_enemy;
						std::pair<double, double> sum = { 0.0, 0.0 };
						int count = 0;
						for (auto const& v2 : vehicles)
						{
							if (v2.getPlayerId() == me.getId())
								continue;
							if (v2.getDurability() == 0)
								continue;
							if (v2.getType() == model::VehicleType::ARRV)
								continue;
							double distance = v.getDistanceTo(v2);
							if (distance < min_distance)
							{
								nearest_enemy = { v2.getX(), v2.getY() };
								min_distance = distance;
							}
							double need_distance = ((me.getRemainingNuclearStrikeCooldownTicks() == 0 || me.getNextNuclearStrikeTickIndex() != -1) ? 60.0 : 150.0);
							if (distance < need_distance + 0.0001)
							{
								std::pair<double, double> vec = { (need_distance - distance) * (v.getX() - v2.getX()) / (std::sqrt(2.0) * distance), (need_distance - distance) * (v.getY() - v2.getY()) / (std::sqrt(2.0) * distance) };
								sum.first += vec.first;
								sum.second += vec.second;
								count++;
								escaping = true;
							}
						}

						if (min_distance < 100.0)
						{
							printf("bomber_on_way = false\r\n");
							bomber_on_way = false;
						}

						if (escaping)
						{
							sum.first /= (double)count;
							sum.second /= (double)count;

							if (std::sqrt(sum.first * sum.first + sum.second * sum.second) > 0.5)
							{
								CMove sel_move;
								sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
								sel_move.setLeft(v.getX() - 0.1);
								sel_move.setTop(v.getY() - 0.1);
								sel_move.setRight(v.getX() + 0.1);
								sel_move.setBottom(v.getY() + 0.1);
								moves.push_back(sel_move);

								CMove move_move;
								move_move.setAction(model::ActionType::MOVE);
								move_move.setX(sum.first);
								move_move.setY(sum.second);
								moves.push_back(move_move);

								printf("escaping: %f, %f\r\n", sum.first, sum.second);
							}
						}

						if (!bomber_on_way && !escaping && min_distance < 5000.0 && me.getRemainingNuclearStrikeCooldownTicks() == 0 && min_distance > 64.0)
						{
							CMove sel_move;
							sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
							sel_move.setLeft(v.getX() - 0.1);
							sel_move.setTop(v.getY() - 0.1);
							sel_move.setRight(v.getX() + 0.1);
							sel_move.setBottom(v.getY() + 0.1);
							moves.push_back(sel_move);

							std::pair<double, double> vec = { (min_distance - 60.0) * (nearest_enemy.first - v.getX()) / (std::sqrt(2.0) * v.getDistanceTo(nearest_enemy.first, nearest_enemy.second)), (min_distance - 60.0) * (nearest_enemy.second - v.getY()) / (std::sqrt(2.0) * v.getDistanceTo(nearest_enemy.first, nearest_enemy.second)) };

							CMove move_move;
							move_move.setAction(model::ActionType::MOVE);
							move_move.setX(vec.first);
							move_move.setY(vec.second);
							moves.push_back(move_move);

							printf("bomber_on_way: %f, %f\r\n", vec.first, vec.second);

							bomber_on_way = true;
						}
					}
					break;
				}
			}

			if (bomber_mode && bomber_mode_fly && me.getScore() <= world.getOpponentPlayer().getScore())
			{
				for (auto const& v : vehicles)
				{
					if (v.getId() != bomber_id)
						continue;
					if (v.getDurability() == 0)
					{
						bomber_id = -1;
						bomber_mode_fly = false;
					}
					break;
				}
			}

			if (bomber_mode && !bomber_mode_fly)
			{
				std::pair<double, double> nearest_enemy;
				double nearest_distance = 10000.0;

				for (auto const& v : vehicles)
				{
					if (v.getPlayerId() == me.getId())
						continue;
					if (v.getDurability() == 0)
						continue;
					if (v.getType() == model::VehicleType::ARRV)
						continue;
					auto distance = v.getDistanceTo(92.0 + 27.0, 92.0 + 27.0);
					if (distance < nearest_distance)
					{
						nearest_distance = distance;
						nearest_enemy = { v.getX(), v.getY() };
					}
				}

				if (nearest_distance < 5000.0)
				{
					nearest_distance = 10000.0;
					model::Vehicle empty;
					std::reference_wrapper<const model::Vehicle> my_fighter(empty);

					for (auto const& v : vehicles)
					{
						if (v.getPlayerId() != me.getId())
							continue;
						if (v.getDurability() == 0)
							continue;
						if (v.getType() != model::VehicleType::FIGHTER)
							continue;
						auto distance = v.getDistanceTo(nearest_enemy.first, nearest_enemy.second);
						if (distance < nearest_distance)
						{
							nearest_distance = distance;
							my_fighter = v;
						}
					}

					if (nearest_distance < 5000.0)
					{
						CMove sel_move;
						sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
						sel_move.setLeft(my_fighter.get().getX() - 0.1);
						sel_move.setTop(my_fighter.get().getY() - 0.1);
						sel_move.setRight(my_fighter.get().getX() + 0.1);
						sel_move.setBottom(my_fighter.get().getY() + 0.1);
						moves.push_back(sel_move);

						CMove dismiss_move;
						dismiss_move.setAction(model::ActionType::DISMISS);
						dismiss_move.setGroup(1);
						moves.push_back(dismiss_move);

						std::pair<double, double> vec = { nearest_enemy.first - my_fighter.get().getX(), nearest_enemy.second - my_fighter.get().getY() };

						CMove move_move;
						move_move.setAction(model::ActionType::MOVE);
						move_move.setX(vec.first - vec.first * 60.0 / std::sqrt(2.0 * (vec.first * vec.first + vec.second + vec.second)));
						move_move.setY(vec.second - vec.second * 60.0 / std::sqrt(2.0 * (vec.first * vec.first + vec.second + vec.second)));
						moves.push_back(move_move);

						bomber_mode_fly = true;
						bomber_id = my_fighter.get().getId();
						bomber_on_way = true;
					}
				}
			}

			auto GetVisionRange = [&] (model::VehicleType type) {
				switch (type)
				{
				case model::VehicleType::ARRV: return game.getArrvVisionRange();
				case model::VehicleType::FIGHTER: return game.getFighterVisionRange();
				case model::VehicleType::HELICOPTER: return game.getHelicopterVisionRange();
				case model::VehicleType::IFV: return game.getIfvVisionRange();
				case model::VehicleType::TANK: return game.getTankVisionRange();
				}
				return 0.0;
			};

			auto GetTerrainWeatherVisionCoef = [&] (bool isAerial, model::TerrainType type1, model::WeatherType type2) {
				if (!isAerial)
				{
					switch (type1)
					{
					case model::TerrainType::FOREST: return game.getForestTerrainVisionFactor();
					case model::TerrainType::PLAIN: return game.getPlainTerrainVisionFactor();
					case model::TerrainType::SWAMP: return game.getSwampTerrainVisionFactor();
					}
				}
				else
				{
					switch (type2)
					{
					case model::WeatherType::CLEAR: return game.getClearWeatherVisionFactor();
					case model::WeatherType::CLOUD: return game.getCloudWeatherVisionFactor();
					case model::WeatherType::RAIN: return game.getRainWeatherVisionFactor();
					}
				}
				return 0.0;
			};

			auto target_group = GetNearestGroupCenter(me.getId(), vehicles);

			if (me.getRemainingNuclearStrikeCooldownTicks() == 0)
			{
				bool STRIIIIIIIIKE = false;

				if (target_group.first && std::sqrt((target_group.second.first - 92.0 - 27.0) * (target_group.second.first - 92.0 - 27.0) + (target_group.second.second - 92.0 - 27.0) * (target_group.second.second - 92.0 - 27.0)) < game.getBaseTacticalNuclearStrikeCooldown() * game.getTankSpeed() * 0.6)
				{
					for (auto const& v : vehicles)
					{
						if (v.getPlayerId() != me.getId())
							continue;
						if (v.getDurability() == 0)
							continue;
						auto distance = v.getDistanceTo(target_group.second.first, target_group.second.second);
						auto vrange = GetVisionRange(v.getType()) * GetTerrainWeatherVisionCoef(v.isAerial(), world.getTerrainByCellXY()[(int)(v.getX() / 32.0)][(int)(v.getY() / 32.0)], world.getWeatherByCellXY()[(int)(v.getX() / 32.0)][(int)(v.getY() / 32.0)]);
						if (50.0 < distance && distance < 0.9 * vrange)
						{
							STRIIIIIIIIKE = true;
							break;
						}
					}
				}
				else
				{
					STRIIIIIIIIKE = true;
				}

				if (STRIIIIIIIIKE)
				{
					std::vector<std::pair<std::reference_wrapper<model::Vehicle const>, double>> targets;

					for (auto const& probable_target : vehicles)
					{
						if (probable_target.getPlayerId() == me.getId())
							continue;
						if (probable_target.getDurability() == 0)
							continue;
						if (probable_target.getDistanceTo(92.0 + 27.0, 92.0 + 27.0) > 1000.0 && !bomber_mode)
							continue;
						bool has_source = false;
						for (auto const& source : vehicles)
						{
							if (source.getPlayerId() != me.getId())
								continue;
							if (source.getDurability() == 0)
								continue;
							auto distance = source.getDistanceTo(probable_target);
							auto vrange = GetVisionRange(source.getType()) * GetTerrainWeatherVisionCoef(source.isAerial(), world.getTerrainByCellXY()[(int)(source.getX() / 32.0)][(int)(source.getY() / 32.0)], world.getWeatherByCellXY()[(int)(source.getX() / 32.0)][(int)(source.getY() / 32.0)]);
							if (distance > 0.9 * vrange)
								continue;
							has_source = true;
							break;
						}
						if (!has_source)
							continue;
						int friendly_kills = 0;
						double friendly_health = 0.0;
						int enemy_kills = 0;
						double enemy_health = 0.0;
						for (auto const& dam : vehicles)
						{
							if (dam.getDurability() == 0)
								continue;
							auto distance = probable_target.getDistanceTo(dam);
							if (distance > game.getTacticalNuclearStrikeRadius())
								continue;
							auto damage = ((game.getTacticalNuclearStrikeRadius() - distance) / game.getTacticalNuclearStrikeRadius()) * game.getMaxTacticalNuclearStrikeDamage();
							if (dam.getPlayerId() == me.getId())
							{
								if (dam.getDurability() <= (int)damage)
								{
									friendly_kills++;
									friendly_health += 1.5 * dam.getMaxDurability();
								}
								friendly_health += (double)std::min(dam.getDurability(), (int)damage);
							}
							else
							{
								if (dam.getDurability() <= (int)damage)
								{
									enemy_kills++;
									enemy_health += 1.5 * dam.getMaxDurability();
								}

								double coef = 1.0;
								for (auto const& heal : vehicles)
								{
									if (heal.getPlayerId() == me.getId())
										continue;
									if (heal.getType() != model::VehicleType::ARRV)
										continue;
									if (heal.getDurability() == 0)
										continue;
									if (dam.getDistanceTo(heal) > game.getArrvRepairRange())
										continue;
									coef *= 0.25;
									if (bomber_mode)
										coef *= 0.0;
								}

								enemy_health += (dam.isAerial() ? 0.75 : 1.0) * coef * (double)std::min(dam.getDurability(), (int)damage);
							}
						}
						if (friendly_kills > enemy_kills)
							continue;
						if (friendly_health > 1.2 * enemy_health)
							continue;
						targets.push_back(std::make_pair(std::reference_wrapper<model::Vehicle const>(probable_target), enemy_health - friendly_health));
					}

					auto best_target = std::max_element(targets.begin(), targets.end(), [] (decltype(targets)::const_reference a, decltype(targets)::const_reference b) {
						return (a.second < b.second);
					});

					if (best_target != targets.end())
					{
						double X = best_target->first.get().getX();
						double Y = best_target->first.get().getY();

						for (auto const& v : vehicles)
						{
							if (v.getPlayerId() != me.getId())
								continue;
							if (v.getDurability() == 0)
								continue;
							auto distance = v.getDistanceTo(X, Y);
							auto vrange = GetVisionRange(v.getType()) * GetTerrainWeatherVisionCoef(v.isAerial(), world.getTerrainByCellXY()[(int)(v.getX() / 32.0)][(int)(v.getY() / 32.0)], world.getWeatherByCellXY()[(int)(v.getX() / 32.0)][(int)(v.getY() / 32.0)]);
							if (50.0 < distance && distance < 0.9 * vrange)
							{
								CMove nuclear_strike_move;
								nuclear_strike_move.setAction(model::ActionType::TACTICAL_NUCLEAR_STRIKE);
								nuclear_strike_move.setX(X);
								nuclear_strike_move.setY(Y);
								nuclear_strike_move.setVehicleId(v.getId());
								moves.push_back(nuclear_strike_move);
								break;
							}
						}
					}
				}
			}

			static bool mode_rotate = false;
			static int lastScaleTick = 0;
			static int lastRotateTick = 0;
			static int rotatePrediction = 0;

			if (!mode_rotate && lastScaleTick + 30 <= world.getTickIndex())
			{
				if (target_group.first)
				{
					CMove sel_move;
					sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
					sel_move.setGroup(1);
					moves.push_back(sel_move);

					std::pair<double, double> direction = { target_group.second.first - 92.0 - 27.0, target_group.second.second - 92.0 - 27.0 };

					if (std::abs(direction.first) > 0.0 || std::abs(direction.second) > 0.0)
					{
						double direction_angle = atan2(direction.second / std::sqrt(direction.first * direction.first + direction.second * direction.second), direction.first / std::sqrt(direction.first * direction.first + direction.second * direction.second));
						double delta_angle = direction_angle - current_angle;

						if (std::abs(delta_angle) > PI / 2.0)
						{
							if (current_angle > 0.0)
								current_angle -= PI;
							else
								current_angle += PI;
							
							delta_angle = direction_angle - current_angle;
						}

						CMove rotate_move;
						rotate_move.setAction(model::ActionType::ROTATE);
						rotate_move.setX(92.0 + 27.0);
						rotate_move.setY(92.0 + 27.0);
						rotate_move.setAngle(delta_angle);
						rotate_move.setMaxAngularSpeed(PI / 800.0);
						moves.push_back(rotate_move);

						rotatePrediction = std::max(30, std::min(120, (int)(std::abs(delta_angle) / (PI / 800.0) + 0.5)));
						lastRotateTick = world.getTickIndex();
					}
				}
				mode_rotate = true;
			}

			if (mode_rotate && lastRotateTick + rotatePrediction <= world.getTickIndex())
			{
				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setGroup(1);
				moves.push_back(sel_move);
				CMove scale_move;
				scale_move.setAction(model::ActionType::SCALE);
				scale_move.setX(92.0 + 27.0);
				scale_move.setY(92.0 + 27.0);
				scale_move.setFactor(0.1);
				moves.push_back(scale_move);

				lastScaleTick = world.getTickIndex();
				mode_rotate = false;
			}
		}
	}

	if (rotating)
	{
		if (rotating_angle > 0)
		{
			rotating_angle -= PI / 800.0;
			current_angle += PI / 800.0;
		}
		else
		{
			rotating_angle += PI / 800.0;
			current_angle -= PI / 800.0;
		}
		if (std::abs(rotating_angle) < PI / 800.0)
		{
			current_angle -= rotating_angle;
			rotating = false;
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
			new_move.setVehicleId(mv.getVehicleId());
			new_move.setVehicleType(mv.getVehicleType());
			new_move.setX(mv.getX());
			new_move.setY(mv.getY());

			current_move++;

			if (enable_rotating)
			{
				if (new_move.getAction() == model::ActionType::ROTATE)
				{
					rotating = true;
					rotating_angle = new_move.getAngle();
				}
				else if (new_move.getAction() == model::ActionType::MOVE || new_move.getAction() == model::ActionType::SCALE)
				{
					rotating = false;
				}
			}
		}
	}
}

MyStrategy::MyStrategy()
{
}