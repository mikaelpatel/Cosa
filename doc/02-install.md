# How to install Cosa?

## Via Arduino IDE Boards Manager

* Add the URL to the Cosa package definition file to the Arduino IDE
Preferences>Additional Boards Manager URLs field.
https://raw.githubusercontent.com/mikaelpatel/Cosa/master/package_cosa_index.json

* Open the Arduino IDE Boards Manager in the Tools>Board menu.

* Select Cosa and install.


## Via GIT

For Arduino 1.0.X:
```shell
cd <custom_path>/sketchbook
mkdir hardware
cd hardware
git clone https://github.com/mikaelpatel/Cosa.git
```

For Arduino 1.5.x and higher an extra avr directory level is needed:
```shell
cd <custom_path>/sketchbook
mkdir hardware
cd hardware
mkdir Cosa
cd Cosa
git clone https://github.com/mikaelpatel/Cosa.git
mv Cosa avr
```

Then you can update the library by using:

```shell
git pull origin master
```

## Via Archives

### Step 1

* Download the [Cosa zip file](https://github.com/mikaelpatel/Cosa/archive/master.zip).
* Create the hardware folder if missing (<custom_path>/sketchbook/hardware).
* Unzip the files to your hardware folder.
* Rename `Cosa-master` to `Cosa`.

### Step 2 (For Arduino 1.5.x and 1.6.x only)

* Create an outer `Cosa` folder (<custom_path>/sketchbook/hardware/Cosa).
* Unzip the files to your `hardware/Cosa` folder.
* Rename inner `Cosa` folder (<custom_path>/sketchbook/hardware/Cosa/Cosa-master) to `avr` (<custom_path>/sketchbook/hardware/Cosa/avr).
* Arduino 1.6.1 does not include the GCC plugin for link time
optimization (LTO). Please rename the file platform.txt-windows-1.6.1
to platform.txt if using 1.6.1 on Windows.

### Step 3 (For Attiny users on Windows version only and Arduino 1.0.X)

* Install [this patch](https://github.com/TCWORLD/ATTinyCore/tree/master/PCREL%20Patch%20for%20GCC)

Do not forget to program the ATtiny device with the bootloader, i.e.,
set the fuse bits, before using the device for the first time.

### Step 4

* Restart the Arduino IDE and Cosa will show up as a number of boards and example sketches.

## Latest AVR Toolchain

The new AVR toolchain with gcc 4.8.1 is supported. There is also a
platform definition file with link-time optimization enabled. This may
be used to reduce memory foot-print (program size) and increase
performance.

## Run your first sketch.

* Open the CosaBlink example sketch in the Sketchbook>hardware>Cosa>avr>Blink menu.
* Select your board in the Tools>Board menu. Use the board with prefix
"Cosa" to get the right build settings.
* Compile and upload.

