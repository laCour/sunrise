# Sunrise

[![Build Status](https://travis-ci.org/laCour/sunrise.svg?branch=master)](https://travis-ci.org/laCour/sunrise)

Sunrise is a D-Bus application and standalone library that calculates sunrise and sunset times for automation use.

## Usage

### D-Bus Service

The following methods are available for D-Bus consumption:

```
Brightness (Double, Double coordinates) ↦ (Double percentage)
Sunrise (Double, Double coordinates) ↦ (UInt64 time)
Sunset (Double, Double coordinates) ↦ (UInt64 time)
```

The current D-Bus service name is `me.lacour.Sunrise1`. Introspection is available.

### Standalone Library

`sun.cpp` and `sun.h` can be dropped into your project for direct usage. This is useful for microcontroller applications.

Advanced parameters can be used, such as setting a custom zenith angle or day offset.

**Example usage:**

```c++
#include <sun.h>

int main() {
  double lat = 35.535113;
  double lng = -115.451481;

  Sun sun(lat, lng);

  bool rises = sun.rises();
  long sunrise = sun.sunrise();

  bool sets = sun.sets();
  long sunset = sun.sunset();

  double brightness = sun.brightness();

  return 0;
}
```

## Installation

Sunrise is currently only available for direct installation on Arch Linux: https://aur.archlinux.org/packages/sunrise/

## Building

The D-Bus development library is required for building from source (`libdbus` on Arch, `libdbus-1-dev` on Debian and Ubuntu, and `dbus-devel` on Fedora).

```bash
git clone https://github.com/laCour/sunrise.git
cd sunrise
make
```

In order to run the binary without connecting to the elevated D-Bus system bus, define `DEBUG` in `main.h`.

## References

TODO
