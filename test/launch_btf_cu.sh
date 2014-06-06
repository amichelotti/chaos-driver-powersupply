#!/bin/sh
 ocem_cu --log-level debug --log-on-console --metadata-server 192.168.143.21:5000 --sc_ocem_id QUATB004 --sc_driver OcemE642X:/dev/ttyr12,4 --sc_ocem_id QUATB003 --sc_driver OcemE642X:/dev/ttyr12,5 --sc_ocem_id DHSTB002 --sc_driver OcemE642X:/dev/ttyr12,6 >& cu1.log &

ocem_cu --log-level debug --log-on-console --metadata-server 192.168.143.21:5000 --sc_ocem_id QUATB002 --sc_driver OcemE642X:/dev/ttyr14,11 --sc_ocem_id QUATB001 --sc_driver OcemE642X:/dev/ttyr14,9 --sc_ocem_id QUATB101 --sc_driver OcemE642X:/dev/ttyr14,3 --sc_ocem_id QUATB102 --sc_driver OcemE642X:/dev/ttyr14,0 --sc_ocem_id DHSTB001 --sc_driver OcemE642X:/dev/ttyr14,4 >& cu2.log &
