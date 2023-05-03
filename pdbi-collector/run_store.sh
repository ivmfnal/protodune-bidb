#!/bin/sh

export PATH=/home/ivm/anaconda2/bin:$PATH
cd ${HOME}/pdbi-collector
python store.py "dbname=protodune_bi_dev" "tmp/*.closed"
