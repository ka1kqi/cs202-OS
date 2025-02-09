#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

static int err_code;
static int num_files;

/*
 * here are some function signatures and macros that may be helpful.
 */

void handle_error(char* fullname, char* action);
bool test_file(char* pathandname);
bool is_dir(char* pathandname);
const char* ftype_to_str(mode_t mode);
void list_file(char* pathandname, char* name, bool list_long);
void list_dir(char* dirname, bool list_long, bool list_all, bool recursive);

/*
 * You can use the NOT_YET_IMPLEMENTED macro to error out when you reach parts
 * of the code you have not yet finished implementing.
 */
#define NOT_YET_IMPLEMENTED(msg)                  \
    do {                                          \
        printf("Not yet implemented: " msg "\n"); \
        exit(255);                                \
    } while (0)

/*
 * PRINT_ERROR: This can be used to print the cause of an error returned by a
 * system call. It can help with debugging and reporting error causes to
 * the user. Example usage:
 *     if ( error_condition ) {
 *        PRINT_ERROR();
 *     }
 */
#define PRINT_ERROR(progname, what_happened, pathandname)               \
    do {                                                                \
        printf("%s: %s %s: %s\n", progname, what_happened, pathandname, \
               strerror(errno));                                        \
    } while (0)

/* PRINT_PERM_CHAR:
 *
 * This will be useful for -l permission printing.  It prints the given
 * 'ch' if the permission exists, or "-" otherwise.
 * Example usage:
 *     PRINT_PERM_CHAR(sb.st_mode, S_IRUSR, "r");
 */
#define PRINT_PERM_CHAR(mode, mask, ch) printf("%s", (mode & mask) ? ch : "-");

#define SET_ERROR(n) err_code |= (1<<n);

/*
 * Get username for uid. Return 1 on failure, 0 otherwise.
 */
static int uname_for_uid(uid_t uid, char* buf, size_t buflen) {
    struct passwd* p = getpwuid(uid);
    if (p == NULL) {
        return 1;
    }
    strncpy(buf, p->pw_name, buflen);
    return 0;
}

/*
 * Get group name for gid. Return 1 on failure, 0 otherwise.
 */
static int group_for_gid(gid_t gid, char* buf, size_t buflen) {
    struct group* g = getgrgid(gid);
    if (g == NULL) {
        return 1;
    }
    strncpy(buf, g->gr_name, buflen);
    return 0;
}

/*
 * Format the supplied `struct timespec` in `ts` (e.g., from `stat.st_mtime`) as a
 * string in `char *out`. Returns the length of the formatted string (see, `man
 * 3 strftime`).
 */
static size_t date_string(struct timespec* ts, char* out, size_t len) {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    struct tm* t = localtime(&ts->tv_sec);
    if (now.tv_sec < ts->tv_sec) {
        // Future time, treat with care.
        return strftime(out, len, "%b %e %Y", t);
    } else {
        time_t difference = now.tv_sec - ts->tv_sec;
        if (difference < 31556952ull) {
            return strftime(out, len, "%b %e %H:%M", t);
        } else {
            return strftime(out, len, "%b %e %Y", t);
        }
    }
}

/*
 * Print help message and exit.
 */
static void help() {
    /* TODO: add to this */
    printf("ls: List files\n");
    printf("\t--help: Print this help\n");
    exit(0);
}

/*
 * call this when there's been an error.
 * The function should:
 * - print a suitable error message (this is already implemented)
 * - set appropriate bits in err_code
 */
void handle_error(char* what_happened, char* fullname) {
    PRINT_ERROR("ls", what_happened, fullname);

    // TODO: your code here: inspect errno and set err_code accordingly.
    return;
}

/*
 * test_file():
 * test whether stat() returns successfully and if not, handle error.
 * Use this to test for whether a file or dir exists
 */
bool test_file(char* pathandname) {
    struct stat sb;
    if (stat(pathandname, &sb)) {
        handle_error("cannot access", pathandname);
        return false;
    }
    return true;
}

/*
 * is_dir(): tests whether the argument refers to a directory.
 * precondition: test_file() returns true. that is, call this function
 * only if test_file(pathandname) returned true.
 */
bool is_dir(char* pathandname) {
    /* TODO: fillin */

    struct stat sb;
    stat(pathandname,&sb);
    return(S_ISDIR(sb.st_mode));
}

/* convert the mode field in a struct stat to a file type, for -l printing */
const char* ftype_to_str(mode_t mode) {
    /* TODO: fillin */

    //first char
    if(S_ISDIR(mode)) {
        return "d";
    }
    else if (S_ISREG(mode)){
        return "-";
    }
    else { 
        //other
        return "?";
    }
}

/* list_file():
 * implement the logic for listing a single file.
 * This function takes:
 *   - pathandname: the directory name plus the file name.
 *   - name: just the name "component".
 *   - list_long: a flag indicated whether the printout should be in
 *   long mode.
 *
 *   The reason for this signature is convenience: some of the file-outputting
 *   logic requires the full pathandname (specifically, testing for a directory
 *   so you can print a '/' and outputting in long mode), and some of it
 *   requires only the 'name' part. So we pass in both. An alternative
 *   implementation would pass in pathandname and parse out 'name'.
 */
