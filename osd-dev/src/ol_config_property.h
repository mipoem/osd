/* -*- mode: C; c-basic-offset: 2; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2009-2011  Tiger Soldier <tigersoldier@gmail.com>
 *
 * This file is part of OSD Lyrics.
 * 
 * OSD Lyrics is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSD Lyrics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OSD Lyrics.  If not, see <http://www.gnu.org/licenses/>. 
 */
/**
 * @file   ol_config_property.h
 * @author Tiger Soldier <tigersoldi@gmail.com>
 * @date   Sun Jun 21 09:34:30 2009
 * 
 * @brief  Defines properties that can be configured
 * 
 * 
 */

#ifndef _OL_CONFIG_PROPERTY_H_
#define _OL_CONFIG_PROPERTY_H_
#include "config.h"

enum {
  PROP_0,

  PROP_OSD_OUTLINE_WIDTH,
  PROP_OSD_WIDTH,
  PROP_OSD_LOCKED,
  PROP_OSD_VISIBLE,
  PROP_OSD_FONT_NAME,
  PROP_OSD_LRC_ALIGN_0,
  PROP_OSD_LRC_ALIGN_1,
  PROP_OSD_ACTIVE_LRC_COLOR,
  PROP_OSD_INACTIVE_LRC_COLOR,
  PROP_OSD_LINE_COUNT,
  PROP_OSD_TRANSLUCENT_ON_MOUSE_OVER,
  PROP_OSD_X,
  PROP_OSD_Y,
  PROP_DOWNLOAD_ENGINE,
  PROP_LRC_PATH,
  PROP_LRC_FILENAME,
  PROP_DOWNLOAD_FIRST_LYRIC,
  PROP_STARTUP_PLAYER,
  PROP_NOTIFY_MUSIC,
  PROP_DISPLAY_MODE,
  PROP_OSD_WINDOW_MODE,
  PROP_OSD_BLUR_RADIUS,
  PROP_PROXY,
  PROP_PROXY_HOST,
  PROP_PROXY_PORT,
  PROP_PROXY_TYPE,
  PROP_PROXY_USERNAME,
  PROP_PROXY_PASSWORD,
  PROP_SCROLL_WIDTH,
  PROP_SCROLL_HEIGHT,
  PROP_SCROLL_FONT_NAME,
  PROP_SCROLL_BG_COLOR,
  PROP_SCROLL_ACTIVE_LRC_COLOR,
  PROP_SCROLL_INACTIVE_LRC_COLOR,
  PROP_SCROLL_OPACITY,
  PROP_SCROLL_SCROLL_MODE,
};

static const char *OL_CONFIG_DEFAULT_DOWNLOAD_ENGINE[] = {
  "ttPlayer",
  "xiami",
  NULL,
};

static const char *OL_CONFIG_ACTIVE_LRC_COLOR[] = {
  "#662600", "#FFFF00", "#FF8000", NULL,
};
static const char *OL_CONFIG_INACTIVE_LRC_COLOR[] = {
  "#99FFFF", "#0000FF", "#99FFFF", NULL,
};

static const char *OL_CONFIG_DEFAULT_LRC_PATH[] = {
  "~/.lyrics",
  "%",
  NULL,
};

static const char *OL_CONFIG_DEFAULT_LRC_FILENAME[] = {
  "%p-%t",
  "%t-%p",
  "%f",
  "%t",
  NULL,
};

typedef struct _OlConfigStrListValue OlConfigStrListValue;
struct _OlConfigStrListValue
{
  int key;
  char *name;
  char *group;
  char *nick;
  char *description;
  int len;
  const char **default_value;
};

typedef struct _OlConfigBoolValue OlConfigBoolValue;
struct _OlConfigBoolValue
{
  int key;
  char *name;
  char *group;
  char *nick;
  char *description;
  gboolean default_value;
};

