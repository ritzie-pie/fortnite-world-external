#pragma once
#pragma warning(disable : 4996)
inline extern void preform_aimbot(double closest_distance, std::shared_ptr<player_data> closest_player);
inline camera_position_s get_camera_postion() {
	camera_position_s local_camera;

	if (!(u)global->local_player or !(u)global->player_controller or !(u)global->world)
		return{};

	local_camera.fov = global->player_controller->get_fov();

	auto location_pointer = read<uintptr_t>((u)global->world + 0x110);
	auto rotation_pointer = read<uintptr_t>((u)global->world + 0x120);

	struct FNRot
	{
		double a; //0x0000
		char pad_0008[24]; //0x0008
		double b; //0x0020
		char pad_0028[424]; //0x0028
		double c; //0x01D0
	}fnRot;

	fnRot.a = read<double>(rotation_pointer);
	fnRot.b = read<double>(rotation_pointer + 0x20);
	fnRot.c = read<double>(rotation_pointer + 0x1d0);

	local_camera.location = read<vector3>(location_pointer);
	local_camera.rotation.x = asin(fnRot.c) * (180.0 / M_PI);
	local_camera.rotation.y = ((atan2(fnRot.a * -1, fnRot.b) * (180.0 / M_PI)) * -1) * -1;

	
	return local_camera;

}
inline void DrawString(const char* text, ImVec2 pos, ImColor textColor, ImColor outlineColor, float fontSize, int outlineThickness = 2, bool centered = false) {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImVec2 textSize = ImGui::CalcTextSize(text);
	ImGuiIO& io = ImGui::GetIO();

	// Center the text if needed
	if (centered) {
		pos.x -= textSize.x / 2;
		pos.y -= textSize.y / 2;
	}

	const ImVec2 offset = ImVec2(1, 1);
	ImVec4 bgColor = outlineColor;

	// Draw the text outline
	
	for (int x = -outlineThickness; x <= outlineThickness; x++) {
		for (int y = -outlineThickness; y <= outlineThickness; y++) {
			if (x != 0 || y != 0) {
				drawList->AddText(ImGui::GetFont(), fontSize, pos + ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(bgColor), text);
			}
		}
	}
	

	// Draw the actual text
	drawList->AddText(ImGui::GetFont(), fontSize, pos, ImGui::ColorConvertFloat4ToU32(textColor), text);
	
}
inline Color get_loot_color(EFortItemTier tier) {
	if (tier == EFortItemTier::I)
		return Color(123 / 255.0f, 123 / 255.0f, 123 / 255.0f, 1.f);
	else if (tier == EFortItemTier::II)
		return Color(58 / 255.0f, 121 / 255.0f, 19 / 255.0f, 1.f);
	else if (tier == EFortItemTier::III)
		return Color(18 / 255.0f, 88 / 255.0f, 162 / 255.0f, 1.f);
	else if (tier == EFortItemTier::IV)
		return Color(189 / 255.0f, 63 / 255.0f, 250 / 255.0f, 1.f);
	else if (tier == EFortItemTier::V)
		return Color(255 / 255.0f, 118 / 255.0f, 5 / 255.0f, 1.f);
	else if (tier == EFortItemTier::VI)
		return Color(220 / 255.0f, 160 / 255.0f, 30 / 255.0f, 1.f);
	else if (tier == EFortItemTier::VII)
		return Color(0 / 255.0f, 225 / 255.0f, 252 / 255.0f, 1.f);

	return Color(123, 123, 123, 1.f);
}
struct TextElement {
	std::string text;
	ImVec2 pos;
	ImColor textColor;
	ImColor outlineColor;
	float fontSize;
	int outlineThickness;
	bool centered;
};
template<typename KeyType, typename ValueType>
struct TPair {
	KeyType First;
	ValueType Second;
};

template<typename Element>
struct TSet {
	Element Value;
	int HashNextID;
	int HashID;
};

template<typename Key, typename Value>
struct TMap {
	tarray<TSet<TPair<Key, Value>>> Data;
};

struct FName {
	uint32_t CompareID;
	uint32_t Name;
};

struct FMeshBoneInfo {
	FName Name;
	int ParentIndex;
};

