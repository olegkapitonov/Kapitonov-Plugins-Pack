Kapitonov Plugins Pack (KPP)
============================

Latest release is 1.2.1.

**Attention!!!**

**DSP model of kpp_distruction and kpp_fuzz plugins changed since version 1.2!
Update carefully!**

> Set of plugins for guitar sound processing.
> Compatible with hosts such as Ardour, Qtractor, Carla,
> guitarix, jack-rack.

Binary files are available for Linux 64-bit systems.
Source code can be compiled for Linux 64-bit or 32-bit.
Windows versions are planned for the future.

![Screenshot](screen.jpg)

### Currently available plugins

1. tubeAmp.
   Advanced guitar tube amp emulator. Contains preamp,
   tonestack, power amp with voltage sag, cabinet emulators.
   Emulation parameters of each component are set by profile files.
2. Bluedream.
   Booster/Tube Screamer pedal with equalizer (tonestack).
   Has GUI
3. Distruction.
   Distortion pedal with equalizer (tonestack).
   Has GUI.
4. Fuzz.
   Vintage fuzz pedal.
   Has GUI.
5. Deadgate.
   Effective Noise Gate/Dead Zone effect plugin.
6. Octaver.
   Analog octaver pedal.
7. Single2Humbucker.
   Plugin for emulation humbucker pickup sound with
   single coil pickup on the guitar. Useful for playing
   heavy-metal on Stratocaster guitar with single coil pickups.


This plugins (except tubeAmp) available in LV2 and LADSPA versions. It is
strongly recommended to use LV2 versions.

tubeAmp is the main and most complex plugin in the set.
It can be used to emulate the sound of any common models
of guitar combo amplifiers.

You can create and edit \*.tapf profiles with **tubeAmp Designer**.

### IMPORTANT!!!

The input level at the beginning of the plugins chain should be -20 dB!
You can use plugins like https://github.com/x42/meters.lv2 to measure
and adjust the signal level.


### Dependencies for using

1. LV2 or LADSPA compatible host on Linux operating system.
   It can be Ardour, Qtractor, Carla, guitarix.
   VST versions for Windows are planned for the future.
2. Cairo library for GUI.
3. Zita-resampler 1.6+ and zita-convolver 4.0+ libraries (not needed for binary versions).

In Ubuntu run:

`apt install libxcb1 libxcb-util1 libxcb-icccm4 libcairo2 libxau6 libxdmcp6 libpixman-1-0 libfontconfig1 libfreetype6 libpng16-16 libxcb-shm0 libxcb-render0 libxrender1 libx11-6 libxext6 zlib1g libbsd0 libexpat1 libfftw3-3`

### Dependencies for building

1. g++ compiler.
2. Meson-0.51+ build system.
3. Cairo library development files (headers, pkg-info).
4. Boost development files.
5. Zita-resampler 1.6+ development files.
6. Zita-convolver 4.0+ development files.
7. LV2 or LADSPA development files.
8. Faust 2.x compiler and libraries.

In Ubuntu run:

`apt install libxcb1-dev libxcb-util-dev libxcb-icccm4-dev libcairo2-dev libpixman-1-dev libfontconfig1-dev libfreetype6-dev libpng-dev libxcb-shm0-dev libxcb-render0-dev libxrender-dev libx11-dev libxext-dev zlib1g-dev libbsd-dev libexpat1-dev libfftw3-dev libboost-all-dev lv2-dev ladspa-sdk libzita-resampler-dev libzita-convolver-dev faust meson`

**Attention!!!** Even in Ubuntu Focal Fossa zita-resampler is old 3.x! So use `thirdparty-included`
branch in this case, instead of `master`!

**Attention!!!** Check version of `faust` in your distro! Ubuntu Bionic Beaver has old 0.9.x version!
In this case build latest version of `faust` from source.

**Attention!!!** Check version of `meson` in your distro! Ubuntu Bionic Beaver has old version!
In this case build latest version of `meson` from source.

### How to build and install

Project now uses meson build system (thanks to [cyclopsian](https://github.com/cyclopsian))

1. Run `meson build` and then `ninja -C build` in the source directory.
2. Run `ninja -C build install` to install to /usr/lib/ladspa and /usr/lib/lv2.
   To install to your user directory, run
  `meson build --reconfigure -Dladspadir=.ladspa -Dlv2dir=.lv2 --prefix $HOME` and then
  `ninja -C build install`.
3. Launch host application (e. g. Ardour). Find desired plugin
   in the library. Names will have `kpp_` prefix.
4. If your distribution has old versions of zita-resampler and zita-convolver,
   use 'thirdparty-included' branch instead of 'master'.


### How to install binary versions

1. For Debian Buster (10) download KPP-1.1-binary-debian10.tar.bz2.
2. For Ubuntu LTS and other distributions download KPP-1.1-binary-ubuntu-bionic.tar.bz2.
3. Copy LV2 bundle or LADSPA .so file to the proper place in your system,
   which is in the plugins PATH for your host application. Usually it is
   `/usr/lib/lv2` for LV2 plugins and `/usr/lib/ladspa` for LADSPA plugins.
4. Launch host application (e. g. Ardour). Find desired plugin in library,
   names will have `kpp_` prefix.

### Quick start guide

[English](https://github.com/olegkapitonov/Kapitonov-Plugins-Pack/blob/master/guide.md)

[Русский](https://github.com/olegkapitonov/Kapitonov-Plugins-Pack/blob/master/guide_ru.md)


## Development

DSP code is written in Faust language. GUI and support code is written in C and C++
with Cairo library for rendering and Xlib library for low-level
operations.


## License

GPLv3+.
