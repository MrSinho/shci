#include "ShCI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning (disable: 4996 4013 4047)
#endif

void shci_call(const char* script, uint8_t pass_on_success, uint8_t log, shci_github_repo_info* info) {
    int exit_code = 0;
    if (log) {
        char call[528];
        strcpy(call, "$$shci call$$:\n");
        info->sizes[info->i] += strlen(call);
        info->logs[info->i] = calloc(strlen(call), 1);
        if (info->logs[info->i] == NULL) { return; }
        strcpy(info->logs[info->i], call);
        info->i++;
        strcpy(call, script);
        strcat(call, "\n");
        info->sizes[info->i] += strlen(call);
        info->logs[info->i] = calloc(strlen(call), 1);
        if (info->logs[info->i] == NULL) { return; }
        strcpy(info->logs[info->i], call);
        info->i++;
        
        puts(call);
#ifdef _MSC_VER
        FILE* file_pipe = _popen(script, "r");
#else
        FILE* file_pipe = popen(script, "r");
#endif // _MSC_VER
        if (!file_pipe) { return; }

        char _log[1024];
        for (; fgets(_log, sizeof(_log), file_pipe)!=NULL; info->i++) {
            info->sizes[info->i] = strlen(_log);
            info->logs[info->i] = calloc(info->sizes[info->i], 1);
            if (info->logs[info->i] == NULL) { break; }
            strcpy(info->logs[info->i], _log);
            printf("%s", _log);
        }

#ifdef _MSC_VER
        exit_code = _pclose(file_pipe);
#else 
        exit_code = pclose(file_pipe);
#endif // _MSC_VER
    }
    else { exit_code = system(script); }
    if (exit_code != 0) { 
        if (pass_on_success) {
            shci_build_failure(info); 
            shci_end_failure(info); 
        }
        return;
    }
    if (pass_on_success) {
        char path[256];
        strcpy(path, info->path);
#ifdef _WIN32
        strcat(path, "/.ShCI/windows-log.md");
#else
        strcat(path, "/.ShCI/linux-log.md");
#endif // _WIN32
        FILE* stream = fopen(path, "w");
        if (stream == NULL) { return; }
        uint32_t offset = 0;
        
        char title[256];
        strcpy(title, "# ");
        strcat(title, info->repository_name);
#ifdef _WIN32
        strcat(title, "\n![windows_badge](windows-status.svg)\n## [Windows build logs:](https://github.com/MrSinho/ShCI)\n  ");
#else 
        strcat(title, "\n![linux_badge](linux-status.svg)\n## [Linux/Unix build logs:](https://github.com/MrSinho/ShCI)\n  ");
#endif // _WIN32
        fwrite(title, 1, strlen(title), stream);
        
        offset += strlen(title);
        fseek(stream, offset, SEEK_SET);

        const char* paragraph_start = "\n```bash\n";
        const char* paragraph_end = "```\n";
        const char* sig = "$$shci call$$:\n";
        for (uint32_t i = 0; i < info->i; i++) {
            if (strcmp(info->logs[i], sig) == 0) {
                if (i != 0) {
                    fwrite(paragraph_end, 1, strlen(paragraph_end), stream);
                    offset += strlen(paragraph_end);
                    fseek(stream, offset, SEEK_SET);
                }
                fwrite(paragraph_start, 1, strlen(paragraph_start), stream);
                offset += strlen(paragraph_start);
                fseek(stream, offset, SEEK_SET);
            }
            fwrite(info->logs[i], 1, info->sizes[i], stream);
            offset += info->sizes[i];
            fseek(stream, offset, SEEK_SET);
        }
        fwrite(paragraph_end, 1, strlen(paragraph_end), stream);
        shci_build_passing(info);
    }
}

void shci_get_toolchain(shci_toolchain_flags flags, shci_github_repo_info* info) {
#ifdef _WIN32
    if (flags & shci_c_toolchain) {
        shci_call("choco install -y curl mingw cmake", 0, 1, info);
        shci_call("mingw-get install gcc mingw32-make gdb", 0, 1, info);    
    }
    if (flags & shci_cxx_toolchain) {
        shci_call("choco install -y curl mingw cmake", 0, 1, info);
        shci_call("mingw-get install g++ mingw32-make gdb", 0, 1, info);    
    }
    if (flags & shci_python2_toolchain) {
        shci_call("choco install -y curl python2", 0, 1, info);    
    }
    if (flags & shci_python3_toolchain) {
        shci_call("choco install -y curl python3", 0, 1, info);    
    }
#else 
    if (flags & shci_c_toolchain) {
        shci_call("apt install -y wget gcc cmake make gdb", 0, 1, info);    
    }
    if (flags & shci_cxx_toolchain) {
        shci_call("apt install -y wget g++ cmake make gdb", 0, 1, info);        
    }
    if (flags & shci_python2_toolchain) {
        shci_call("apt install -y wget python2", 0, 1, info);        
    }
    if (flags & shci_python3_toolchain) {
        shci_call("apt install -y wget python3", 0, 1, info);        
    }
#endif // __linux__    
    
}

