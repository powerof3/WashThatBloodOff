#include "Settings.h"

void Settings::LoadSettings()
{
	const auto path = fmt::format("Data/SKSE/Plugins/{}.ini", Version::PROJECT);

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path.c_str());

	ini::get_value(ini, swimming.enable, "Swimming", "Enable", ";Wash blood off when swimming");
	ini::get_value(ini, swimming.affectNPC, "Swimming", "Affect NPCs", ";Wash blood off when NPCs are swimming");

	ini::get_value(ini, raining.enable, "Raining", "Enable", ";Wash blood off when raining");
	ini::get_value(ini, raining.affectNPC, "Raining", "Affect NPCs", ";Wash NPC blood off when raining");
	ini::get_value(ini, raining.preventBlood, "Raining", "No Blood Splatter", ";Characters will not get bloody when raining");

	(void)ini.SaveFile(path.c_str());
}

bool Settings::GetAllowSwimming() const
{
	return swimming.enable;
}

bool Settings::GetAllowSwimmingNPC() const
{
	return swimming.affectNPC;
}

bool Settings::GetAllowRaining() const
{
	return raining.enable;
}

bool Settings::GetAllowRainingNPC() const
{
	return raining.affectNPC;
}

bool Settings::GetAllowRainingNoBlood() const
{
	return raining.preventBlood;
}
