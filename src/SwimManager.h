#pragma once

namespace Swim
{
	struct detail
	{
		static void ClearBloodInWater(const RE::Character* a_actor, const RE::hkpCollidable* a_collidable);
	};

	struct ProcessInWater
	{
		struct Player
		{
			static bool thunk(RE::PlayerCharacter* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
			{
				detail::ClearBloodInWater(a_actor, a_collidable);

				return func(a_actor, a_collidable, a_waterHeight, a_deltaTime);
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t idx{ 0x9C };
		};

		struct NPC
		{
			static bool thunk(RE::Character* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
			{
				detail::ClearBloodInWater(a_actor, a_collidable);

				return func(a_actor, a_collidable, a_waterHeight, a_deltaTime);
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t idx{ 0x9C };
		};
	};

	void Install();
}
