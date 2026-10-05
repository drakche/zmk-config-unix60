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
 * Only act on a real change of profile. ZMK raises this event from three
 * places -- set_profile_address(), connected() and disconnected() -- so it
 * also fires while a bond is being written during pairing. Renaming calls
 * bt_le_adv_stop() and restarts advertising, and doing that mid-pairing kills
 * the procedure; the host reports it as a wrong PIN or passkey. Tracking the
 * last name applied keeps advertising untouched unless the profile really
 * changed, which is the only time the name needs to move.
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

/* Which name is currently applied; -1 until the first one is set. */
static int applied_index = -1;

static int profile_name_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *ev = as_zmk_ble_active_profile_changed(eh);

    if (ev == NULL || ev->index >= ARRAY_SIZE(profile_names)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    /* Connect, disconnect and bond-write all raise this event for the profile
     * already in use. Renaming then would stop advertising for no reason. */
    if (ev->index == applied_index) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    int err = zmk_ble_set_device_name(profile_names[ev->index]);
    if (err) {
        LOG_ERR("Failed to rename for profile %d (err %d)", ev->index, err);
        return ZMK_EV_EVENT_BUBBLE;
    }

    applied_index = ev->index;

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(unix60_ble_names, profile_name_listener);
ZMK_SUBSCRIPTION(unix60_ble_names, zmk_ble_active_profile_changed);
