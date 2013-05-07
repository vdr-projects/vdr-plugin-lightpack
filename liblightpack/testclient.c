/*
 *  See the README file for copyright information and how to reach the author.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "liblightpack.h"

int main(void)
{
    if( lightpack_init() == false )
    {
        printf("could not initilized lightpack\n");
        return 1;
    }
    printf("lightpack sucessfully initialized\n");

    if( lightpack_connect("127.0.0.1", 3636) == false )
    {
        printf("connect failed\n");
        return 1;
    }
    printf("lightpack connected\n");

    printf("disconnect ...\n");
    lightpack_disconnect();

    if( lightpack_connect("127.0.0.1", 3636) == false )
    {
        printf("connect failed\n");
        return 1;
    }
    
    printf("lightpack connected\n");


    FILE *fp = NULL;
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    char confFile[256];
    sprintf(confFile, "%s/.Prismatik/main.conf", homedir);
    printf("confFile: %s\n", confFile);
    fp = fopen(confFile, "r");
    if( fp != NULL )
    {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        char setting[256], value[256];

        while( (read = getline(&line, &len, fp)) != -1 ) {
            if( sscanf(line, "%[^=]=%s", setting, value) == 2 )
            {
                if( !strcmp(setting, "AuthKey") )
                {
                    printf("found apikey: %s\n", value);
                    int login_ret = 0;
                    if( (login_ret = lightpack_login(value)) != 0 )
                    {
                        printf("login failed: %d\n", login_ret);
                        return 1;
                    }
                }
            }
        }
        if (line)
            free(line);        
    } else
    {
        printf("can't get apikey\n");
        if( lightpack_login("schirrmie") != 0 )
        {
            printf("login failed\n");
            return 1;
        }
    }
    printf("sucessfully login\n");


    printf("\n##### testing #####\n");
    char *ret;

    char *status = lightpack_getstatus();
    if( status != NULL )
    {
        printf("\tgetstatus: %s\n", status);
        if( !strcmp(status, "on") )
            ret = lightpack_setstatus("off");
        else
            ret = lightpack_setstatus("on");
        if( ret != NULL )
        {
            printf("\tsetstatus: %s\n", ret);
            free(ret);
        } else
            printf("\tsetstatus error\n");

        free(status);
    } else
        printf("\tgetstatus error\n");

    char *statusapi = lightpack_getstatusapi();
    if( statusapi != NULL )
    {
        printf("\tgetstatusapi: %s\n", statusapi);
        free(statusapi);
    } else
        printf("\tgetstatuspi error\n");

    char *profile = lightpack_getprofile();
    if( profile != NULL )
    {
        printf("\tgetprofile: %s\n", profile);
        free(profile);
    } else
        printf("\tgetprofile error\n");

    char *profiles = lightpack_getprofiles();
    if( profiles != NULL )
    {
        printf("\tgetprofiles: %s\n", profiles);
        free(profiles);
    } else
        printf("\tgetprofiles error\n");

    char *countleds = lightpack_getcountleds();
    if( countleds != NULL )
    {
        printf("\tgetcountleds: %s\n", countleds);
        free(countleds);
    } else
        printf("\tgetcontleds error\n");

    char *leds = lightpack_getleds();
    if( leds != NULL )
    {
        printf("\tgetleds: %s\n", leds);
        ret = lightpack_setleds(leds);
        if( ret != NULL )
        {
            printf("\tsetleds: %s\n", ret);
            free(ret);
        } else
            printf("\tsetleds error\n");
        free(leds);
    } else
        printf("\tgetleds error\n");

    char *colors = lightpack_getcolors();
    if( colors != NULL )
    {
        printf("\tgetcolors: %s\n", colors);
        ret = lightpack_setcolor(colors);
        if( ret != NULL )
        {
            printf("\tsetcolor: %s\n", ret);
            free(ret);
        } else
            printf("\tsetcolor error\n");
        free(colors);
    } else
        printf("\tgetcolors error\n");

    char *fps = lightpack_getfps();
    if( fps != NULL )
    {
        printf("\tgetfps: %s\n", fps);
        free(fps);
    } else
        printf("\tgetfps error\n");

    char *screeensize = lightpack_getscreensize();
    if( screeensize != NULL )
    {
        printf("\tgetscreeensize: %s\n", screeensize);
        free(screeensize);
    } else
        printf("\tgetscreeensize error\n");

    char *mode = lightpack_getmode();
    if( mode != NULL )
    {
        printf("\tgetmode: %s\n", mode);
        if( !strcmp(mode, "ambilight") )
        {
            printf("\tset mode to moodlamp: ");
            ret = lightpack_setmode("moodlamp");
        }
        else
        {
            printf("\tset mode to ambilight: ");
            ret = lightpack_setmode("ambilight");
        }
        if( ret != NULL )
        {
            printf("%s\n", ret);
            free(ret);
        } else
            printf("error\n");

        free(mode);
    } else
        printf("\tgetmode error\n");

    printf("\tset gamma to 5.2: ");
    ret = lightpack_setgamma(5.2);
    if( ret != NULL )
    {
        printf("%s\n", ret);
        free(ret);
    } else
        printf("error\n");
    
    printf("\tset brightness to 50: ");
    ret = lightpack_setbrightness(50);
    if( ret != NULL )
    {
        printf("%s\n", ret);
        free(ret);
    } else
        printf("error\n");

    printf("\tset smooth to 127: ");
    ret = lightpack_setsmooth(127);
    if( ret != NULL )
    {
        printf("%s\n", ret);
        free(ret);
    } else
        printf("error\n");
    printf("##### testing #####\n");

    printf("\ndisconncted\n");
    lightpack_disconnect();
    return 0;
}
