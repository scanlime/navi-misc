annie - Neural Network Library
http://annie.sourceforge.net
Version 0.5

Last Modified On: January 12, 2003

Author(s):
    - Asim Shankar
      http://www.geocities.com/asimshankar
      asimshankar@users.sourceforge.net

NOTES:
* ($ANNIE) in this readme refers to the directory in which you've kept annie.
  For example, if you placed all annie files in /home/asim/annie then
  ($ANNIE) = /home/asim/annie
  and if you placed them in "E:\Development\annie" then
  ($ANNIE) = e:\Development\annie

NEW FEATURES
------------

Since 0.0:
 * Hopfield Networks can now be created and used using annie
 * An example of the Hopfield Network interface has been added
 * tansig() activation function added (Courtesy Jose Ramos (warta@users.sourceforge.net))
 * Other little things I don't quite remember
 * Matlab Extension Pack - Separate download from the website.
   Allows you to create training sets in C++, import them into Matlab,
   train networks in Matlab and then export them back to a format that
   is understood by annie so that you can use the network in your C++
   applications.

BUG FIXES
---------

Since 0.0:
  * Memory leaks in Layer and Neuron fixed
  * Error in loading a MultiLayerNetwork from a file was fixed

CREATING THE LIBRARY
--------------------

The library file will be placed in ($ANNIE)/lib/
1) Linux
- Run "make lib"
- The library file generated will be libannie.a

2) Windows
- Download the Visual Studio.NET solution files from the annie website
  and unzip the file into in ($ANNIE)/windows/
- Build
- The library file generated with be:
  annied.lib - For the Debug configuration
  annier.lib - The for Release configuration


LINKING WITH THE LIBRARY
------------------------

1) Add "($ANNIE)/include/" to your include path
2) Add "($ANNIE)/lib/" to your library path
3) Link with
   - libannie.a (Linux)
   - annied.lib (Windows - Debug configuration)
   - annier.lib (Windows - Release configuration)

For example, in Linux you could try something like:
g++ myfile.cpp -I /home/asim/annie/include -L /home/asim/annie/lib -lannie
(Presuming that annie is present in /home/asim/annie)

DIRECTORY STRUCTURE
-------------------

bin		: Executable utilities and examples.
		  Construct utils using "make utils" and examples using "make examples" in Linux
		  and by building the solution in Windows

doc		: Documentation generated used doxygen
		  Construct docs using "make doc" in Linux
		  Or download from the annie website

examples: Source files for some example programs.

include	: All header files relevant to annie. You need to include only
		  ($ANNIE)/include/annie.h in your program and 
		  add ($ANNIE)/include to your include path to use the library

lib     : Library binaries are placed here

obj     : Object files generated in Windows are placed here (.obj)

src     : The source files.
		  In Linux the object files will also be here.
		  You can get rid of them using "make clean"

windows : (optional, may not be present depending on what you downloaded)
		  This is where the Visual Studio solution files are placed.
