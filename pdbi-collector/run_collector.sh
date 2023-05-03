#!/bin/sh

DIPBASE=$HOME/dip-5.6.0
export LD_LIBRARY_PATH=$DIPBASE/lib64:$LD_LIBRARY_PATH
export DIPNS=dipnsgpn1

cd ${HOME}/pdbi-collector
exec ./dip-client tmp `cat $1`
