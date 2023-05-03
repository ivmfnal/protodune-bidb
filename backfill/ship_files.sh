#!/bin/sh

storage=`pwd`/out
target="dbiapp@dbweb5.fnal.gov:ifbeam/storage/cern_long"

export KRB5CCNAME=/dev/shm/krb.cc

kinit -k \
	-t /home/ivm/pdbi-collector/collector.kt \
	-c $KRB5CCNAME \
	pdbidb_data/dune/protodune-bidb-dev.cern.ch@FNAL.GOV

find $storage -type f -name "*.data" -print \
	-exec rsync -e "ssh -k" -t {} $target \; \
	-delete 
