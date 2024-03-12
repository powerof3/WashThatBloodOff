#pragma once

namespace util
{
    inline bool can_show_blood(const RE::Actor* a_actor)
	{
		const auto race = a_actor->GetRace();
		return race && race->data.flags.none(RE::RACE_DATA::Flag::kDontShowBloodDecal);
	}

    inline void clear_decals(const RE::Actor* a_actor)
	{
		for (std::uint32_t i = 0; i < 2; i++) {
			const auto root = a_actor->Get3D(i);
			if (const auto skinnedDecalNode = root ? static_cast<RE::BGSDecalNode*>(root->GetObjectByName(RE::FixedStrings::GetSingleton()->skinnedDecalNode)) : nullptr) {
				for (auto& decal : skinnedDecalNode->decals) {
					if (decal) {
					    decal->lifetime = 0.0f;
					}
				}
			}
			if (const auto& biped = a_actor->GetBiped(i); biped) {
				for (auto j = std::to_underlying(RE::BIPED_OBJECT::kHandToHandMelee); j < std::to_underlying(RE::BIPED_OBJECT::kTotal); j++) {
					if (auto& weapon = biped->objects[j].partClone; weapon) {
						RE::BSTempEffectWeaponBlood::ClearEffectForWeapon(weapon.get());
					}
				}
			}
		}
	}

	inline void clear_decals(const RE::Actor* a_actor, bool a_alreadyRaining)
	{
		if (a_alreadyRaining) {
			return;
		}

		if (a_actor->IsPlayerRef()) {
			RE::ScreenSplatter::GetSingleton()->Clear();
		}

		clear_decals(a_actor);
	}

    inline void clear_decals_all()
	{
        if (const auto processLists = RE::ProcessLists::GetSingleton()) {
			for (auto& actorHandle : processLists->highActorHandles) {
				if (const auto actor = actorHandle.get(); actor && can_show_blood(actor.get())) {
					clear_decals(actor.get());
				}
			}
		}
	}
}
