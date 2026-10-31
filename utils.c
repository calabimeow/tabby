#define _POSIX_C_SOURCE 200809L
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #define PATH_SEP '\\'
    #define PATH_SEP_ALT '/'
#else
    #include <unistd.h>
    #define PATH_SEP '/'
    #define PATH_SEP_ALT '\\'
#endif

int dir_exists(const char *path)
{
    struct stat stats;

    if (stat(path, &stats) == 0)
        return S_ISDIR(stats.st_mode);

    return 0;
}

static char *get_executable_path(void)
{
    #ifdef _WIN32
        char buf[4096];
        DWORD len = GetModuleFileNameA(NULL, buf, sizeof(buf));

        char *out = malloc(len + 1);
        memcpy(out, buf, len + 1);
        return out;
    #else
        char buf[4096];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        buf[len] = '\0';

        char *out = malloc(len + 1);
        memcpy(out, buf, len + 1);
        return out;
    #endif
}

static char *find_last_sep(char *path)
{
    char *s1 = strrchr(path, '/');
    char *s2 = strrchr(path, '\\');

    if (!s1)
        return s2;
    if (!s2)
        return s1;

    return s1 > s2 ? s1 : s2;
}

char *get_project_name(void)
{
    char *path = get_executable_path();

    char *slash = find_last_sep(path);
    char *base = slash ? slash + 1 : path;

    char *dot = strrchr(base, '.');

    if (dot != NULL)
        *dot = '\0';

    char *out = malloc(strlen(base) + 1);
    strcpy(out, base);

    free(path);
    return out;
}

char *get_project_root(void)
{
    char *path = get_executable_path();

    char *slash = find_last_sep(path);

    *slash = '\0';

    char *out = malloc(strlen(path) + 1);
    strcpy(out, path);

    free(path);
    return out;
}
