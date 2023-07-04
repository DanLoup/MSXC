#Hacked together build script that does not compile unchanged files because i not bothering dealing with make
libs = "-lSDL -lGL -lz"
comp = "g++"
flags = "-ggdb -O3"
exec = "msxc"
buildfd="build"
from genericpath import getmtime
from pathlib import Path
from os import path
import subprocess
import os
import sys
nms = []
nns = []
nmsh = []
nnsh = []
if os.path.exists("./"+buildfd+"/check.txt") == False:
    f = open("./"+buildfd+"/check.txt", mode='w', encoding='utf-8')
    f.writelines(nms)
    f.close()
if os.path.exists("./"+buildfd+"/checkh.txt") == False:
    f = open("./"+buildfd+"/checkh.txt", mode='w', encoding='utf-8')
    f.writelines(nms)
    f.close()


if len(sys.argv)>1:
    if sys.argv[1]=="burn":
        print("burning everything to the ground")
        f = open("./"+buildfd+"/check.txt", mode='w', encoding='utf-8')
        f.writelines(nms)
        f.close()

p = Path("./src/")
for fl in p.iterdir():
    if (fl.name.find("cpp")>-1):
        nms.append(fl.name+"\n")
        nms.append(str(getmtime(fl))+"\n")
    if (fl.name.find(".h")>-1):
        nmsh.append(fl.name+"\n")
        nmsh.append(str(getmtime(fl))+"\n")

f = open("./"+buildfd+"/check.txt", mode='r', encoding='utf-8')
nns = f.readlines()
f.close()

f = open("./"+buildfd+"/checkh.txt", mode='r', encoding='utf-8')
nnsh = f.readlines()
f.close()

burn = 0
for a in range(0,len(nmsh)-1,2):
    try:
        i = nnsh.index(nmsh[a])
        if nmsh[a+1]!=nnsh[i+1]:
            burn = 1
    except ValueError:
        burn = 1
if burn == 1:
    nns = []
    print (".h file changed, too dumb to do a proper check, rebuilding everything")
changed = []
for a in range(0,len(nms)-1,2):
    try:
        i = nns.index(nms[a])
        if nms[a+1]!=nns[i+1]:
            changed.append(nms[a].strip())

    except ValueError:
        print ("new file:"+nms[a])
        changed.append(nms[a].strip())
pc = 0
pistack=[]
if len(changed)<1:
    print ("Nothing to be done")
    exit()
for a in range(len(changed)):
    pistack.append(subprocess.Popen([comp,"-ggdb","-O0","-c","./src/"+changed[a],"-o","./"+buildfd+"/"+changed[a].replace("cpp","o")]))
    print ("building:"+changed[a])
    try:
        while len(pistack)>3:
            for m in range(len(pistack)):
                if pistack[m].poll()!=None:
                    if (pistack[m].poll()>0):
                        raise Exception("File error")
                    pistack.remove(pistack[m])
                    break
    except Exception as e:
        print ("Compilation failed, exiting")
        for m in pistack:
            m.kill()
        exit()

try:
    while len(pistack)>0:
        for m in range(len(pistack)):
            if pistack[m].poll()!=None:
                if (pistack[m].poll()>0):
                    raise Exception("File error")
                pistack.remove(pistack[m])
                break
except Exception as e:
    print ("Compilation failed, exiting")
    for m in pistack:
        m.kill()
    exit()
f = open("./build/check.txt", mode='w', encoding='utf-8')
f.writelines(nms)
f.close()
f = open("./build/checkh.txt", mode='w', encoding='utf-8')
f.writelines(nmsh)
f.close()

print ("assembling executable")
os.system(comp+" "+flags+" ./"+buildfd+"/*.o -o ./"+exec+"/"+exec+" "+libs)
