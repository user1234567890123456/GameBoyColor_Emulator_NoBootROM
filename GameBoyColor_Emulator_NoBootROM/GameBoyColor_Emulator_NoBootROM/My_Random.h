#pragma once

#include "DebugUtility.h"

#include <random>

using namespace std;

class My_Random
{
private:
	static My_Random *singleton;

	mt19937 mt_engine;
	random_device::result_type rnd_seed = (unsigned int)0;

	My_Random();
	~My_Random();
public:
	static My_Random *get_instance_ptr();

	static void _make_singleton();
	static void _delete_singleton();

	void set_random_seed();
	void set_replay_seed(uint32_t replay_seed);

	random_device::result_type get_seed();

	uint32_t get_random_uint32();
};
