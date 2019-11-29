unicornDock is a unicornly cool desktop panel that magically will change the color of all icons
in the dock to the colors of a  unicornly rainbow.


![unicornDock in action](unicorndock.png)


It's an adaptation of KSmoothDock.


Notes
=====

This is a very personal project, so it is adapted to my computer. This means:
It works only with 4k resolution (will be too large by default for Full HD)
and it needs Moka icons. Very special ones. My ones. Will upload them, if
anyone asks for them. Many things needs still to be done and fixed, but well,
whatever, works for me now.



---


KSmoothDock is a cool desktop panel with parabolic zooming effect for KDE Plasma 5, with the focus on smooth zooming effect and being simple and easy to use.

KSmoothDock is written in C++ and depends on Qt 5 and KDE Frameworks 5.

For more information, please visit KSmoothDock's homepage: https://dangvd.github.io/ksmoothdock

---

Dependencies: to build from the source code, several Qt 5 and KDE Frameworks 5 development packages are required.
- On Debian-based distributions, they can be installed by running:
$ sudo apt install gettext extra-cmake-modules qtbase5-dev libkf5activities-dev libkf5config-dev libkf5coreaddons-dev libkf5dbusaddons-dev libkf5i18n-dev libkf5iconthemes-dev libkf5xmlgui-dev libkf5widgetsaddons-dev libkf5windowsystem-dev
- For Fedora, install the following packages: extra-cmake-modules kf5-plasma-devel qt5-devel kf5-kactivities-devel kf5-kdbusaddons-devel kf5-ki18n-devel kf5-kiconthemes-devel kf5-kxmlgui-devel kf5-kwidgetsaddons-devel kf5-kwindowsystem-devel

To build, run:
$ cmake src
$ make

To launch the program:
$ ./ksmoothdock

To install, run:
$ sudo make install

After the installation, KSmoothDock can be launched from the Application Menu (Utilities category), or from the command line:
$ ksmoothdock

To uninstall, run:
$ sudo make uninstall

To execute the automated tests, run:
$ make test
