#!/bin/sh
TMPDIR="$XDG_RUNTIME_DIR"
DIR="$TMPDIR/pip"

function run() {
    env PYTHONPATH=$DIR/lib/python3.7/site-packages $DIR/bin/gcovr "$@"
}

if [ -f "$DIR/bin/gcovr" ];
then
    run "$@"
else
    mkdir -p $DIR
    PYTHONUSERBASE=$DIR pip install --user gcovr
    run "$@"
fi
