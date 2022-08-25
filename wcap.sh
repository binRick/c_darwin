#!/usr/bin/env bash
set -eou pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CAPTURE_INTERVAL=1
ARCHIVE_INTERVAL=${ARCHIVE:-5}
DEBUG_MODE=${DEBUG:-0}
PACK_SIZE_MB=1
COMPRESSION=max
KFC_PROFILE=sexy_colorfulcolors
ARCHIVE_DIR=~/.wcap.db
STORAGE_DIR=/tmp/.wcap
ARCHIVE_PASSWORD=28D0BDB0-9133-49EC-BCEF-6040F0DF2F3C
CAPTURE_IMAGES_BIN="$(pwd)/build-muon/wrec-capture-test/wrec-capture-test"
APP_TITLE=wcap

init_env(){
  export PATH="/usr/local/bin:$PATH" \
    RESTIC_REPOSITORY="$ARCHIVE_DIR" \
    RESTIC_PASSWORD="$ARCHIVE_PASSWORD" \
    RESTIC_PACK_SIZE="$PACK_SIZE_MB" \
    RESTIC_COMPRESSION="$COMPRESSION"
}

init_storage_dir(){
  [[ -d "$STORAGE_DIR" ]] || $MKDIRP_CMD "$STORAGE_DIR"
  true
}

init_archive(){
  [[ -d "$ARCHIVE_DIR" ]] || eval "$ARCHIVE_INIT_CMD"
  true
}

init_cmds(){
  CMD_RM="$(command -v rm)"
  CMD_MKDIR="$(command -v mkdir)"
  CMD_PV="$(command -v pv)"
  CMD_FIND="$(command -v FIND)"
  CMD_DATE="$(command -v date)"
  CMD_RESTIC="$(command -v restic)"
  CMD_VTERMCTRL="$(command -v vterm-ctrl)"
  CMD_KFC="$(command -v kfc)"
  CMD_ANSI="$(command -v ansi)"

	PV_CMD="$CMD_PV --name '$($CMD_ANSI -n --yellow Capture Rate)' -r -f -c -l"
	RMRF_CMD="$CMD_RM -rf"
  MKDIRP_CMD="$CMD_MKDIR -p"
  ARCHIVE_INIT_CMD="$CMD_RESTIC init -q"
  ARCHIVE_BACKUP_CMD="$CMD_RESTIC backup $STORAGE_DIR -q"
}

init_vars(){
  START_TS="$($CMD_DATE +%s)"
  CAPTURED_QTY=0
  CAPTURED_BYTES=0
  ARCHIVED_QTY=0
  ARCHIVED_BYTES=0
  START_MSG_CONFIG="$(echo -e "\n\t|Capture Interval:$($CMD_ANSI -n --blue $CAPTURE_INTERVAL)\
    \n\t|PID\t\t\t\t:\t$($CMD_ANSI -n --blue $$)\
    \n\t|Archive Interval\t\t:\t$($CMD_ANSI -n --blue $ARCHIVE_INTERVAL)\
    \n\t|Capture Path\t\t\t:\t$($CMD_ANSI -n --blue $STORAGE_DIR)\
    \n\t|Archive Path\t\t\t:\t$($CMD_ANSI -n --blue $RESTIC_REPOSITORY)")"
  START_MSG="$($CMD_ANSI -n --green --bold "Window Capture")$START_MSG_CONFIG"
}

update_title(){
  local title="$APP_TITLE"
  if [[ "$ARCHIVED_QTY" -gt 0 && "$ARCHIVED_BYTES" -gt 0 ]]; then 
    local dur="$(($($CMD_DATE +%s)-$START_TS))"
    ARCHIVED_RATE="$(($ARCHIVED_QTY/$dur))"
    ARCHIVED_SIZE="${ARCHIVED_BYTES}B"
    ARCHIVED_BYTES_RATE="$(($ARCHIVED_BYTES/$dur))"
    ARCHIVED_SIZE_RATE="$ARCHIVED_BYTES"
    if [[ "$ARCHIVED_BYTES" -gt $((1024*1024*1024)) ]]; then
      ARCHIVED_SIZE="$(($ARCHIVED_BYTES/1024/1024/1024))GB"
      ARCHIVED_SIZE_RATE="$(($ARCHIVED_BYTES_RATE/1024/1024/1024))GB"
    elif [[ "$ARCHIVED_BYTES" -gt $((1024*1024)) ]]; then 
      ARCHIVED_SIZE="$(($ARCHIVED_BYTES/1024/1024))MB"
      ARCHIVED_SIZE_RATE="$(($ARCHIVED_BYTES_RATE/1024/1024))MB"
    elif [[ "$ARCHIVED_BYTES" -gt 1024 ]]; then
      ARCHIVED_SIZE="$(($ARCHIVED_BYTES/1024))KB"
      ARCHIVED_SIZE_RATE="$(($ARCHIVED_BYTES_RATE/1024))KB"
    fi
    title+=" [$ARCHIVED_QTY@$ARCHIVED_RATE/s|$ARCHIVED_SIZE@$ARCHIVED_SIZE_RATE/s]"
  fi
  $CMD_VTERMCTRL title "$title"
}

init() {
  init_cmds
	$CMD_ANSI --save-palette
  $CMD_VTERMCTRL altscreen on
  $CMD_ANSI -n --reset
	$CMD_KFC -p "$KFC_PROFILE" 2>/dev/null
  $CMD_ANSI --reset
	$CMD_VTERMCTRL cursor off
  init_env
  init_vars
  init_storage_dir
	init_archive
  update_title
	trap shutdown EXIT
}

shutdown() {
  [[ "$DEBUG_MODE" == 0 ]] && [[ -d "$STORAGE_DIR" ]] && $RMRF_CMD "$STORAGE_DIR"
  $CMD_VTERMCTRL title ""
	$CMD_VTERMCTRL cursor on
  $CMD_VTERMCTRL altscreen off
	$CMD_ANSI --restore-palette
}

purge_storage_dir() {
	[[ -d "$STORAGE_DIR" ]] && $RMRF_CMD "$STORAGE_DIR"
	init_storage_dir
  init_archive
}

storage_dir_report(){
    local captured_qty="$($CMD_FIND "$STORAGE_DIR" -type f -name "*.png"|wc -l)"
    local captured_bytes="$(du -cb $($CMD_FIND $STORAGE_DIR -type f -name "*.png")|grep total|tr -s ' '|tr -s '[[:space:]]' '\n'|head -n1)"

		CAPTURED_QTY="$(($CAPTURED_QTY + $captured_qty))"
    CAPTURED_BYTES="$(($CAPTURED_BYTES+$captured_bytes))"

		ARCHIVED_QTY="$(($ARCHIVED_QTY + $CAPTURED_QTY))"
		ARCHIVED_BYTES="$(($ARCHIVED_QTY + $CAPTURED_BYTES))"
}

archive_storage_dir() {
  init_archive
	$ARCHIVE_BACKUP_CMD
}


capture_loop(){
		init_archive
		eval "$CAPTURE_IMAGES_BIN" 2>&1 | grep 'png$' | eval "$PV_CMD" >/dev/null
		interval="$(($interval + 1))"

		sleep $CAPTURE_INTERVAL
}

main_loop(){
		interval=0
		while [[ "$interval" -lt $ARCHIVE_INTERVAL ]]; do capture_loop; done
    $CMD_ANSI -n --up=$interval --delete-lines=$interval
    storage_dir_report
		archive_storage_dir
		purge_storage_dir
    update_title
}

main() {
	init
  printf "%s\n" "$START_MSG"
	while :; do main_loop; done
}

main
