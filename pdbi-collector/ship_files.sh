#!/bin/sh

storage=$1
target=$2

export KRB5CCNAME=/dev/shm/krb.cc

if [ -z "$target" ]; then
    echo 'Usage: export_rsync.sh <source dir> <target user@host:/dir>'
    exit 1
fi

while true; do
    find $storage -type f -name "*.closed" -exec gzip {} \; 2>>/tmp/ship_files.err
    find $storage -type f -name "*.closed.gz" -exec rsync -e "ssh -k" -t {} $target \; -delete 2>>/tmp/ship_files.err
    sleep 20
done
