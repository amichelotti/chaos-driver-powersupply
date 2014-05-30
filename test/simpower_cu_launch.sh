#!/bin/sh
if [ ! -n "$1" ]; then 
echo "## you must specify a valid metadataserver"
exit 1
fi
CUNAME="simpower"
if [ -n "$2" ]; then 
CUNAME=$2
fi

echo "Using metadata server \"$1\", cu name \"$CUNAME\""
powersupply_cu --log-level debug --log-on-console --metadata-server $1:5000 --sc_ps_id $CUNAME --sc_driver SimPSupply:/dev/ttyr12,4,100000:200000,200000:300000,100:10 
