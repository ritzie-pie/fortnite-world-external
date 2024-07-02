#pragma once
#include "../util.hpp"
#//include "driver/driver.h"
#include "structs.hpp"
#define u uintptr_t

inline extern uintptr_t process_id;
inline extern uintptr_t base_address;

template <typename T>
inline extern T read(uint64_t address);
inline extern std::string read_string(uintptr_t Address, wchar_t* Buffer, SIZE_T Size);
template<typename T>
inline extern void write(uint64_t address, T buffer);
class aactor;
class aplayerstate;
inline void print_ptr(uintptr_t p, const char* s);
class aplayercameramanager {
public:
	camera_position_s get_camera_cache() const {
		if (!(uintptr_t)this) return {};
		return read<camera_position_s>((uintptr_t)this + 0x1ae0 + 0x10);
	}
};
struct FBoxSphereBounds
{
	vector3 Origin;
	vector3 BoxExtent;
	double SphereRadius;
};
class uschenecomponent {
public:
	vector3 get_relative_location() const {
		if (!(u)this) return {};
		return read<vector3>((u)this + 0x120);
	}
	vector3 get_relative_rotation() const {
		if (!(u)this) return {};
		return read<vector3>((u)this + 0x138);
	}
	FBoxSphereBounds get_bounds() {
		if (!(uintptr_t)this) return {};
		return read<FBoxSphereBounds>((uintptr_t)this + 0xDEADBEEF);
	}
};
class uskeletalmeshcomponent {
public:
	vector3 get_bone_3d(int id) const {
		if (!(uintptr_t)this) return {};
		int isCached = read<int>((u)this + 0x5e0);
		auto bone_transform = read<FTransform>(read<u>((u)this + 0x10 * isCached + 0x598) + 0x60 * id);
		
		FTransform ComponentToWorld = read<FTransform>((u)this + 0x1c0);
		
		D3DMATRIX Matrix = {};
		Matrix = MatrixMultiplication(bone_transform.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
		
		return vector3(Matrix._41, Matrix._42, Matrix._43);
	}
	std::vector<vector3> get_bones() {
		int isCached = read<int>((u)this + 0x5e0);
		auto dtc = read<tarray<FTransform>>((uintptr_t)this + 0x10 * isCached + 0x598);
		auto bones = dtc.getiter();
		FTransform ComponentToWorld = read<FTransform>((uintptr_t)this + 0x1c0);
		std::vector<vector3> return_bones;
		for (int i = 0; i < bones.size(); i++) {
			D3DMATRIX Matrix = MatrixMultiplication(bones[i].ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
			return_bones.push_back(vector3(Matrix._41, Matrix._42, Matrix._43));
		}
		return return_bones;
	}
	vector3 get_head() const {
		if (!(uintptr_t)this) return {};
		return get_bone_3d(109);
	}
	vector3 get_base() const {
		if (!(uintptr_t)this) return {};
		return get_bone_3d(0);
	}
	float get_last_submit_time() const {
		if (!(uintptr_t)this) return 0;
		return read<float>((u)this + 0x2E8);
	}
	float get_last_render_time() const {
		if (!(uintptr_t)this) return 0;
		return read<float>((u)this + 0x2F0);
	}
	bool test_visible() const {
		return get_last_submit_time() == get_last_render_time();

	}
	bool get_bvisible() const {
		float fLastSubmitTime = get_last_submit_time();
		float fLastRenderTimeOnScreen = get_last_render_time();
		const float fVisionTick = 0.06f;
		bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
		return bVisible;
	}

};
enum class EFortItemTier : uint8_t
{
	No_Tier = 0,
		I = 1,
		II = 2,
		III = 3,
		IV = 4,
		V = 5,
		VI = 6,
		VII = 7,
		VIII = 8,
		IX = 9,
		X = 10,
		NumItemTierValues = 11,
		EFortItemTier_MAX = 12
};
class afortweapon {
public:
	std::string get_weapon_name() const {
		if (!(u)this) return "";
		
		auto itemdef = read<uint64_t>((u)this + 0x4d0); //WeaponData		
		if (!itemdef) return "";
		//std::cout << itemdef << std::endl;
		auto DisplayName = read<uint64_t>(itemdef + 0x30); // DisplayName (FText
		if (!DisplayName) return "";
		auto WeaponLength = read<uint32_t>(DisplayName + 0x30); // FText -> Length
		wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];
		
		read_string((ULONG64)read<PVOID>(DisplayName + 0x28), WeaponName, WeaponLength * sizeof(wchar_t));
		
		return wchar_to_char(WeaponName);
	}
	void rapid_fire() {
		if (!(uintptr_t)this) return;

		
		//static float a = read<float>((uintptr_t)this + 0xad8); //LastFireTime Offset
		//static float b = read<float>((uintptr_t)this + 0xadc); //LastFireTimeVerified Offset
		//write<float>((uintptr_t)this + 0xad8, a + b - 0.3333); //LastFireTime Offset

	}
};
class ucapsulecomponent {
public:
	FBoxSphereBounds get_bounds() {
		if (!(uintptr_t)this) return {};
		return read<FBoxSphereBounds>((uintptr_t)this + 0xDEADBEEF);
	}
};
class aactor {
public:
	
