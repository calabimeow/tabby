#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

static int cmd_help(void)
{
    printf("usage: tabby [command] [project]\n"
            "commands:\n"
            "   newproj         creates new project\n"
            "   compile         compile project\n"
            "   run             run project\n"
            "   compile run     compile and run project\n"
            "   update          update tabby's files in a project\n");
    return 0;
}

static int write_cmd(const char *fmt, ...)
{
    char cmd[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(cmd, sizeof(cmd), fmt, args);
    va_end(args);
    return system(cmd);
}

static void copy_project_files(int game_arg, char **argv)
{
    char *game = argv[game_arg];

    #ifdef _WIN32
        write_cmd("copy \"tabby.h\" \"%s/scripts/tabby.h\"", game);
        write_cmd("copy \"launcher.exe\" \"%s/%s.exe\"", game, game);
        write_cmd("copy \"tabby.dll\" \"%s/engine/tabby.dll\"", game);
        write_cmd("copy \"extern\\win\\SDL3.dll\" \"%s/SDL3.dll\"", game);
        write_cmd("copy \"extern\\win\\SDL3_ttf.dll\" \"%s/engine/SDL3_ttf.dll\"", game);
        write_cmd("copy \"extern\\win\\SDL3_image.dll\" \"%s/engine/SDL3_image.dll\"", game);
        write_cmd("copy \"extern\\win\\SDL3_mixer.dll\" \"%s/engine/SDL3_mixer.dll\"", game);
    #else
        write_cmd("cp tabby.h %s/scripts/tabby.h", game);
        write_cmd("cp launcher %s/%s", game, game);
        write_cmd("cp libtabby.so %s/engine/libtabby.so", game);
        write_cmd("cp -r extern/lin/. %s/engine/", game);
    #endif
}

static int cmd_newproj(int game_arg, char **argv)
{
    char *game = argv[game_arg];

    if (dir_exists(game))
    {
        printf("Project already exists\n");
        return 0;
    }

    write_cmd("mkdir \"%s\" && cd \"%s\" && mkdir scripts && mkdir engine", game, game);

    #ifdef _WIN32
        write_cmd("copy \"game_template.c\" \"%s/scripts/%s.c\"", game, game);
    #else
        write_cmd("cp game_template.c %s/scripts/%s.c", game, game);
    #endif

    copy_project_files(game_arg, argv);

    return 0;
}

static int cmd_compile(int game_arg, char **argv)
{
    char *game = argv[game_arg];
    
    #ifdef _WIN32
        return write_cmd("gcc -shared -O2 -I. -L\"%s/\" -L\"%s/engine/\" "
                "-lsdl3 -lsdl3_image -lsdl3_mixer -lsdl3_ttf "
                "-ltabby -o %s/%s.dll %s/scripts/*.c", game, game, game, game, game);
    #else
        return write_cmd("gcc -fPIC -shared -O2 -I. %s/scripts/*.c "
            "-L\"extern/lin\" -L\".\" -ltabby "
            "-l:libSDL3.so.0 -l:libSDL3_image.so.0 "
            "-l:libSDL3_mixer.so.0 -l:libSDL3_ttf.so.0 "
            "-Wl,-rpath,'$ORIGIN/engine' -o %s/engine/lib%s.so", game, game, game);
    #endif
}

static int cmd_run(int game_arg, char **argv)
{
    char *game = argv[game_arg];

    #ifdef _WIN32
        return write_cmd("cd %s && .\\%s", game, game);
    #else
        return write_cmd("cd %s && LD_LIBRARY_PATH=$PWD/engine ./%s", game, game);
    #endif
}

static int cmd_update(int game_arg, char **argv)
{
    char *game = argv[game_arg];

    if (!dir_exists(game))
    {
        printf("Project doesn't exist\n");
        return 1;
    }

    copy_project_files(game_arg, argv);

    return 0;
}

int main(int argc, char **argv)
{
    char *project_root = get_project_root();

    #ifdef _WIN32
        SetCurrentDirectoryA(project_root);
    #else
        chdir(project_root);
    #endif

    if (argc < 2)
        return cmd_help();

    if (strcmp(argv[1], "newproj") == 0)
        return cmd_newproj(2, argv);

    else if (strcmp(argv[1], "update") == 0)
        return cmd_update(2, argv);

    else if (argc >= 3 && strcmp(argv[1], "compile") == 0 && strcmp(argv[2], "run") == 0)
    {
        if (cmd_compile(3, argv) != 0)
            return 1;
        return cmd_run(3, argv);
    }

    else if (strcmp(argv[1], "compile") == 0)
        return cmd_compile(2, argv);

    else if (strcmp(argv[1], "run") == 0)
        return cmd_run(2, argv);
    
    return cmd_help();
}
