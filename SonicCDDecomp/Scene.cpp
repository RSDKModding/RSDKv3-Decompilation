#include "RetroEngine.h"

void Scene::InitFirstStage(void) {

}

void Scene::ProcessStage(void) {

	switch (StageMode)
	{
	case 0: //Startup

		for (int i = 0; i < PLAYER_COUNT; i++) {
			MEM_ZERO(Player::Players[i]);
			Player::Players[i].Visible = 1;
			Player::Players[i].Gravity = 1; //Air
			Player::Players[i].TileCollisions = true;
			Player::Players[i].ObjectInteractions = true; 
		}

		break;
	case 1: //Regular
		break;
	case 2: //Paused
		break;
	}

}