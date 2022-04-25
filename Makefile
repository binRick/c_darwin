CC=$(shell command -v gcc)
PASSH=$(shell command -v passh)
BAT=$(shell command -v bat) --style=plain --force-colorization --theme="Monokai Extended Origin"

INSTALLDIR=/usr/local/bin
BIN=bin
LOGS=./logs

WEBSOCKET_PORT=8088

CC_OPTIONS=
CC_OPTIONS+=-O0
CC_OPTIONS+=-std=c99
CC_OPTIONS+=-Wdeprecated-declarations
#CC_OPTIONS+=-Werror
#CC_OPTIONS+=-Wall
CC_OPTIONS+=-Wextra

FRAMEWORKS=
FRAMEWORKS+=-framework ApplicationServices
FRAMEWORKS+=-framework Carbon
FRAMEWORKS+=-framework Foundation
FRAMEWORKS+=-framework AppKit 
FRAMEWORKS+=-framework IOKit
FRAMEWORKS+=-framework CoreFoundation

INCLUDE_PATHS=-I/usr/local/include
INCLUDE_PATHS+=-I../bash-loadable-wireguard/src
INCLUDE_PATHS+=-I./deps
INCLUDE_PATHS+=-I./include
INCLUDE_PATHS+=-I./src
INCLUDE_PATHS+=-I./include/fs
INCLUDE_PATHS+=-I./include/string
INCLUDE_PATHS+=-I./c_scriptexec/include
INCLUDE_PATHS+=-I./c_scriptexec/src
INCLUDE_PATHS+=-I./deps/c_fsio/include
INCLUDE_PATHS+=-I./deps/c_scriptexec/include
INCLUDE_PATHS+=-I./deps/c_string_buffer/include
INCLUDE_PATHS+=-I./deps/libconfuse/include
INCLUDE_PATHS+=-I./deps/uptime/include

LIBRARY_PATHS=-L/usr/local/lib
LIBRARY_PATHS+=$(shell pkg-config libconfuse --libs --cflags)

LINKED_LIBRARIES=-lparson
LINKED_LIBRARIES+=-luv
LINKED_LIBRARIES+=-lpthread
LINKED_LIBRARIES+=-lm
LINKED_LIBRARIES+=-lcurl
LINKED_LIBRARIES+=-lsqlite3

SOURCES=src/keylogger.c
EXECUTABLE=keylogger
PLIST=keylogger.plist

CFLAGS=$(FRAMEWORKS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKED_LIBRARIES)  -g
CC_CMD=$(CC) \
	    $(CC_OPTIONS) \
		$(CFLAGS) \
		$(SOURCES) \
		-o $(BIN)/$(EXECUTABLE)

DEPS = \
	   deps-clib deps-c_fsio deps-c_string_buffer deps-confuse deps-c_scriptexec deps-httpserver \
	   deps-uv deps-timequick \
	   deps-parson \
	   deps-uptime \
	   deps-wslay \
	   deps-nettle \
	   deps-tercontrol \
	   deps-str-truncate

websocket: cc-websocket websocket-server

websocket-client:
	@ws ws://127.0.0.1:8088

cc-websocket:
	@[[ -f bin/websocket-server ]] && unlink bin/websocket-server
	@$(CC) -Wall -O2 -g -o $(BIN)/websocket-server src/websocket-server.c -L./deps/wslay/lib/.libs -I./deps/wslay/lib/includes -lwslay -lnettle

websocket-server:
	@$(BIN)/websocket-server $(WEBSOCKET_PORT)

cc:
	@$(PASSH) $(CC_CMD)

dev:
	@$(PASSH) -L logs/dev.log ./bin/dev.sh

cc-dev:
	@ansi --yellow --bg-black --italic "$(CC_CMD)"

cc-bat: cc-dev
	@make cc || { make cc | $(BAT); }
	@$(PASSH) -L $(LOGS)/$(EXECUTABLE).log $(BIN)/$(EXECUTABLE)

all: cc-bat

init:
	@mkdir -p deps bin

setup: init deps-install

install:
	mkdir -p $(INSTALLDIR)
	cp $(EXECUTABLE) $(INSTALLDIR)

uninstall:
	rm $(INSTALLDIR)/$(EXECUTABLE)
	rm /Library/LaunchDaemons/$(PLIST)

startup:
	cp $(PLIST) /Library/LaunchDaemons

clean: rm init

rm:
	rm -rf deps bin

tidy:
	./bin/tidy.sh

deps-tercontrol:
	@[[ -d deps/tercontrol ]] || git clone \
			https://github.com/ZackeryRSmith/tercontrol \
			deps/tercontrol

deps-parson:
	@[[ -d deps/parson ]] || git clone \
			https://github.com/kgabis/parson \
			deps/parson

deps-timequick:
	@[[ -d deps/timequick ]] || git clone \
			https://github.com/mbarbar/timequick \
			deps/timequick

deps-str-truncate:
	@[[ -d deps/str-truncate.c ]] || git clone \
			https://github.com/stephenmathieson/str-truncate.c \
			deps/str-truncate.c
deps-uv:
	@[[ -d deps/libuv ]] || git clone \
			https://github.com/libuv/libuv \
			deps/libuv
	@cd deps/libuv && cmake .
	@cd deps/libuv && make

deps-httpserver:
	@[[ -d deps/httpserver.h ]] || git clone https://github.com/jeremycw/httpserver.h deps/httpserver.h

deps-c_scriptexec:
	@[[ -d deps/c_scriptexec ]] || git clone https://github.com/sagiegurari/c_scriptexec deps/c_scriptexec

deps-confuse:
	@[[ -d deps/libconfuse ]] || git clone https://github.com/libconfuse/libconfuse deps/libconfuse

deps-clib:
	@clib install 

deps-c_string_buffer:
	@[[ -d deps/c_string_buffer ]] || git clone https://github.com/sagiegurari/c_string_buffer deps/c_string_buffer

deps-nettle:
	@[[ -d deps/nettle ]] || git clone \
			https://github.com/breadwallet/nettle \
			deps/nettle
	@cd deps/wslay && autoreconf -i && ./configure && make -j && make check

deps-uptime:
	@[[ -d deps/uptime ]] || git clone \
			https://github.com/qwercik/uptime \
			deps/uptime
deps-wslay:
	@[[ -d deps/wslay ]] || git clone \
			https://github.com/tatsuhiro-t/wslay \
			deps/wslay
	@cd deps/wslay && cmake .
	@cd deps/wslay && make -j


deps-c_fsio:
	@[[ -d deps/c_fsio ]] || git clone https://github.com/sagiegurari/c_fsio deps/c_fsio
	@cd deps/c_fsio && ./build.sh


commit:
	@git commit -am 'Automated Commit'

push:
	@git push

git: tidy commit push

deps-install: $(DEPS)

webserver-test:
	@curl -s http://localhost:8085/t

