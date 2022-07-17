#include "CRC.h"

CRC* CRC::singleton = nullptr;

CRC::CRC() {
	make_crc_table();
}

CRC::~CRC() {
}
