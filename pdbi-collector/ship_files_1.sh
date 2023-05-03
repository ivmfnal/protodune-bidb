#!/bin/sh

storage=$1
target=$2

export KRB5CCNAME=/home/ivm/pdbi-collector/krb.cc

if [ -z "$target" ]; then
    echo 'Usage: export_rsync.sh <source dir> <target user@host:/dir>'
    exit 1
fi

cd $storage

while true; do
    list=`find -type f -name "*.closed"`
    if [ ! -z "${list}" ]; then
        for f in $list ; do
		rsync -e "ssh -k" -t ${f} $target
		status=$?
		if [ "$status" = "0" ]
		then
				echo Copied $f
				rm ${f}
		else
			echo "rsync failed status: $status -- $f"
		fi
        done
    fi
    sleep 20
done
