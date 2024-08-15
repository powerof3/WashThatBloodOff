#include "RainManager.h"

namespace Rain
{
	void Manager::Install()
	{
		std::array targets{
			std::make_pair(RELOCATION_ID(25682, 26229), OFFSET(0x463, 0x766)),  //Sky::Update
			std::make_pair(RELOCATION_ID(25679, 26222), OFFSET(0xAA, 0x128)),   //Sky::SetMode
		};

		for (const auto& [id, offset] : targets) {
			REL::Relocation<std::uintptr_t> target{ id, offset };
			stl::write_thunk_call<UpdatePrecipitation>(target.address());
		}

		REL::Relocation<std::uintptr_t> load_interior{ RELOCATION_ID(13171, 13316), OFFSET(0x2E6, 0x46D) };
		stl::write_thunk_call<SetInterior>(load_interior.address());

		REL::Relocation<std::uintptr_t> leave_interior{ RELOCATION_ID(13172, 13317), OFFSET(0x2A, 0x1E) };
		stl::write_thunk_call<SetInterior>(leave_interior.address());
	}

	void Decal::Actor::Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(36682, 37690) };  // Actor::CreateBlood
		stl::write_thunk_call<GetScreenSplatterCount>(target.address() + OFFSET(0x13A, 0x127));

	    stl::write_thunk_call<AddDecal>(target.address() + OFFSET(0x925, 0x9B5));  // Player

		if (Settings::GetSingleton()->GetAllowRainingNPC()) {
			stl::write_thunk_call<AddDecal>(target.address() + OFFSET(0x10D5,0x1194));  // NPC
		}
	}

	void Decal::Weapon::Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(36683, 37691), OFFSET(0x89,0x88) }; // CreateBloodOnWeapon
		stl::write_thunk_call<GetUnk317>(target.address());
	}

	void Install()
	{
		Manager::Install();

		if (Settings::GetSingleton()->GetAllowRainingNoBlood()) {
			Decal::Actor::Install();
			Decal::Weapon::Install();
		}

		logger::info("Installed rain manager");
	}
}
