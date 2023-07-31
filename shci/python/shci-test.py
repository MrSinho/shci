import os
import sys
import platform
import shutil
from io import TextIOWrapper

def shci_test_read_text(path: str) -> str:
    file:TextIOWrapper = open(path, "r")
    data:str = str(file.read())
    file.close()
    return data

def main():#python shci-test.py shvulkan #copy this file to the folder before the git repository
    
    #os.system("cd shci_root && git stash && git pull")
    #shutil.copy2(f"shci_root/shci/python/shci.py", f"{str(sys.argv[1])}/shci.py")

    system_name:str = platform.system()
    if (system_name == "Windows"):
        system_name = "windows"
    else:
        system_name = "linux"

    args:str = shci_test_read_text(f"{sys.argv[1]}/.shci/{system_name}/info.txt").replace("\n", " ")

    print(f"shci test args: {args}\n")

    cmd:str = ""
    if (system_name == "windows"):
        cmd = f"cd {sys.argv[1]} && python .shci/shci/shci/python/shci.py {args}"
        print(f"shci test command: {cmd}\n")
    else:
        cmd = f"cd {sys.argv[1]} && python3.9 .shci/shci/shci/python/shci.py {args}"
        print(f"shci test command: {cmd}\n")

    os.system(cmd)

    return


if __name__ == "__main__":
    main()
