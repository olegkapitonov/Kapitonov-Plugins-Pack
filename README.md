Kapitonov Plugins Pack (KPP)
============================

__WORK IN PROGRESS !!!__

Last release is 0.1 beta.

> Set of plugins for guitar sound processing.
> Compatible with hosts such as Ardour, Qtractor,
> guitarix, jack-rack.

Source code and binary files are available for Linux 64-bit systems.
Windows versions are planned for the future.

[Screenshot](screen.jpg)

### Currently avaliable plugins

1. Bluedream.
   Booster/Tube Screamer pedal with equalizer (tonestack).
   Has GUI.
2. Distruction.
   Distortion pedal with equalizer (tonestack).
   Has GUI.
3. Deadgate.
   Effective Noise Gate/Dead Zone effect plugin.


This plugins available in LV2 and LADSPA versions. It is
strongly recommended to use LV2 versions. LADSPA versions
are mainly intended for use with guitarix.
 

### Plugins in development, will be available soon

4. tubeAmp.
   Advanced guitar tube amp emulator. Contains preamp,
   tonestack, power amp, cabinet emulators. Emulation
   parameters of each component are set by profile files.
5. single2humbucker.
   Plugin for emulation humbucker pickup sound with
   single coil pickup on the guitar. Useful for playing 
   heavy-metal on Stratocaster guitar with single coil pickups.


tubeAmp is the main and most complex plugin in the set.
It can be used to emulate the sound of any common models 
of guitar combo amplifiers.


### Dependencies for using

1. LV2 or LADSPA compatible host on Linux operating system.
   LV2 is highly recommended. It can be Ardour, Qtractor, 
   or guitarix with LADPSA versions.
   VST versions for Windows are planned for the future.
2. Cairo library for GUI.

### Dependencies for building

1. g++ compiler.
2. Cairo library development files (headers, pkg-info).
3. Faust 2.x compiler and libraries, for generating code from
   DSP source files. Not needed for ordinary build, if you
   don't change the DSP code.


### How to build and install

1. Each plugin sorce code folder contains `build.sh` shell script.
   Run it.
2. LV2 plugin bundle will be in `plugin_name.lv2` folder inside
   the plugin source code folder. LADSPA .so file will be in the
   plugin source code folder.
3. Copy LV2 bundle or LADSPA .so file to the proper place in your system,
   which is in the plugins PATH for your host application. Usually it is
   `/usr/lib/lv2` for LV2 plugins and `/usr/lib/ladspa` for LADSPA plugins.
4. Launch the host application (e. g. Ardour). Find the desired plugin 
   in the library. Names will have `kpp_` prefix.


### How to install binary versions

1. Copy LV2 bundle or LADSPA .so file to the proper place in your system,
   which is in the plugins PATH for your host application. Usually it is
   `/usr/lib/lv2` for LV2 plugins and `/usr/lib/ladspa` for LADSPA plugins.
2. Launch the host application (e. g. Ardour). Find the desired plugin in library,
   names will have `kpp_` prefix.


## Development

DSP code is written in Faust language. GUI code is written in C
with Cairo library for rendering and Xlib library for low-level
operations.


## License

GPLv3+.
