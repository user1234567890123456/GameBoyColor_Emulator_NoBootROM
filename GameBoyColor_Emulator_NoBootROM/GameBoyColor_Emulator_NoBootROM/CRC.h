#pragma once

#include <stdint.h>
#include <windows.h>
#include <vector>

using namespace std;

class CRC
{
private:
    static CRC* singleton;

    uint32_t file_crc_32___c;

    uint32_t crc_table[256] = { 0 };

    void make_crc_table(void) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int j = 0; j < 8; j++) {
                c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
            }
            crc_table[i] = c;
        }
    }

    CRC();
    ~CRC();
public:
    static void Init() {
        singleton = new CRC();
    }

    static void End() {
        delete singleton;
    }

    static CRC* get_instance_ptr() {
        return singleton;
    }

    uint32_t crc32(uint8_t* buf, size_t len) {
        uint32_t c = 0xFFFFFFFF;
        for (size_t i = 0; i < len; i++) {
            c = crc_table[(c ^ buf[i]) & 0xFF] ^ (c >> 8);
        }
        return c ^ 0xFFFFFFFF;
    }

    uint32_t vector_crc32(const vector<uint8_t>& vector_data) {
        uint32_t c = 0xFFFFFFFF;
        for (size_t i = 0; i < vector_data.size(); i++) {
            c = crc_table[(c ^ vector_data[i]) & 0xFF] ^ (c >> 8);
        }
        return c ^ 0xFFFFFFFF;
    }

    //ファイル専用CRC用変数の初期化
    void init_file_crc32() {
        file_crc_32___c = 0xFFFFFFFF;
    }
    //一バイトずつ計算する
    void calc_file_crc32(uint8_t byte_data) {
        file_crc_32___c = crc_table[(file_crc_32___c ^ byte_data) & 0xFF] ^ (file_crc_32___c >> 8);
    }
    //ファイル専用CRCの値を返す
    uint32_t get_file_crc32() {
        return file_crc_32___c ^ 0xFFFFFFFF;
    }
};
