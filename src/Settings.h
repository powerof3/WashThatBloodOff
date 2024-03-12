#pragma once

class Settings : public ISingleton<Settings>
{
public:
	struct WashData
	{
		bool enable{ true };
		bool affectNPC{ true };
		bool preventBlood{ true };
	};

	void LoadSettings();

    [[nodiscard]] bool GetAllowSwimming() const;
    [[nodiscard]] bool GetAllowSwimmingNPC() const;
    [[nodiscard]] bool GetAllowRaining() const;
	[[nodiscard]] bool GetAllowRainingNPC() const;
    [[nodiscard]] bool GetAllowRainingNoBlood() const;

private:
	WashData swimming;
	WashData raining;
};