void list_file(char* pathandname, char* name, bool list_long) {
    /* TODO: fill in*/
    if(!test_file(pathandname)){ 
        //pathandname does not lead to a valid file
        //handle_error("File doesn't exist: ",pathandname);
        return;
    }
    num_files++;

    struct stat sb;
    stat(pathandname, &sb);

    if(list_long) {
        char uid_buff[128];
        char gid_buff[128];
        int err_uname=uname_for_uid(sb.st_uid,uid_buff,128);
        int err_grp=group_for_gid(sb.st_gid,gid_buff,128);

        int64_t fsize=sb.st_size; //long long

        struct timespec ts={sb.st_mtime,0}; //timespec from stat causing error, created own timespec struct to 
                                            //pass to date_string()
        char date_buf[32];
        date_string(&ts,date_buf,32); 

        const char *ftype_buff=ftype_to_str(sb.st_mode);

        //permissions string
        printf("%s",ftype_buff);
        //user
        PRINT_PERM_CHAR(sb.st_mode,S_IRUSR,"r");
        PRINT_PERM_CHAR(sb.st_mode,S_IWUSR,"w");
        PRINT_PERM_CHAR(sb.st_mode,S_IXUSR,"x");
        //group
        PRINT_PERM_CHAR(sb.st_mode,S_IRGRP,"r");
        PRINT_PERM_CHAR(sb.st_mode,S_IWGRP,"w");
        PRINT_PERM_CHAR(sb.st_mode,S_IXGRP,"x");
        //others
        PRINT_PERM_CHAR(sb.st_mode,S_IROTH,"r");
        PRINT_PERM_CHAR(sb.st_mode,S_IWOTH,"w");
        PRINT_PERM_CHAR(sb.st_mode,S_IXOTH,"x");

        //print in order
        printf(" %s %s %ld %s ",uid_buff,gid_buff,fsize,date_buf);
        
    }
    //lists the file regardless of list_long
    if(S_ISDIR(sb.st_mode)&&strcmp(name,".")!=0&&strcmp(name,"..")!=0) //if directory then we need to print the "/"s
        printf("%s/\n",name);
    else 
        printf("%s\n",name);
}

/* list_dir():
 * implement the logic for listing a directory.
 * This function takes:
 *    - dirname: the name of the directory
 *    - list_long: should the directory be listed in long mode?
 *    - list_all: are we in "-a" mode?
 *    - recursive: are we supposed to list sub-directories?
 */
void list_dir(char* dirname, bool list_long, bool list_all, bool recursive) {
    /* TODO: fill in
     *   You'll probably want to make use of:
     *       opendir()
     *       readdir()
     *       list_file()
     *       snprintf() [to make the 'pathandname' argument to
     *          list_file(). that requires concatenating 'dirname' and
     *          the 'd_name' portion of the dirents]
     *       closedir()
     *   See the lab description for further hints
     */
    if(!test_file(dirname)) {
        //handle_error()
        return;
    }

    DIR *dir;
    dir=opendir(dirname);  
    struct dirent *file;
    while((file=readdir(dir))) {
        //parent or current diretory

        char newpath[256];
        snprintf(newpath,256,"%s",dirname);
        strncat(newpath,"/",2);
        strncat(newpath,file->d_name,strlen(file->d_name));

        if(file->d_type==DT_DIR) { 
            //if we are recursive and we reach a directory
            if(list_all && (strcmp(file->d_name,".")==0||strcmp(file->d_name,"..")==0)) {
                list_file(newpath,file->d_name,list_long);
            }

            if(recursive) {
                //create new path
                //recur
                printf("%s:\n",file->d_name);
                list_dir(newpath,list_long,list_all,recursive);
                printf("\n");
            }
        } 
        else if (file->d_type==DT_REG){
            list_file(newpath,file->d_name,list_long);
        }
        //list files of current dir
    }

    closedir(dir);
    
    
}

int main(int argc, char* argv[]) {
    // This needs to be int since C does not specify whether char is signed or
    // unsigned.


    //testing
    char cwd[256];
    getcwd(cwd,256);
    list_dir(cwd,1,1,1);


    /* DONT TOUCH FOR NOW
    int opt;
    err_code = 0;
    bool list_long = false, list_all = false;
    // We make use of getopt_long for argument parsing, and this
    // (single-element) array is used as input to that function. The `struct
    // option` helps us parse arguments of the form `--FOO`. Refer to `man 3
    // getopt_long` for more information.
    struct option opts[] = {
        {.name = "help", .has_arg = 0, .flag = NULL, .val = '\a'}};

    // This loop is used for argument parsing. Refer to `man 3 getopt_long` to
    // better understand what is going on here.
    while ((opt = getopt_long(argc, argv, "1a", opts, NULL)) != -1) {
        switch (opt) {
            case '\a':
                // Handle the case that the user passed in `--help`. (In the
                // long argument array above, we used '\a' to indicate this
                // case.)
                help();
                break;
            case '1':
                // Safe to ignore since this is default behavior for our version
                // of ls.
                break;
            case 'a':
                list_all = true;
                break;
                // TODO: you will need to add items here to handle the
                // cases that the user enters "-l" or "-R"
            case 'l':
                list_long=true;
                break;
            case 'R':
                break;
            default:
                printf("Unimplemented flag %d\n", opt);
                break;
        }
    }

    

    // TODO: Replace this.
    if (optind < argc) {
        printf("Optional arguments: ");
    }
    for (int i = optind; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    if (optind < argc) {
        printf("\n");
    }

    NOT_YET_IMPLEMENTED("Listing files");
    exit(err_code);
    */
}
