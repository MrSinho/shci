# ShCI
[![linux_badge](.ShCI/linux-status.svg)](https://github.com/MrSinho/ShCI/tree/main/.ShCI/linux-log.md)
[![windows_badge](.ShCI/windows-status.svg)](https://github.com/MrSinho/ShCI/tree/main/.ShCI/windows-log.md)

An open source tool for implementing continuous integration in your github project, you can find some example with the following projects:
 * [PlyImporter](https://github.com/MrSinho/PlyImporter)
 * [ShSerial](https://github.com/MrSinho/ShSerial)
 * [ShECS](https://github.com/MrSinho/ShECS)
 * [bmp280_driver](https://github.com/MrSinho/bmp280_driver)

## Functionalities:
 - [x] Cross platform (Windows and Linux support)
 - [x] Writes an output file for each platform [![linux_badge](.ShCI/linux-status.svg)](https://github.com/MrSinho/ShCI/tree/main/.ShCI/linux-log.md) [![windows_badge](.ShCI/windows-status.svg)](https://github.com/MrSinho/ShCI/tree/main/.ShCI/windows-log.md)

## How it works

You can use an old pc or a virtual machine as host device for building your github project. Create a simple executable using this library as following:
```c
int main(void) {    
    //Clone repository
    const uint8_t recursive = 1;
    shci_github_repo_info info = 
                shci_get_github_repo("username", "repo-name", recursive, "access-token", "repo-dir");    
    
    //you can install the tools on your own with a system command, this is not necessary
    shci_toolchain_flags flags =    shci_c_toolchain | shci_cxx_toolchain | shci_python3_toolchain;
    shci_get_toolchain(flags, &info);

    //Run your scripts
    const uint8_t log = 1;
    const uint8_t pass = 1;
    const char* prerequisites_script = shci_read_text("bash_commands.sh");
    shci_call(prerequisites_script, pass-1, log, &info);
    const char* compile_script = "cd repo-dir && mkdir build && cd build && cmake .. && make";
    shci_call(compile_script, pass, log, &info);

    shci_end_success(&info);
    
    return 0;
}
```
In repo `README.md`:
```markdown
[![linux_badge](.ShCI/linux-status.svg)](https://github.com/user/repo/tree/main/.ShCI/linux-log.md)

[![windows_badge](.ShCI/windows-status.svg)](https://github.com/user/repo/tree/main/.ShCI/windows-log.md)
```