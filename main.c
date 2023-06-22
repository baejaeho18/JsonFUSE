#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <errno.h>
#include "parser.h"

static const char *filecontent = "I'm the content of the only file available there\n";

static int getattr_callback(const char *path, struct stat *stbuf) ;
static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) ;
static int open_callback(const char *path, struct fuse_file_info *fi) ;
static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) ;
// static int write_callback(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) ;

// Fuse operations
static struct fuse_operations fuse_example_operations = {
    .getattr = getattr_callback,
    .readdir = readdir_callback,
    .open = open_callback,
    .read = read_callback,
    // .write = write_callback,
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <json_file> <target_directory>\n", argv[0]);
        return 1;
    }

    struct json_object* fs_json = json_object_from_file(argv[1]) ;
    if (fs_json == NULL) {
        fprintf(stderr, "Failed to read JSON file\n") ;
        return 1 ;
    }
    print_json(fs_json);
    parse_json_file(argv[1]) ;
    // Mount file system
    return fuse_main(argc-1, argv+1, &fuse_example_operations, NULL);
}

// // Callback function for getattr
// static int getattr_callback(const char* path, struct stat* stbuf) {
//     memset(stbuf, 0, sizeof(struct stat));
//     printf("%s", path) ;
//     // Check if the path is the root directory
//     if (strcmp(path, "/") == 0) {
//         stbuf->st_mode = S_IFDIR | 0755;
//         stbuf->st_nlink = 2;
//         return 0;
//     }

//     for (int i = 0 ; i < sizeof(files) / sizeof(struct _file) ; i++)
//     {
//         if (strcmp(path + 1, files[i].inode) == 0)
//         {
//             // printf("%d %s %s", files[i].inode, files[i].name, files[i].type) ;
//             if (strcmp(files[i].type, "dir"))
//             {
//                 stbuf->st_mode = S_IFDIR | 0755;
//                 stbuf->st_nlink = 2;
//                 return 0;
//             }
//             else {
//                 stbuf->st_mode = S_IFREG | 0666;
//                 stbuf->st_nlink = 1;
//                 stbuf->st_size = strlen(files[i].data);
//                 return 0;
//             }
//         }
//     }

//     return -ENOENT;
// }

// // Callback function for readdir
// static int readdir_callback(const char* path, void* buf, fuse_fill_dir_t filler,
//     off_t offset, struct fuse_file_info* fi) {

//     // Check if the path is the root directory
//     if (strcmp(path, "/") == 0) {
//         filler(buf, ".", NULL, 0);
//         filler(buf, "..", NULL, 0);

//         // Add file entries
//         for (int i = 0 ; i < sizeof(files) / sizeof(struct _file) ; i++)
//             filler(buf, files[i].name, NULL, 0);

//         return 0;
//     }

//     return -ENOENT;
// }

// // Callback function for open
// static int open_callback(const char* path, struct fuse_file_info* fi) {
//     return 0 ;
// }

// // Callback function for read
// static int read_callback(const char* path, char* buf, size_t size, off_t offset,
//     struct fuse_file_info* fi) {
//     // Find the file entry
//     for (int i = 0 ; i < sizeof(files) / sizeof(struct _file) ; i++)
//     {
//         if (strcmp(files[i].type, "reg") == 0) {
//             size_t len = strlen(files[i].data);
//             if (offset >= len) {
//                 return 0;
//             }

//             size_t remaining = len - offset;
//             size_t read_size = (size < remaining) ? size : remaining;
//             memcpy(buf, files[i].data + offset, read_size);
//             return read_size;
//         }
//     }

//     return -ENOENT;
// }

// static int write_callback(const char *path, const char *buf, size_t size, off_t offset,
//                           struct fuse_file_info *fi) {
                            
//     if (strcmp(path, "/file") == 0) {
//         // 파일을 열 때 FUSE 라이브러리에서 제공하는 파일 핸들을 사용하여 파일을 쓸 수 있습니다.
//         FILE *file = fopen("mnt/file.txt", "w");
//         if (file == NULL) {
//             return -errno;
//         }

//         // 파일을 열고 오프셋으로 이동합니다.
//         if (fseek(file, offset, SEEK_SET) < 0) {
//             fclose(file);
//             return -errno;
//         }

//         // 버퍼에서 파일로 데이터를 씁니다.
//         if (fwrite(buf, size, 1, file) != 1) {
//             fclose(file);
//             return -errno;
//         }

//         fclose(file);

//         // 쓰여진 데이터의 크기를 반환합니다.
//         return size;
//     }

//     return -ENOENT;
// }
static int getattr_callback(const char *path, struct stat *stbuf) {
  memset(stbuf, 0, sizeof(struct stat));

  if (strcmp(path, "/") == 0) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }

  if (strcmp(path, "/file") == 0) {
    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_size = strlen(filecontent);
    return 0;
  }

  return -ENOENT;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {

  if (strcmp(path, "/") == 0) {

	  filler(buf, ".", NULL, 0);
	  filler(buf, "..", NULL, 0);
	  filler(buf, "file", NULL, 0);
	  return 0;
  }

  return -ENOENT ;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
  return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi) {

  if (strcmp(path, "/file") == 0) {
    size_t len = strlen(filecontent);
    if (offset >= len) {
      return 0;
    }

    if (offset + size > len) {
      memcpy(buf, filecontent + offset, len - offset);
      return len - offset;
    }

    memcpy(buf, filecontent + offset, size);
    return size;
  }

  return -ENOENT;
}