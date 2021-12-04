# ShCI

An open source tool for implementing continuous integration in your github project.

## How it works

You can use an old pc or a virtual machine as host device for building your github project. Create a simple executable using this library as following:
```c
int main(void) {    
    //Clone repository
    shci_github_repo_info info = 
                shci_get_github_repo("username", "repo-name", "access-token", "repo-dir);    
    
    //you can install the tools on your own with a system command, this is not necessary
    shci_toolchain_flags flags =    shci_c_toolchain | shci_cxx_toolchain | shci_python3_toolchain;
    shci_get_toolchain(flags, info);

    //Run your scripts
    //Install required dev kits 
    const char* prerequisites_script = shci_read_text("bash_commands.sh");
    shci_call(prerequisites_script, info);
    //Compile the code
    const char* compile_script = "cd repo-dir && mkdir build && cd build && cmake .. && make";
    shci_call(compile_script, info);

    //Successfull build
    shci_build_passing(info);
    shci_end_success();
    
    return 0;
}
```
In your `README.md`:
```markdown
![Status_badge](.ShCI/status.svg)
```
It could be either a passing badge ![passing_badge](https://img.shields.io/badge/build-passing-green.svg) or a failing badge ![failing_badge](https://img.shields.io/badge/build-failing-red.svg).