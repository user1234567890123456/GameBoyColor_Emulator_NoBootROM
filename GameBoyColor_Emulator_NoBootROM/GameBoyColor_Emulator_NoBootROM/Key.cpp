#include "Key.h"

Key::Key()
{
	for (int i = 0; i < ALL_MIX_INPUT_NUMBER; i++) {
		mix_input_state[i] = 0;
	}
}

Key::~Key()
{
}
