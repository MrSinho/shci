import io
import os
import sys
import platform
import requests
import time

from io import TextIOWrapper
from os import _wrap_close
from requests import Response
from subprocess import *


class shci_github_repo_info:
    owner:str        = ""
    access_token:str = ""
    repo_name:str    = ""
    recursive:bool   = True
    dir:str          = ""
    push:bool        = True
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

def shci_read_arg(arg:str, repo:shci_github_repo_info):
    if (arg.startswith("owner=")):
        repo.owner = arg.removeprefix("owner=")
    elif (arg.startswith("access_token=")):
        repo.access_token = arg.removeprefix("access_token=")
    elif (arg.startswith("repo_name=")):
        repo.repo_name = arg.removeprefix("repo_name=")
    elif (arg.startswith("recursive=")):
        repo.recursive = bool(arg.removeprefix("recursive="))
    elif (arg.startswith("repo_dir=")):
        repo.dir = arg.removeprefix("repo_dir=")
    elif (arg.startswith("push=")):
        repo.push = bool(arg.removeprefix("push="))
    elif (arg.startswith("prerequisites=")):
        repo.prerequisites_file = arg.removeprefix("prerequisites=")
    elif (arg.startswith("prerequisites_output=")):
        repo.prerequisites_output_file = arg.removeprefix("prerequisites_output=")
    elif (arg.startswith("build=")):
        repo.build_file = arg.removeprefix("build=")
    elif (arg.startswith("build_output=")):
        repo.build_output_file = arg.removeprefix("build_output=")
    return

def shci_markdown_setup(repo:shci_github_repo_info):
    repo.markdown = f"""
# {repo.repo_name}

![{repo._os}-badge](exit_code.svg)

## [{repo._os} build logs:](https://github.com/mrsinho/shci)

"""#`build ran for` /// `calls`
    return

def shci_call(repo:shci_github_repo_info, command_file:str, output_file:str) -> int:

    build_script:str = ""

    if (repo._os == "windows"):
        build_script = f"call {repo.dir}/{command_file}"
    else:
        build_script = f"sudo bash {repo.dir}/{command_file}"
        
    print(f"shci executing: {build_script}")
    r:_wrap_close = os.popen(build_script)
    exit_code:int = r._proc.wait()

    cmd:str = shci_read_text(f"{repo.dir}/{command_file}")
    output:str = shci_read_text(f"{repo.dir}/{output_file}")

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


def shci_clone_github_repo(repo:shci_github_repo_info):
    
    print(f"""shci:
    owner: {repo.owner},
    repo name: {repo.repo_name},
    recursive flag: {str(repo.recursive)},
    repo directory: {repo.dir},
    push: {repo.push},
    prerequisites file: {repo.prerequisites_file},
    prerequisites output file: {repo.prerequisites_output_file},
    build file: {repo.build_file},
    build output file: {repo.build_output_file}
    """)


    cmd:str = ""
    if (repo.recursive == True):
        cmd += "git clone --recursive "
    else:
        cmd += "git clone "

    cmd += f"https://{repo.access_token}@github.com/{repo.owner}/{repo.repo_name} {repo.dir}"
    
    print(f"shci cloning repo: {cmd}\n")

    try:
        os.system(cmd)
    except Exception:
        print("shci: Script is running")

    return

def shci_pull_repo(repo:shci_github_repo_info):
    pull:str = f"cd {repo.dir} && git clean -df && git pull && git submodule update --init --recursive"
    print(f"shci: {pull}")
    os.system(pull)
    return

def shci_build_status(repo:shci_github_repo_info, exit_code:int):
    print(f"shci repo dir: {repo.dir}")
    badge_file:TextIOWrapper = open(f"{repo.dir}/.shci/{repo._os}/exit_code.svg", "wb")
    if (exit_code == 0):#success
        print("shci: Build success\n")
        clone_badge:Response = requests.get(f"https://img.shields.io/badge/{repo._os}-passing-green.svg")
        badge_file.write(clone_badge.content)
    else:
        print("shci: Build failure\n")
        clone_badge:Response = requests.get(f"https://img.shields.io/badge/{repo._os}-failure-red.svg")
        badge_file.write(clone_badge.content)
    badge_file.flush()
    badge_file.close()

    end:float = time.time()
    repo.markdown += f"""

Build ran for `{str("%.2f" % (end - repo.start))}s`

---
"""
    repo.markdown += repo.bodies
    repo.markdown += f"""

Build terminated with exit code {exit_code}

---

"""
    shci_write_text(f"{repo.dir}/.shci/{repo._os}/log.md", repo.markdown)

    push:str = f"dir && cd {repo.dir} && git config user.name \"shci\" && git config user.email \"none\""
    push += f" && git add --all && git commit -a -m \"shci exit_code\" && git push https://{repo.access_token}@github.com/{repo.owner}/{repo.repo_name}"
    if (repo.push == True):
        print(f"shci: {push}")
        os.system(push)

    return

def main():

    repo:shci_github_repo_info = shci_github_repo_info()

    for i in range (0, len(sys.argv), 1):
        shci_read_arg(str(sys.argv[i]), repo)

    shci_markdown_setup(repo)

    #shci_clone_github_repo(repo)

    shci_pull_repo(repo)

    shci_call(repo, repo.prerequisites_file, repo.prerequisites_output_file)
    r:int = shci_call(repo, repo.build_file, repo.build_output_file)

    shci_build_status(repo, r)

    return


if __name__ == "__main__":
    main()
