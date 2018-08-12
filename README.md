# ORB_SLAM2-PythonBindings
This repository is adapted from jskinn's repository with the same name. 

NOTE: Although you will see many mentions of Python 3, we have actually modified the code to work with Python 2.

A python wrapper for ORB_SLAM2, which can be found at [https://github.com/raulmur/ORB_SLAM2](https://github.com/raulmur/ORB_SLAM2).
This is designed to work with the base version of ORB_SLAM2, with a couple of minimal API changes to access the system output.
It has been tested on ubuntu 14.04 and 16.04 and built against Python3, although it does not rely on any python3 features.

## Installation

### Prerequesities

- ORBSLAM2 source code
- ORBSLAM2 compiliation dependencies (Pangolin, Eigen, OpenCV)
- Boost, specifically its python component (python-35)
- Numpy development headers (to represent images in python, automatically converted to cv::Mat)

### Setup

#### Modifying ORBSLAM2

*Note: "orbslam-changes.diff" has changed from jskinn's original version. Below is the original documentation, but we've introduced additional changes.*

First, we need an additional API method from ORBSLAM to extract completed trajectories.
Apply the patch file "orbslam-changes.diff" to the ORBSLAM2 source, which should create an additional method and add some installation instructions to the end of CMakeLists.txt. The command to do this is

```
git apply ../ORB_SLAM2-PythonBindings/orbslam-changes.diff
```

When compiling on Ubuntu 18.04, you might see a message saying that usleep is not declared. This can be fixed by adding the following three lines to each file with this problem. This solution is obtained from wangzheqie's post: https://github.com/raulmur/ORB_SLAM2/issues/337

```
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
```

Build orbslam as normal, and then run `make install`. This will install the ORBSLAM2 headers and .so to /usr/local
(if an alternative installation directory is desired, specify it to cmake using `-DCMAKE_INSTALL_PREFIX=/your/desired/location`).

#### Compilation
Return to the ORBSLAM-Python source, build and install it by running
```
mkdir build
cd build
cmake ..
make
make install
```
This will install the .so file to /usr/local/lib/python3.5/dist-packages, such that it should 
If you have changed the install location of ORBSLAM2, you need to indicate where it is installed using ``-DORB_SLAM2_DIR=/your/desired/location``,
which should be the same as the install prefix above (and contain 'include' and 'lib' folders).

Verify your installation by typing
```
python3
>>> import orbslam2
```
And there should be no errors.

#### Examples

ORBSLAM2's examples have been re-implemented in python in the examples folder.
Run them with the same parameters as the ORBSLAM examples, i.e.:
```
python3 orbslam_mono_kitti.py [PATH_TO_ORBSLAM]/Vocabulary/ORBvoc.txt [PATH_TO_ORBSLAM]/Examples/Monocular/KITTI00-02.yaml [PATH_TO_KITTI]/sequences/00/
```

#### Alternative Python Versions

At the moment, CMakeLists is hard-coded to use python 3.5. If you wish to use a different version, simply change the boost component used (python-35) to the desired version (say, python-27), on line 38 of CMakeLists.txt.
You will also need to change the install location on line 73 of CMakeLists.txt to your desired dist/site packages directory.

## License
This code is licensed under the BSD Simplified license, although it requires and links to ORB_SLAM2, which is available under the GPLv3 license

It uses pyboostcvconverter (https://github.com/Algomorph/pyboostcvconverter) by Gregory Kramida under the MIT licence (see pyboostcvconverter-LICENSE).

