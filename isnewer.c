#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#include <stdbool.h>

typedef struct file_times_t
{
    size_t create;
    size_t access;
    size_t write;
} file_times_t;

bool get_file_times(HANDLE file, file_times_t *times)
{
    FILETIME create, access, write;

    if (!GetFileTime(file, &create, &access, &write))
        return false;

    times->create = (ULARGE_INTEGER){create.dwLowDateTime, create.dwHighDateTime}.QuadPart;
    times->access = (ULARGE_INTEGER){access.dwLowDateTime, access.dwHighDateTime}.QuadPart;
    times->write  = (ULARGE_INTEGER){write .dwLowDateTime, write .dwHighDateTime}.QuadPart;

    return true;
}

void __stdcall mainCRTStartup(void)
{
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    HANDLE stderr = GetStdHandle(STD_ERROR_HANDLE);

    if (argc != 3)
    {
        char string[] = "Usage: isnewer [file 1] [file 2]\n";

        int written;
        WriteFile(stderr, string, sizeof(string)-1, &written, NULL);
    }

    HANDLE file1 = CreateFileW(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE file2 = CreateFileW(argv[2], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    file_times_t times1;

    if (!get_file_times(file1, &times1))
        ExitProcess(-1);

    file_times_t times2;

    if (!get_file_times(file2, &times2))
        ExitProcess(-1);

    ExitProcess(times1.write > times2.write);
}