typedef struct _OlConfigStringValue OlConfigStringValue;
struct _OlConfigStringValue
{
  int key;
  char *name;
  char *group;
  char *nick;
  char *description;
  char *default_value;
};

typedef struct _OlConfigIntValue OlConfigIntValue;
struct _OlConfigIntValue
{
  int key;
  char *name;
  char *group;
  char *nick;
  char *description;
  int min;
  int max;
  int default_value;
};

typedef struct _OlConfigDoubleValue OlConfigDoubleValue;
struct _OlConfigDoubleValue
{
  int key;
  char *name;
  char *group;
  char *nick;
  char *description;
  double min;
  double max;
  double default_value;
};

typedef struct _OlConfigInfo OlConfigInfo;
struct _OlConfigInfo
{
  int key;
  char *name;
  char *group;
};

static const OlConfigStrListValue config_str_list[] = {
  {PROP_DOWNLOAD_ENGINE, "download-engine", "Download",
   "Download engine", "Select the source where LRC files are downloaded from",
   -1, OL_CONFIG_DEFAULT_DOWNLOAD_ENGINE},
  {PROP_OSD_ACTIVE_LRC_COLOR, "active-lrc-color", "OSD", "Active lyric color",
   "Colors of active lyrics", 3, OL_CONFIG_ACTIVE_LRC_COLOR},
  {PROP_OSD_INACTIVE_LRC_COLOR, "inactive-lrc-color", "OSD", "Inactive lyric color",
   "Colors of inactive lyrics", 3, OL_CONFIG_INACTIVE_LRC_COLOR},
  {PROP_LRC_PATH, "lrc-path", "General", "Path of lrc files",
   "Path lists to search LRC files. Following patterns are allowed:\n"
   "  ~/<path> : Subdirectories in Home directory;\n"
   "  % : The same directory to the music;\n"
   "  Others : Treated as normal path.",
   -1, OL_CONFIG_DEFAULT_LRC_PATH},
  {PROP_LRC_FILENAME, "lrc-filename", "General", "LRC filename patterns",
   "Filename patterns to find LRC files. \n"
   "Patterns can contains following place holders:\n"
   "  %t: Title\n"
   "  %p: Artist\n"
   "  %a: Album\n"
   "  %n: Track number\n"
   "  %f: File name without extension",
   -1, OL_CONFIG_DEFAULT_LRC_FILENAME},
};
static const OlConfigBoolValue config_bool[] = {
  {PROP_OSD_LOCKED, "locked", "OSD", "Lock", "Whether the OSD is locked", TRUE},
  {PROP_OSD_VISIBLE, "visible", "OSD", "Visible", "Show/hide", TRUE},
  {PROP_OSD_TRANSLUCENT_ON_MOUSE_OVER, "translucent-on-mouse-over", "OSD",
   "Translucent on mouse over",
   "When the pointer is on the lyrics, make them semi-transparent",
   FALSE},
  {PROP_DOWNLOAD_FIRST_LYRIC, "download-first-lyric", "Download",
   "Download the first lyric candidate", 
   "If there are more than one lyrics matched, download the first one automately",
   FALSE},
  {PROP_NOTIFY_MUSIC, "notify-music", "General",
   "Notify Music", "Display music notification",
   TRUE},
};

static const OlConfigIntValue config_int[] = {
  {PROP_OSD_WIDTH, "width", "OSD", "OSD Width", "The width of the OSD", 1, 10000, 1024},
  {PROP_OSD_WIDTH, "outline-width", "OSD", "OSD Outline Width", "The width of the outline of lyrics", 0, 10, 3},
  {PROP_OSD_LINE_COUNT, "line-count", "OSD", "OSD line count", "The number of lyric lines in OSD", 1, 2, 1},
  {PROP_OSD_X, "x", "OSD", "OSD X position", "The horizontal position of OSD", 0, 10000, 0},
  {PROP_OSD_Y, "y", "OSD", "OSD Y position", "The vertical position of OSD", 0, 10000, 0},
  {PROP_PROXY_PORT, "proxy-port", "Download", "Proxy Port", "The port of proxy", 7070},
  {PROP_SCROLL_WIDTH, "width", "ScrollMode", "Scroll Window Width",
   "The width of the scroll window", 1, 10000, 500},
  {PROP_SCROLL_HEIGHT, "height", "ScrollMode", "Scroll Window Height",
   "The height of the scroll window", 1, 10000, 400},
};