struct idd_info {
	int id;
	int parentid;
};
void draw_bone_connections(const std::vector<idd_info>& ids, const std::vector<vector3>& bones) {
	// Get the ImGui draw list to draw the lines
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	// Iterate through all the ids and draw lines between the bones
	for (const auto& id : ids) {
		vector2 a = w2s(bones[id.id]);
		vector2 b = w2s(bones[id.parentid]);

		// Convert vector3 to ImVec2 for ImGui
		ImVec2 a_imvec(a.x, a.y);
		ImVec2 b_imvec(b.x, b.y);

		// Draw the line with ImGui
		draw_list->AddLine(a_imvec, b_imvec, ImColor(100, 255, 100));
	}
}
float distance(const vector3& a, const vector3& b) {
	return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2));
}

void draw_closest_bone_connections(const std::vector<vector3>& bones) {
	// Get the ImGui draw list to draw the lines
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	// Iterate through all the bones and find the closest bone
	for (size_t i = 0; i < bones.size(); ++i) {
		float min_distance = FLT_MAX;
		size_t closest_bone_index = i;

		for (size_t j = 0; j < bones.size(); ++j) {
			if (i == j) continue;

			float dist = distance(bones[i], bones[j]);
			if (dist < min_distance) {
				min_distance = dist;
				closest_bone_index = j;
			}
		}

		// Draw a line between the current bone and its closest bone
		vector2 a = w2s(bones[i]);
		vector2 b = w2s(bones[closest_bone_index]);

		ImVec2 a_imvec(a.x, a.y);
		ImVec2 b_imvec(b.x, b.y);

		draw_list->AddLine(a_imvec, b_imvec, ImColor(100, 255, 100));
	}
}
inline void actor_loop() {



	std::unique_lock<std::mutex> lock(global->data_mutex);
	
	camera_postion = get_camera_postion();
	debug_3d(camera_postion.location, "camera_postion.location");
	debug_3d(camera_postion.rotation, "camera_postion.rotation");
	debug_1d(camera_postion.fov, "camera_postion.fov");

	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	char fpsinfo[64];
	//sprintf(fpsinfo, ("fps: %03d"), get_fps());
	sprintf(fpsinfo, ("FPS: %03d"), (int)ImGui::GetIO().Framerate);
	draw_list->AddText(ImGui::GetFont(), 14, ImVec2(20, 20), ImColor(0, 225, 0), fpsinfo);

	if (config.DrawFov)
		draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), config.AimbotFov, ImColor(55, 55, 255), 33, 1);
	

	if ((u)global->current_weapon) {
		/*__int64 xor_key = ((read<__int8>((u)global->current_weapon + 5231) - 60985) & 0x82FFFFFE | 0x4C000001);
		__int64 offset = (u)global->current_weapon + 4 * read<__int8>((u)global->current_weapon + 5230) + 5214;
		write<int>(offset, 1000000 ^ xor_key);*/
	}
	struct FLinearColor {
		float r;
		float g;
		float b;
		float a;
	};
	
	enum class EMaterialParameterAssociation : uint8_t
	{
		LayerParameter = 0,
		BlendParameter = 1,
		GlobalParameter = 2,
		EMaterialParameterAssociation_MAX = 3,
	};
	struct FMaterialParameterInfo {
		FName Name;
		EMaterialParameterAssociation Association;
		int32_t Index;
	};
	struct FGuild {
		int32_t a;
		int32_t b;
		int32_t c;
		int32_t d;
	};
	struct FVectorParameterValue {
		FMaterialParameterInfo ParameterInfo;
		FLinearColor ParameterValue;
		FGuild ExpressionGUID;
	};
	if ((u)global->local_pawn) {
		//auto PawnMaterials_ALL = read<tarray<u>>((u)global->local_pawn + 0x5440).getiter(); //0x44b0
		//
		//for (auto PawnMaterials : PawnMaterials_ALL) {
		//	auto VectorParameterValues = read<tarray<FVectorParameterValue>>(PawnMaterials + 0x138).getiter();
		//	//std::cout << VectorParameterValues.size() << std::endl;
		//	for (auto VectorParameterValue : VectorParameterValues) {
		//		//uintptr_t valueAddress = (uintptr_t)(&VectorParameterValue.ParameterValue);
		//		//write<FLinearColor>(valueAddress, FLinearColor(0, 0, 0, 1));
		//	}
		//}
		//auto root_comp = (u)global->local_root_component;
		//auto my_scale = read<vector3>(root_comp + 0x158);
		//for (uintptr_t i = 0; i < 0x5000; i += 0x4) {
		//	if (i == 0x158) continue;
		//	auto my_scale_2 = read<vector3>(root_comp + i);
		//	if (my_scale_2.x == my_scale.x && my_scale_2.y == my_scale.y && my_scale_2.z == my_scale.z) {
		//		std::cout << "0x" << std::hex << i << std::endl;
		//	}
		//}
		//std::vector<uintptr_t> offsets = { 0x158 , 0x280, 0x400, 0x4a0, 0x720}; //0x9f0, 0x3dc0
		//for (auto offset : offsets) {
		//	write<vector3>(root_comp + offset, vector3(5,5,5));
		//}
		//auto root_comp = (u)global->local_root_component;
		//auto c2w = read<FTransform>(root_comp + 0x240);
		////c2w.rot = fquat(10, c2w.rot.y,1000,c2w.rot.w);
		//c2w.translation = vector3(10, c2w.rot.y,1000);
		////std::cout << c2w.rot.z << std::endl;
		//write<FTransform>(root_comp + 0x240, c2w);

		//write<bool>(root_comp + 0x188 >> 1, true);
		//write<vector3>((u)global->player_controller + 0x358 + 0x18, vector3(1,1,1));
		//write<vector3>((u)global->player_controller + 0x358, vector3(1, 1, 1));
		//write<vector3>((u)global->player_controller + 0x300, vector3(1, 1, 1));
		//write<vector3>((u)global->player_camera_manager + 0x22b0 + 0x10 + 0x18, vector3(1, 1, 1));
		
		//print_3d(read<vector3>((u)global->player_controller + 0x300), "rot");
	}
	std::vector<TextElement> textElements;
	for (auto actor : global->actors) {
		//actor->bounds = actor->root_component->get_bounds();
		/*auto bounds = actor->bounds;

		vector2 ScreenPosition = w2s(bounds.Origin);
		auto Size = ImGui::CalcTextSize(actor->fname.c_str());
		int ScreenX = ScreenPosition.x - (Size.x / 2);
		int ScreenY = ScreenPosition.y;
		
		draw_list->AddText(ImVec2(ScreenX, ScreenY), ImColor(30, 200, 200), actor->fname.c_str());*/
		c_config::itemoptions option;
		switch (actor->actor_type) {
		case loot:
		{
			option = config.loot;
			
			option.NameEspColor = get_loot_color(actor->loot_tier);
			
			break;
		}
		case animal:
		{
			option = config.animal;
			auto c2w = read<FTransform>((u)actor->actor->get_mesh() + 0x240).translation;
			auto nowloc = vector3(c2w.x, c2w.y, actor->bounds.Origin.z);
			actor->bounds.Origin = nowloc;
			auto bones = actor->actor->get_mesh()->get_bones();


			/*uintptr_t MeshDeformerInstanceSize = 8;
			uintptr_t ComponentSpaceTransformsArraySize = 0x10;
			uintptr_t PreviousBoneVisibilityStatesSize = 0x10;
			uintptr_t PreviousComponentSpaceTransformsArraySize = 0x10;
			uintptr_t LastStreamerUpdateBoundsRadiusSize = sizeof(float);
			uintptr_t CurrentEditableComponentTransformsSize = sizeof(int32_t);
			uintptr_t CurrentReadComponentTransformsSize = sizeof(int32_t);
			uintptr_t CurrentBoneTransformRevisionNumberSize = sizeof(uint32_t);
			uintptr_t LeaderBoneMapCacheCountSize = sizeof(int32_t);
			uintptr_t FollowerPoseComponentsSize = 0x10;

			uintptr_t totalSize = MeshDeformerInstanceSize +
				ComponentSpaceTransformsArraySize +
				PreviousBoneVisibilityStatesSize +
				PreviousComponentSpaceTransformsArraySize +
				LastStreamerUpdateBoundsRadiusSize +
				CurrentEditableComponentTransformsSize +
				CurrentReadComponentTransformsSize +
				CurrentBoneTransformRevisionNumberSize +
				LeaderBoneMapCacheCountSize +
				FollowerPoseComponentsSize;

			uintptr_t LeaderBoneMapOffset = 0x5e8 + totalSize;*/


			
			/*auto SkeletalMesh = read<u>((u)actor->actor->get_mesh() + 0x5a8);
			auto Skeleton = read<u>(SkeletalMesh + 0xf8);
			struct FName {
				int CompareIdx;
				int Name;
			};
			enum class EBoneTranslationRetargetingMode : uint8_t
			{
				Animation = 0,
				Skeleton = 1,
				AnimationScaled = 2,
				AnimationRelative = 3,
				OrientAndScale = 4,
				EBoneTranslationRetargetingMode_MAX = 5,
			};
			struct FBoneNode {
				FName Name;
				int32_t ParentIndex;
				EBoneTranslationRetargetingMode TranslationRetargetingMode;
			};
			auto BoneTree = read<tarray<FBoneNode>>(Skeleton + 0x38).getiter();
			std::cout << (int)BoneTree[94].ParentIndex << std::endl;*/
			//94
			//DrawRaptorSkeletonImGui(bones);
			//for (int i = 0; i < bones.size(); i++) {
			//	auto bone = bones[i];
			//	auto loc = w2s(bone);
			//	if (!loc.valid_location())
			//		continue;
			//	std::string s = std::to_string(i);
			//	const char* text = s.c_str();
			//	//draw_list->AddText(ImGui::GetFont(), 17, ImVec2(loc.x, loc.y), ImColor(100, 150, 0), text);
			//	DrawString(text, ImVec2(loc.x, loc.y), ImColor(100, 150, 100), ImColor(0, 0, 0), 16, 1, true);
			//}
			//actor->bounds.Origin = actor->actor->get_mesh()->get_base();
			//actor->bounds = actor->actor->get_capsule_component()->get_bounds();
			//print_ptr((u)actor->actor->get_mesh(), "mesh");
			
			//std::vector<idd_info> ids;
			//auto mesh = actor->actor->get_mesh();
			//auto SkeletalMesh = (u)read<u>((u)mesh + 0x5a8);
			//////auto skelton = read<u>(SkeletalMesh + 0xf8);
			//uintptr_t ref_skeleton = SkeletalMesh + 0x2E8;
			//auto things = read<tarray<FMeshBoneInfo>>(ref_skeleton + 0x20).getiter();
			////std::cout << GetBoneName((u)player->mesh, 0) << std::endl;
			//for (int i = 0; i < things.size(); i++) {
			//	auto thing = things[i];
			//	//std::cout << thing.ParentIndex << std::endl;
			//	/*auto fname = get_fname(thing.Name.CompareID);
			//	if (strstr(fname.c_str(), "_"))
			//		std::cout << fname << std::endl;*/
			//	struct idd_info idd;
			//	if (thing.ParentIndex > 0 && thing.ParentIndex < 500) {
			//		idd.id = i;
			//		idd.parentid = thing.ParentIndex;
			//		ids.push_back(idd);
			//		//std::cout << i << std::endl;
			//		//std::cout << get_fname(thing.Name.Name) << std::endl;

			//	}
			//}
			draw_closest_bone_connections(actor->actor->get_mesh()->get_bones());
			//auto FinalNameToIndexMapData = read<TMap<FName, int>>(ref_skeleton + 0x40).Data.getiter();//SDK::GetFinalNameToIndexMap(Mesh);

			//for (int i = 0; i < FinalNameToIndexMapData.size(); i++) {
			//	auto Element = FinalNameToIndexMapData[i];

			//	std::string FinalBoneName = get_fname(Element.Value.First.CompareID);
			//	if (Element.Value.Second > 0 && Element.Value.Second < 500)
			//		std::cout << Element.Value.Second << "   " << FinalBoneName << std::endl;
			//	/*if (FinalBoneName == BoneName) {
			//		return Element.Value.Second;
			//	}*/
			//}
			//std::cout << GetBoneIndex((u)actor->actor->get_mesh(), "BipedArm_A_Wrist_L") << std::endl;
			break;
		}
		case chest:
		{
			option = config.chest;
			break;
		}
		case traps:
		{
			option = config.traps;
			draw_closest_bone_connections(actor->actor->get_mesh()->get_bones());
			break;
		}
		}
		
		if (actor->actor_distance > option.MaxDistance) continue;
		
		if (option.Esp) {
			auto bounds = actor->bounds;

			vector2 screen_position = w2s(bounds.Origin);
			if (!screen_position.valid_location())
				continue;
			std::string name = actor->item_name;

			

			if (option.BoxEsp) {
				auto v000 = bounds.Origin - bounds.BoxExtent;
				auto v111 = bounds.Origin + bounds.BoxExtent;
				ImVec2 a = ImVec2(w2s(v000).x, w2s(v000).y); //top
				ImVec2 b = ImVec2(w2s(v111).x, w2s(v111).y); //bottom
				if (option.BoxMode == 0) {

					if (option.OutlineBox) {
						if (option.Round2DBox)
							draw_list->AddRect(a, b, option.BoxOutlineColor.to_u32(), 10, ImDrawCornerFlags_All, option.OutlineBoxThickness);
						else
							draw_2d_box(bounds, option.OutlineBoxThickness, option.BoxOutlineColor.to_u32(), camera_postion);

					}

					if (option.Round2DBox)
						draw_list->AddRect(a, b, option.BoxColor.to_u32(), 10, ImDrawCornerFlags_All, option.BoxThickness);
					else
						draw_2d_box(bounds, option.BoxThickness, option.BoxColor.to_u32(), camera_postion);

				}
				else if (option.BoxMode == 1) {
					if (option.OutlineBox)
						draw_corner_box(bounds, option.OutlineBoxThickness, option.BoxOutlineColor.to_u32());

					draw_corner_box(bounds, option.BoxThickness, option.BoxColor.to_u32());
				}
				else if (option.BoxMode == 2) {
					if (option.OutlineBox)
						draw_3d_box(bounds, option.OutlineBoxThickness, option.BoxOutlineColor.to_u32());
					draw_3d_box(bounds, option.BoxThickness, option.BoxColor.to_u32());
				}
				
			}
			if (option.NameEsp) {
				auto Size = ImGui::CalcTextSize(name.c_str());
				int ScreenX = screen_position.x - (Size.x / 2);
				int ScreenY = screen_position.y;

				TextElement textElement;
				textElement.text = name;
				textElement.pos = ImVec2(screen_position.x, screen_position.y);
				textElement.textColor = option.NameEspColor.to_u32();
				textElement.outlineColor = option.OutlineNameEspColor.to_u32();
				textElement.fontSize = 13;
				textElement.outlineThickness = 2;
				textElement.centered = true;

				textElements.push_back(textElement);
			}
		}
		
	}
	for (const TextElement& textElement : textElements) {
		DrawString(textElement.text.c_str(), textElement.pos, textElement.textColor, textElement.outlineColor, textElement.fontSize, textElement.outlineThickness, textElement.centered);
	}
	//global->current_weapon->rapid_fire();
	
    if (global->players.size() < 1) return;

	
	double closest_distance = DBL_MAX;
	std::shared_ptr<player_data> closest_player{};
    for (auto player : global->players) {
		//std::vector<uintptr_t> offsets = { 0x158 , 0x280, 0x400, 0x4a0/*, 0x720*/,  0x3dc0 }; //0x9f0, 0x3dc0
		//for (auto offset : offsets) {
		//	write<vector3>((u)player->root_component + offset, vector3(3, 3, 3));
		//}
		//if ((u)player->root_component && (u)player->root_component != (u)global->local_root_component) {
		//	auto thing = read<uintptr_t>((u)player->player + 0x320);
		//	//if (GetAsyncKeyState(VK_F1)) {
		//		FTransform ComponentToWorld = read<FTransform>(thing + 0x240);
		//		//print_3d(ComponentToWorld.scale, "ComponentToWorld.scale");
		//		ComponentToWorld.scale = vector3(3, 3, 3);
		//		//write<FTransform>(thing + 0x240, ComponentToWorld);

		//		write<vector3>(thing + 0x158, vector3(2, 2, 2));
		//	//}
		//}
		//94 0x790   94 0x7A0
		auto mesh = (u)player->mesh;
		//int isCached = read<int>(mesh + 0x638);
		//auto dtc = read<tarray<FTransform>>(mesh + 0x10 * isCached + 0x5F0).getiter();
		//for (uintptr_t i = 0x5e8; i < 0x5e8 + 0x400; i+=0x4) {
		//	auto thing = read<tarray<int32_t>>(mesh + i).getiter();
		//	if (/*thing.size() > 0 && thing.size() < 200*/thing.size() == dtc.size()) {
		//		//std::cout << thing.size() << " "  << std::hex << i << std::endl;
		//	}
		//}
		//auto map = read<tarray<int32_t>>(mesh + 0x600).getiter();
		//std::cout << map[2] << std::endl;
		
		//auto skeletal_mesh = (u)read<u>((u)mesh + 0x5a8);
		//uintptr_t ref_skeleton = skeletal_mesh + 0x2E8;
		//auto things = read<tarray<FMeshBoneInfo>>(ref_skeleton + 0x20).getiter();
		//for (auto thing : things) {
		//	//std::cout << thing.ParentIndex << std::endl;
		//	std::cout << get_fname(thing.Name.Name) << std::endl;
		//}
		//std::cout << thing.size() << std::endl;


		vector3 head_3d = player->mesh->get_head();
		vector2 head_2d = w2s(head_3d)/*w2s(vector3(head_3d.x, head_3d.y, head_3d.z + 20)*/;
		debug_2d(head_2d, "head_2d");
		
		vector3 base_3d = player->mesh->get_base();
		vector2 base_2d = w2s(base_3d);

        float box_hight = abs(head_2d.y - base_2d.y);
        float box_width = box_hight * config.BoxWidth;

		//if (GetAsyncKeyState(VK_F1)) {
		//	auto root_comp = read<u>((u)global->local_pawn + 0x320);
		//	//write<vector3>(read<u>((u)player->player + 0x320) + 0x158, vector3(4, 4, 4));
		//	auto c2w = read<FTransform>(root_comp + 0x240);
		//	//c2w.rot = fquat(10, c2w.rot.y,1000,c2w.rot.w);
		//	c2w.translation = vector3(head_3d.x, head_3d.y + 10, head_3d.z);
		//	//std::cout << c2w.rot.z << std::endl;
		//	write<FTransform>(root_comp + 0x240, c2w);

		//	write<bool>(root_comp + 0x188 >> 1, true);
		//}
		
		//std::cout << player->player_distance << std::endl;
		head_2d = w2s(vector3(head_3d.x, head_3d.y, head_3d.z + config.BoxHeightTop));
		if (!head_2d.valid_location())
			continue;

		if (config.Esp) {
			if (config.BoxEsp) {
				if (config.BoxMode == 0) {
					ImVec2 a = ImVec2(head_2d.x - box_width / 2, head_2d.y); //top
					ImVec2 b = ImVec2((head_2d.x - box_width / 2) + box_width, head_2d.y + (box_hight * config.BoxHeightBottom)); //bottom
					if (config.OutlineBox) {
						if (config.RoundBox)
							draw_list->AddRect(a, b, config.BoxOutlineColor.to_u32(), 10, ImDrawCornerFlags_All, config.OutlineBoxThickness);
						else
							draw_list->AddRect(a, b, config.BoxOutlineColor.to_u32(), 0, 0, config.OutlineBoxThickness);

					}
					
					if (config.RoundBox)
						draw_list->AddRect(a, b, config.BoxColor.to_u32(), 10, ImDrawCornerFlags_All, config.BoxThickness);
					else
						draw_list->AddRect(a, b, config.BoxColor.to_u32(), 0, 0, config.BoxThickness);
					
				}
				else if (config.BoxMode == 1) {
					if (config.OutlineBox)
						DrawCornerBox(head_2d.x - box_width / 2, head_2d.y, box_width, box_hight, config.OutlineBoxThickness, config.BoxOutlineColor.to_u32());

					DrawCornerBox(head_2d.x - box_width / 2, head_2d.y, box_width, box_hight, config.BoxThickness, config.BoxColor.to_u32());
				}
				else if (config.BoxMode == 2) {
					//draw 3d box here.....
				}
			}
			
			if (config.LineEsp)
				draw_list->AddLine(ImVec2(screen_width / 2, 0), ImVec2(head_2d.x, head_2d.y), config.LineColor.to_u32(), 2);
		}
		
			
		

		auto dist = GetCrossDistance(head_2d.x, head_2d.y, screen_width / 2, screen_height / 2);
		if (dist < config.AimbotFov && dist < closest_distance && player->player_distance < config.MaxAimbotDistance) {
			closest_distance = dist;
			closest_player = player;
		}
			
    }
	preform_aimbot(closest_distance, closest_player);
	lock.unlock();
			

}