	uschenecomponent* get_root_component() const {
		if (!(u)this) return 0;
		return read<uschenecomponent*>((u)this + 0x198);
	}
	aplayerstate* get_player_state() {
		if (!(u)this) return 0;
		return read<aplayerstate*>((u)this + 0x2b0);
	}
	uskeletalmeshcomponent* get_mesh() const {
		if (!(u)this) return 0;
		return read<uskeletalmeshcomponent*>((u)this + 0x318);
	}
	ucapsulecomponent* get_capsule_component() const {
		if (!(u)this) return 0;
		return read<ucapsulecomponent*>((u)this + 0x328);
	}
	afortweapon* get_current_weapon() const {
		if (!(u)this) return 0;
		return read<afortweapon*>((u)this + 0xa20);
	}
	int get_actor_id() const {
		if (!(u)this) return 0;
		return read<int>((u)this + 0x18);
	}
	std::string get_pickup_name() const {
		if (!(u)this) return "";

		auto pickup = read<u>((u)this + 0x350 + 0x18); //PrimaryPickupItemEntry		
		//print_ptr(pickup, "pickup");
		if (!pickup) return "";

		auto DisplayName = read<uint64_t>(pickup + 0x30); // DisplayName (FText
		if (!DisplayName) return "";
		auto WeaponLength = read<uint32_t>(DisplayName + 0x30); // FText -> Length
		wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

		read_string((ULONG64)read<PVOID>(DisplayName + 0x28), WeaponName, WeaponLength);

		return wchar_to_char(WeaponName);
	}
	EFortItemTier get_loot_tier() const {
		if (!(u)this) return EFortItemTier::No_Tier;
		auto pickup = read<u>((u)this + 0x350 + 0x18); //PrimaryPickupItemEntry		
		//print_ptr(pickup, "pickup");
		if (!pickup) return EFortItemTier::No_Tier;
		auto teir = read<EFortItemTier>(pickup + 0x73);
		return teir;
	}
	std::string get_trap_name() const {
		if (!(u)this) return "";

		auto trap = read<u>((u)this + 0xc18); //TrapData		
		if (!trap) return "";

		auto DisplayName = read<uint64_t>(trap + 0x30); // DisplayName (FText
		if (!DisplayName) return "";
		auto WeaponLength = read<uint32_t>(DisplayName + 0x30); // FText -> Length
		wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

		read_string((ULONG64)read<PVOID>(DisplayName + 0x28), WeaponName, WeaponLength);

		return wchar_to_char(WeaponName);
	}
	std::string getcapture_name() const {
		if (!(u)this) return "";

		auto trap = read<u>((u)this + 0x288); //TrapData		
		if (!trap) return "";

		auto DisplayName = read<uint64_t>(trap + 0x30); // DisplayName (FText
		if (!DisplayName) return "";
		auto WeaponLength = read<uint32_t>(DisplayName + 0x30); // FText -> Length
		wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

		read_string((ULONG64)read<PVOID>(DisplayName + 0x28), WeaponName, WeaponLength);

		return wchar_to_char(WeaponName);
	}
	
};
class aplayerstate {
public:
	aactor* get_pawn_private() const {
		if (!(u)this) return 0;
		return read<aactor*>((u)this + 0x308);
	}
	int get_player_id() const {
		if (!(u)this) return 0;
		return read<int>((u)this + 0x10f1);
	}
};
class aplayercontroller {
public:
	aactor* get_acknowledged_pawn() const {
		if (!(u)this) return 0;
		return read<aactor*>((u)this + 0x338);
	}
	