shci_github_repo_info shci_get_github_repo(const char* username, const char* repo_name, int recursive, const char* access_token, const char* path) {
    char del[256];
#ifdef _WIN32
    strcpy(del, "rmdir -r ");
#else
    strcpy(del, "rm -rf ");
#endif
    strcat(del, path);
    shci_call(del, 0, 0, NULL);
#ifdef _WIN32
    shci_call("choco install -y git", 0, 0, NULL);
#else
    shci_call("apt install -y git-core", 0, 0, NULL);
#endif // _WIN32
    char clone[1024];
    (recursive) ? strcpy(clone, "git clone --recursive https://") : strcpy(clone, "git clone https://");
    strcat(clone, access_token);
    strcat(clone, "@github.com/");
    strcat(clone, username);
    strcat(clone, "/");
    strcat(clone, repo_name);
    strcat(clone, " ");
    strcat(clone, path);
    shci_github_repo_info info = { username, repo_name, access_token, path, 0, 0, 0 };
    shci_call(clone, 0, 0, &info);
    char pull[256];
    strcpy(pull, "cd ");
    strcat(pull, path);
    strcat(pull, " && git pull");
    shci_call(pull, 0, 0, &info);
    return info;
} 

char* shci_read_text(const char* path) {
    FILE* stream = fopen(path, "r");

    if (stream == NULL) { return NULL; }

    fseek(stream, 0, SEEK_END);
    int code_size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    char* code = (char*)malloc(code_size);
    if (code == NULL) { free(stream); return NULL; }

    fread(code, code_size, 1, stream);
    
    free(stream);

    return code;
}

void shci_write_text(const char* buffer, const char* path) {
    FILE* stream = fopen(path, "w");
    if (stream == NULL) { return; }
    fwrite(buffer, strlen(path), 1, stream);
    free(stream);
}

void shci_log(shci_github_repo_info* info) {

}

void shci_build_passing(shci_github_repo_info* info) {
    char set[1024];
    strcpy(set, "cd ");
    strcat(set, info->path);
    strcat(set, " && mkdir .ShCI");
    shci_call(set, 0, 0, info);
    strcpy(set, "cd ");
    strcat(set, info->path);
    strcat(set, " && git config user.name \"ShCI\" && git config user.email \"none\"");
#ifdef _WIN32
    strcat(set, " && cd .ShCI && curl https://img.shields.io/badge/windows-passing-green.svg -o windows-status.svg");
#else
    strcat(set, " && cd .ShCI && wget https://img.shields.io/badge/linux-passing-green.svg  && mv linux-passing-green.svg linux-status.svg");
#endif
    strcat(set, " && cd .. && git add --all && git commit -a -m \"ShCI status\" && git push https://");
    strcat(set, info->access_token);
    strcat(set, "@github.com/");
    strcat(set, info->username);
    strcat(set, "/");
    strcat(set, info->repository_name);
    shci_call(set, 0, 0, info);
}

void shci_build_failure(shci_github_repo_info* info) {
    char set[1024];
    strcpy(set, "cd ");
    strcat(set, info->path);
    strcat(set, " && mkdir .ShCI");
    shci_call(set, 0, 0, NULL);
    strcpy(set, "cd ");
    strcat(set, info->path);
    strcat(set, " && git config user.name \"ShCI\" && git config user.email \"none\"");
#ifdef _WIN32
    strcat(set, " && cd .ShCI && curl https://img.shields.io/badge/windows-failing-red.svg -o windows-status.svg");
#else 
    strcat(set, " && cd .ShCI && wget https://img.shields.io/badge/linux-failing-red.svg && mv linux-failing-red.svg linux-status.svg");
#endif
    strcat(set, "&& cd .. && git add --all && git commit -a -m \"ShCI status\" && git push https://");
    strcat(set, info->access_token);
    strcat(set, "@github.com/");
    strcat(set, info->username);
    strcat(set, "/");
    strcat(set, info->repository_name);
    shci_call(set, 0, 0, NULL);
}

void shci_end_success(shci_github_repo_info* info) {
    printf("shci message: all scripts have been executed successfully\n");
}

void shci_end_failure(shci_github_repo_info* info) {
    printf("shci message: failed running scripts\n");
}
