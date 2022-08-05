#pragma once

namespace util
{
	using RACE_FLAG = RE::RACE_DATA::Flag;

    inline bool can_show_blood(const RE::Actor* a_actor)
	{
		const auto race = a_actor->GetRace();
		return race && race->data.flags.none(RACE_FLAG::kDontShowBloodDecal);
	}

    inline bool can_show_blood_and_swim(const RE::Actor* a_actor)
	{
		using Flag = RE::RACE_DATA::Flag;

		const auto race = a_actor->GetRace();
		return race && race->data.flags.none(RACE_FLAG::kDontShowBloodDecal) && race->data.flags.any(RACE_FLAG::kSwims);
	}

    inline void clear_decals(const RE::Actor* a_actor, bool a_isRaining)
	{
		if (a_isRaining) {
			return;
		}

        RE::ScreenSplatter::GetSingleton()->Clear();

		for (std::uint32_t i = 0; i < 2; i++) {
			const auto root = a_actor->Get3D(i);
			if (const auto skinnedDecalNode = root ? static_cast<RE::BGSDecalNode*>(root->GetObjectByName(RE::FixedStrings::GetSingleton()->skinnedDecalNode)) : nullptr) {
				for (auto& decal : skinnedDecalNode->decals) {
					if (decal) {
						decal->lifetime = 0.0f;
					}
				}
			}
			if (const auto& biped = a_actor->GetBiped(i)) {
				for (auto j = stl::to_underlying(RE::BIPED_OBJECT::kHandToHandMelee); j < stl::to_underlying(RE::BIPED_OBJECT::kTotal); j++) {
					if (auto& weapon = biped->objects[j].partClone) {
						RE::BSTempEffectWeaponBlood::ClearEffectForWeapon(weapon.get());
					}
				}
			}
		}
	}

    inline void clear_decals_all()
	{
		if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
			for (auto& actorHandle : processLists->highActorHandles) {
				if (const auto actor = actorHandle.get(); actor && can_show_blood(actor.get())) {
					clear_decals(actor.get(), true);
				}
			}
		}
	}
}
