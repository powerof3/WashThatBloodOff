#pragma once

#include "Settings.h"

namespace util
{
	bool can_show_blood(const RE::ActorPtr& a_actor);

	bool can_show_blood_and_swim(const RE::Actor* a_actor);

	void clear_decals(const RE::Actor* a_actor, bool a_skipRainCheck = false);

	void clear_decals_all();
};

namespace Swim
{
	class Animation : public RE::BSTEventSink<RE::BSAnimationGraphEvent>, RE::BSTEventSink<RE::TESObjectLoadedEvent>, RE::BSTEventSink<RE::TESSwitchRaceCompleteEvent>
	{
	public:
		[[nodiscard]] static Animation* GetSingleton()
		{
			static Animation singleton;
			return &singleton;
		}

		static void Register(const RE::Actor* a_actor);
		static void Register();

	private:
		RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*) override;

		RE::BSEventNotifyControl ProcessEvent(const RE::TESObjectLoadedEvent* evn, RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override;
		RE::BSEventNotifyControl ProcessEvent(const RE::TESSwitchRaceCompleteEvent* evn, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*) override;

		Animation() = default;
		Animation(const Animation&) = delete;
		Animation(Animation&&) = delete;

		~Animation() override = default;

		Animation& operator=(const Animation&) = delete;
		Animation& operator=(Animation&&) = delete;

		inline constexpr static std::string_view swimEvent{ "SoundPlay.FSTSwimSwim"sv };
	};
}

namespace Rain
{
	class Manager
	{
	public:
		[[nodiscard]] static Manager* GetSingleton()
		{
			static Manager singleton;
			return &singleton;
		}

		static void Install()
		{
			REL::Relocation<std::uintptr_t> precipitationUpdate{ REL::ID(25638), 0x238 };
			stl::write_thunk_call<ToggleWaterRipples>(precipitationUpdate.address());

			REL::Relocation<std::uintptr_t> load_interior{ REL::ID(13171) };
			stl::write_thunk_call<SetInterior>(load_interior.address() + 0x2E6);

			REL::Relocation<std::uintptr_t> leave_interior{ REL::ID(13172), 0x2A };
			stl::write_thunk_call<SetInterior>(leave_interior.address());

			logger::info("Installed rain manager");
		}

		bool IsRaining() const { return _raining; }
		void SetRaining(const bool a_isRaining) { _raining = a_isRaining; }

	protected:
		struct ToggleWaterRipples
		{
			static void thunk(RE::TESWaterSystem* a_manager, bool a_toggle, float a_unk03)
			{
				func(a_manager, a_toggle, a_unk03);

				const auto manager = GetSingleton();
				if (const bool showRipple = a_manager->showProcedualWater; manager->IsRaining() != showRipple) {
					manager->SetRaining(showRipple);

					if (manager->IsRaining()) {
						util::clear_decals_all();
					}
				}
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct SetInterior
		{
			static void thunk(bool a_isInterior)
			{
				func(a_isInterior);

				const auto manager = GetSingleton();
				if (a_isInterior) {
					manager->SetRaining(false);
				} else if (const auto sky = RE::Sky::GetSingleton()) {
					manager->SetRaining(sky->GetIsRaining());
					if (manager->IsRaining()) {
						util::clear_decals_all();
					}
				}
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

	private:
		Manager() = default;
		Manager(const Manager&) = delete;
		Manager(Manager&&) = delete;

		~Manager() = default;

		Manager& operator=(const Manager&) = delete;
		Manager& operator=(Manager&&) = delete;

		std::atomic_bool _raining{ false };
	};

	namespace Decal::Actor
	{
		struct AddDecal
		{
			static void thunk(RE::TESObjectCELL* a_cell, RE::DECAL_CREATION_DATA& a_data, bool a_unk03)
			{
				if (!Manager::GetSingleton()->IsRaining()) {
					func(a_cell, a_data, a_unk03);
				}
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct GetScreenSplatterCount
		{
			static std::int32_t thunk(float a_damage)
			{
				auto count = func(a_damage);
				if (Manager::GetSingleton()->IsRaining()) {
					count = 0;
				}
				return count;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		inline void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(36682) };  //Actor::CreateBlood

			stl::write_thunk_call<GetScreenSplatterCount>(target.address() + 0x13A);

			stl::write_thunk_call<AddDecal>(target.address() + 0x925);  // Player

			if (Settings::GetSingleton()->GetAllowRainingNPC()) {
				stl::write_thunk_call<AddDecal>(target.address() + 0x10D5);  // NPC
			}

			logger::info("Installed no rain blood splatter");
		}
	}

	namespace Decal::Weapon
	{
		struct GetUnk317
		{
			static bool thunk(RE::AIProcess* a_process)
			{
				auto result = func(a_process);
				if (result) {
					result = Manager::GetSingleton()->IsRaining();
				}
				return result;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		inline void Install()
		{
			REL::Relocation<std::uintptr_t> createBloodOnWeapon{ REL::ID(36683), 0x89 };
			stl::write_thunk_call<GetUnk317>(createBloodOnWeapon.address());
		}
	}

	void Install();
}
