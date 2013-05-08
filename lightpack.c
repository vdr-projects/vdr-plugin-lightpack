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

int LightpackProfileIndex = 0;

class cLibLightpack libLightpack;

// --- myMenuSetup ------------------------------------------------------------
class myMenuSetup : public cMenuSetupPage
{
    private:
        cStringList Profiles;

    protected:
        virtual void Store()
        {
            SetupStore("Gamma", LightpackGamma);
            SetupStore("Brightness", LightpackBrightness);
            SetupStore("Smooth", LightpackSmooth);
            SetupStore("ProfileIndex", LightpackProfileIndex);
            
            SetLightpackSettings();
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
            if( libLightpack.GetProfiles( Profiles ) ) {
                if( LightpackProfileIndex < 0 || LightpackProfileIndex > Profiles.Size() )
                    LightpackProfileIndex = 0;
                Add(new cMenuEditStraItem(tr("Profile"), &LightpackProfileIndex, Profiles.Size(), &Profiles[0]));
            }
            else
                Add(new cOsdItem(tr("can't get profiles list"), osUnknown, false));
        }
        void SetLightpackSettings(void)
        {
            cStringList Profiles;
            if( libLightpack.GetProfiles( Profiles ) ) {
                if( LightpackProfileIndex < 0 || LightpackProfileIndex > Profiles.Size() )
                    LightpackProfileIndex = 0;
                if( !libLightpack.SetProfile( Profiles[LightpackProfileIndex] ) )
                    syslog(LOG_ERR, "lightpack can't set profile to %s", Profiles[LightpackProfileIndex] );
            }
            else
                syslog(LOG_ERR, "lightpack can't get profile list");
            
            if( !libLightpack.SetGamma( (double) LightpackGamma / 10.0 ) )
                syslog(LOG_ERR, "lightpack can't set gamma");
            if( !libLightpack.SetBrightness( LightpackBrightness ) )
                syslog(LOG_ERR, "lightpack can't set brightness");
            if( !libLightpack.SetSmooth( LightpackSmooth ) )
                syslog(LOG_ERR, "lightpack can't set smooth");
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
    SetLightpackSettings();
    return true;
}

bool cPluginLightpack::Start(void)
{
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
    SetLightpackSettings();
}

bool cPluginLightpack::SetupParse(const char *Name, const char *Value)
{
    if(!strcmp("Gamma", Name)) {
        LightpackGamma = atof(Value);
        if( LightpackGamma < 0 || LightpackGamma > 100 )
            LightpackGamma = 50;
    } else if(!strcmp("Brightness", Name)) {
        LightpackBrightness = atoi(Value);
        if( LightpackBrightness < 0 || LightpackBrightness > 100 )
            LightpackBrightness = 50;
    } else if(!strcmp("Smooth", Name)) {
        LightpackSmooth = atoi(Value);
        if( LightpackSmooth < 0 || LightpackSmooth > 255 )
            LightpackSmooth = 127;
    } else if(!strcmp("ProfileIndex", Name)) {
        LightpackProfileIndex = atoi(Value);
    } else
        return false;
    return true;
}

bool cPluginLightpack::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}
static const char *SVDRPHelpText[] = {
    "STATUS <ON/OFF>\n" "\040   set lightpack status\n\n"
    "    available options are\n"
    "    ON set lightpack on\n"
    "    OFF set lightpack off\n",
    "MODE <AMBILIGHT/LAMP>\n" "\040   set lightpack mode\n\n"
    "    available options are\n"
    "    AMBILIGHT set mode to ambilight\n"
    "    LAMP set mode to lamp\n",
    "GAMMA <value>\n" "\040   set gamma value\n\n"
    "    The value must be between 0 - 100\n"
    "    the value will be devided by 10, because prismatik handle values between 0.0 - 10.0\n",
    "BRIGHT <value>\n" "\040   set brightness value\n\n"
    "    The value must be between 0 - 100\n",
    "SMOOTH <value>\n" "\040   set smooth value\n\n"
    "    The value must be between 0 - 255\n",
    "PROFILE <profile>\n" "\040   set the profile\n\n",
    NULL
};

const char **cPluginLightpack::SVDRPHelpPages(void)
{
    return SVDRPHelpText;
}

