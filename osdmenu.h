#ifndef __OSDMENU_H__
#define __OSDMENU_H__

#include <vdr/menu.h>
#include <vdr/status.h>

class myOsdMenu : public cOsdMenu
{
private:
    int LastGamma, LastBrightness, LastSmooth;
    int LastStatus, LastMode;

    void MySetHelp(void);
public:
    myOsdMenu();
    ~myOsdMenu();

    virtual eOSState ProcessKey(eKeys Key);
};


#endif