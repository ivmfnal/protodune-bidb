#!/bin/sh

cd /home/ivm/pdbi-collector
kinit -k -t collector.kt -c /dev/shm/krb.cc pdbidb_data/dune/protodune-bidb-dev.cern.ch@FNAL.GOV
