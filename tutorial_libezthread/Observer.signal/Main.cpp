#include <stdio.h>
#include <stdlib.h>
 
 #include "FunUsbStorageKey.h"

CFunUsbStorageKey __withCallback_bob("bob");
CFunUsbStorageKey __withCallback_tom("tom");


int main(int argc, char *argv[])
{
   printf("Hello World\n");

        __withCallback_bob.Start();
        sleep(3);
        __withCallback_tom.Start();

        sleep(5);

        __withCallback_bob.Stop();
        sleep(3);
        __withCallback_tom.Stop();
   
   return 0;
}
