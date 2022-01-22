#pragma once

class Settings
{
public:
	struct WashData
	{
		bool enable{ true };
		bool affectNPC{ true };
		bool preventBlood{ true };
	};

	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	void LoadSettings()
	{
		constexpr auto path = L"Data/SKSE/Plugins/po3_WashThatBloodOff.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		detail::get_value(ini, swimming.enable, "Swimming", "Enable", ";Wash blood off when swimming");
		detail::get_value(ini, swimming.affectNPC, "Swimming", "Affect NPCs", ";Wash blood off when NPCs are swimming");

		detail::get_value(ini, raining.enable, "Raining", "Enable", ";Wash blood off when raining");
		detail::get_value(ini, raining.affectNPC, "Raining", "Affect NPCs", ";Wash NPC blood off when raining");
		detail::get_value(ini, raining.preventBlood, "Raining", "No Blood Splatter", ";Characters will not get bloody when raining");

		ini.SaveFile(path);
	}

	[[nodiscard]] bool GetAllowSwimming() const { return swimming.enable; }
	[[nodiscard]] bool GetAllowSwimmingNPC() const { return swimming.affectNPC; }
	[[nodiscard]] bool GetAllowRaining() const { return raining.enable; };
	[[nodiscard]] bool GetAllowRainingNPC() const { return raining.affectNPC; }
	[[nodiscard]] bool GetAllowRainingNoBlood() const { return raining.preventBlood; }

private:
	struct detail
	{
		static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
			a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);
		}
	};

	Settings() = default;
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	~Settings() = default;

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;

	WashData swimming;
	WashData raining;
};
