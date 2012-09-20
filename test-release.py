#!/usr/bin/env python
import os, sys, subprocess, shutil, distutils.dir_util

def printinfo(message):
  print "\033[1;36m"+message+"\033[1;m"

def printerror(message):
  print "\033[1;31m"+message+"\033[1;m"
  
def runQmakeMake():
  if subprocess.call("qmake", shell=True) != 0:
    printerror("qmake failed"); sys.exit(1)
  if subprocess.call("make", shell=True) != 0:
    printerror("make failed"); sys.exit(1)

os.chdir(sys.path[0]) # change current working dir to script dir
os.mkdir("./temp")
os.chdir("./temp")

# === QCustomPlot package ===
printinfo("QCustomPlot package:")
os.mkdir("QCustomPlot")
os.chdir("QCustomPlot")
shutil.copy2("../../QCustomPlot.tar.gz", "./")
if subprocess.call("tar -xf QCustomPlot.tar.gz", shell=True) != 0:
  printerror("Failed to untar QCustomPlot.tar.gz"); sys.exit(1)

# interaction-example:
printinfo("interaction-example")
os.chdir("interaction-example")
runQmakeMake()
if subprocess.call("./interaction-example", shell=True) != 0:
  printerror("Execution unsuccessful")
os.chdir("..")
# plot-examples:
printinfo("plot-examples")
os.chdir("plot-examples")
runQmakeMake()
if subprocess.call("./plot-examples", shell=True) != 0:
  printerror("Execution unsuccessful")
os.chdir("..")

os.chdir("..")

# === QCustomPlot-sharedlib package ===
printinfo("QCustomPlot-sharedlib package:")
os.mkdir("QCustomPlot-sharedlib")
os.chdir("QCustomPlot-sharedlib")
shutil.copy2("../../QCustomPlot-sharedlib.tar.gz", "./")
if subprocess.call("tar -xf QCustomPlot-sharedlib.tar.gz", shell=True) != 0:
  printerror("Failed to untar QCustomPlot-sharedlib.tar.gz"); sys.exit(1)
shutil.copy2("../../QCustomPlot-source.tar.gz", "./")
if subprocess.call("tar -xf QCustomPlot-source.tar.gz", shell=True) != 0:
  printerror("Failed to untar QCustomPlot-source.tar.gz"); sys.exit(1)
# sharedlib compile:
printinfo("sharedlib compile")
os.chdir("sharedlib")
runQmakeMake()
subprocess.call("cp libqcustomplot* ../sharedlib-demo", shell=True)
os.chdir("..");
# sharedlib use:
printinfo("sharedlib use")
os.chdir("sharedlib-demo");
runQmakeMake()
if subprocess.call("export LD_LIBRARY_PATH=.; ./sharedlib-demo", shell=True) != 0:
  printerror("Execution unsuccessful")
os.chdir("..")

os.chdir("..")

# cleanup:
printinfo("Press any key to remove temp directory...")
raw_input()
os.chdir("..")
shutil.rmtree("./temp")
printinfo("done.")

