#pragma once
void preform_aimbot(double closest_distance, std::shared_ptr<player_data> closest_player) {
	if (closest_distance != DBL_MAX && config.Aimbot && (u)global->local_pawn) {
		config.aimbot_bind.update();
		if (config.aimbot_bind.enabled) {
			vector2 target_loc{};
			if (config.TargetBone == 0)
				target_loc = w2s(closest_player->mesh->get_head());
			else if (config.TargetBone == 1)
				target_loc = w2s(closest_player->mesh->get_bone_3d(66));
			else if (config.TargetBone == 2) {
				vector2 chest_left = w2s(closest_player->mesh->get_bone_3d(37));
				vector2 chest_right = w2s(closest_player->mesh->get_bone_3d(8));
				target_loc = { chest_left.x + (chest_right.x - chest_left.x) / 2, chest_left.y };
			}
			move_mouse(target_loc);

			if (config.TriggerBot && closest_distance < 50 && closest_player->player_distance < 15 && strstr(global->weapon_name.c_str(), "Shotgun")) {
				//ifykyk

			}


		}
	}
}