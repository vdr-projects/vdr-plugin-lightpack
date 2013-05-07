#ifndef __LIGHTPACK_H__
#define __LIGHTPACK_H__

/*
 * lightpack.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/config.h>
#include <vdr/menu.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "osdmenu.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Lightpack plugin";
static const char *MAINMENUENTRY  = "Lightpack";

extern int LightpackGamma;
extern int LightpackBrightness;
extern int LightpackSmooth;

extern class cLibLightpack libLightpack;

class cPluginLightpack : public cPlugin {
private:
  // Add any member variables or functions you may need here.

  void LoadConfig(void);
  char * GetConfigValue(const char *Filename, const char *Setting);
public:
  cPluginLightpack(void);
  virtual ~cPluginLightpack();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};


class cLibLightpack {
    bool is_init;
    bool is_connected;

    double gamma;
    int brightness;
    int smooth;
public:
    cLibLightpack();
    ~cLibLightpack();

    cString Server;
    cString Port;
    cString ApiKey;

    int Connect(void);
    bool isConnected(void);
    void Disconnect(void);

    int SetGamma(double Value);
    int SetBrightness(int Value);
    int SetSmooth(int Value);

    int SetStatus(bool Value);
 
    // 1 = Ambilight
    // 2 = Lamp
    int SetMode(int Value);

    double GetGamma(void);
    int GetBrightness(void);
    int GetSmooth(void);

    // 0 = Error
    // 1 = On
    // 2 = Off
    int GetStatus(void);

    // 0 = Error
    // 1 = Ambilight
    // 2 = Lamp
    int GetMode(void);

    int GetProfile(cString &Profile);

    double GetFps(void);
};

#endif
