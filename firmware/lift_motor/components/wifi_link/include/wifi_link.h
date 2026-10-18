// Wi-Fi in APSTA mode: the dock's own access point is always up, and it
// additionally joins a configured network when one is in range.
// Implements spec FR7, FR10, FR12, FR13.
//
// Local control must never depend on infrastructure -- the dock lives on a
// moving vehicle and a field has no SSID to join. So the SoftAP stays up
// unconditionally and station mode is strictly additive: joining a hotspot
// lets the operator's phone keep its cellular data, and lets the dock reach a
// depot network later, but losing it can never make the dock unreachable.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#define WIFI_AP_SSID      "drone-in-a-box"
#define WIFI_AP_PASSWORD  "liftmotor"       // >= 8 chars, or the AP comes up open
#define WIFI_AP_CHANNEL   1
#define WIFI_AP_MAX_CONN  4

#define WIFI_HOSTNAME     "dock"            // so the app can use dock.local

// Reconnect backoff: fast at first so a brief glitch recovers quickly, then
// slow so a hotspot that is simply switched off does not cost power forever.
#define WIFI_RETRY_MIN_MS  1000
#define WIFI_RETRY_MAX_MS  30000

void wifi_link_start(void);

// Re-read credentials from `secrets` and rejoin, without a reboot. Called
// after provisioning over serial, and later over BLE.
void wifi_link_reprovision(void);

bool wifi_link_station_joined(void);

// "192.168.1.47" when joined, or "-" when not. Points into static storage.
const char *wifi_link_station_ip(void);

// ---------------------------------------------------------------------------
// Scanning (so the operator can pick a network instead of spelling one out)
// ---------------------------------------------------------------------------
// A station scan is not free here, and both costs are the caller's problem:
//
//   1. The radio hops channels to listen, so every client of our own SoftAP --
//      including the browser serving the UI -- loses the link for the duration.
//      The AP comes back by itself; nothing is torn down.
//   2. It blocks for roughly WIFI_SCAN_BUDGET_MS, which is longer than the
//      dead-man timeout in `session`. Never scan while the actuator is
//      energised: the watchdog would stop the motor mid-move.
//
// Neither can be fixed here -- one radio cannot both hold a channel and sweep
// them -- so they are documented rather than hidden.

#define WIFI_SCAN_MAX_APS     16        // bounds the JSON the web API builds
#define WIFI_SSID_TEXT_MAX    33        // 32 bytes + NUL, as 802.11 allows

// Roughly what a sweep costs: 14 channels at the dwell times set in
// wifi_link.c, which deliberately include a window back on our own channel so
// SoftAP clients are not blacked out for the whole sweep.
#define WIFI_SCAN_BUDGET_MS   3000

typedef struct {
    char    ssid[WIFI_SSID_TEXT_MAX];   // NUL-terminated; arbitrary bytes
    int8_t  rssi;                       // dBm, negative, closer to 0 is stronger
    bool    open;                       // joinable with no passphrase
    uint8_t authmode;                   // a wifi_auth_mode_t, widened
} wifi_link_ap_t;

// Sweep for access points and write at most `cap` of them to `out`, strongest
// first, one entry per SSID. Hidden networks are skipped: they announce no
// SSID, so there would be nothing to show or to join by name.
//
// Blocks for about WIFI_SCAN_BUDGET_MS. Returns ESP_ERR_INVALID_STATE if the
// radio is not up yet or a scan is already running, and *found is 0 on every
// error.
esp_err_t wifi_link_scan(wifi_link_ap_t *out, size_t cap, size_t *found);

// "open", "WPA2-PSK", ... for a wifi_link_ap_t's authmode. Always a printable
// ASCII literal, so callers may drop it into JSON unescaped.
const char *wifi_link_auth_name(uint8_t authmode);
