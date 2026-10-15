#!/bin/bash
# Log whether the keyboard is awake, to tell a flat battery apart from a board
# that will not stay asleep.
#
#   ./tools/battery-watch.sh [ble-name] [logfile]
#
# A keyboard in deep sleep has its radio off, so the host sees no device. This
# samples that every 30 s. Two transitions over a night -- one to sleep, one
# when you wake it -- means the firmware is behaving and any charge lost is the
# cell's doing. Extra transitions mean something is waking the board, which on
# a keyboard means the matrix: a marginal switch or a short.
#
# macOS only; it reads the IORegistry. Run it under caffeinate, or the Mac
# sleeps, sampling stops and the night is wasted:
#
#   nohup caffeinate -is ./tools/battery-watch.sh >/dev/null 2>&1 &
#
# Gaps over ~90 s in the timestamps mean it slept anyway and that stretch is
# void. Note the battery percentage when you start and before you touch the
# keyboard afterwards -- waking it costs a reconnect and skews the reading.
#
# Stop it with `kill <pid>`, taking the pid from `ps`. Do not reach for
# `pkill -f battery-watch`, which matches the command running the pattern.
NAME="${1:-Unix60-1}"
LOG="${2:-$HOME/Library/Logs/unix60-battery-watch.log}"

present() { ioreg -r -c IOHIDDevice -d 1 2>/dev/null | grep -c "\"Product\" = \"$NAME\""; }

printf '=== started %s  device=%s  (1=awake, 0=asleep) ===\n' "$(date '+%F %T')" "$NAME" >> "$LOG"
LAST=""
while true; do
  P=$(present); TS=$(date '+%F %T')
  if [ -n "$LAST" ] && [ "$P" != "$LAST" ]; then
    printf '%s  state=%s   <<< CHANGE\n' "$TS" "$P" >> "$LOG"
  else
    printf '%s  state=%s\n' "$TS" "$P" >> "$LOG"
  fi
  LAST=$P
  sleep 30
done
