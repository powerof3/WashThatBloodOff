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

    [[nodiscard]] bool GetAllowSwimming() const;
    [[nodiscard]] bool GetAllowSwimmingNPC() const;
    [[nodiscard]] bool GetAllowRaining() const;
	[[nodiscard]] bool GetAllowRainingNPC() const;
    [[nodiscard]] bool GetAllowRainingNoBlood() const;

private:
	struct detail
	{
		static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
			a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);
		}
	};

	Settings();
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	~Settings() = default;

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;

	WashData swimming;
	WashData raining;
};
