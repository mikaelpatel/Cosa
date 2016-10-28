Configuration
=============
Some symlinks need to be created in the PlatformIO's home folder residing "<home dir>./platformio" as follows:
    ./platformio/platforms/cosa -> <Cosa project root>/build/PlatformIO/platform/cosa
    ./platformio/packages/framework-cosa -> <Cosa project root>

A special CosaBuild project for PlatformIO is used to rebuild Cosa under this build system. The project configuration file is "build/PlatformIO/CosaBuild/platformio.ini" contains a separate section for each build to be built against. All sections must have the following values set:
    platform = cosa
    framework = cosa

Board configurations reside in "<Cosa project root>/build/PlatformIO/platform/cosa/boards" and only a few have been added so far" (TODO).

Build
=====
Enter "<Cosa project root>/build/PlatformIO/CosaBuild" and run
    platformio run


Support
=======
The build system has so far been tested only on Windows, so some glitches may be expected on Linux, althouth it is supposed to be cross-platform.

Currently, the build script "<Cosa project root>/build/PlatformIO/platform/cosa/builder/frameworks/cosa.py" prints some debug info about the library folders detected and added to the build path (lines 76-77), and this should be disabled in the future after more cross-platform testing.