/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 *  See the README file for copyright information and how to reach the author.
 */

// for asprintf
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>

#define MAXRECVBUFF 1024

int sockfd = 0;
char recvBuff[MAXRECVBUFF+1];
struct sockaddr_in serv_addr; 

char *lightpack_version;
int   lightpack_count_leds;


int lightpack_read(char *buf, int len)
{
    int n = 0;
    n = recv(sockfd, buf, len, 0);
    if( n < 0 ) {
        if( errno == EAGAIN || errno == EWOULDBLOCK )
            return -1;
        else
            return 0;
    }
    return n;
}

bool lightpack_write(char *buf, int len)
{
    int n = 0;
    n = write(sockfd, buf, len);
    if( n < 0 ) {
        return false;
    }
    return true;
}

bool lightpack_writestring(char *str)
{
    return lightpack_write(str, strlen(str));
}

bool lightpack_init(void)
{
    memset(recvBuff, '0',sizeof(recvBuff));
    return true;
}

bool lightpack_connect(const char *host, int port)
{
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        return false;
    
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); 

    if( inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0 )
        return false;

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 )
        return false;

    int n = lightpack_read(recvBuff, MAXRECVBUFF);
    if( n <= 0 )
        return false;

    // Put the socket in non-blocking mode:
    //if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK) < 0) {
    //    return false;
    //}

    return true;
}

void lightpack_disconnect(void)
{
    char *buf = "exit\n";

    lightpack_writestring(buf);
    close(sockfd);
}

// 0 sucess login
// 1 apikey false
// 2 socket error
//
int lightpack_login(const char *apikey)
{
    char *buf;
    int n = asprintf(&buf, "apikey:%s\n", apikey);

    if( n == -1 )
        return 2;

    if( lightpack_writestring(buf) == false) {
        free(buf);
        return 2;
    }
    free(buf);

    memset(recvBuff, 0, MAXRECVBUFF);

    n = lightpack_read(recvBuff, MAXRECVBUFF);
    if( n <= 0 ) {
        return 2;
    }

    char *find = strstr(recvBuff, "ok");
    if( find != NULL )
        return 0;

    return 1;
}

bool lightpack_lock(void)
{
    char *buf = "lock\n";
    if( lightpack_writestring(buf) == false )
        return 2;
    
    memset(recvBuff, 0, MAXRECVBUFF);

    int n = lightpack_read(recvBuff, MAXRECVBUFF);
    if( n <= 0 ) {
        return false;
    }
    
    if( strstr(recvBuff, "lock:success") != NULL )
        return true;
    
    if( strstr(recvBuff, "lock:busy") != NULL )
        return false;

    return false;
}

bool lightpack_unlock(void)
{
    char *buf = "unlock\n";
    if( lightpack_writestring(buf) == false )
        return 2;
    
    memset(recvBuff, 0, MAXRECVBUFF);

    int n = lightpack_read(recvBuff, MAXRECVBUFF);
    if( n <= 0 ) {
        return false;
    }
    
    if( strstr(recvBuff, "unlock:success") != NULL )
        return true;
    
    if( strstr(recvBuff, "unlock:not locked") != NULL )
        return false;

    return false;
}

// commandname without get prefix
char * lightpack_getcommand(char *command)
{
    char *buf, *retBuf;
    asprintf(&buf, "get%s\n", command);
    if( lightpack_writestring(buf) == false )
        return NULL;
    free(buf);

    memset(recvBuff, 0, MAXRECVBUFF);

    int n = lightpack_read(recvBuff, MAXRECVBUFF);
    if( n <= 0 ) {
        return NULL;
    }
    
    // command name + :
    int len = strlen(recvBuff);
    int start = strlen(command) + 1;
    len = len - start - 1;
    retBuf = (char *)malloc( sizeof(char) * len );
    if( retBuf == NULL )
        return NULL;
   
    strncpy( retBuf, recvBuff + start, len - 1 );
    retBuf[len - 1] = '\0';

    return retBuf;
}

char * lightpack_getstatus(void)
{
    return lightpack_getcommand("status");
}

char * lightpack_getstatusapi(void)
{
    return lightpack_getcommand("statusapi");
}
char * lightpack_getprofile(void)
{
    return lightpack_getcommand("profile");
}

char * lightpack_getprofiles(void)
{
    return lightpack_getcommand("profiles");
}

char * lightpack_getcountleds(void)
{
    return lightpack_getcommand("countleds");
}

char * lightpack_getleds(void)
{
    return lightpack_getcommand("leds");
}

char * lightpack_getcolors(void)
{
    return lightpack_getcommand("colors");
}

char * lightpack_getfps(void)
{
    return lightpack_getcommand("fps");
}

char * lightpack_getscreensize(void)
{
    return lightpack_getcommand("screensize");
}

char * lightpack_getmode(void)
{
    return lightpack_getcommand("mode");
}


char * lightpack_setcommand(const char *command, const char *value)
{
    if( lightpack_lock() == false )
    {
        printf("can't lock\n");
        return NULL;
    }

    char *buf, *retBuf;
    asprintf(&buf, "set%s:%s\n", command, value);
    if( lightpack_writestring(buf) == false )
        return NULL;
    free(buf);

    memset(recvBuff, 0, MAXRECVBUFF);

    int n = lightpack_read(recvBuff, MAXRECVBUFF);
    if( n <= 0 ) {
        printf("can't read\n");
        return NULL;
    }

    int len = strlen(recvBuff);
    retBuf = (char *)malloc( sizeof(char) * len );
    if( retBuf == NULL )
        return NULL;
    
    strncpy( retBuf, recvBuff, len-1 );
    retBuf[len-1] = '\0';

    lightpack_unlock();

    return retBuf;
}

char * lightpack_setcolor(const char *value)
{
    return lightpack_setcommand("color", value);
}

char * lightpack_setleds(const char *value)
{
    return lightpack_setcommand("leds", value);
}

char * lightpack_setgamma(double value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", value);
    return lightpack_setcommand("gamma", buf);
}

char * lightpack_setbrightness(int value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return lightpack_setcommand("brightness", buf);
}

char * lightpack_setsmooth(int value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return lightpack_setcommand("smooth", buf);
}

char * lightpack_setprofile(const char *value)
{
    return lightpack_setcommand("profile", value);
}

char * lightpack_setstatus(const char *value)
{
    return lightpack_setcommand("status", value);
}

char * lightpack_setmode(const char *value)
{
    return lightpack_setcommand("mode", value);
}
