#pragma once

#include "MyDirectXSystem.h"
#include "MyDirectXImage.h"
#include "Key.h"
#include "GameBoyColor.h"
#include "Main.h"
#include "MyDirectXDraw.h"

class GameManager
{
private:
	static GameManager* singleton;

	GameBoyColor* gbx = nullptr;

	MyDirectXSystem *myDirectXSystem;

	//LPD3DXSPRITE sprite;

	Key *key;

	GameManager(MyDirectXSystem* myDirectXSystem, Key* key/*, LPD3DXSPRITE sprite*/);
	~GameManager();
public:
	static void Init(MyDirectXSystem* myDirectXSystem, Key* key) {
		singleton = new GameManager(myDirectXSystem, key);
	}

	static void End() {
		delete singleton;
	}

	static GameManager* get_instance_ptr() {
		return singleton;
	}

	void execute_game_process();

	GameBoyColor* get_gameboy() {
		return gbx;
	}
};
