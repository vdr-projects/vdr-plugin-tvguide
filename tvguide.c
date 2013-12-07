/*
 * tvguide.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <time.h>
#include <getopt.h>
#include <vdr/osd.h>
#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/menu.h>

#define DEFINE_CONFIG 1
#include "config.h"
#include "setup.h"
#include "tvguideosd.h"


#if defined(APIVERSNUM) && APIVERSNUM < 20000
#error "VDR-2.0.0 API version or greater is required!"
#endif

static const char *VERSION        = "1.1.0";
static const char *DESCRIPTION    = "A fancy 2d EPG Viewer";
static const char *MAINMENUENTRY  = "Tvguide";

class cPluginTvguide : public cPlugin {
private:
  bool logoPathSet;
  bool imagesPathSet;
  bool iconsPathSet;
public:
  cPluginTvguide(void);
  virtual ~cPluginTvguide();
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
  virtual const char *MainMenuEntry(void) { return (tvguideConfig.showMainMenuEntry)?MAINMENUENTRY:NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginTvguide::cPluginTvguide(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  logoPathSet = false;
  imagesPathSet = false;
  iconsPathSet = false;
}

cPluginTvguide::~cPluginTvguide()
{
  // Clean up after yourself!
}

const char *cPluginTvguide::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
    return 
         "  -i <IMAGESDIR>, --epgimages=<IMAGESDIR> Set directory where epgimages are stored.\n"
	 "  -c <ICONDIR>, --icons=<ICONDIR>         Set directory where icons are stored.\n"
         "  -l <LOGODIR>, --logodir=<LOGODIR>       Set directory where logos are stored.\n";
}

bool cPluginTvguide::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static const struct option long_options[] = {
    { "epgimages", required_argument, NULL, 'i' },
    { "icons", required_argument, NULL, 'c' },
    { "logopath", required_argument, NULL, 'l' },
    { 0, 0, 0, 0 }
  };

  int c;
  cString *path = NULL;
  while ((c = getopt_long(argc, argv, "i:c:l:", long_options, NULL)) != -1) {
    switch (c) {
      case 'i':
        path = new cString(optarg);
        tvguideConfig.SetImagesPath(*path);
        imagesPathSet = true;
        break;
      case 'c':
        path = new cString(optarg);
	tvguideConfig.SetIconsPath(*path);
	iconsPathSet = true;
	break;
      case 'l':
        path = new cString(optarg);
        tvguideConfig.SetLogoPath(*path);
        logoPathSet = true;
        break;
      default:
        return false;
    }
    if (path) 
        delete path;
  }
  return true;
}

bool cPluginTvguide::Initialize(void) {
    esyslog("tvguide: Initialize");
    esyslog("tvguide: OSD Wwidth %d, OSD Height %d", cOsd::OsdWidth(), cOsd::OsdHeight());
    esyslog("tvguide: numRows: %d, numCols: %d", tvguideConfig.channelRows, tvguideConfig.channelCols);
    return true;
}

bool cPluginTvguide::Start(void)
{
    if (!logoPathSet) {
        cString path = cString::sprintf("%s/channellogos/", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
        tvguideConfig.SetLogoPath(path);
        logoPathSet = true;
    }
    
    if (!imagesPathSet) {
        cString path = cString::sprintf("%s/epgimages/", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
        tvguideConfig.SetImagesPath(path);
        logoPathSet = true;
    }
    
    if (!iconsPathSet) {
        cString path = cString::sprintf("%s/icons/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
        tvguideConfig.SetIconsPath(path);
        iconsPathSet = true;
    }

    return true;
}

void cPluginTvguide::Stop(void)
{
  // Stop any background activities the plugin is performing.
}

void cPluginTvguide::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginTvguide::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginTvguide::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginTvguide::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginTvguide::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cTvGuideOsd;
}

cMenuSetupPage *cPluginTvguide::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cTvguideSetup();
}

bool cPluginTvguide::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return tvguideConfig.SetupParse(Name, Value);
}

bool cPluginTvguide::Service(const char *Id, void *Data)
{
  if (strcmp(Id, "MainMenuHooksPatch-v1.0::osSchedule") == 0 && tvguideConfig.replaceOriginalSchedule != 0) {
      if (Data == NULL)
         return true;
      cOsdObject **guide = (cOsdObject**) Data;
      if (guide)
         *guide = MainMenuAction();
      return true;
  }
  return false;
}

const char **cPluginTvguide::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginTvguide::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginTvguide); // Don't touch this!
