#include "osdmenu.h"
#include "lightpack.h"

myOsdMenu::myOsdMenu() : cOsdMenu("Lightpack", 32)
{
    Gamma = LastGamma = LightpackGamma;
    Brightness = LastBrightness = LightpackBrightness;
    Smooth = LastSmooth = LightpackSmooth;
    ProfileIndex = LastProfileIndex = LightpackProfileIndex;

    if( libLightpack.isConnected() ) {
        Add(new cOsdItem(tr("Appearance"), osUnknown, false));
        Add(new cMenuEditIntItem(tr("Gamma"), &Gamma, 0, 100));
        Add(new cMenuEditIntItem(tr("Brightness"), &Brightness));
        Add(new cMenuEditIntItem(tr("Smooth"), &Smooth));
        
        if( libLightpack.GetProfiles( Profiles ) ) {
            cString Profile;
            libLightpack.GetProfile( Profile );
            ProfileIndex = LastProfileIndex = Profiles.Find( *Profile );
            if( ProfileIndex < 0 || ProfileIndex > Profiles.Size() )
                ProfileIndex = 0;
            Add(new cMenuEditStraItem(tr("Profile"), &ProfileIndex, Profiles.Size(), &Profiles[0]));
        }
        else
            Add(new cOsdItem(tr("can't get profiles list"), osUnknown, false));
        MySetHelp();
    } else {
        Add(new cOsdItem(tr("Lightpack not available"), osUnknown, false));
        cString Error = libLightpack.GetLastError();
        Add(new cOsdItem(*Error, osUnknown, false));
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
            Skins.Message(mtError, libLightpack.GetLastError() );
    } else if( LastStatus == 2 ) {
        if( LastMode == 1 )
            SetHelp(tr("Start"), tr("Lamp"), NULL, NULL);
        else if( LastMode == 2 )
            SetHelp(tr("Start"), tr("Ambilight"), NULL, NULL);
        else
            Skins.Message(mtError, libLightpack.GetLastError() );
    } else
        Skins.Message(mtError, libLightpack.GetLastError() );
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

    if( LastGamma != Gamma )
    {
        
        if( libLightpack.SetGamma( (double) Gamma / 10.0) )
            LastGamma = Gamma;
        else {
            Gamma = LastGamma;
            Skins.Message(mtError, libLightpack.GetLastError() );
        }
    }
    if( LastBrightness != Brightness )
    {
        if( libLightpack.SetBrightness(Brightness) )
            LastBrightness = Brightness;
        else {
            Brightness = LastBrightness;
            Skins.Message(mtError, libLightpack.GetLastError() );
        }
    }
    if( LastSmooth != Smooth )
    {
        if(  libLightpack.SetSmooth(Smooth) )
            LastSmooth = Smooth;
        else {
            Smooth = LastSmooth;
            Skins.Message(mtError, libLightpack.GetLastError() );
        }
    }
    if( LastProfileIndex != ProfileIndex )
    {
        cStringList Profiles;
        if( libLightpack.GetProfiles( Profiles ) ) {
            if( ProfileIndex < 0 || ProfileIndex > Profiles.Size() )
                ProfileIndex = 0;
            if( libLightpack.SetProfile( Profiles[ProfileIndex] ) )
                LastProfileIndex = ProfileIndex;
            else {
                ProfileIndex = LastProfileIndex;
                Skins.Message(mtError, libLightpack.GetLastError() );
            }
        }
        else
            Skins.Message(mtError, libLightpack.GetLastError() );
    }

    switch(Key)
    {
        case kRed:
            LastStatus = libLightpack.GetStatus();
            if( LastStatus == 1 ) {
               
                if( !libLightpack.SetStatus( false ) )
                    Skins.Message(mtError, libLightpack.GetLastError() );
            } else if( LastStatus == 2 ) {
                if( !libLightpack.SetStatus( true ) )
                    Skins.Message(mtError, libLightpack.GetLastError() );
            }
            MySetHelp();
            break;
         case kGreen:
            LastMode = libLightpack.GetMode();
            if( LastMode == 1 ) {
                if( !libLightpack.SetMode( 2 ) )
                    Skins.Message(mtError, libLightpack.GetLastError() );
            } else if( LastMode == 2 ) {
                if( !libLightpack.SetMode( 1 ) )
                    Skins.Message(mtError, libLightpack.GetLastError() );
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
    return state;
}
