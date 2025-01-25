/**
 * @brief   5-card-stud
 * @author  Eric Carr, Thomas Cherryhomes, (insert names here)
 * @license gpl v. 3
 * @verbose main
 */

#ifdef _CMOC_VERSION_
#include "coco/coco_bool.h"
#include <cmoc.h>
#else
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#endif /* _CMOC_VERSION_ */
#include "vars.h"
#include "fujinet-network.h"

// Store default server endpoint in case lobby did not set app key
unsigned char serverEndpoint[50] = "N:TCP://136.53.106.114:21000/"; // "N: for apple, but not C64"

unsigned char query[50] = "";

unsigned char recv[128];

unsigned char kbhit (void) { 
  return (unsigned char)inkey();
}

char cgetc (void) {
  return (char)waitkey(0);
}

bool IsValidChar(unsigned char c)
{
  switch(c)
  {
    case '/':
    case '>':
    case '@':
    case ':':
    case '#':
    case '$':
    case '%':
    case ' ':
      return true;
  }
  return false;
}

void
SendCommand(unsigned char *cmd)
{
  network_write(serverEndpoint, cmd, strlen((const char *)cmd));
  network_write(serverEndpoint, (unsigned char*)"\n", 1);
}


#ifdef _CMOC_VERSION_
int main(void)
#else
void main(void)
#endif /* _CMOC_VERSION_ */
{ 
  bool bExited = false;
  char inputKey;
#define MAX_SEND 63
  char send[MAX_SEND+1];
  char sendlen = 0;
  int16_t result;
  int16_t reducePollInterval = 0;
  unsigned char *s;


  memset(send,0,sizeof(send));

#ifdef _CMOC_VERSION_
  network_init();
  network_open(serverEndpoint,12,0);
  
#endif

  printf("USERNAME? \n");
  s = readline();

  sprintf(query, "/login @%s", s);

  SendCommand(query);
  SendCommand((unsigned char *)"/join #main");

  printf("TYPE /EXIT TO QUIT\n");

  // Main in-game loop
  while (!bExited)
  {  
    if (kbhit())
    {
      // Handle other key presses
      //inputKey = cgetc();
      s = readline();
      if (0==strcmp((const char *)s, "/EXIT"))      
      {
            bExited = true;
      }
      else
      {
        network_write(serverEndpoint, (unsigned char *)"#main ", 6);
        network_write(serverEndpoint, s, strlen((const char *)s));
        network_write(serverEndpoint, (unsigned char*)"\n", 1);
        printf("\n");
      }
    }


    if ((reducePollInterval++)%50==0)
    {
      memset(recv, 0, sizeof(recv));
      result = network_read_nb(serverEndpoint, recv, sizeof(recv));
      if (result>0)
      {
        printf("%s\n", recv);
        printf("%s", send);
      }
    }
  }

#ifdef _CMOC_VERSION_
  network_close(serverEndpoint);
  return 0;
#endif /* CMOC_VERSION_  */
}
