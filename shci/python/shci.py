import io
import os
import sys
import platform
import requests
import time
import psutil

from io import TextIOWrapper
from os import _wrap_close
from requests import Response
from subprocess import *


class shci_github_repo_info:
    _os:str          = ""
    markdown:str     = ""
    start:float      = 0.0

    prerequisites_file:str        = ""
    prerequisites_output_file:str = ""
    
    build_file:str        = ""
    build_output_file:str = ""

    bodies:str            = ""

    def __init__(self):
        self.start = time.time()
        _os:str = platform.system()
        if (_os == "Windows"):
            self._os = "windows"
        else:
            self._os = "linux"

def shci_write_text(path:str, text: str):
    file:TextIOWrapper = open(path, "w")
    file.write(text)
    file.flush()
    file.close()
    return

def shci_read_text(path: str) -> str:
    file:TextIOWrapper = open(path, "r")
    data:str = str(file.read())
    file.close()
    return data

def shci_read_arg(arg:str, repo:shci_github_repo_info):
    if (arg.startswith("prerequisites=")):
        repo.prerequisites_file = arg.removeprefix("prerequisites=")
    elif (arg.startswith("prerequisites_output=")):
        repo.prerequisites_output_file = arg.removeprefix("prerequisites_output=")
    elif (arg.startswith("build=")):
        repo.build_file = arg.removeprefix("build=")
    elif (arg.startswith("build_output=")):
        repo.build_output_file = arg.removeprefix("build_output=")
    return

def shci_markdown_setup(repo:shci_github_repo_info):
    uname = platform.uname()
    
    cpu_info:str = f"""
system        : {platform.system()}
version       : {platform.version()}
platform      : {platform.platform()}
processor     : {str(platform.processor())}
min frequency : {str(psutil.cpu_freq().min)} MHz
max frequency : {str(psutil.cpu_freq().max)} MHz
cores         : {str(psutil.cpu_count(logical=False))}
"""

    print(f"shci cpu info: {cpu_info}")
    
    repo.markdown = f"""
# {repo._os.capitalize()} build logs

![]({repo._os}-exit-code.svg)

```{cpu_info}```

"""#`build ran for` /// `calls`
    return

def shci_call(repo:shci_github_repo_info, command_file:str, output_file:str) -> int:

    build_script:str = ""

    if (repo._os == "windows"):
        build_script = f"call ./{command_file}"
    else:
        build_script = f"sudo bash ./{command_file}"
        
    print(f"shci executing: {build_script}")
    r:_wrap_close = os.popen(build_script)
    exit_code:int = r._proc.wait()

    cmd:str    = shci_read_text(f"./{command_file}")
    output:str = shci_read_text(f"./{output_file}")

    print(f"shci call command: {cmd}")
    print(f"shci call command output: {output}")

    repo.bodies += f"""
```bash
{cmd}
```

```bash
{output}
```

---
    """

    return exit_code

def shci_print_info(repo:shci_github_repo_info):
    
    print(f"""shci:
    prerequisites file: {repo.prerequisites_file},
    prerequisites output file: {repo.prerequisites_output_file},
    build file: {repo.build_file},
    build output file: {repo.build_output_file}
    """)
    
    return

def shci_build_status(repo:shci_github_repo_info, exit_code:int):
    badge_file:TextIOWrapper = open(f"./.shci/{repo._os}/{repo._os}-exit-code.svg", "wb")
    if (exit_code == 0):#success
        print("shci: Build success\n")
        clone_badge:Response = requests.get(f"https://img.shields.io/badge/shci_test_passing-006400?style=for-the-badge&logo={repo._os}&logoColor=white&labelColor=003200&label={repo._os}#.svg")
        badge_file.write(clone_badge.content)
    else:
        print("shci: Build failure\n")
        clone_badge:Response = requests.get(f"https://img.shields.io/badge/shci_test_failure-640000?style=for-the-badge&logo={repo._os}&logoColor=white&labelColor=320000&label={repo._os}#.svg")
        badge_file.write(clone_badge.content)
    badge_file.flush()
    badge_file.close()

    end:float = time.time()
    repo.markdown += f"""build ran for `{str("%.2f" % (end - repo.start))} s` and terminated with exit code `{exit_code}`

---
"""
    repo.markdown += repo.bodies
    repo.markdown += f"""
build ran for `{str("%.2f" % (end - repo.start))} s` and terminated with exit code `{exit_code}`

---

"""
    shci_write_text(f"./.shci/{repo._os}/log.md", repo.markdown)

    return

def main():

    repo:shci_github_repo_info = shci_github_repo_info()

    for i in range (0, len(sys.argv), 1):
        shci_read_arg(str(sys.argv[i]), repo)

    shci_print_info(repo)
    
    #shci_pull_repo(repo)
    
    shci_markdown_setup(repo)

    shci_call(repo, repo.prerequisites_file, repo.prerequisites_output_file)
    r:int = shci_call(repo, repo.build_file, repo.build_output_file)

    shci_build_status(repo, r)

    return


if __name__ == "__main__":
    main()
