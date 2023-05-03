#!/bin/sh

cd /home/ivm/pdbi-collector
./run_collector.sh pdune_subscriptions >/tmp/collector.out 2>/tmp/collector.err </dev/null &
