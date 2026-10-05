/*
 * Copyright (c) 2026 The Unix60 Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Give each Bluetooth profile its own advertised name.
 *
 * ZMK advertises CONFIG_ZMK_KEYBOARD_NAME for every profile, so a keyboard
 * paired to several hosts shows up as five identical "Unix60" entries. This
 * listens for the active profile changing and renames the device to match,
 * which zmk_ble_set_device_name() applies before advertising restarts.
 *
 * The names are numbered by the key that selects the profile, not by ZMK's
 * own profile index, which starts at 0: Fn+Ctrl+1 is profile 0, "Unix60-1".
 *
 * A host caches the name it saw when it bonded. Renaming therefore only shows
 * up on hosts paired after this firmware is flashed -- an existing bond keeps
 * displaying "Unix60" until it is removed and re-paired.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

LOG_MODULE_REGISTER(unix60_ble_names, CONFIG_ZMK_LOG_LEVEL);

/* Not const: zmk_ble_set_device_name() takes a non-const char *. */
static char profile_names[][16] = {
    "Unix60-1", "Unix60-2", "Unix60-3", "Unix60-4", "Unix60-5",
};

static int profile_name_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *ev = as_zmk_ble_active_profile_changed(eh);

    if (ev == NULL || ev->index >= ARRAY_SIZE(profile_names)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    int err = zmk_ble_set_device_name(profile_names[ev->index]);
    if (err) {
        LOG_ERR("Failed to rename for profile %d (err %d)", ev->index, err);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(unix60_ble_names, profile_name_listener);
ZMK_SUBSCRIPTION(unix60_ble_names, zmk_ble_active_profile_changed);