static const OlConfigDoubleValue config_double[] = {
  {PROP_OSD_LRC_ALIGN_0, "lrc-align-0", "OSD", "Lyric alignment 0",
   "Alignment of the first lyric line",
   0.0, 1.0, 0.0},
  {PROP_OSD_LRC_ALIGN_1, "lrc-align-1", "OSD", "Lyric alignment 1",
   "Alignment of the second lyric line",
   0.0, 1.0, 1.0},
  {PROP_SCROLL_OPACITY, "opacity", "ScrollMode", "Scroll Window Opacity",
   "The background opacity of the scroll window",
   0.0, 1.0, 0.9},
  {PROP_OSD_BLUR_RADIUS, "blur-radius", "OSD", "OSD Blur Radius",
   "Blur radius of shadow of texts in OSD window",
   0.0, 5.0, 2.0},
};

static const OlConfigStringValue config_str[] = {
  {PROP_OSD_FONT_NAME, "font-name", "OSD", "OSD Font Name",
   "Lyrics font in OSD window, like ``serif 30",
   "serif 30"},
  {PROP_PROXY, "proxy", "Download",
   "Proxy", "The proxy to download lyrics. Available settings are no, system, or manual", "no"},
  {PROP_PROXY_TYPE, "proxy-type", "Download",
   "Proxy Type",
   "The protocol used by manual proxy. Available settings are http, socks4, or socks5", "http"},
  {PROP_PROXY_HOST, "proxy-host", "Download",
   "Proxy Host", "The hostname of proxy", ""},
  {PROP_PROXY_USERNAME, "proxy-username", "Download",
   "Proxy Username", "The username of proxy", ""},
  {PROP_PROXY_PASSWORD, "proxy-password", "Download",
   "Proxy Password", "The password of proxy", ""},
  {PROP_STARTUP_PLAYER, "startup-player", "General",
   "Startup player", "Startup the player if no available player detected.",
   ""},
  {PROP_DISPLAY_MODE, "display-mode", "General",
   "Display Mode", "The means of displaying lyrics, either OSD or scroll.",
   "OSD"},
  {PROP_OSD_WINDOW_MODE, "osd-window-mode", "OSD",
   "OSD window mode", "The display mode of OSD Window, either dock or normal.",
   "dock"},
  {PROP_SCROLL_FONT_NAME, "font-name", "ScrollMode", "Scroll Window Font Name",
   "The font information of scroll window, like ``Sans Bold Italic 12''",
   "Sans 12"},
  {PROP_SCROLL_BG_COLOR, "bg-color", "ScrollMode", "Scroll Window Background Color",
   "The background color of scroll window", "#000000"},
  {PROP_SCROLL_ACTIVE_LRC_COLOR, "active-lrc-color", "ScrollMode",
   "Active lyric color", "Color of active lyrics", "#E3CF00"},
  {PROP_SCROLL_INACTIVE_LRC_COLOR, "inactive-lrc-color", "ScrollMode",
   "Inactive lyric color", "Colors of inactive lyrics", "#FAEBD6"},
  {PROP_SCROLL_SCROLL_MODE, "scroll-mode", "ScrollMode", "Scroll Modes",
   "Means of scrolling. Use \"always\" to keep scrolling by time, "
   "or \"lines\" to scroll only when lyric changes", "always"},
};

#endif /* _OL_CONFIG_PROPERTY_H_ */
