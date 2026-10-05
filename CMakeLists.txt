# C sources belonging to this module. The shield itself is devicetree and
# Kconfig only; this exists solely for the per-profile Bluetooth naming in
# src/bluetooth_profile_name.c, which is useless without BLE.
#
# This adds to the `app` target rather than declaring a zephyr_library(). That
# looks like the wrong way round -- zephyr_library() is the canonical pattern
# for a module -- but a library of our own gets only ZMK's exported headers
# under app/module/include, and zmk/ble.h is app-private under app/include. A
# zephyr_library() build therefore fails with "zmk/ble.h: No such file or
# directory". Compiling into `app` inherits its include paths, which is how
# every zmk-config with custom C does it.
#
# The ordering holds: Zephyr creates `app` in kernel.cmake (zephyr_library_named
# at line 211) immediately before add_subdirectory(${ZEPHYR_BASE}) at line 214,
# and modules are added from within that, so the target already exists here.
if(CONFIG_ZMK_BLE)
  target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/bluetooth_profile_name.c)
endif()
