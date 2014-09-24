#!/bin/sh
if [ -z "$1" ]; then
echo "usage is $0 <metadataserver>"
exit 1
fi


echo "Using metadata server $1 log into powersupply-`hostname`.log"

powersupply_cu --metadata-server $1:5000 --log-on-console --unit_server_alias POWERSUPPLY-`hostname` --unit_server_enable true >& powersupply-`hostname`.log &



