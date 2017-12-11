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
			result.first += v.getX();
			result.second += v.getY();
		}
	}
	if (count == 0)
		return { 0.0, 0.0 };
	return { result.first / (double)count, result.second / (double)count };
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

std::pair<bool, std::pair<double, double>> GetNearestGroupCenter(long long pid, std::vector<model::Vehicle> const& vehicles, std::pair<double, double> current_position)
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
				if (v.getSquaredDistanceTo(groups[i][j]) < 20.0 * 20.0)
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
	double minSquaredDistance = 1024.0 * 1024.0;
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
		double squared_distance = (current_position.first - X) * (current_position.first - X) + (current_position.second - Y) * (current_position.second - Y);
		if (squared_distance < minSquaredDistance)
		{
			minSquaredDistance = squared_distance;
			minX = X;
			minY = Y;
			minGroup = i;
		}
	}

	return std::make_pair(minGroup != -1, std::make_pair(minX, minY));
}

__forceinline double GetTerrainWeatherVisionCoef(model::Game const& game, model::World const& world, model::Vehicle const& v)
{
	int X = (int)(v.getX() / 32.0);
	int Y = (int)(v.getY() / 32.0);
	if (!v.isAerial())
	{
		switch (world.getTerrainByCellXY()[X][Y])
		{
		case model::TerrainType::FOREST: return game.getForestTerrainVisionFactor();
		case model::TerrainType::PLAIN: return game.getPlainTerrainVisionFactor();
		case model::TerrainType::SWAMP: return game.getSwampTerrainVisionFactor();
		}
	}
	else
	{
		switch (world.getWeatherByCellXY()[X][Y])
		{
		case model::WeatherType::CLEAR: return game.getClearWeatherVisionFactor();
		case model::WeatherType::CLOUD: return game.getCloudWeatherVisionFactor();
		case model::WeatherType::RAIN: return game.getRainWeatherVisionFactor();
		}
	}
	return 0.0;
};

