#include "My_Random.h"

My_Random* My_Random::singleton = nullptr;

My_Random *My_Random::get_instance_ptr() {
	return singleton;
}

void My_Random::_make_singleton() {
	M_debug_printf("My_Random::_make_singleton()...\n");

	if (singleton == nullptr) {
		singleton = new My_Random();
	}
}

void My_Random::_delete_singleton() {
	M_debug_printf("My_Random::_delete_singleton()...\n");

	delete singleton;
}

My_Random::My_Random()
{
}

My_Random::~My_Random()
{
}

void My_Random::set_random_seed()
{
	M_debug_printf("My_Random::set_random_seed()...\n");

	random_device rnd_dev;
	rnd_seed = rnd_dev();//ランダムにシード値を生成する

	mt19937 engine(rnd_seed);
	mt_engine = engine;

	M_debug_printf("=================================\n");
	M_debug_printf("rnd_seed = %u\n", (unsigned int)rnd_seed);
	M_debug_printf("=================================\n");
}

void My_Random::set_replay_seed(uint32_t replay_seed) {
	M_debug_printf("My_Random::set_replay_seed(uint32_t replay_seed)...\n");

	rnd_seed = (random_device::result_type)replay_seed;

	mt19937 engine(rnd_seed);
	mt_engine = engine;

	M_debug_printf("=================================\n");
	M_debug_printf("rnd_seed = %u\n", (unsigned int)rnd_seed);
	M_debug_printf("=================================\n");
}

random_device::result_type My_Random::get_seed()
{
	return rnd_seed;
}

uint32_t My_Random::get_random_uint32() {
	return (uint32_t)mt_engine();
}