cString cPluginLightpack::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
    if (!strcasecmp(Command, "STATUS")) {
        if( !strcasecmp(Option, "ON")) {
            if( libLightpack.SetStatus( true ) )
                return "Successful set status on";
            return cString::sprintf("Error set status on: %s", libLightpack.GetLastError() );
        } else if( !strcasecmp(Option, "OFF")) {
            if( libLightpack.SetStatus( false ) )
                return "Successful set status off";
            return cString::sprintf("Error set status off: %s", libLightpack.GetLastError() );
        } else
            return "Error unknown status command. Use ON/OFF";
    } else if (!strcasecmp(Command, "MODE")) {
        if( !strcasecmp(Option, "Ambilight")) {
            if( libLightpack.SetMode( 1 ) )
                return "Successful set mode to ambilight";
            return cString::sprintf("Error set mode to ambilight: %s", libLightpack.GetLastError() );
        } else if( !strcasecmp(Option, "Lamp")) {
            if( libLightpack.SetMode( 2 ) )
                return "Successful set mode to lamp";
            return cString::sprintf("Error set mode to lamp: %s", libLightpack.GetLastError() );
        } else
            return "Error unknown mode command. Use AMBILIGHT/LAMP";
    } else if (!strcasecmp(Command, "GAMMA")) {
        int gamma = atoi(Option);
        if( gamma >= 0 && gamma <= 100 ) {
            if( libLightpack.SetGamma( (double) gamma / 10.0 ) )
                return "Successful set gamma";
            return cString::sprintf("Error set gamma: %s", libLightpack.GetLastError() );
        }
        
        return "Error set gamma. Value not in range. The value must be between 0 - 100!";
    } else if (!strcasecmp(Command, "BRIGHT")) {
        int bright = atoi(Option);
        if( bright >= 0 && bright <= 100 ) {
            if( libLightpack.SetBrightness( bright) )
                return "Successful set brightness";
            return cString::sprintf("Error set brightness: %s", libLightpack.GetLastError() );
        }
        
        return "Error set brightness. Value not in range. The value must be between 0 - 100!";
    } else if (!strcasecmp(Command, "SMOOTH")) {
        int smooth = atoi(Option);
        if( smooth >= 0 && smooth <= 255 ) {
            if( libLightpack.SetSmooth( smooth ) )
                return "Successful set smooth";
            return cString::sprintf("Error set smooth: %s", libLightpack.GetLastError() );
        }
        
        return "Error set smooth. Value not in range. The value must be between 0 - 255!";
    } else if (!strcasecmp(Command, "PROFILE")) {
        if( libLightpack.SetProfile( Option ) )
            return "Successful set profile";
        return cString::sprintf("Error set profile: %s", libLightpack.GetLastError() );
    }
    return NULL;
}

void cPluginLightpack::SetLightpackSettings(void)
{
    cStringList Profiles;
    if( libLightpack.GetProfiles( Profiles ) ) {
        if( LightpackProfileIndex < 0 || LightpackProfileIndex > Profiles.Size() )
            LightpackProfileIndex = 0;
        if( !libLightpack.SetProfile( Profiles[LightpackProfileIndex] ) )
            syslog(LOG_ERR, "lightpack can't set profile to %s", Profiles[LightpackProfileIndex] );
    }
    else
        syslog(LOG_ERR, "lightpack can't get profile list");
    
    if( !libLightpack.SetGamma( (double) LightpackGamma / 10.0 ) )
        syslog(LOG_ERR, "lightpack can't set gamma");
    if( !libLightpack.SetBrightness( LightpackBrightness ) )
        syslog(LOG_ERR, "lightpack can't set brightness");
    if( !libLightpack.SetSmooth( LightpackSmooth ) )
        syslog(LOG_ERR, "lightpack can't set smooth");
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
    LastError = "";
    is_connected = false;
    if( lightpack_init() == false )
        is_init = false;
    else
        is_init = true;
}
cLibLightpack::~cLibLightpack()
{
}

const char* cLibLightpack::GetLastError(void)
{
    //syslog(LOG_ERR, "lightpack LastError: %s", *LastError);
    
    Error = LastError;
    LastError = "";
    return *Error;
}

bool cLibLightpack::isConnected()
{
    return is_connected;
}

bool cLibLightpack::Connect(void)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }

    int port = atoi( *Port );
    if( lightpack_connect( *Server, port) == false ) {
        LastError = "connection failed";
        return false;
    }

    int ret = 0;
    if( (ret = lightpack_login(*ApiKey)) != 0 ) {
        if( ret == 1 )
            LastError = "authentication failed, apikey wrong";
        else
            LastError = "authentication failed, socket error";
        return false;
    }
    is_connected = true;

    return true;
}

void cLibLightpack::Disconnect(void)
{
    is_connected = false;
    if( !is_init )
        return;

    lightpack_disconnect();
    LastError = "";
}

bool cLibLightpack::SetGamma(double Value)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }

    char *lightret = lightpack_setgamma(Value);
    if( lightret != NULL ) {
        if( strstr(lightret, "ok") ) {
            gamma = Value;
            free(lightret);
            return true;
        } else {
            LastError = cString::sprintf("set failed return: %s", lightret);
            free(lightret);
            return false;
        }
    }
    LastError = "set failed unknown error";
    return false;
}

