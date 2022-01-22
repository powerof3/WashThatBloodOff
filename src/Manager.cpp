#include "Manager.h"

namespace util
{
	bool can_show_blood(const RE::ActorPtr& a_actor)
	{
		using Flag = RE::RACE_DATA::Flag;

		const auto race = a_actor->GetRace();
		return race && race->data.flags.none(Flag::kDontShowBloodDecal);
	}

	bool can_show_blood_and_swim(const RE::Actor* a_actor)
	{
		using Flag = RE::RACE_DATA::Flag;

		const auto race = a_actor->GetRace();
		return race && race->data.flags.none(Flag::kDontShowBloodDecal) && race->data.flags.any(Flag::kSwims);
	}

	void clear_decals(const RE::Actor* a_actor, bool a_skipRainCheck)
	{
		if (!a_skipRainCheck && Rain::Manager::GetSingleton()->IsRaining()) {
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

	void clear_decals_all()
	{
		clear_decals(RE::PlayerCharacter::GetSingleton(), true);

		if (!Settings::GetSingleton()->GetAllowRainingNPC()) {
			return;
		}

		if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
			for (auto& actorHandle : processLists->highActorHandles) {
				if (const auto actor = actorHandle.get(); actor && can_show_blood(actor)) {
					clear_decals(actor.get(), true);
				}
			}
		}
	}
}

namespace Swim
{
	void Animation::Register()
	{
		if (const auto scripts = RE::ScriptEventSourceHolder::GetSingleton()) {
			scripts->AddEventSink<RE::TESObjectLoadedEvent>(GetSingleton());
			scripts->AddEventSink<RE::TESSwitchRaceCompleteEvent>(GetSingleton());
			logger::info("Registered {}"sv, typeid(Animation).name());
		}
	}

	void Animation::Register(const RE::Actor* a_actor)
	{
		if (a_actor->IsPlayerRef() || Settings::GetSingleton()->GetAllowSwimmingNPC() && util::can_show_blood_and_swim(a_actor)) {
			a_actor->AddAnimationGraphEventSink(GetSingleton());
		}
	}

	RE::BSEventNotifyControl Animation::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*)
	{
		if (!a_event || !a_event->holder || a_event->tag != swimEvent) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const auto actor = a_event->holder->As<RE::Actor>();
		if (!actor) {
			return RE::BSEventNotifyControl::kContinue;
		}

        util::clear_decals(actor);

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl Animation::ProcessEvent(const RE::TESObjectLoadedEvent* evn, RE::BSTEventSource<RE::TESObjectLoadedEvent>*)
	{
		if (!evn || !evn->loaded) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const auto actor = RE::TESForm::LookupByID<RE::Actor>(evn->formID);
		if (!actor) {
			return RE::BSEventNotifyControl::kContinue;
		}

		Register(actor);

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl Animation::ProcessEvent(const RE::TESSwitchRaceCompleteEvent* evn, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*)
	{
		if (!evn || !evn->subject) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const auto actor = evn->subject->As<RE::Actor>();
		if (!actor) {
			return RE::BSEventNotifyControl::kContinue;
		}

		Register(actor);

		return RE::BSEventNotifyControl::kContinue;
	}
}

namespace Rain
{
	void Install()
	{
		Manager::Install();

		if (Settings::GetSingleton()->GetAllowRainingNoBlood()) {
			Decal::Actor::Install();
			Decal::Weapon::Install();
		}
	}
}
