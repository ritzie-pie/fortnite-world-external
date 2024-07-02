#include "util.hpp"
#include "cheat/actors/esp.hpp"
#include "cheat/actors/aimbot.hpp"

Overlay::DXOverlay* pOverlay = nullptr;


int main() {

	process_id = find_process("FortniteClient-Win64-Shipping.exe");
	Driver::AttachProcess(L"FortniteClient-Win64-Shipping.exe");

	uint64_t processBaseAddress = Driver::GetProcessBaseAddress();
	printf("ProcessBaseAddress: 0x%llX\n", processBaseAddress);
	Driver::baseAddress = processBaseAddress;
	base_address = processBaseAddress;

	uint64_t ProcessPEB = Driver::GetPebAddress();
	printf("Peb address: 0x%llX\n", ProcessPEB);


	Overlay::DXOverlay::GetWindow(FindWindow(0, "Fortnite  "));
	auto InitOverlay = pOverlay->InitOverlay();

	std::thread p(cache_actors_combined);
	p.detach();


	std::filesystem::create_directories("C:\\evac");
	std::filesystem::create_directories("C:\\evac\\configs");
	std::filesystem::create_directories("C:\\evac\\configs\\fortnite");

	config.refresh_configs();
	config.load_config(0);
	switch (InitOverlay)
	{
	case 0: { break; }
	case 1:
	{
		const double target_fps = 240.0;
		
		MouseController::Init();
		while (pOverlay->MainLoop(actor_loop)) {
			
			
		}
		break;
	}
	}
}