SYSTEM_HEADER_PROJECTS="libc kernel"
PROJECTS="libc kernel"

export MAKE=${MAKE:-make}
export HOST=${HOST:-$(./default-host.sh)}

if [ -z "$AR" ]; then
    export AR=${HOST}-ar
fi

export AS=${HOST}-as

if [ -z "$CC" ]; then
    export CC=${HOST}-gcc
fi

if [ -z "$OBJCOPY" ]; then
    export OBJCOPY=objcopy
fi

if [ -z "$OBJCOPY_OUT_FMT" ]; then
    export OBJCOPY_OUT_FMT="elf32-i386"
fi

if [ -z "$OBJCOPY_ARCH" ]; then
    export OBJCOPY_ARCH="i386"
fi

export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export BOOTDIR=/boot
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

if [ -z "$OPTLEVEL" ]; then
    export OPTLEVEL=0
fi

export CFLAGS="-O${OPTLEVEL}"

if [ -z "$RELEASE" ]; then
    export CFLAGS="${CFLAGS} -g"
fi

export CPPFLAGS=''

# Configure the cross-compiler to use the desired system root.
export SYSROOT="$(pwd)/sysroot"
export CC="$CC --sysroot=$SYSROOT"

# Work around that the -elf gcc targets doesn't have a system include directory
# because it was configured with --without-headers rather than --with-sysroot.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi
