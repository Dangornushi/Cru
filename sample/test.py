import os
import subprocess
from termcolor import colored, cprint

def checkReturnCode(command):
    subprocess.run(command)
    return 0

def compile(filename, extension):
    noExtension = filename[0 : len(filename)-4]

    cprint("running: " + filename, "red")

    print("Run LLVM IR:");
    # llvm ir
    checkReturnCode(["../build/cru", filename])
    checkReturnCode(["lli", noExtension+".ll"])
    print("");

    print("Run clang:");
    # clang and run
    checkReturnCode(["../build/cru", "-C", filename])
    checkReturnCode(["clang", "-o", noExtension, noExtension+".c"])
    checkReturnCode(["./" + noExtension])
    print("");

    print("Run python3:");
    # Pytohn run
    checkReturnCode(["../build/cru", "-P", filename])
    checkReturnCode(["python3", noExtension+".py"])
    print("");

    print();

path = "."

files = os.listdir(path)

for f in files:
    filename = os.path.join(path, f) 
    extension = filename[len(filename)-4 : len(filename)]

    if (extension == ".cru"):
        compile(filename[2:len(filename)], extension)


