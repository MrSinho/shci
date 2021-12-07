#include "ShCI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void shci_call(const char* script, int pass_on_success, shci_github_repo_info info) {
    if (system(script) != 0) { 
        shci_build_failure(info); 
        shci_end_failure(); 
        return;
    }
    if (pass_on_success) {
        shci_build_passing(info);
    }
}

void shci_get_toolchain(shci_toolchain_flags flags, shci_github_repo_info info) {
#ifdef _WIN32
    if (flags & shci_c_toolchain) {
        system("choco install -y wget mingw cmake");
        system("mingw-get install gcc mingw32-make gdb");    
    }
    if (flags & shci_cxx_toolchain) {
        system("choco install -y wget mingw cmake");
        system("mingw-get install g++ mingw32-make gdb");    
    }
    if (flags & shci_python2_toolchain) {
        system("choco install -y wget python2");    
    }
    if (flags & shci_python3_toolchain) {
        system("choco install -y wget python3");    
    }
#else 
    if (flags & shci_c_toolchain) {
        system("apt install -y wget gcc cmake make gdb");    
    }
    if (flags & shci_cxx_toolchain) {
        system("apt install -y wget g++ cmake make gdb");        
    }
    if (flags & shci_python2_toolchain) {
        system("apt install -y wget python2");        
    }
    if (flags & shci_python3_toolchain) {
        system("apt install -y wget python3");        
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
    system(del);
    system("apt install git-core");
    char clone[1024];
    if (recursive) {
        strcpy(clone, "git clone --recursive https://");
    }
    else {
        strcpy(clone, "git clone https://");
    }
    strcat(clone, access_token);
    strcat(clone, "@github.com/");
    strcat(clone, username);
    strcat(clone, "/");
    strcat(clone, repo_name);
    strcat(clone, " ");
    strcat(clone, path);
    shci_github_repo_info info = { username, repo_name, access_token, path };
    shci_call(clone, 0, info);
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

void shci_build_passing(const shci_github_repo_info info) {
    char set[1024];
    strcpy(set, "cd ");
    strcat(set, info.path);
    strcat(set, " && mkdir .ShCI");
    system(set);
    strcpy(set, "cd ");
    strcat(set, info.path);
    strcat(set, " && git config user.name \"ShCI\" && git config user.email \"none\"");
#ifdef _WIN32
    strcat(set, " && cd .ShCI && wget https://img.shields.io/badge/build-passing-green.svg && ren build-passing-green.svg status.svg");
#else
    strcat(set, " && cd .ShCI && wget https://img.shields.io/badge/build-passing-green.svg && mv build-passing-green.svg status.svg");
#endif
    strcat(set, " && cd .. && git add .ShCI && git commit -m \"ShCI status\" && git push https://");
    strcat(set, info.access_token);
    strcat(set, "@github.com/");
    strcat(set, info.username);
    strcat(set, "/");
    strcat(set, info.repository_name);
    system(set);
}

void shci_build_failure(const shci_github_repo_info info) {
    char set[1024];
    strcpy(set, "cd ");
    strcat(set, info.path);
    strcat(set, " && mkdir .ShCI");
    system(set);
    strcpy(set, "cd ");
    strcat(set, info.path);
    strcat(set, " && git config user.name \"ShCI\" && git config user.email \"none\"");
#ifdef _WIN32
    strcat(set, " && cd .ShCI && wget https://img.shields.io/badge/build-failing-red.svg && ren build-failing-red.svg status.svg");
#else 
    strcat(set, " && cd .ShCI && wget https://img.shields.io/badge/build-failing-red.svg && mv build-failing-red.svg status.svg");
#endif
    strcat(set, "&& cd .. && git add .ShCI && git commit -m \"ShCI status\" && git push https://");
    strcat(set, info.access_token);
    strcat(set, "@github.com/");
    strcat(set, info.username);
    strcat(set, "/");
    strcat(set, info.repository_name);
    system(set);
}

void shci_end_success() {
    printf("shci message: all scripts have been executed successfully\n");
}

void shci_end_failure() {
    printf("shci message: failed running scripts\n");
}
