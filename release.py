#!/usr/bin/env python
import os, sys, subprocess, shutil, distutils.dir_util

def printinfo(message):
  print "\033[1;36m"+message+"\033[1;m"

def printerror(message):
  print "\033[1;31m"+message+"\033[1;m"
  
def removeExcludeBlockFromFile(fname):
  os.rename(fname, fname+".temp")
  original = open(fname+".temp", "r")
  new = open(fname, "w")
  excludeBlock = False;
  line = original.readline();
  while line:
    if "release-exclude-start" in line:
      excludeBlock = True
    if not excludeBlock:
     new.write(line);
    if "release-exclude-end" in line:
      excludeBlock = False
    line = original.readline();
  original.close()
  new.close()
  os.remove(fname+".temp")

tarcommand = "GZIP=\"-9\" tar -caf" # -a means determine compressor by archive suffix
tarsuffix = ".tar.gz"

os.chdir(sys.path[0]) # change current working dir to script dir
os.mkdir("./temp")
os.chdir("./temp")

# documentation:
printinfo("Building QCustomPlot-doc package")
distutils.dir_util.copy_tree("../doc/html", "./html")
shutil.copy2("../doc/qthelp/qcustomplot.qch", "./")
subprocess.call(tarcommand+" QCustomPlot-doc"+tarsuffix+" *", shell=True)
shutil.move("QCustomPlot-doc"+tarsuffix, "../")
subprocess.call("rm -r *", shell=True)

# source only:
printinfo("Building QCustomPlot-source package")
for f in ["../qcustomplot.h", "../qcustomplot.cpp", "../GPL.txt", "../changenotes.txt"]:
  shutil.copy2(f, "./")
subprocess.call(tarcommand+" QCustomPlot-source"+tarsuffix+" *", shell=True)
shutil.move("QCustomPlot-source"+tarsuffix, "../")
subprocess.call("rm -r *", shell=True)

# source and examples:
printinfo("Building QCustomPlot package")
for f in ["../qcustomplot.h", "../qcustomplot.cpp", "../GPL.txt", "../changenotes.txt"]:
  shutil.copy2(f, "./")
distutils.dir_util.copy_tree("../plot-examples", "./plot-examples")
distutils.dir_util.copy_tree("../interaction-example", "./interaction-example")

os.chdir("./plot-examples");
subprocess.call("qmake", shell=True)
subprocess.call("make clean", shell=True)
for f in ["./plot-examples.pro.user", "./plot-examples", "./Makefile"]:
  if os.path.isfile(f):
    os.remove(f)
shutil.rmtree("./screenshots")
removeExcludeBlockFromFile("plot-examples.pro");

os.chdir("../interaction-example");
subprocess.call("qmake", shell=True)
subprocess.call("make clean", shell=True)
for f in ["./interaction-example.pro.user", "./interaction-example", "./Makefile"]:
  if os.path.isfile(f):
    os.remove(f)

os.chdir("../");
subprocess.call(tarcommand+" QCustomPlot"+tarsuffix+" *", shell=True)
shutil.move("QCustomPlot"+tarsuffix, "../")
subprocess.call("rm -r *", shell=True)

# shared lib:
printinfo("Building QCustomPlot-sharedlib package")
shutil.copy2("../sharedlib/readme.txt", "./");
os.mkdir("sharedlib");
shutil.copy2("../sharedlib/sharedlib.pro", "./sharedlib/");
os.mkdir("sharedlib-demo");
for f in ["../sharedlib-demo/sharedlib-demo.pro", "../plot-examples/main.cpp", "../plot-examples/mainwindow.h", "../plot-examples/mainwindow.cpp", "../plot-examples/mainwindow.ui"]:
  shutil.copy2(f, "./sharedlib-demo/")
subprocess.call(tarcommand+" QCustomPlot-sharedlib"+tarsuffix+" *", shell=True)
shutil.move("QCustomPlot-sharedlib"+tarsuffix, "../")
subprocess.call("rm -r *", shell=True)

# clean up:
os.chdir("../")
os.rmdir("./temp")
printinfo("done")


