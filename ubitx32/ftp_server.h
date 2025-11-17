
#pragma once

//#include <FTPServer.h>
#include <MultiFTPServer.h>

void ftp_task(void *parameter)
{
  FtpServer ftpSrv;  
  ftpSrv.begin("admin","admin"); 
  while(1) {
    ftpSrv.handleFTP();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }  
}

