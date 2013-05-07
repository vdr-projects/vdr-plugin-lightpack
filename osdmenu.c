#include "osdmenu.h"
#include "lightpack.h"

myOsdMenu::myOsdMenu() : cOsdMenu("Lightpack", 32)
{
    LastGamma = LightpackGamma;
    LastBrightness = LightpackBrightness;
    LastSmooth = LightpackSmooth;
    
    int ret = libLightpack.Connect();
    if( ret == 0 ) {
        Add(new cOsdItem(tr("Appearance"), osUnknown, false));
        Add(new cMenuEditIntItem(tr("Gamma"), &LightpackGamma, 0, 100));
        Add(new cMenuEditIntItem(tr("Brightness"), &LightpackBrightness));
        Add(new cMenuEditIntItem(tr("Smooth"), &LightpackSmooth));
        
        MySetHelp();
    } else if( ret == 1 ) {
        Add(new cOsdItem(tr("Lightpack not available"), osUnknown, false));
        Add(new cOsdItem(tr("init error"), osUnknown, false));
    } else if( ret == 2 ) {
        Add(new cOsdItem(tr("Lightpack not available"), osUnknown, false));
        Add(new cOsdItem(tr("connect error (Server/Port wrong? Prismatik running?)"), osUnknown, false));
    } else if( ret == 3 ) {
        Add(new cOsdItem(tr("Lightpack not available"), osUnknown, false));
        Add(new cOsdItem(tr("login error (ApiKey wrong?)"), osUnknown, false));
    }
    
    Display();
}

void myOsdMenu::MySetHelp()
{
    LastStatus = libLightpack.GetStatus();
    LastMode = libLightpack.GetMode();
    if( LastStatus == 1 ) {
        if( LastMode == 1 )
            SetHelp(tr("Stop"), tr("Lamp"), NULL, NULL);
        else if( LastMode == 2 )
            SetHelp(tr("Stop"), tr("Ambilight"), NULL, NULL);
        else
            Skins.Message(mtError, tr("can't get lightpack mode"));
    } else if( LastStatus == 2 ) {
        if( LastMode == 1 )
            SetHelp(tr("Start"), tr("Lamp"), NULL, NULL);
        else if( LastMode == 2 )
            SetHelp(tr("Start"), tr("Ambilight"), NULL, NULL);
        else
            Skins.Message(mtError, tr("can't get lightpack mode"));
    } else
        Skins.Message(mtError, tr("can't get lightpack status"));
}

myOsdMenu::~myOsdMenu()
{
    libLightpack.Disconnect();
}

eOSState myOsdMenu::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);
    if( !libLightpack.isConnected() )
        return state;

    int lightRet = 0;
    
    if( LastGamma != LightpackGamma )
    {
        lightRet = libLightpack.SetGamma( (double) LightpackGamma / 10.0);
        if( lightRet == 0 )
            LastGamma = LightpackGamma;
        else {
            LightpackGamma = LastGamma;
            Skins.Message(mtError, tr("lightpack error"));
        }
    }
    if( LastBrightness != LightpackBrightness )
    {
        lightRet = libLightpack.SetBrightness(LightpackBrightness);
        if( lightRet == 0 )
            LastBrightness = LightpackBrightness;
        else {
            LightpackBrightness = LastBrightness;
            Skins.Message(mtError, tr("lightpack error"));
        }
    }
    if( LastSmooth != LightpackSmooth )
    {
        lightRet = libLightpack.SetSmooth(LightpackSmooth);
        if( lightRet == 0 )
            LastSmooth = LightpackSmooth;
        else {
            LightpackSmooth = LastSmooth;
            Skins.Message(mtError, tr("lightpack error"));
        }
    }

    switch(Key)
    {
        case kRed:
            LastStatus = libLightpack.GetStatus();
            if( LastStatus == 1 ) {
                lightRet = libLightpack.SetStatus( false );
                if( lightRet > 0 )
                    Skins.Message(mtError, tr("lightpack error"));
            } else if( LastStatus == 2 ) {
                lightRet = libLightpack.SetStatus( true );
                if( lightRet > 0 )
                    Skins.Message(mtError, tr("lightpack error"));
            }
            MySetHelp();
            break;
         case kGreen:
            LastMode = libLightpack.GetMode();
            if( LastMode == 1 ) {
                lightRet = libLightpack.SetMode( 2 );
                if( lightRet > 0 )
                    Skins.Message(mtError, tr("lightpack error"));
            } else if( LastMode == 2 ) {
                lightRet = libLightpack.SetMode( 1 );
                if( lightRet > 0 )
                    Skins.Message(mtError, tr("lightpack error"));
            }
            MySetHelp();
            break;
        case kYellow:
            break;
        case kBlue:
            break;
        default:
            break;
    }
    //syslog(LOG_ERR, "lightpack Gamma=%d Brightness=%d Smooth=%d", LightpackGamma, LightpackBrightness, LightpackSmooth);
    return state;
}
