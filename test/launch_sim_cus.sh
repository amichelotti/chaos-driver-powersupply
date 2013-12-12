#!/bin/sh
echo "Using metadata server $1"
powersupply_cu --log-level debug --log-on-console --metadata-server $1:5000 --sc_ps_id SQUATB004 --sc_driver SimPSupply:/dev/ttyr12,4,100000:200000,200000:300000,100:10 --sc_ps_id SQUATB003 --sc_driver SimPSupply:/dev/ttyr12,5,100000:200000,200000:300000,100:10 --sc_ps_id SDHSTB002 --sc_driver SimPSupply:/dev/ttyr12,6,100000:200000,200000:300000,100:10 >& cu1.log &
sleep 1
powersupply_cu --log-level debug --log-on-console --metadata-server $1:5000 --sc_ps_id SQUATB002 --sc_driver SimPSupply:/dev/ttyr14,11,100000:200000,200000:300000,100:10 --sc_ps_id SQUATB001 --sc_driver SimPSupply:/dev/ttyr14,9,100000:200000,200000:300000,100:10 --sc_ps_id SQUATB101 --sc_driver SimPSupply:/dev/ttyr14,3,100000:200000,200000:300000,100:10 --sc_ps_id SQUATB102 --sc_driver SimPSupply:/dev/ttyr14,0,100000:200000,200000:300000,100:10 --sc_ps_id SDHSTB001 --sc_driver SimPSupply:/dev/ttyr14,4,100000:200000,200000:300000,100:10 >& cu2.log &