	float get_fov() const {
		if (!(u)this) return 0;
		return read<float>((u)this + 0x394) * 90.f;
	}
	aplayercameramanager* get_camera_manager() const {
		if (!(uintptr_t)this) return 0;
		return (aplayercameramanager*)read<uintptr_t>((uintptr_t)this + 0x348);
	}
};
class ulocalplayer {
public:
	aplayercontroller* get_player_controller() const {
		if (!(u)this) return 0;
		return read<aplayercontroller*>((u)this + 0x30);
	}

};
class ugameinstance {
public:
	ulocalplayer* get_local_player() const {
		if (!(u)this) return 0;
		return read<ulocalplayer*>((u)read<ulocalplayer*>((u)this + 0x38));
	}

};

class agamestate {
public:
	tarray<aplayerstate*> get_player_array() const {
		if (!(u)this) return {};
		return read<tarray<aplayerstate*>>((u)this + 0x2a8);
	}

};
class ulevel {
public:
	std::vector<aactor*> get_actor_array() {
		if (!(u)this) return {};
		return read<tarray<aactor*>>((u)this + 0xA0).getiter();
	}

};
class uworld {
public:
	ulevel* get_persistent_level() const {
		if (!(u)this) return 0;
		return read<ulevel*>((u)this + 0x30);
	}
	ugameinstance* get_game_instance() const {
		if (!(u)this) return 0;
		return read<ugameinstance*>((u)this + 0x1d0);
	}
	agamestate* get_game_state() const {
		if (!(u)this) return 0;
		return read<agamestate*>((u)this + 0x158);
	}
	std::vector<ulevel*> get_levels_array() const {
		if (!(u)this) return {};
		return read<tarray<ulevel*>>((u)this + 0x170).getiter();
	}
};

inline std::string get_fname_from_chunch(DWORD chunck) {
	ULONG64 blockoffset = HIWORD(chunck);
	USHORT nameoffset = (USHORT)chunck;

	ULONG64 namedpoolblock = read<ULONG64>(base_address + 0x11944BC0 + (8 * blockoffset) + 16) + (UINT)(2 * nameoffset);
	USHORT namedentry = read<USHORT>(namedpoolblock);

	int nameLength = namedentry >> 6;
	char buff[1024]{};

	if ((uint32_t)nameLength)
	{
		for (int x = 0; x < nameLength; ++x)
		{
			buff[x] = read<char>(namedpoolblock + 2 + x);
		}
		char* v2 = buff;
		int v4; // ecx
		unsigned int v5 = nameLength;
		__int64 v6; // rdx
		char v7; // al
		v4 = 9;
		if (v5)
		{
			v6 = v5;
			do
			{
				v5 = v4 + ~*v2;
				v4 += 7947;
				*v2++ = v5;
				--v6;
			} while (v6);
		}
		buff[nameLength] = '\0';
		return std::string(buff);
	}
	else
	{
		return "";
	}
}
inline std::string get_fname(int actor_id) {
	uint32_t ChunkOffset = (uint32_t)((int)(actor_id) >> 16);
	uint16_t NameOffset = (uint16_t)actor_id;

	uint64_t NamePoolChunk = read<uint64_t>(base_address + 0x11944BC0 + (8 * ChunkOffset) + 16) + (unsigned int)(2 * NameOffset);

	if (read<uint16_t>(NamePoolChunk) < 64)
	{
		DWORD a1 = read<DWORD>(NamePoolChunk + 2);
		return get_fname_from_chunch(a1);
	}
	else
	{
		return get_fname_from_chunch(actor_id);
	}
}
