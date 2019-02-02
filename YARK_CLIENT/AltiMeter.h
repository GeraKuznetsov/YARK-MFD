#pragma once

#define WARNING_ALTADTUDES 11
const int warn_altitudes[WARNING_ALTADTUDES] = { 1000,500,400,300,200,100,50,40,30,20,10 };
Sound* warn_altitudes_sounds[WARNING_ALTADTUDES];

float RALT_LastAlt = 0;

void RadioAltimeterTick() {
	if (RegInt("ENABLE_RADIO-ALT", 0)) {
		for (int i = 0; i < WARNING_ALTADTUDES; i++) {
			if (RALT_LastAlt > warn_altitudes[i] && client.Vessel.RAlt < warn_altitudes[i]) {
				PlaySound(warn_altitudes_sounds[i]);
				RALT_LastAlt = client.Vessel.RAlt;
				return;
			}
		}
	}
	RALT_LastAlt = client.Vessel.RAlt;
}