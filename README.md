# CleverHand configuraton GUI
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Aightech/qt-clvhd-config)](https://github.com/Aightech/qt-clvhd-config/latest)
[![GitHub](https://img.shields.io/github/license/Aightech/qt-clvhd-config)](https://github.com/Aightech/qt-clvhd-config/blob/main/LICENSE)



Simple Graphical interface to configurate a CleverHand master and stream EMG samples. 

# GUI
![GUI](docs/gui.png)


# Building source code
To build the project run:
```bash
cd qt-clvhd-config
mkdir build && cd build
cmake .. && make
```

# Demonstration app
When the project have been built, you can run:
```bash
./qt-clvhd-config -h
```
to get the demonstration app usage.

# Example
Open the ![main.cpp](src/main.cpp) file to get an example how to use the lib.
