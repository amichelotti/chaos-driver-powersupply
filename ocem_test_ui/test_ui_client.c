#include <stdio.h>
#include <stdlib.h>
#include <chaos/ui_toolkit/ChaosUIToolkitCWrapper.h>

int main (int argc, char* argv[] )
{

  int err = 0;
  char tmpInitString[256];
  uint32_t devID;
  const char *mdsServer = argv[1];
  const char *devName = argv[2];
  const char *attributeName = argv[3];
  char * attributeValue = NULL;
    
  if(argc!=4){
    printf("Usage is %s <MDS address> <CUname> <attribute name>\n",argv[0]);
    return -1;
  }
  printf("using as MDS %s and CU %s\n",mdsServer,devName);
  //create init string
  //\nlog-on-console=true
  sprintf(tmpInitString, "metadata-server=%s", mdsServer);
    
  //init the toolkit
  err = initToolkit(tmpInitString);
  if (err != 0) {
    printf("Error initToolkit %d\n", err);
    return -1;
  }
    
  err = getNewControllerForDeviceID(devName, &devID);
  if (err != 0) {
    printf("Error getNewControllerForDeviceID %d\n", err);
    return -1;
  }
    
  err = setControllerTimeout(devID, 10000);
  if (err != 0) {
    printf("Error setting timeout %d\n", err);
    return -1;
  }
    
  printf("Init device 0x%x\n",devID);
  err = initDevice(devID);
  if (err != 0) {
    printf("Error initDevice %d\n", err);
    return -1;
  }
  printf("Starting device 0x%x\n",devID);
  err = startDevice(devID);
  if (err != 0) {
    printf("Error startDevice %d\n", err);
    return -1;
  }
    
  printf("Fetching device 0x%x\n",devID);
  err = fetchLiveData(devID);
  if (err != 0) {
    printf("Error fetchLiveData %d\n", err);
    return -1;
  }
      
  err = getStrValueForAttribute(devID, attributeName, &attributeValue);
  if (err != 0) {
    printf("Error getStrValueForAttribute %d for value \"%s\"\n", err, attributeName);
  }else {
    if(attributeValue) {
      printf("Got the value for %s with value %s\n", attributeName, attributeValue);
      free(attributeValue);
    }
  }
      
 
  // send on
  // send setcurrent
  sleep(1);
  // send standby
  printf("sending standby\n");
  err = submitSlowControlCommand(devID,"{\"mode_type\":0}",
				 1,
				 50,
				 0,
				 0,
				 0 ,
				 0);
  if(err!=0){
    printf("## error submitting commands\n");
    return -1;
  }
  sleep(1);
  // send standby
  printf("sending on\n");
  err = submitSlowControlCommand(devID,"{\"mode_type\":1}",
				 1,
				 50,
				 0,
				 0,
				 0 ,
				 0);
  if(err!=0){
    printf("## error submitting commands\n");
    return -1;
  }
  
  err = deinitToolkit();
  if (err != 0) {
    printf("Error deinitToolkit %d", err);
    return -1;
  }
  return 0;
}

