#include "SwimManager.h"
#include "RainManager.h"

namespace Swim
{
	void detail::clear_blood(const RE::Character* a_actor, const RE::hkpCollidable* a_collidable)
	{
		const auto colLayer = static_cast<RE::COL_LAYER>(a_collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);

		if (colLayer == RE::COL_LAYER::kCharController && a_actor->IsSwimming() && util::can_show_blood(a_actor)) {
			util::clear_decals(a_actor, Rain::Manager::GetSingleton()->IsRaining());
		}
	}

    void Install()
	{
		stl::write_vfunc<RE::PlayerCharacter, ProcessInWater::Player>();

		if (Settings::GetSingleton()->GetAllowSwimmingNPC()) {
			stl::write_vfunc<RE::Character, ProcessInWater::NPC>();
		}

		logger::info("Installed swim manager");
	}
}
