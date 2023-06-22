#include <json-c/json.h>
#include <stdio.h>
#include <string.h>

struct _entry {
    int inode ;
    char * name ;
} ;

struct _file {
    int inode ;
    char * data ;
} ;

struct _dir {
    int inode ;
    struct _entry * entries ;
    int entry_cnt ;
} ;

struct _inode_name_matching_table {
    int inode ;
    char * name ;
} ;


int parse_json_file(const char * json_file) ;

void print_json(struct json_object * json) ;

void print_files_dirs() ;