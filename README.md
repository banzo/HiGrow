Higrow software
======

Adapted from https://github.com/eXtgmA/HiGrow/ with following changes:

* change code structure for Platform.io instead of Android IDE
* return json instead of html
* disabled mqtt and bluetoot stuff
* disabled the deep sleep features because deprecated (also it seems the device does not work well on battery)
* moved SSID and password to separate file `secrets.h`

Quick Start
---------

* Install platform.io
* `pio lib install`
* copy `include/secrets.h.sample` to `secrets.h` and fill in your wifi credentials
* flash the device:
  * adapt `platform.io` config, e.g. set `upload_port`
  * build and upload
  * press
