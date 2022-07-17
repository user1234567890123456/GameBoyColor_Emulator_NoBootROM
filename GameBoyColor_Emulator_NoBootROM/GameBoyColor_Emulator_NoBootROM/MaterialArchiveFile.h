#pragma once

#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>

#include "DebugUtility.h"
#include "CRC.h"
#include "XOR.h"

using namespace std;

#define ARCHIVE_FILE_PATH "RESOURCE_DATA"

#define MAP_NAME "GAMEBOY_COLOR_EMULATOR_RESOURCE_ARCHIVE"

struct file_info {
	string file_path_name;
	uint32_t file_size;
	uint64_t data_top_index;
};

extern void __app_safe_exit_error__();

class MaterialArchiveFile
{
private:
	HANDLE f_handle;
	HANDLE map_handle;

	BYTE* mapped_file_buffer = nullptr;
	uint64_t file_data_top_point;//ファイルのデータ部の位置

	vector<file_info> file_info_list;
	uint32_t file_num;

	static MaterialArchiveFile* singleton;

	MaterialArchiveFile();
	~MaterialArchiveFile();
public:
	static void Init() {
		singleton = new MaterialArchiveFile();
	}

	static void End() {
		delete singleton;
	}

	static MaterialArchiveFile* get_instance_ptr() {
		return singleton;
	}

	file_info* get_file_info_ptr(const char* file_path_name);

	void read_file_data(uint64_t file_index, uint32_t read_size, void* buffer);

	void read_file_data_decrypt_xor_256(uint64_t file_index, uint32_t read_size, void* buffer, BYTE* read_file_top_buffer_ptr);

	BYTE* get_mapped_file_buffer_address();
};
