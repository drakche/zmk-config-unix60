# C sources belonging to this module. The shield itself is devicetree and
# Kconfig only; this exists solely for the per-profile Bluetooth naming in
# src/bluetooth_profile_name.c, which is useless without BLE.
if(CONFIG_ZMK_BLE)
  zephyr_library()
  zephyr_library_sources(src/bluetooth_profile_name.c)
endif()
