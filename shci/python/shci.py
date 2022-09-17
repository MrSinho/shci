from io import TextIOWrapper
import os
from os import _wrap_close
import sys
import platform


class shci_github_repo_info:
    owner:str
    access_token:str
    repo_name:str
    recursive:bool
    dir:str
    push:bool
    _os:str
    markdown:str

    def __init__(self, _owner:str, _access_token:str, _repo_name:str, _recursive:bool, _dir:str, _push:bool):
        self.owner = _owner
        self.access_token = _access_token
        self.repo_name = _repo_name
        self.recursive = _recursive
        self.dir = _dir
        self.push = _push

        _os:str = platform.system()
        if (_os == "Windows"):
            self._os = "windows"
        else:
            self._os = "linux"

        self.markdown = f"""
# {_repo_name}

![{_os}-badge]({_repo_name}/.shci/{_os}-status.svg)

## [{_os} build logs:](https://github.com/mrsinho/shci)

        """#`build ran for` /// `calls`



def shci_call(repo:shci_github_repo_info, cmd:str) -> bool:
    r:_wrap_close = os.popen(cmd)
    chunk:str = f"""
```bash
{cmd}
```

```bash
{r.read()}
```

---

    """
    repo.markdown += chunk

    print(f"shci: {chunk}")

    exit_code:bool
    

    return 1#

def shci_write_text(path:str, text: str):
    file:TextIOWrapper = open(path, "w")
    file.write(text)
    file.close()
    return

def shci_read_text(path: str) -> str:
    file:TextIOWrapper = open(path, "r")
    data:str = str(file.read())
    file.close()
    return data


def shci_clone_github_repo(owner:str, access_token:str, repo_name:str, recursive:bool, dir:str, push:bool) -> shci_github_repo_info:
    cmd:str = "";
    if (recursive == True):
        cmd += "git clone --recursive "
    else:
        cmd += "git clone "

    cmd += f"https://{access_token}@github.com/{owner}/{repo_name} {dir}"
    
    print(f"shci: {cmd}\n")

    try:
        os.system(cmd)
    except Exception:
        print("shci: Script is running")

    pull:str = f"cd {dir} && git stash && git pull"
    print(f"shci: {pull}")
    os.system(pull)

    repo:shci_github_repo_info = shci_github_repo_info(owner, access_token, repo_name, recursive, dir, push)
    return repo

def shci_build_status(repo:shci_github_repo_info, status:bool):
    setup_log_dir:str = f"cd {repo.dir} && mkdir .shci" 
    print(f"shci: {setup_log_dir}")
    os.system(setup_log_dir)

    clone_badge:str = f"cd {repo.dir} && git config user.name \"shci\" && git config user.email \"none\""
    if (status == True):#success
        print("shci: Build success\n")
        clone_badge += f"&& cd .shci && curl https://img.shields.io/badge/{repo._os}-passing-green.svg -o {repo._os}-status.svg"
    else:
        print("shci: Build failure\n")
        clone_badge += f"&& cd .shci && curl https://img.shields.io/badge/{repo._os}-failure-red.svg -o {repo._os}-status.svg"
    print(f"shci: {clone_badge}")
    os.system(clone_badge)

    push:str = f"git add --all && git commit -a -m \"shci status\" && git push https://{repo.access_token}@github.com/{repo.owner}/{repo.repo_name}"
    print(f"shci: {push}")
    if (repo.push):
        os.system(push)

    shci_write_text(f"{repo.dir}/.shci/{repo._os}-log.md", repo.markdown)

    return

def main():
    #example call: python shci.py ../../settings.txt
    args = []

    if (len(sys.argv) == 2):
        path:str = sys.argv[1]#script path
        args = shci_read_text(path).split()
    
    else:
        print("shci: Called shci.py with wrong arguments\n")
        return

    owner:str = args[0]
    access_token:str = args[1]
    repo_name:str = args[2]
    recursive:bool = bool(args[3])
    repo_dir:str = args[4]
    push:bool = bool(args[5])

    if (len(args) < 8):
        print("shci: Missing repository data")
        return

    print(f"""shci:
    owner: {owner},
    repo name: {repo_name},
    recursive flag: {str(recursive)},
    repo directory: {repo_dir}
    """)

    repo:shci_github_repo_info = shci_clone_github_repo(owner, access_token, repo_name, recursive, repo_dir, push)

    prerequisites:str = args[6]#batch or shell file, depends on the current system
    build_script:str = args[7]

    prerequisites = shci_read_text(prerequisites)
    print(f"shci: {prerequisites}")
    os.system(prerequisites)
    
    r:bool = shci_call(repo, shci_read_text(build_script))
    shci_build_status(repo, r)

    return


if __name__ == "__main__":
    main()