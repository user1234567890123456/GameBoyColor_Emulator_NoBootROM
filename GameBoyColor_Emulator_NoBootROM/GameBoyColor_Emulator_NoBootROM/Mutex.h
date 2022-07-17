#pragma once

#include <windows.h>

#include "SystemDefine.h"

class Mutex
{
private:
    static Mutex* singleton;

    HANDLE mutex_handle = nullptr;

    Mutex() {
    }

    ~Mutex() {
    }
public:
    static void Init() {
        singleton = new Mutex();
    }

    static void End() {
        delete singleton;
    }

    static Mutex* get_instance_ptr() {
        return singleton;
    }

    void create_app_mutex() {
        mutex_handle = CreateMutex(NULL, TRUE, __MUTEX_NAME__);
        if (mutex_handle == nullptr) {
            MessageBox(NULL, _T("MutexÇ≈ÉGÉâÅ[Ç™î≠ê∂ÇµÇ‹ÇµÇΩ"), _T("ERROR"), MB_OK | MB_ICONERROR);

            exit(EXIT_FAILURE);
        }
        else if (GetLastError() == ERROR_ALREADY_EXISTS) {
            MessageBox(NULL, _T("ï°êîìØéûãNìÆÇÕÇ≈Ç´Ç‹ÇπÇÒ"), _T("ERROR"), MB_OK | MB_ICONERROR);

            ReleaseMutex(mutex_handle);
            CloseHandle(mutex_handle);

            mutex_handle = nullptr;

            exit(EXIT_FAILURE);
        }
    }

    void delete_app_mutex() {
        if (mutex_handle != nullptr) {
            ReleaseMutex(mutex_handle);
            CloseHandle(mutex_handle);

            mutex_handle = nullptr;
        }
    }
};
