#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <copyfile.h>
#else
#include <sys/sendfile.h>
#endif
#include <pwd.h>
#include <sys/wait.h>

#define TEMPLATE    ".icpc_template"
#define PATH_SIZE   256

// This is redundent. Used to eliminate vscode error
extern char *optarg;
extern char **environ;

void *salloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        perror("malloc");
        exit(1);
    }
    return p;
}

void print_usage(char *progname) {
    fprintf(stderr, "usage: %s [-i contest] | [-g problem_id] | [-c problem_id] | [-r problem_id] | [-e problem_id]\n", progname);
}

// Return 0 if succeeded
// Return non-zero if dir cannot be created
int create_contest_directory(char *dirname) {
    return mkdir(dirname, 0777) != 0;
}

// Return 0 if succeeded
// Return non-zero if input file cannot be created
int create_contest_input_file(char *dirname) {
    char fname[PATH_SIZE];
    strcpy(fname, dirname);
    strcat(fname, "/in.txt");
    return creat(fname, S_IWUSR | S_IWGRP | S_IWOTH) == -1;
}

int init_contest(char *dirname) {
    if (create_contest_directory(dirname))
        return 1;
    if (create_contest_input_file(dirname))
        return 1;
    return 0;
}

// Obtain home directory
const char *get_home_directory_name() {
    const char *hdir;
    if ((hdir = getenv("HOME")) == NULL)
        hdir = getpwuid(getuid())->pw_dir;
    return hdir;
}

void append_suffix_to_problem_id(char *fname, char *problem_id) {
    sprintf(fname, "%s.cpp", problem_id);
}

// Return 0 if succeeded
// Return non-zero if input file cannot be created
int copy_template_file(char *problem_id) {
    int template_fid, problem_fid;

    char tmp_fname[PATH_SIZE];
    sprintf(tmp_fname, "%s/%s", get_home_directory_name(), TEMPLATE);
    char problem_fname[PATH_SIZE];
    append_suffix_to_problem_id(problem_fname, problem_id);

    if ((template_fid = open(tmp_fname, O_RDONLY)) == -1) {
        return 1;
    }

    if ((problem_fid = creat(problem_fname, 0600)) == -1) {
        close(template_fid);
        return 1;
    }

    ssize_t r;

#if defined(__APPLE__) || defined(__FreeBSD__)
    r = fcopyfile(template_fid, problem_fid, 0, COPYFILE_ALL);
#else
    struct stat finfo = { 0 };
    if (fstat(template_fid, &finfo) == -1) {
        close(template_fid);
        close(problem_fid);
        return 1;
    }
    
    r = sendfile(problem_fid, template_fid, NULL, finfo.st_size);
#endif

    close(template_fid);
    close(problem_fid);

    return r == -1;
}

char **prepare_arguments(char *problem_id) {
    char *progname = salloc(PATH_SIZE);
    strcpy(progname, "clang++");
    char *macro = salloc(PATH_SIZE);
    strcpy(macro, "-D__TESTING__");
    char *opt = salloc(PATH_SIZE);
    strcpy(opt, "-o");
    char *code_fname = salloc(PATH_SIZE);
    append_suffix_to_problem_id(code_fname, problem_id);

    char **args = salloc(sizeof(char *) * 6);

    args[0] = progname;
    args[1] = macro;
    args[2] = opt;
    args[3] = problem_id;
    args[4] = code_fname;
    args[5] = NULL;

    return args;
}

// Return 0 if succeeded
// Return non-zero if input file cannot be created
int compile(char *problem_id) {
    int fval = fork();
    int statloc;

    switch (fval) {
        case -1:
            perror("fork");
            exit(1);
        case 0:
            if (execve("/usr/bin/clang++", prepare_arguments(problem_id), environ) == -1) {
                perror("clang++");
                exit(127);
            }
        default:
            wait(&statloc);
            int state = WEXITSTATUS(statloc);
            return state != 0;
    }
}

int execute(char *problem_id) {
    int fval = fork();
    int statloc;

    char progname[PATH_SIZE];
    sprintf(progname, "./%s", problem_id);

    char *empty_args[] = { NULL };

    switch (fval) {
        case -1:
            perror("fork");
            exit(1);
        case 0:
            if (execve(progname, empty_args, environ) == -1) {
                perror(progname);
                exit(127);
            }
        default:
            wait(&statloc);
            int state = WEXITSTATUS(statloc);
            return state != 0;
    }
}

// Command line arguments
// icpc -i <contest>        -> generate contest directory
// icpc -g <problem id>     -> generate problem with id
// icpc -c <problem id>     -> compile with id
// icpc -r <problem id>     -> execute problem id
// icpc -e <problem id>     -> compile and execute problem id
int main(int argc, char **argv) {
    int ch;

    while ((ch = getopt(argc, argv, "i:g:c:r:e:")) != EOF){
        switch (ch) {
            case 'i':
                if (init_contest(optarg)) {
                    perror("init_contest");
                    return 1;
                }
                break;
            case 'g':
                if (copy_template_file(optarg)) {
                    perror("copy_template_file");
                    return 1;
                }
                break;
            case 'c':
                if (compile(optarg)) {
                    perror("compile");
                    return 1;
                }
                break;
            case 'r':
                if (execute(optarg)) {
                    perror("execute");
                    return 1;
                }
                break;
            case 'e':
                // printf("%d\n", optarg);
                if (compile(optarg)) {
                    perror("compile");
                    return 1;
                }

                if (execute(optarg)) {
                    perror("execute");
                    return 1;
                }
                break;
            case '?':
            default:
                print_usage(argv[0]);
                return 1;
        }
    }


    return 0;
}
