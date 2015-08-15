Configurations
--------------

The Arduino IDE is configured with the platform.txt and boards.txt files.
There are several variants for attributes and recipes in these files.

This directory contains some alternative configurations.

[platform.txt-classic](./platform.txt-classic).
This configuration file is for the classical variant (1.5.0-1.6.1).

[platform.txt-flto](./platform.txt-flto).
Link-time-optimization enabled. Requires that the AVR GCC toolchain
contains the neccessary plug-in. Does not work with 1.6.4 for
Windows.

Legacy versions of boards manager package definition files
(package_cosa-VERSION_index.json) are also saved here. Use the raw
file URL to load legacy versions of Cosa in the Arduino IDE.

