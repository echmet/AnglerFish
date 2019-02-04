AnglerFish
===

Introduction
---
AnglerFish is a graphical frontend for the [ElmigParamsFitter](https://github.com/echmet/EMPFitter) library. It shares the constituent database and user interface components with [PeakMaster NG](https://github.com/echmet/PeakMasterNG). Individual buffers can be seamlessly transferred between AnglerFish and [PeakMaster NG](https://github.com/echmet/PeakMasterNG).

Building
---
AnglerFish depends of the following tools and libraries:

- C++14-aware compiler
- [Qt 5 toolkit](https://www.qt.io/) (Qt 5.6 or higher is required)
- [Qwt toolkit](http://qwt.sourceforge.net/)
- [SQLite 3](https://www.sqlite.org/index.html)
- [ECHMETCoreLibs](https://github.com/echmet/ECHMETCoreLibs)
- [ElmigParamsFitter](https://github.com/echmet/EMPFitter)
- [ECHMETUpdateCheck](https://github.com/echmet/ECHMETUpdateCheck)

Refer to official documentation of the respective projects for details how to set them up.

### Generate makefiles with CMake
- Linux
1. `cd` into the source directory
2. Run `mkdir build` and `cd build`
3. Run `cmake .. -DCMAKE_BUILD_TYPE=Release -DECHMET_CORE_LIBS_DIR=<path_to_ECHMETCoreLibs_installation> -DEMPFITTER_DIR=<path_to_ElmigParamsFitter_installation> -DECHMET_UPDATECHECK_DIR=<path_to_ECHMETUpdateCheck_installation> -DQWTPLOT_DIR=<path_to_QwtPlot_installation> -DSQLITE3_DIR=<path_to_SQLite3_installation>`
4. Run `make`

*Note:* If any of the dependencies is installed as a system-wide dependency, it is not necessary to specify its installation path explicitly.

1. You may use CMake GUI to set up the required configuration options as it is described in the Linux section of this README.
2. Use `CMAKE_PREFIX_PATH` to specify the directory with your Qt5 installation.
3. Generate project files. MSVC 14 is currently the preferred compiler on Windows.

*Note:* The project is known to build correctly with GCC and Clang on Linux and MSVC 14 on Windows. While other compilers may by used as well it has not been tested by the authors.

Licensing
---
The AnglerFish project is distributed under the terms of **The GNU General Public License v3** (GNU GPLv3). See the enclosed `LICENSE` file for details.

As permitted by section 7. *Additional Terms* of The GNU GPLv3 license, the authors require that any derivative work based on AnglerFish clearly refers to the origin of the software and its authors. Such reference must include the address of this source code repository (https://github.com/echmet/AnglerFish) and names of all authors and their affiliation stated in section [Authors](#Authors) of this README file.

<a name="Authors"></a>
Authors
---
Bohuslav Gaš (1)   
Jana Šteflová (1)    
Vlastimil Hruška (2)    
Milan Boublík (1)    
Pavel Dubský (1)

(1)  
Group of Electromigration and Chromatographic Methods (http://echmet.natur.cuni.cz)

Department of Physical and Macromolecular Chemistry  
Faculty of Science, Charles University, Czech Republic

(2)  
Liquid Phase Separations Division, Agilent Technologies Deutschland GmbH & Co. KG   
Waldbronn, Germany
