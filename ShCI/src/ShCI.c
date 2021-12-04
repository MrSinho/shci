#include "ShCI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void shci_call(const char* script, shci_github_repo_info info) {
    if (system(script) != 0) { 
        shci_build_failure(info); 
        shci_end_failure(); 
    }
}

void shci_get_toolchain(shci_toolchain_flags flags, shci_github_repo_info info) {
#ifdef __linux
    if (flags & shci_c_toolchain) {
        const char* script = "apt install -y wget gcc cmake make gdb";    
        shci_call(script, info);    
    }
    if (flags & shci_cxx_toolchain) {
        const char* script = "apt install -y wget g++ cmake make gdb";    
        shci_call(script, info);        
    }
    if (flags & shci_python2_toolchain) {
        const char* script = "apt install -y wget python2";    
        shci_call(script, info);        
    }
    if (flags & shci_python3_toolchain) {
        const char* script = "apt install -y wget python3";     
        shci_call(script, info);       
    }
#endif // __linux__    
    
}

shci_github_repo_info shci_get_github_repo(const char* username, const char* repo_name, const char* access_token, const char* path) {
    char del[256];
#ifdef WIN32
    strcpy(del, "rmdir -r ");
#else
    strcpy(del, "rm -rf ");
#endif
    strcat(del, path);
    system(del);
    system("apt install git-core");
    char clone[1024];
    strcpy(clone, "git clone --recursive https://");
    strcat(clone, access_token);
    strcat(clone, "@github.com/");
    strcat(clone, username);
    strcat(clone, "/");
    strcat(clone, repo_name);
    strcat(clone, " ");
    strcat(clone, path);
    shci_github_repo_info info = { username, repo_name, access_token, path };
    shci_call(clone, info);
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

void shci_build_passing(const shci_github_repo_info info) {
    char set[1024];
    strcpy(set, "cd ");
    strcat(set, info.path);
    strcat(set, " && mkdir .ShCI");
    system(set);
    strcpy(set, "cd ");
    strcat(set, info.path);
    strcat(set, " && git config user.name \"ShCI\" && git config user.email \"none\"");
#ifdef WIN32
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
#ifdef WIN32
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