#include "MaterialArchiveFile.h"

MaterialArchiveFile* MaterialArchiveFile::singleton = nullptr;

MaterialArchiveFile::MaterialArchiveFile() {
	M_debug_printf("MaterialArchiveFile::MaterialArchiveFile() START\n");

	FILE* achive_fp;
	fopen_s(&achive_fp, ARCHIVE_FILE_PATH, "rb");
	if (achive_fp == NULL) {
		MessageBox(NULL, _T("アーカイブの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	fread(&file_num, sizeof(file_num), 1, achive_fp);//ファイルの数を読み取る
	XOR::xor_256byte((BYTE*)(&file_num), sizeof(file_num));

	for (int i = 0; i < file_num; i++) {
		//ファイルの相対パスを読み取る
		
		uint32_t file_path_name_size;
		fread(&file_path_name_size, sizeof(file_path_name_size), 1, achive_fp);//ファイルパスのサイズを読み取る
		XOR::xor_256byte((BYTE*)(&file_path_name_size), sizeof(file_path_name_size));
		M_debug_printf("file_path_name_size = %u\n", file_path_name_size);

		size_t file_path_name_length = (size_t)file_path_name_size + sizeof(char);
		char* file_path_name = (char*)malloc(file_path_name_length);
		if (file_path_name == nullptr) {
			MessageBox(NULL, _T("アーカイブの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

			__app_safe_exit_error__();

			return;
		}
		memset(file_path_name, '\0', file_path_name_length);
		fread(file_path_name, file_path_name_size, 1, achive_fp);//ファイルパスを読み取る
		XOR::xor_256byte((BYTE*)file_path_name, file_path_name_size);
		M_debug_printf("file_path_name = %s\n", file_path_name);

		uint32_t file_size;
		fread(&file_size, sizeof(file_size), 1, achive_fp);//ファイルのサイズを読み取る
		XOR::xor_256byte((BYTE*)(&file_size), sizeof(file_size));
		M_debug_printf("file_size = %u\n", file_size);

		file_info fi;
		fi.file_path_name = file_path_name;
		fi.file_size = file_size;

		file_info_list.push_back(fi);

		free(file_path_name);
	}
	
	file_data_top_point = (uint64_t)ftell(achive_fp);//ファイルのデータ部の位置を取得する

	fclose(achive_fp);

	uint64_t tmp_index = file_data_top_point;
	for (int i = 0; i < file_num; i++) {
		file_info_list[i].data_top_index = tmp_index;

		tmp_index += (uint64_t)file_info_list[i].file_size;
	}

#ifdef GAMEBOYCOLOR_EMULATOR_DEBUG
	for (int i = 0; i < file_num; i++) {
		M_debug_printf("file_info_list[%d] [path = %s] [size = %u] [data_index = %lu]\n", i, file_info_list[i].file_path_name.c_str(), file_info_list[i].file_size, file_info_list[i].data_top_index);
	}
#endif

	//##################################################

	f_handle = CreateFile(
		ARCHIVE_FILE_PATH,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (f_handle == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, _T("アーカイブの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();
	}

	map_handle = CreateFileMapping(
		f_handle,
		0,
		PAGE_READONLY,
		0,
		0,
		MAP_NAME);
	if (map_handle <= (HANDLE)0) {
		CloseHandle(f_handle);

		MessageBox(NULL, _T("アーカイブの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();
	}

	mapped_file_buffer = (BYTE*)MapViewOfFile(
		map_handle,
		FILE_MAP_READ,
		0,
		0,
		0);
	if (mapped_file_buffer == NULL) {
		CloseHandle(f_handle);
		CloseHandle(map_handle);

		MessageBox(NULL, _T("アーカイブの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();
	}

	M_debug_printf("MaterialArchiveFile::MaterialArchiveFile() END\n");
}

MaterialArchiveFile::~MaterialArchiveFile() {
	M_debug_printf("MaterialArchiveFile::~MaterialArchiveFile() START\n");

	UnmapViewOfFile(mapped_file_buffer);

	CloseHandle(f_handle);
	CloseHandle(map_handle);

	M_debug_printf("MaterialArchiveFile::~MaterialArchiveFile() END\n");
}

file_info* MaterialArchiveFile::get_file_info_ptr(const char* file_path_name) {
	for (int i = 0; i < file_num; i++) {
		if (strcmp(file_path_name, (file_info_list[i].file_path_name.c_str())) == 0) {//ファイルが見つかったとき
			return &(file_info_list[i]);
		}
	}

	M_debug_printf("==========================================\n");
	M_debug_printf("ARCHIVE FILE_NOT_FOUND\n");
	M_debug_printf("file_path_name = %s\n", file_path_name);
	M_debug_printf("file_info_list[i].file_path_name.c_str() = %s\n", file_info_list[62].file_path_name.c_str());
	M_debug_printf("==========================================\n");
	return nullptr;//ファイルが見つからないとき
}

void MaterialArchiveFile::read_file_data(uint64_t file_index, uint32_t read_size, void* buffer) {
	BYTE* read_file_buffer_ptr = (BYTE*)((uint32_t)mapped_file_buffer + (uint32_t)file_index);
	memcpy(buffer, read_file_buffer_ptr, read_size);
}


void MaterialArchiveFile::read_file_data_decrypt_xor_256(uint64_t file_index, uint32_t read_size, void* buffer, BYTE* read_file_top_buffer_ptr) {
	BYTE* read_file_buffer_ptr = (BYTE*)((uint32_t)mapped_file_buffer + (uint32_t)file_index);
	memcpy(buffer, read_file_buffer_ptr, read_size);

	BYTE* byte_buffer = (BYTE*)buffer;

	uint32_t key_256_index = (uint32_t)read_file_buffer_ptr - (uint32_t)read_file_top_buffer_ptr;
	key_256_index %= 256;
	for (uint32_t i = 0; i < read_size; i++) {
		byte_buffer[i] ^= key_256[key_256_index];
		key_256_index++;
		key_256_index %= 256;
	}
}


BYTE* MaterialArchiveFile::get_mapped_file_buffer_address() {
	return mapped_file_buffer;
}
