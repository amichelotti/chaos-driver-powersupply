#include <stdio.h>
#include <stdlib.h>
#include "driver/powersupply/core/ChaosUIPowersupplyCWrapper.h"

int main (int argc, char* argv[] )
{
  int err = 0;
  unsigned int devID;
  const char *mdsServer = argv[1];
  const char *devName = argv[2];
  double curr;
  
    
  if(argc!=3){
    printf("Usage is %s <MDS address> <CUname>\n",argv[0]);
    return -1;
  }
  printf("using as MDS %s and CU %s\n",mdsServer,devName);
  err = initPowerSupply(mdsServer,devName,&devID);
  printf("err init %d\n", err);
  if(err!=0){
    printf("cannot initialise \"%s\" on %s, err =%d\n",devName,mdsServer,err);
    return -3;
  }

  setPol(devID, 1);
  on(devID);
  setCurrent(devID, 100.0);
  setCurrent(devID, 50.0);
  setCurrent(devID, 15.0);
  setCurrent(devID, 0.0);
  printf("initialisation ok\n");
 // while(1){
  //  if(getCurrent(devID,&curr)==0){
   //   printf("current %2.2f\r",curr);
   // }
 // }
  return 0;
}

