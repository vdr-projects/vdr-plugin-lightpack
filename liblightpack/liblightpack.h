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

#ifndef __LIBLIGHTPACK_H__
#define __LIBLIGHTPACK_H__

#ifdef __cplusplus
extern "C" {
#endif

bool lightpack_init(void);
bool lightpack_connect(const char *host, int port);
void lightpack_disconnect(void);

// 0 sucess login
// 1 apikey false
// 2 socket error
//
int lightpack_login(const char *apikey);

char * lightpack_getstatus(void);
char * lightpack_getstatusapi(void);
char * lightpack_getprofile(void);
char * lightpack_getprofiles(void);
char * lightpack_getcountleds(void);
char * lightpack_getleds(void);
char * lightpack_getcolors(void);
char * lightpack_getfps(void);
char * lightpack_getscreensize(void);
char * lightpack_getmode(void);

char * lightpack_setcolor(const char *value);
char * lightpack_setleds(const char *value);
char * lightpack_setgamma(double value);
char * lightpack_setbrightness(int value);
char * lightpack_setsmooth(int value);
char * lightpack_setprofile(const char *value);
char * lightpack_setstatus(const char *value);
char * lightpack_setmode(const char *value);

#ifdef __cplusplus
}
#endif

#endif
