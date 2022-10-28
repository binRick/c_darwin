FROM archlinux:latest
RUN pacman -T --noconfirm
RUN pacman -Sy --noconfirm
RUN pacman -Fy --noconfirm
RUN pacman -S --noconfirm \
  meson automake cmake gcc clang make \
  python3 ansible \
  fish zsh git openssh rsync \
  ansible \
  clang distcc clang13 \
  bash gcc-objc sudo python-pip which \
  bat ripgrep jq restic
RUN pip3 install json2yaml
RUN command -v json2yaml
RUN command -v yaml2json
RUN mkdir -p /Users/rick/repos
RUN env n=passh sh -c "\
  command git clone https://github.com/binRick/\$n /Users/rick/repos/\$n && cd /Users/rick/repos/passh && make && cp passh /bin/passh;\
"
RUN env n=c_vector sh -c "\
  command git clone https://github.com/binRick/\$n /Users/rick/repos/\$n;\
"
RUN env n=c_ansi sh -c "\
  command git clone https://github.com/binRick/\$n /Users/rick/repos/\$n;\
"
RUN env n=c_deps sh -c "\
  command git clone https://github.com/binRick/\$n /Users/rick/repos/\$n;\
"
RUN env n=c_db sh -c "\
  command git clone https://github.com/binRick/\$n /Users/rick/repos/\$n;\
"
RUN env n=c_greatest sh -c "\
  command git clone https://github.com/binRick/\$n /Users/rick/repos/\$n;\
"
RUN sh -c "find */Users/rick/repos/*/submodules -maxdepth 0 -type d -name submodules|grep -v c_deps \
  |xargs -I % echo '\
#!/usr/bin/env bash\
  set -x\
  && mv % %.backup \
  && ln -sf /c_deps/submodules % \
  && ln -sf /c_deps %/c_deps \
  && ln -sf /c_ansi %/c_ansi\
  ' > /root/setup.sh && chmod 700 /root/setup.sh && echo /root/setup.sh >> /root/.bashrc"
#RUN env RESTIC_PASSWORD=123123 restic list snapshots -r /.restic.db
#RUN env RESTIC_PASSWORD=123123 restic --target /Users/rick/c_deps/submodules -r /.restic.db
#RUN sh -c "cd /Users/rick/repos/c_deps && env CC=clang OBJC=clang CXX=clang++ OBJCXX=clang++ meson setup build"
