#pragma once

namespace lunaris {
    namespace __internal {
        #ifdef _WIN32
            void get_regedit_dword_value(char* path, char* key, uint32_t* _dwValue){
                HKEY hKey;
                DWORD dwType = REG_DWORD;
                DWORD dwValue = 0;
                DWORD dwBufSize = sizeof(dwValue);

                if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) return;

                if (RegQueryValueExA(hKey, key, NULL, &dwType, (LPBYTE)&dwValue, &dwBufSize) != ERROR_SUCCESS) {
                    RegCloseKey(hKey);
                    return;
                };

                *_dwValue = (uint32_t)dwValue;
                RegCloseKey(hKey);
            }
        #endif
    };
};