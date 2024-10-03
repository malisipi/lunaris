namespace lunaris::ui::native_dialogs {
    std::string open_file(){
        #ifndef _WIN32
            const char* command = "kdialog --getopenfilename ~"; // TODO: Add support for Zenity
            char buffer[4096];
            std::string result = "";

            FILE* pipe = popen(command, "r");
            if (!pipe) return NULL;

            while (fgets(buffer, sizeof(buffer), pipe) != NULL) result += buffer;
            result = result.substr(0, result.size()-1); // Remove "\n" char // TODO: Improve this behavior

            pclose(pipe);
            return result;
        #else 
            wchar_t file_path_w[4096] = L"";
            char file_path[4096] = "";

            OPENFILENAMEW ofn; 
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = file_path_w;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(file_path_w);
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameW(&ofn) && (WideCharToMultiByte(CP_UTF8, 0, file_path_w, -1, file_path, sizeof(file_path), NULL, NULL) > 0)) {
                return file_path;
            }; // else
            return NULL;
        #endif
    };
};