bool cLibLightpack::SetBrightness(int Value)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }

    char *lightret = lightpack_setbrightness(Value);
    if( lightret != NULL ) {
        if( strstr(lightret, "ok") )
        {
            brightness = Value;
            free(lightret);
            return true;
        } else {
            LastError = cString::sprintf("set failed return: %s", lightret);
            free(lightret);
            return false;
        }
    }
    LastError = "set failed unknown error";
    return false;
}

bool cLibLightpack::SetSmooth(int Value)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }
    char *lightret = lightpack_setsmooth(Value);
    if( lightret != NULL ) {
        if( strstr(lightret, "ok") ) { 
            smooth = Value;
            free(lightret);
            return true;
        } else {
            LastError = cString::sprintf("set failed return: %s", lightret);
            free(lightret);
            return false;
        }
    }
    LastError = "set failed unknown error";
    return false;
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

bool cLibLightpack::SetStatus(bool Value)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }

    char *lightret = NULL;
    if( Value )
        lightret = lightpack_setstatus("on");
    else
        lightret = lightpack_setstatus("off");

    if( lightret != NULL ) {
        if( strstr(lightret, "ok") ) {
            free(lightret);
            return true;
        } else {
            LastError = cString::sprintf("set failed return: %s", lightret);
            free(lightret);
            return false;
        }
    }
    LastError = "set failed unknown error";
    return false;
}

bool cLibLightpack::SetMode(int Value)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }

    char *lightret = NULL;
    if( Value == 1)
        lightret = lightpack_setmode("ambilight");
    else if( Value == 2 )
        lightret = lightpack_setmode("moodlamp");
    else {
        LastError = "wrong mode";
        return false;
    }

    if( lightret != NULL ) {
        if( strstr(lightret, "ok") ) {
            free(lightret);
            return true;
        } else {
            free(lightret);
            LastError = cString::sprintf("set failed return: %s", lightret);
            return false;
        }
    }
    LastError = "set failed unknown error";
    return false;
}

bool cLibLightpack::SetProfile(cString Profile)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }

    char *lightret = NULL;
    lightret = lightpack_setprofile(*Profile);
    if( lightret != NULL ) {
        if( strstr(lightret, "ok") ) {
            free(lightret);
            return true;
        } else {
            LastError = cString::sprintf("set failed return: %s", lightret);
            free(lightret);
            return false;
        }
    }
    LastError = "set failed unknown error";
    return false;
}

int cLibLightpack::GetStatus(void)
{
    if( !is_init ) {
        LastError = "not initialized";
        return 0;
    }
    if( !is_connected ) {
        if( !Connect() )
            return 0;
    }
    int ret = 0;
    char *status = lightpack_getstatus();
    if( status != NULL ) {
        if( strstr(status, "on") )
            ret = 1;
        else 
            ret = 2;
        free(status);
    } else {
        LastError = "set failed unknown error";
        ret = 0;
    }

    return ret; 
}

int cLibLightpack::GetMode(void)
{
    if( !is_init ) {
        LastError = "not initialized";
        return 0;
    }
    if( !is_connected ) {
        if( !Connect() )
            return 0;
    }
    int ret = 0;
    char *mode = lightpack_getmode();
    if( mode != NULL ) {
        if( strstr(mode, "ambilight") )
            ret = 1;
        else
            ret = 2;
        free(mode);
    } else {
        LastError = "set failed unknown error";
        ret = 0;
    }

    return ret; 
}

bool cLibLightpack::GetFps(double &Fps)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }
    char *fps = lightpack_getmode();
    if( fps != NULL ) {
        Fps = atof(fps);
        free(fps);
        return true;
    }
    LastError = "set failed unknown error";

    return false; 
}

bool cLibLightpack::GetProfile(cString &Profile)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }
    char *profile = lightpack_getprofile();
    if( profile != NULL ) {
        Profile = strdup(profile);
        free(profile);
        return true;
    }
    LastError = "set failed unknown error";

    return false; 
}

bool cLibLightpack::GetProfiles(cStringList &Profiles)
{
    if( !is_init ) {
        LastError = "not initialized";
        return false;
    }
    if( !is_connected ) {
        if( !Connect() )
            return false;
    }
    char *profiles = lightpack_getprofiles();
    if( profiles != NULL ) {
        char *tok;
        tok = strtok(profiles, ";");
        while( tok != NULL )
        {
            Profiles.Append( strdup(tok) );
            tok = strtok(NULL, ";");
        }
        free(profiles);
        return true;
    }
    LastError = "set failed unknown error";

    return false; 
}

VDRPLUGINCREATOR(cPluginLightpack); // Don't touch this!
