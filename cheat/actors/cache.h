#pragma once
#include "../../util.hpp"
#include "../sdk.hpp"
//#include <tbb/parallel_for.h>
//#include <tbb/concurrent_vector.h>
#include <atomic>
#include <unordered_set>
#include <unordered_map>
enum actor_type_e {
	player,
	animal,
	loot,
	traps,
	chest,
	ammobox,
	llama,
	supplydrop,
	vechicle
};
extern struct ImColor;
struct actor_data {
	aactor* actor{};
	uschenecomponent* root_component{};
	vector3 relative_location{};
	FBoxSphereBounds bounds{};
	int actor_distance{};
	int actor_id{};
	std::string fname{};
	std::string item_name{};
	EFortItemTier loot_tier{};
	actor_type_e actor_type{};
	actor_data() = default;
};
struct player_data {
	aactor* player{};
	uskeletalmeshcomponent* mesh{};
	uschenecomponent* root_component{};
	aplayerstate* player_state{};
	vector3 relative_location{};
	FBoxSphereBounds bounds{};
	int player_team_id{};
	int player_distance{};
	bool blocked_by_builds{};
	bool visible{};
	player_data() = default;
};

struct global_s {
	uworld* world{};
	ugameinstance* game_instance{};
	ulevel* persistent_level{};
	ulocalplayer* local_player{};
	aplayercontroller* player_controller{};
	
	aactor* local_pawn{};
	uschenecomponent* local_root_component{};
	aplayerstate* local_player_state{};
	aplayercameramanager* player_camera_manager{};
	afortweapon* current_weapon{};
	std::string weapon_name{};
	int local_player_team_id{};

	agamestate* game_state{};
	std::vector<std::shared_ptr<actor_data>> actors{};
	std::vector<std::shared_ptr<player_data>> players{};
	std::vector<FBoxSphereBounds> builds;
	std::mutex data_mutex;
};  
inline std::shared_ptr<global_s> global = std::make_shared<global_s>();

inline int fps = 0;
//inline std::vector<int> blacklisted_actor_ids;
struct id_info {
	
	actor_type_e actor_type{};
	int id{};
	std::string item_name;
	std::string fname;
	EFortItemTier loot_tier{};
};
inline std::unordered_map<int, id_info> whitelisted_actor_ids;
inline std::unordered_set<int> blacklisted_actor_ids;
inline ulevel* cached_level{};

void cache_actors_combined();

inline bool populate_global_data(const std::shared_ptr<global_s>& global_temp);
inline bool populate_actor_data(const std::shared_ptr<global_s>& global_temp,
	const std::shared_ptr<actor_data>& actor_data_temp,
	aactor* actor);
inline bool populate_player_data(const std::shared_ptr<global_s>& global_temp,
	const std::shared_ptr<player_data>& player_data_temp,
	const std::shared_ptr<actor_data>& actor_data_temp);

