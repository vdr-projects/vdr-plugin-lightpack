/*
 * lightpack.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include "lightpack.h"
#include "liblightpack.h"

// Gamma = Gamma * 10
int LightpackGamma = 50;
int LightpackBrightness = 50;
int LightpackSmooth = 127;

class cLibLightpack libLightpack;

// --- myMenuSetup ------------------------------------------------------------
class myMenuSetup : public cMenuSetupPage
{
    private:
        int menuGroupCount;
        cStringList SwitchGroupKeyTexts;
        cStringList SwitchWithOKTexts;
        cStringList HideGroupsAtTexts;

    protected:
        virtual void Store()
        {
            SetupStore("Gamma", LightpackGamma);
            SetupStore("Brightness", LightpackBrightness);
            SetupStore("Smooth", LightpackSmooth);
        }
    public:
        ~myMenuSetup()
        {
        }
        myMenuSetup()
        {
            Add(new cOsdItem(tr("Appearance"), osUnknown, false));
            Add(new cMenuEditIntItem(tr("Gamma"), &LightpackGamma, 0, 100));
            Add(new cMenuEditIntItem(tr("Brightness"), &LightpackBrightness, 0, 100));
            Add(new cMenuEditIntItem(tr("Smooth"), &LightpackSmooth, 0, 255));
        }
};



cPluginLightpack::cPluginLightpack(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginLightpack::~cPluginLightpack()
{
  // Clean up after yourself!
}

const char *cPluginLightpack::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginLightpack::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginLightpack::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  LoadConfig();
  return true;
}

bool cPluginLightpack::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginLightpack::Stop(void)
{
  // Stop any background activities the plugin is performing.
}

void cPluginLightpack::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginLightpack::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginLightpack::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginLightpack::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginLightpack::MainMenuAction(void)
{
  return new myOsdMenu();
}

cMenuSetupPage *cPluginLightpack::SetupMenu(void)
{
    return new myMenuSetup();
}

bool cPluginLightpack::SetupParse(const char *Name, const char *Value)
{
    if(!strcmp("Gamma", Name))
        LightpackGamma = atof(Value);
    else if(!strcmp("Brightness", Name))
        LightpackBrightness = atoi(Value);
    else if(!strcmp("Smooth", Name))
        LightpackSmooth = atoi(Value);
    else
        return false;
    return true;
}

bool cPluginLightpack::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginLightpack::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginLightpack::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

void cPluginLightpack::LoadConfig(void)
{
  cString ConfigDir;
  cString ConfigFile;
  FILE *fp;

  ConfigDir = ConfigDirectory("lightpack");
  ConfigFile = AddDirectory(ConfigDir, "lightpack.conf");

  fp = fopen( *ConfigFile, "r");
  if( !fp )
  {
    fp = fopen(ConfigFile, "w");
    fprintf(fp, "server=127.0.0.1\n");
    fprintf(fp, "port=3636\n");
    
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    char confFile[256];
    sprintf(confFile, "%s/.Prismatik/main.conf", homedir);
    char *ApiKey = GetConfigValue(confFile, "AuthKey");
    if( ApiKey )
    {
      fprintf(fp, "apikey=%s\n", ApiKey);
      free(ApiKey);
    } else
      fprintf(fp, "apikey=[APIKEY]\n");
    
    fclose(fp);
    fp = fopen(ConfigFile, "r");
    if( !fp )
    {
      syslog(LOG_ERR, "lightpack can't open configfile: %s!", (const char*)ConfigFile);
      return;
    }
  }
  fclose(fp);
  
  libLightpack.Server = GetConfigValue(ConfigFile, "server");
  libLightpack.Port = GetConfigValue(ConfigFile, "port");
  libLightpack.ApiKey = GetConfigValue(ConfigFile, "apikey");
  
  syslog(LOG_ERR, "lightpack server=%s port=%s apikey=%s", *libLightpack.Server, *libLightpack.Port, *libLightpack.ApiKey);
}

char * cPluginLightpack::GetConfigValue(const char *Filename, const char *Setting)
{
  char *Value;
  Value = (char*) malloc( sizeof(char) * 1024);
  char setting[1024];
  FILE *fp;
  
  fp = fopen(Filename, "r");
  if( fp != NULL )
  {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while( (read = getline(&line, &len, fp)) != -1 ) {
      if( sscanf(line, "%[^=]=%s", setting, Value) == 2 ) {
        if( !strcmp(setting, Setting) ) {
          free(line);
          return Value;
        }
      }
    }
    if (line)
      free(line);
  }
  return NULL;
}


cLibLightpack::cLibLightpack()
{
    is_connected = false;
    if( lightpack_init() == false )
        is_init = false;
    else
        is_init = true;
}
cLibLightpack::~cLibLightpack()
{
}

bool cLibLightpack::isConnected()
{
    return is_connected;
}

int cLibLightpack::Connect(void)
{
    if( !is_init )
        return 1;

    int port = atoi( *Port );
    if( lightpack_connect( *Server, port) == false )
    {
        return 2;
    }

    if( lightpack_login(*ApiKey) != 0 )
    {
        return 3;
    }
    is_connected = true;

    return 0;
}

void cLibLightpack::Disconnect(void)
{
    is_connected = false;
    if( !is_init )
        return;

    lightpack_disconnect();
}

int cLibLightpack::SetGamma(double Value)
{
    if( !is_init )
        return 1;
    if( !is_connected )
        return 2;

    int ret = 4;
    char *lightret = lightpack_setgamma(Value);
    if( lightret != NULL )
    {
        syslog(LOG_ERR, "lightpack SetGamma=%s", lightret);
        if( strstr(lightret, "ok") )
        {
            gamma = Value;
            ret = 0;
        } else
            ret = 3;
        free(lightret);
    } else
        ret = 4;
    return ret;
}
int cLibLightpack::SetBrightness(int Value)
{
    if( !is_init )
        return 1;
    if( !is_connected )
        return 2;

    int ret = 4;
    char *lightret = lightpack_setbrightness(Value);
    if( lightret != NULL )
    {
        syslog(LOG_ERR, "lightpack SetBrightness=%s", lightret);
        if( strstr(lightret, "ok") )
        {
            brightness = Value;
            ret = 0;
        } else
            ret = 3;
        free(lightret);
    } else
        ret = 4;
    return ret;
}
int cLibLightpack::SetSmooth(int Value)
{
    if( !is_init )
        return 1;
    if( !is_connected )
        return 2;

    int ret = 4;
    char *lightret = lightpack_setsmooth(Value);
    if( lightret != NULL )
    {
        syslog(LOG_ERR, "lightpack SetSmooth=%s", lightret);
        if( strstr(lightret, "ok") )
        {
            smooth = Value;
            ret = 0;
        } else
            ret = 3;
        free(lightret);
    } else
        ret = 4;
    return ret;
}

double cLibLightpack::GetGamma(void)
{
    return gamma;
}
int cLibLightpack::GetBrightness(void)
{
    return brightness;
}
int cLibLightpack::GetSmooth(void)
{
    return smooth;
}

int cLibLightpack::SetStatus(bool Value)
{
    if( !is_init )
        return 1;
    if( !is_connected )
        return 2;

    int ret = 4;
    char *lightret = NULL;
    if( Value )
        lightret = lightpack_setstatus("on");
    else
        lightret = lightpack_setstatus("off");

    if( lightret != NULL )
    {
        if( strstr(lightret, "ok") )
        {
            ret = 0;
        } else
            ret = 3;
        free(lightret);
    } else
        ret = 4;
    return ret;
}

int cLibLightpack::SetMode(int Value)
{
    if( !is_init )
        return 1;
    if( !is_connected )
        return 2;

    int ret = 4;
    char *lightret = NULL;
    if( Value == 1)
        lightret = lightpack_setmode("ambilight");
    else if( Value == 2 )
        lightret = lightpack_setmode("moodlamp");
    else
        return ret;

    if( lightret != NULL )
    {
        if( strstr(lightret, "ok") )
        {
            ret = 0;
        } else
            ret = 3;
        free(lightret);
    } else
        ret = 4;
    return ret;
}

int cLibLightpack::GetStatus(void)
{
    int ret = 0;
    char *status = lightpack_getstatus();
    if( status != NULL )
    {
        if( strstr(status, "on") )
            ret = 1;
        else
            ret = 2;
        free(status);
    } else
        ret = 0;

    return ret; 
}

int cLibLightpack::GetMode(void)
{
    int ret = 0;
    char *mode = lightpack_getmode();
    if( mode != NULL )
    {
        if( strstr(mode, "ambilight") )
            ret = 1;
        else
            ret = 2;
        free(mode);
    } else
        ret = 0;

    return ret; 
}

double cLibLightpack::GetFps(void)
{
    double ret = -1;
    char *fps = lightpack_getmode();
    if( fps != NULL )
    {
        ret = atof(fps);
        free(fps);
    } else
        ret = -1;

    return ret; 
}

int cLibLightpack::GetProfile(cString &Profile)
{
    int ret = 0;
    char *profile = lightpack_getprofile();
    if( profile != NULL )
    {
        Profile = strdup(profile);
        ret = 1;
        free(profile);
    } else
        ret = 0;

    return ret; 
}

VDRPLUGINCREATOR(cPluginLightpack); // Don't touch this!
