#include "cache.h"
inline std::atomic_bool is_running = true;


void cache_actors_combined() {
	auto next_update_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);

	while (is_running) {
		std::unique_lock<std::mutex> lock(global->data_mutex);
		auto current_global = global;
		lock.unlock();
		auto global_temp = std::make_shared<global_s>();

		if (!populate_global_data(global_temp))
			continue;

		if (std::chrono::steady_clock::now() >= next_update_time) {
			std::vector<ulevel*> levels = global_temp->world->get_levels_array();
			//std::cout << levels.size() << std::endl;
			std::vector<std::shared_ptr<actor_data>> actors_temp;
			std::vector<std::shared_ptr<player_data>> players_temp;
			
			for (int q = 0; q < levels.size(); q++) {
				auto level = levels[q];
				std::vector<aactor*> actors = level->get_actor_array();
				//std::cout << actors.size() << std::endl;
				for (int i = 0; i < actors.size(); i++) {
					auto actor_data_temp = std::make_shared<actor_data>();
					auto player_data_temp = std::make_shared<player_data>();

					if (populate_actor_data(global_temp, actor_data_temp, actors[i])) {
						if (actor_data_temp->actor_type == actor_type_e::player) {
							if (populate_player_data(global_temp, player_data_temp, actor_data_temp))
								players_temp.push_back(player_data_temp);
						}
						else
							actors_temp.push_back(actor_data_temp);
					}
				}
			}
			
			global_temp->actors = std::move(actors_temp);
			global_temp->players = std::move(players_temp);

			next_update_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
		}
		else {
			std::vector<std::shared_ptr<actor_data>> actors_temp;
			std::vector<std::shared_ptr<player_data>> players_temp;

			for (auto actor : current_global->actors) {
				auto actor_data_temp = std::make_shared<actor_data>();
				if (populate_actor_data(global_temp, actor_data_temp, actor->actor)) {

					actors_temp.push_back(actor_data_temp);
				}

			}
			for (auto player : current_global->players) {
				auto player_data_temp = std::make_shared<player_data>();
				auto actor_data_temp = std::make_shared<actor_data>();
				if (populate_actor_data(global_temp, actor_data_temp, player->player)) {
					if (populate_player_data(global_temp, player_data_temp, actor_data_temp))
						players_temp.push_back(player_data_temp);
				}
			}
			global_temp->actors = std::move(actors_temp);
			global_temp->players = std::move(players_temp);
		}
		std::unique_lock<std::mutex> lock_global(global->data_mutex);
		global = global_temp;
		lock_global.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


inline bool populate_global_data(const std::shared_ptr<global_s>& global_temp) {
	global_temp->world = read<uworld*>(base_address + 0x11770D88);
	debug_ptr((u)global_temp->world, "world");
	if (!(u)global_temp->world) return false;

	global_temp->game_instance = global_temp->world->get_game_instance();
	debug_ptr((u)global_temp->game_instance, "game_instance");
	if (!(u)global_temp->game_instance) return false;

	global_temp->local_player = global_temp->game_instance->get_local_player();
	debug_ptr((u)global_temp->local_player, "local_player");
	if (!(u)global_temp->local_player) return false;

	global_temp->player_controller = global_temp->local_player->get_player_controller();
	debug_ptr((u)global_temp->player_controller, "player_controller");
	if (!(u)global_temp->player_controller) return false;

	global_temp->player_camera_manager = global_temp->player_controller->get_camera_manager();
	debug_ptr((u)global_temp->player_camera_manager, "player_camera_manager");

	global_temp->local_pawn = global_temp->player_controller->get_acknowledged_pawn();
	debug_ptr((u)global_temp->local_pawn, "local_pawn");

	global_temp->local_root_component = global_temp->local_pawn->get_root_component();
	debug_ptr((u)global_temp->local_root_component, "local_root_component");

	global_temp->local_player_state = global_temp->local_pawn->get_player_state();
	debug_ptr((u)global_temp->local_player_state, "local_player_state");

	global_temp->local_player_team_id = global_temp->local_player_state->get_player_id();
	debug_ptr(global_temp->local_player_team_id, "local_player_team_id");



	if ((u)global_temp->local_pawn)
		global_temp->current_weapon = global_temp->local_pawn->get_current_weapon();
	if ((u)global_temp->current_weapon)
		global_temp->weapon_name = global_temp->current_weapon->get_weapon_name();

	auto persistent_level = global_temp->world->get_persistent_level();
	if ((u)persistent_level != (u)cached_level) {
		blacklisted_actor_ids.clear();
		cached_level = persistent_level;
	}
	global_temp->persistent_level = persistent_level;
	debug_ptr((u)global_temp->persistent_level, "persistent_level");
	if (!(u)global_temp->persistent_level) return false;

	return true;
}

inline bool populate_actor_data(const std::shared_ptr<global_s>& global_temp,
	const std::shared_ptr<actor_data>& actor_data_temp,
	aactor* actor) {


	actor_data_temp->actor = actor;
	//debug_ptr((u)actor_data_temp->actor, "actor");
	if (!(u)actor_data_temp->actor) return false;
	if ((u)global_temp->local_pawn == (u)actor_data_temp->actor) return false;

	actor_data_temp->actor_id = actor_data_temp->actor->get_actor_id();
	if (!actor_data_temp->actor_id) return false;
	debug_1d((u)actor_data_temp->actor_id, "actor_id");
	bool valid_id = false;
	id_info valid_id_info{};

	
	auto whitelisted_id_iter = whitelisted_actor_ids.find(actor_data_temp->actor_id);
	if (whitelisted_id_iter != whitelisted_actor_ids.end()) {
		valid_id = true;
		valid_id_info = whitelisted_id_iter->second;
	}

	if (!valid_id) {
		bool invalid_id = blacklisted_actor_ids.count(actor_data_temp->actor_id) > 0;
		if (invalid_id) return false;
	}
	
	
	if (valid_id) {
		actor_data_temp->fname = valid_id_info.fname;
		actor_data_temp->actor_type = valid_id_info.actor_type; 
		actor_data_temp->actor_id = valid_id_info.id;
		actor_data_temp->item_name = valid_id_info.item_name;
		

	}
	else {
		id_info new_valid_id{};
		actor_data_temp->fname = get_fname(actor_data_temp->actor_id);
		debug_string(actor_data_temp->fname);
		if (strstr(actor_data_temp->fname.c_str(), "Trap")) {

			actor_data_temp->item_name = actor_data_temp->actor->get_trap_name();
			
			actor_data_temp->actor_type = actor_type_e::traps;
		}
		else if (strstr(actor_data_temp->fname.c_str(), "Tiered_Chest")) {
			actor_data_temp->item_name = "Chest";
			actor_data_temp->actor_type = actor_type_e::chest;
		}
		else if (strstr(actor_data_temp->fname.c_str(), "FortPickupAthena") or strstr(actor_data_temp->fname.c_str(), "Fort_Pickup_Creative_C")) {
			actor_data_temp->item_name = actor_data_temp->actor->get_pickup_name();
			actor_data_temp->loot_tier = actor_data_temp->actor->get_loot_tier();
			new_valid_id.loot_tier = actor_data_temp->loot_tier;
			actor_data_temp->actor_type = actor_type_e::loot;
			//print_string(actor_data_temp->item_name);
		}
		else if (strstr(actor_data_temp->fname.c_str(), "NPC_Pawn")) {
			if (strstr(actor_data_temp->fname.c_str(), "NPC_Pawn_Irwin_Prey_Nug_Loot_C"))
				actor_data_temp->item_name = "Chicken";
			else if (strstr(actor_data_temp->fname.c_str(), "NPC_Pawn_Irwin_Prey_NugS_Base_C"))
				actor_data_temp->item_name = "Chicken";
			else if (strstr(actor_data_temp->fname.c_str(), "NPC_Pawn_Irwin_Simple_Smackie_Loot_C"))
				actor_data_temp->item_name = "Frog";
			else if (strstr(actor_data_temp->fname.c_str(), "NPC_Pawn_Irwin_Prey_Burt_Loot_C"))
				actor_data_temp->item_name = "Boar";
			else if (strstr(actor_data_temp->fname.c_str(), "NPC_Pawn_Irwin_Predator_Grandma_LootVar02_C"))
				actor_data_temp->item_name = "Wolf";
			else if (strstr(actor_data_temp->fname.c_str(), "NPC_Pawn_Irwin_Predator_Robert_C"))
				actor_data_temp->item_name = "Raptor";
			actor_data_temp->actor_type = actor_type_e::animal;
		}
		else if (strstr(actor_data_temp->fname.c_str(), "PlayerPawn")) {
			actor_data_temp->actor_type = actor_type_e::player;
		}
		else {

			blacklisted_actor_ids.insert(actor_data_temp->actor_id);
			return false;
		}
		new_valid_id.fname = actor_data_temp->fname;
		new_valid_id.item_name = actor_data_temp->item_name;
		new_valid_id.id = actor_data_temp->actor_id;
		new_valid_id.actor_type = actor_data_temp->actor_type;
		//deleted line of code for paster proof. if you have a brain you can easily code it, the context is the lines above^
		
	}
	
	actor_data_temp->root_component = actor_data_temp->actor->get_root_component();
	debug_ptr((u)actor_data_temp->root_component, "root_component");
	if (!(u)actor_data_temp->root_component) return false;

	actor_data_temp->relative_location = actor_data_temp->root_component->get_relative_location();
	debug_3d(actor_data_temp->relative_location, "relative_location");
	if (actor_data_temp->relative_location.x == 0) return false;

	actor_data_temp->actor_distance = camera_postion.location.distance(actor_data_temp->relative_location) / 100;
	debug_1d(actor_data_temp->actor_distance, "distance");
	

	if ((u)global_temp->local_pawn)
		if (actor_data_temp->actor_distance > config.MaxEspDistance) return false;

	actor_data_temp->bounds = actor_data_temp->root_component->get_bounds();
	if (/*valid_id &&*/ actor_data_temp->actor_type == actor_type_e::loot) {
		actor_data_temp->item_name = actor_data_temp->actor->get_pickup_name();
		actor_data_temp->loot_tier = actor_data_temp->actor->get_loot_tier();
	}
	return true;
}

inline bool populate_player_data(const std::shared_ptr<global_s>& global_temp,
	const std::shared_ptr<player_data>& player_data_temp,
	const std::shared_ptr<actor_data>& actor_data_temp) {
	

	player_data_temp->player_state = actor_data_temp->actor->get_player_state();
	debug_ptr((u)player_data_temp->player_state, "player_state");
	if (!(u)player_data_temp->player_state) return false;
	
	player_data_temp->player_team_id = player_data_temp->player_state->get_player_id();
	

	if (player_data_temp->player_team_id == global_temp->local_player_team_id && player_data_temp->player_team_id != 0) return false;

	player_data_temp->player = actor_data_temp->actor;
	player_data_temp->root_component = actor_data_temp->root_component;
	player_data_temp->relative_location = actor_data_temp->relative_location;
	player_data_temp->player_distance = actor_data_temp->actor_distance;

	player_data_temp->mesh = player_data_temp->player->get_mesh();
	debug_ptr((u)player_data_temp->mesh, "mesh");
	if (!(u)player_data_temp->mesh) return false;
	
	
	player_data_temp->visible = player_data_temp->mesh->get_bvisible();
	player_data_temp->bounds = actor_data_temp->bounds;
	return true;
}




