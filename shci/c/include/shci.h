#ifndef SHCI_H
#define SHCI_H

#include <stdint.h>
#include <time.h>

typedef enum shci_toolchain_flags {
    shci_c_toolchain = 0b0001,
    shci_cxx_toolchain = 0b0010,
    shci_python2_toolchain = 0b0100,
    shci_python3_toolchain = 0b1000
} shci_toolchain_flags;


typedef struct shci_github_repo_info {
    const char* username;   
    const char* repository_name;
    const char* access_token;
    const char* path;
    char* logs[1024];
    uint32_t sizes[1024];
    uint32_t i;
    clock_t start;
    clock_t end;
} shci_github_repo_info;



extern void shci_call(const char* script, uint8_t pass_on_success, uint8_t log, shci_github_repo_info* info);

extern void shci_get_toolchain(shci_toolchain_flags flags, shci_github_repo_info* info);

extern shci_github_repo_info shci_get_github_repo(const char* username, const char* repo_name, int recursive, const char* access_token, const char* path);

extern char* shci_read_text(const char* path);

extern void shci_write_text(const char* buffer, const char* path);

extern void shci_log(shci_github_repo_info* info);

extern void shci_build_passing(shci_github_repo_info* info);

extern void shci_build_failure(shci_github_repo_info* info);

extern void shci_end_success(shci_github_repo_info* info);

extern void shci_end_failure(shci_github_repo_info* info);

#endif // SHCI_H
