#include "parser.h"

struct _dir * dirs = NULL ;
struct _file * files = NULL ;
struct _inode_name_matching_table * inode_table = NULL ;

int dir_cnt = 0 ;
int reg_cnt = 0 ;
int inode_cnt = 0 ;

// Function to parse JSON file and populate file and directory entries
int parse_json_file(const char* json_file) 
{
    // Read JSON file
    json_object * fs_json = json_object_from_file(json_file) ;
    if (fs_json == NULL) {
        fprintf(stderr, "Failed to read JSON file\n") ;
        return 1 ;
    }

    // // print_json(fs_json);
    // dirs = malloc(sizeof(struct _dir));
    // files = malloc(sizeof(struct _file));
    int inode ;
    char * name ;
    char * type ;
    char * data ;
    struct _entry * entries ;
    int entry_length ;
    // char * path ;

    printf("%d", json_object_array_length(fs_json));
    for ( int i = 0 ; i < json_object_array_length(fs_json) ; i++ )
    {
		json_object * obj = json_object_array_get_idx(fs_json, i) ;
        // might be here!
		json_object_object_foreach(obj, key, val) 
        {   
			if (strcmp(key, "type") == 0) {
            	type = strdup(json_object_get_string(val)) ;
            }
            if (strcmp(key, "inode") == 0) 
				inode = (int) json_object_get_int(val) ;
			
            if (strcmp(key, "data") == 0){
                data = strdup(json_object_get_string(val)) ;
            }
            if (strcmp(key, "entries") == 0)
            {
                entry_length = json_object_array_length(val) ;
                printf("%d", inode_cnt) ;
                inode_table = realloc(inode_table, sizeof(struct _inode_name_matching_table) * (inode_cnt + entry_length)) ;
                entries = malloc(sizeof(struct _entry) * entry_length) ;
                for ( int j = 0 ; j < entry_length ; j++, inode_cnt++ )
                {
                    json_object * ent = json_object_array_get_idx(val, j) ;
                    json_object_object_foreach(ent, ent_key, ent_val)
                    {
                        if (strcmp(ent_key, "inode") == 0) 
                        {
                            entries[j].inode = (int) json_object_get_int(ent_val) ;
                            inode_table[inode_cnt].inode = (int) json_object_get_int(ent_val) ;
                        }
                        if (strcmp(ent_key, "name" ) == 0)
                        {
                            entries[j].name = strdup(json_object_get_string(ent_val)) ;
                            inode_table[inode_cnt].name = strdup(json_object_get_string(ent_val)) ;
                        }
                    }
                }
            }

            if (strcmp(key, "name") == 0) {
			    name = strdup(json_object_get_string(val)) ;
            }

            printf("%s ", key);
        }

        // printf("%d %s\n", inode, type) ;
        
        if (strcmp(type, "reg") == 0)
        {   
            int temp = ++reg_cnt ;
            files = realloc(files, sizeof(struct _file) * temp) ;
            strcpy(files[temp-1].data, data) ;
            files[temp-1].inode = inode ;
            // printf("%d, %s : ", inode, data) ;
            // printf("%d, %s\n", files[temp-1].inode, files[temp-1].data) ;
        }
        else if (strcmp(type, "dir") == 0)    // "dir"
        {
            int temp = ++dir_cnt ;
            dirs = realloc(dirs, sizeof(struct _dir) * temp) ;
            dirs[temp-1].inode = inode ;  // test
            dirs[temp-1].entry_cnt = entry_length ;
            dirs[temp-1].entries = malloc(sizeof(struct _entry) * entry_length) ;
            for (int j = 0 ; j < entry_length ; j++) 
            {
                dirs[temp-1].entries[j].inode = entries[j].inode ;
                dirs[temp-1].entries[j].name = strdup(entries[j].name) ;                 
            }
            
            if (inode == 0)
            {
                inode_table = realloc(inode_table, sizeof(struct _inode_name_matching_table) * inode_cnt++) ;
                inode_table[inode_cnt-1].inode = inode ;
                inode_table[inode_cnt-1].name = strdup("root") ;
            }
            free(entries) ;
        }
        else
        {
            printf("wrong type") ;
        }

        print_files_dirs() ;
    }

    // Clean up JSON object
    json_object_put(fs_json) ;

    return 0 ;
}

void print_files_dirs() 
{
    printf("Directories:\n");
    for (int i = 0 ; i < dir_cnt; i++) 
    { 
        printf("%d : ", dirs[i].inode) ;
        for (int j = 0 ; j < dirs[i].entry_cnt ; j ++ )
        {
            printf("{%d ", dirs[i].entries[j].inode) ;
            printf("%s} ", dirs[i].entries[j].name) ;
        }
        printf("\n");
    }

    printf("Files:\n");
    for (int k = 0 ; k < reg_cnt ; k++)
        printf("%d %s\n", files[k].inode, files[k].data) ;
        
    printf("Inode Tables:\n");
    for (int l = 0 ; l < inode_cnt ; l++)
    {
        printf("%d : {%s %d}\n", l, inode_table[l].name, inode_table[l].inode) ;
    }


    printf("\n");
}

void print_json(json_object * json) 
{
    printf("[\n");
	for ( int i = 0 ; i < json_object_array_length(json) ; i++ ) {
		json_object * obj = json_object_array_get_idx(json, i) ;

		printf("   {\n") ;
		json_object_object_foreach(obj, key, val) {
			if (strcmp(key, "inode") == 0) 
				printf("      \"inode\": %d\n", (int) json_object_get_int(val)) ;

			if (strcmp(key, "type") == 0) 
				printf("      \"type\": \"%s\"\n", (char *) json_object_get_string(val)) ;

			if (strcmp(key, "name" ) == 0)
				printf("      \"name\": \"%s\"\n", (char *) json_object_get_string(val)) ;
			
            if (strcmp(key, "entries") == 0)
            {
                printf("      \"entries\":\n      [\n") ;
                for ( int j = 0 ; j < json_object_array_length(val) ; j++ ) 
                {
                    printf("         {") ;
                    json_object * ent = json_object_array_get_idx(val, j) ;
                    json_object_object_foreach(ent, ent_key, ent_val) {
                        if (strcmp(ent_key, "name" ) == 0)
                            printf("\"name\": \"%s\", ", (char *) json_object_get_string(ent_val)) ;

                        if (strcmp(ent_key, "inode") == 0) 
                            printf("\"inode\": %d", (int) json_object_get_int(ent_val)) ;
                    }
                    printf("}\n") ;
                }
                printf("      ]\n") ;
            }

            if (strcmp(key, "data") == 0)
                printf("      \"data\": \"%s\"\n", (char *) json_object_get_string(val)) ;
		}
		printf("   }\n") ;
	}
    printf("]\n");
}