void MyStrategy::move(model::Player const& me, model::World const& world, model::Game const& game, model::Move & new_move)
{
	static std::vector<model::Vehicle> vehicles;
	static std::vector<CMove> moves = { CMove() };
	static int current_move = 1;
	static int last_moving_move = 0;
	static int last_new_vehicles_tick = -1;

	// update vehicles list

	for (auto const& v : world.getNewVehicles())
	{
		if (v.getPlayerId() == me.getId())
			last_new_vehicles_tick = world.getTickIndex();
		vehicles.push_back(v);
	}

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
	if (last_moving_move + 2 < world.getTickIndex())
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
	static std::pair<double, double> current_position = { 92.0 + 27.0, 92.0 + 27.0 };

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
			scale_move.setX(current_position.first);
			scale_move.setY(current_position.second);
			scale_move.setFactor(0.1);
			scale_move.m_wait_completion = true;
			moves.push_back(scale_move);
		}
		{
			CMove rotate_move;
			rotate_move.setAction(model::ActionType::ROTATE);
			rotate_move.setX(current_position.first);
			rotate_move.setY(current_position.second);
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

	static bool global_enable_rotating = false;
	static bool global_rotating = false;
	static double global_rotating_angle = 0.0;

	static bool global_enable_moving = false;
	static bool global_moving = false;
	static std::pair<double, double> global_moving_position = { 0.0, 0.0 };

	if (mode == 4 && current_move == (int)moves.size() && (!moves[current_move - 1].m_wait_completion || stopped))
	{
		if (!global_enable_rotating)
			global_enable_rotating = true;
		if (!global_enable_moving)
			global_enable_moving = true;

		if (nuclearTickIndex == -1 && nuclearWaitTicks > 0)
		{
			if (nuclearWaitTicks == 1)
			{
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
				if (v.getSquaredDistanceTo(nuclearScaleX, nuclearScaleY) < game.getTacticalNuclearStrikeRadius() * game.getTacticalNuclearStrikeRadius())
				{
					detected = true;
					break;
				}
			}

			if (detected)
			{
				nuclearTickIndex = world.getOpponentPlayer().getNextNuclearStrikeTickIndex();
				nuclearWaitTicks = nuclearTickIndex - world.getTickIndex();

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
			auto target_group = GetNearestGroupCenter(me.getId(), vehicles, current_position);

			if (me.getRemainingNuclearStrikeCooldownTicks() == 0)
			{
				bool STRIIIIIIIIKE = false;

				if (target_group.first && (target_group.second.first - current_position.first) * (target_group.second.first - current_position.first) + (target_group.second.second - current_position.second) * (target_group.second.second - current_position.second) < game.getBaseTacticalNuclearStrikeCooldown() * game.getTankSpeed() * 0.6 * game.getBaseTacticalNuclearStrikeCooldown() * game.getTankSpeed() * 0.6)
				{
					for (auto const& v : vehicles)
					{
						if (v.getPlayerId() != me.getId())
							continue;
						if (v.getDurability() == 0)
							continue;
						auto squared_distance = v.getSquaredDistanceTo(target_group.second.first, target_group.second.second);
						if (squared_distance > 120.0 * 120.0)
							continue;
						auto vrange = 0.9 * v.getVisionRange() * GetTerrainWeatherVisionCoef(game, world, v);
						if (50.0 * 50.0 < squared_distance && squared_distance < vrange * vrange)
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
						if (probable_target.getSquaredDistanceTo(current_position.first, current_position.second) > 512.0 * 512.0)
							continue;
						bool has_source = false;
						for (auto const& source : vehicles)
						{
							if (source.getPlayerId() != me.getId())
								continue;
							if (source.getDurability() == 0)
								continue;
							auto squared_distance = source.getSquaredDistanceTo(probable_target);
							if (squared_distance > 120.0 * 120.0)
								continue;
							auto vrange = 0.9 * source.getVisionRange() * GetTerrainWeatherVisionCoef(game, world, source);
							if (squared_distance > vrange * vrange)
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
							auto squared_distance = probable_target.getSquaredDistanceTo(dam);
							if (squared_distance > game.getTacticalNuclearStrikeRadius() * game.getTacticalNuclearStrikeRadius())
								continue;
							auto damage = ((game.getTacticalNuclearStrikeRadius() - probable_target.getDistanceTo(dam)) / game.getTacticalNuclearStrikeRadius()) * game.getMaxTacticalNuclearStrikeDamage();
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
									if (dam.getSquaredDistanceTo(heal) > game.getArrvRepairRange() * game.getArrvRepairRange())
										continue;
									coef *= 0.25;
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
							auto squared_distance = v.getSquaredDistanceTo(X, Y);
							if (squared_distance > 120.0 * 120.0)
								continue;
							auto vrange = 0.9 * v.getVisionRange() * GetTerrainWeatherVisionCoef(game, world, v);
							if (50.0 * 50.0 < squared_distance && squared_distance < vrange * vrange)
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

			static long long recruiting_fid = -1;
			static int last_brake_recruiting_tick = -1;
			const int recruiting_limit = 300;
			bool need_selection = false;

			int alive_count = 0;
			for (auto const& v : vehicles)
			{
				if (v.getPlayerId() != me.getId())
					continue;
				if (v.getDurability() == 0)
					continue;
				alive_count++;
				if (!v.isSelected())
					need_selection = true;
			}

			if (recruiting_fid != -1)
			{
				if (last_new_vehicles_tick + 60 < world.getTickIndex() || alive_count >= recruiting_limit)
				{
					CMove prod_move;
					prod_move.setAction(model::ActionType::SETUP_VEHICLE_PRODUCTION);
					prod_move.setFacilityId(recruiting_fid);
					prod_move.setVehicleType(model::VehicleType::_UNKNOWN_);
					moves.push_back(prod_move);

					last_brake_recruiting_tick = world.getTickIndex();
					recruiting_fid = -1;
				}
			}
			else
			{
				if (last_brake_recruiting_tick + 360 < world.getTickIndex() && alive_count < recruiting_limit)
				{
					for (auto const& f : world.getFacilities())
					{
						if (f.getOwnerPlayerId() != me.getId())
							continue;
						if (f.getType() != model::FacilityType::VEHICLE_FACTORY)
							continue;
						double squared_distance = (current_position.first - f.getLeft() - 32.0) * (current_position.first - f.getLeft() - 32.0) + (current_position.second - f.getTop() - 32.0) * (current_position.second - f.getTop() - 32.0);
						if (squared_distance > 32.0 * 32.0)
							continue;
						CMove scale_move;
						scale_move.setAction(model::ActionType::SCALE);
						scale_move.setX(current_position.first);
						scale_move.setY(current_position.second);
						scale_move.setFactor(1.0);
						moves.push_back(scale_move);
						CMove prod_move;
						prod_move.setAction(model::ActionType::SETUP_VEHICLE_PRODUCTION);
						prod_move.setFacilityId(f.getId());
						prod_move.setVehicleType(model::VehicleType::IFV);
						moves.push_back(prod_move);
						recruiting_fid = f.getId();
						last_new_vehicles_tick = world.getTickIndex() + 1;
						break;
					}
				}
			}

			if (need_selection)
			{
				CMove sel_move;
				sel_move.setAction(model::ActionType::CLEAR_AND_SELECT);
				sel_move.setLeft(0.0);
				sel_move.setTop(0.0);
				sel_move.setRight(game.getWorldWidth());
				sel_move.setBottom(game.getWorldHeight());
				moves.push_back(sel_move);
			}

			enum class mode_action
			{
				rotate,
				move,
				scale
			};

			static mode_action last_mode = mode_action::scale;
			static int lastScaleTick = 0;
			static int lastRotateTick = 0;
			static int rotatePrediction = 0;
			static int lastMoveTick = 0;
			static int movePrediction = 0;

			if (last_mode == mode_action::scale && (stopped || lastScaleTick + 36 <= world.getTickIndex()))
			{
				last_mode = mode_action::rotate;
				lastRotateTick = world.getTickIndex();
				rotatePrediction = 0;
				if (target_group.first)
				{
					std::pair<double, double> direction = { target_group.second.first - current_position.first, target_group.second.second - current_position.second };
					double length = std::sqrt(direction.first * direction.first + direction.second * direction.second);

					if (10.0 < length && length < 512.0)
					{
						double direction_angle = atan2(direction.second / length, direction.first / length);
						double delta_angle = direction_angle - current_angle;

						if (std::abs(delta_angle) > PI)
						{
							if (delta_angle > 0.0)
								delta_angle -= 2.0 * PI;
							else
								delta_angle += 2.0 * PI;
						}

						if (std::abs(delta_angle) > PI / 2.0)
						{
							if (current_angle > 0.0)
								current_angle -= PI;
							else
								current_angle += PI;

							delta_angle = direction_angle - current_angle;

							if (std::abs(delta_angle) > PI)
							{
								if (delta_angle > 0.0)
									delta_angle -= 2.0 * PI;
								else
									delta_angle += 2.0 * PI;
							}
						}

						if (std::abs(delta_angle) > 0.1)
						{
							CMove rotate_move;
							rotate_move.setAction(model::ActionType::ROTATE);
							rotate_move.setX(current_position.first);
							rotate_move.setY(current_position.second);
							rotate_move.setAngle(delta_angle);
							rotate_move.setMaxAngularSpeed(PI / 800.0);
							moves.push_back(rotate_move);

							rotatePrediction = std::max(6, std::min(18, (int)(std::abs(delta_angle) / (PI / 800.0) + 0.5)));
						}
					}
				}
			}

			if (last_mode == mode_action::rotate && lastRotateTick + rotatePrediction <= world.getTickIndex())
			{
				last_mode = mode_action::move;
				lastMoveTick = world.getTickIndex();
				movePrediction = 0;
				if (recruiting_fid == -1)
				{
					bool f_found = false;
					double minSquaredDistance = 1024.0 * 1024.0;
					double minX = 0.0;
					double minY = 0.0;
					for (auto const& f : world.getFacilities())
					{
						if (f.getOwnerPlayerId() == me.getId())
						{
							/*
							if (f.getType() == model::FacilityType::VEHICLE_FACTORY && f.getVehicleType() == model::VehicleType::_UNKNOWN_)
							{
							CMove prod_move;
							prod_move.setAction(model::ActionType::SETUP_VEHICLE_PRODUCTION);
							prod_move.setFacilityId(f.getId());
							prod_move.setVehicleType(model::VehicleType::TANK);
							moves.push_back(prod_move);
							}
							*/
							continue;
						}
						if (!(64.0 - 0.1 < f.getLeft() && f.getLeft() < game.getWorldWidth() - 64.0 - 64.0 + 0.1))
							continue;
						if (!(64.0 - 0.1 < f.getTop() && f.getTop() < game.getWorldHeight() - 64.0 - 64.0 + 0.1))
							continue;
						double squared_distance = (current_position.first - f.getLeft() - 32.0) * (current_position.first - f.getLeft() - 32.0) + (current_position.second - f.getTop() - 32.0) * (current_position.second - f.getTop() - 32.0);
						if (squared_distance < minSquaredDistance)
						{
							f_found = true;
							minX = f.getLeft() + 32.0;
							minY = f.getTop() + 32.0;
							minSquaredDistance = squared_distance;
						}
					}
					if (f_found && minSquaredDistance > 10.0 * 10.0)
					{
						CMove move_move;
						move_move.setAction(model::ActionType::MOVE);
						move_move.setX(minX - current_position.first);
						move_move.setY(minY - current_position.second);
						move_move.setMaxSpeed(game.getTankSpeed() * 0.6);
						moves.push_back(move_move);

						movePrediction = std::max(6, std::min(36, (int)(std::sqrt(minSquaredDistance) / (game.getTankSpeed() * 0.6) + 0.5)));
					}
				}
			}

			if (last_mode == mode_action::move && lastMoveTick + movePrediction <= world.getTickIndex())
			{
				lastScaleTick = world.getTickIndex();
				last_mode = mode_action::scale;

				static int realLastScaleTick = -1;
				if (realLastScaleTick + 6 <= world.getTickIndex())
				{
					realLastScaleTick = world.getTickIndex();
					CMove scale_move;
					scale_move.setAction(model::ActionType::SCALE);
					scale_move.setX(current_position.first);
					scale_move.setY(current_position.second);
					scale_move.setFactor(0.1);
					moves.push_back(scale_move);
				}
			}
		}
	}

	if (global_rotating)
	{
		if (std::abs(global_rotating_angle) < PI / 800.0)
		{
			current_angle += global_rotating_angle;
			global_rotating = false;
		}
		else
		{
			if (global_rotating_angle > 0)
			{
				global_rotating_angle -= PI / 800.0;
				current_angle += PI / 800.0;
			}
			else
			{
				global_rotating_angle += PI / 800.0;
				current_angle -= PI / 800.0;
			}
		}
	}

	if (global_moving)
	{
		if ((global_moving_position.first - current_position.first) * (global_moving_position.first - current_position.first) + (global_moving_position.second - current_position.second) * (global_moving_position.second - current_position.second) < game.getTankSpeed() * 0.6 * game.getTankSpeed() * 0.6)
		{
			current_position = global_moving_position;
			global_moving = false;
		}
		else
		{
			double distance = std::sqrt((global_moving_position.first - current_position.first) * (global_moving_position.first - current_position.first) + (global_moving_position.second - current_position.second) * (global_moving_position.second - current_position.second));
			if (global_moving_position.first > current_position.first)
			{
				current_position.first += (std::abs(global_moving_position.first - current_position.first) / distance) * game.getTankSpeed() * 0.6;
			}
			else
			{
				current_position.first -= (std::abs(global_moving_position.first - current_position.first) / distance) * game.getTankSpeed() * 0.6;
			}
			if (global_moving_position.second > current_position.second)
			{
				current_position.second += (std::abs(global_moving_position.second - current_position.second) / distance) * game.getTankSpeed() * 0.6;
			}
			else
			{
				current_position.second -= (std::abs(global_moving_position.second - current_position.second) / distance) * game.getTankSpeed() * 0.6;
			}
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

			if (global_enable_rotating)
			{
				if (new_move.getAction() == model::ActionType::ROTATE)
				{
					global_rotating = true;
					global_rotating_angle = new_move.getAngle();
				}
				else if (new_move.getAction() == model::ActionType::MOVE || new_move.getAction() == model::ActionType::SCALE)
				{
					global_rotating = false;
				}
			}

			if (global_enable_moving)
			{
				if (new_move.getAction() == model::ActionType::MOVE)
				{
					global_moving = true;
					global_moving_position = { current_position.first + new_move.getX(), current_position.second + new_move.getY() };
				}
				else if (new_move.getAction() == model::ActionType::ROTATE || new_move.getAction() == model::ActionType::SCALE)
				{
					global_moving = false;
				}
			}
		}
	}
}

MyStrategy::MyStrategy()
{
}