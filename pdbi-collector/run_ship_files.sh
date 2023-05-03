#!/bin/sh

TOPDIR=/home/ivm/pdbi-collector

cd $TOPDIR
./renew_credentials.sh > /tmp/credentials.out 2>& 1
exec ./ship_files.sh tmp dbiapp@dbweb5.fnal.gov:ifbeam/storage/cern_long &
