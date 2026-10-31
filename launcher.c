#include <SDL3/SDL.h>
#include "utils.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

int main(void)
{
    char *project_root = get_project_root();

    #ifdef _WIN32
        SetCurrentDirectoryA(project_root);

        char engine_path[MAX_PATH];
        snprintf(engine_path, sizeof(engine_path), "%s/engine", project_root);

        wchar_t engine_path_w[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, engine_path, -1, engine_path_w, MAX_PATH);

        SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        AddDllDirectory(engine_path_w);

        SDL_SharedObject *engine = SDL_LoadObject("engine/tabby.dll");
    #else

        chdir(project_root);
        SDL_SharedObject *engine = SDL_LoadObject("engine/libtabby.so");
    #endif

    char *name = get_project_name();

    if (!engine)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    int (*run)(const char *) = (int(*)(const char *))SDL_LoadFunction(engine, "tabby_run");

    int r = run(name);

    SDL_UnloadObject(engine);
    free(name);
    free(project_root);
    return r;
}
