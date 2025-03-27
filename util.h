#pragma once
#include <graphics.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "C:/Libraries/eigen-3.4.0/Eigen/Dense"

using namespace std;

constexpr int graph_side_length = 900;
constexpr int block_size = 20;
constexpr int scale = 45;

inline wstring string_to_wstring(const string& str) {
	if (str.empty()) {
		return L"";
	}
	int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), nullptr, 0);
	if (size_needed == 0) {
		return L"";
	}
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
	return wstr;
}
#define string_to_lpcwstr(x) string_to_wstring(x).c_str()
bool clear_directory(LPCWSTR path) {
    WIN32_FIND_DATAW findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    wstring searchPath = path + wstring(L"\\*");

    hFind = FindFirstFileW(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        if (wcscmp(findData.cFileName, L".") == 0 ||
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }
        wstring fullPath = path + wstring(L"\\") + findData.cFileName;
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!clear_directory(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
            if (!RemoveDirectoryW(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
        }
        else {
            if (!DeleteFileW(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
        }
    } while (FindNextFileW(hFind, &findData) != 0);
    FindClose(hFind);
    return 1;
}