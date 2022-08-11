#pragma once

#include "Settings.h"
#include "Util.h"

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

		static void Install();

		bool IsRaining() const { return _raining; }
		void SetRaining(const bool a_isRaining) { _raining = a_isRaining; }

	protected:
		struct UpdatePrecipitation
		{
			static void thunk(RE::Precipitation* a_precip)
			{
				func(a_precip);

				const auto manager = GetSingleton();
				const auto sky = RE::Sky::GetSingleton();

				if (bool isRaining = sky->IsRaining(); manager->IsRaining() != isRaining) {
					manager->SetRaining(isRaining);

					if (manager->IsRaining()) {
						util::clear_decals(RE::PlayerCharacter::GetSingleton(), false);
					    if (Settings::GetSingleton()->GetAllowRainingNPC()) {
							util::clear_decals_all();
						}
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
				} else if (const auto sky = RE::Sky::GetSingleton(); sky) {
					util::clear_decals(RE::PlayerCharacter::GetSingleton(), false);
					if (Settings::GetSingleton()->GetAllowRainingNPC()) {
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

		void Install();
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

		void Install();
	}

	void Install();
}
