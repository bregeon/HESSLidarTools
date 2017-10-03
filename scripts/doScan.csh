#!/bin/env tcsh
source /data/Hess/work/pro/setup.csh
set i=0
while ( $i <= 20 )
  echo $i
  root -q 'acScan.C("/data/Hess/data/root_files/run_067220_Lidar_001.root", '$i')'
  @ i++
end
