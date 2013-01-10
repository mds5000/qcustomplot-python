Installing on Linux
====================
 1. Install packages.
    sudo apt-get install python-sip-dev pyqt4-dev-tools

 2. Build QCustomPlot Bindings
    - git clone this repository, switch to python branch
    - build the shared library, under sharedlib/sharedlib-compillation
      qmake, make, (make install)
    - Copy the library (*.so) to the root of the repository
    FIXME: Should really make the configure.py smart enough to do this.
    - now in the root of the repository, run:
      python configure.py
      make

  3. Test
    - Add current directory to library path and start python to test.
    LD_LIBRARY_PATH=`pwd` python
    import qcustomplot
    dir(qcustomplot)

Installing on Windows (MSVC 2008)
==================================
 0. Install python 2.7
 1. Install Microsoft Visual C++ 2008. It must be 2008.
    I used MSVC 2008 Express Edition with SP1.
    http://www.microsoft.com/en-us/download/details.aspx?id=20682

 2. Install Qt4 (5 may work) built using MSVC 2008.
    Getting the pre-built binaries are fine.

 3. Install SIP
    - Extract into folder.
    - Open a Visual Studio 2008 Command Prompt
    - python configure.py --platform=win32-msvc2008
    - nmake
    - nmake install

 4. Install PyQt4 (I built from source, but MSVC2008 binaries should work)
    - Extract, and open MSVC 2008 prompt
    - python configure.py
    - nmake
    - nmake install

 5. Build QCustomPlot Bindings
 FIXME: amalgamate is a unix only program, need to have qcustomplot.h/.cpp already created on windows.
    - git clone this repository.
    - Again, open MSVC 2008 prompt
    - cd to sharedlib/sharedlib-compillation
    - qmake, nmake to build
    - cd back to the root repository
    - python configure.py --platform win32-msvc2008
    
    OK Now the tricky part... I can't get configure.py to create a suitable
    Makefile. SO...
     * Format the linker args properly for LINK.EXE. 
        - use /LIBPATH:C:/path/tolibs.
        - Remove the -llibraryname, and replace with libraryname.lib

     * And to fix DLL symbol exports, its necessary to use the Q_DECL_IMPORT
     macros. So, in qcustomplot.h, remove the preprocessor if/then to force
     QCP_LIB_DECL to be Q_DECL_IMPORT. There is probably a better way of doing
     this...

    OK, Now build it.
    - nmake

Installing on Windows (MinGW) BROKEN!!!
==================================

 0. You need to have python installed. I'm using 2.7.

 1. Install the latest MinGW from here:
   http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/

   Run the downloaded .exe file.
   Make sure to include the C++ compiler and MinGW Dev tools.

   Finally, add C:\MinGW\bin to your PATH.

 2. Install the MinGW-built binaries for Qt4.
 http://releases.qt-project.org/qt4/source/qt-win-opensource-4.8.4-mingw.exe
 - Ignored error about w32api version not being compatable...

 3. Install SIP
   - Follow the directions here:
   http://www.riverbankcomputing.com/static/Docs/sip4/installation.html

   -Basically; download, extract, and run:
   python configure.py --platform win32-g++
   mingw32-make
   mingw32-make install

 4. Install PyQt4 (build from source, the binaries are built with MSVC :( )
   http://www.riverbankcomputing.com/software/pyqt/download
   PyQt-win-gpl-4.9.6.zip

   python configure.py
   mingw32-make
   mingw32-make install


 5. git clone git://path/to/my/qcustomplot/
   - switch to the python branch
   - under sharedlib, qmake, mingw32-make

   BROKEN: I was able to compile all of this, load the python module, but
   program crashes when trying to call any function. No error message.

