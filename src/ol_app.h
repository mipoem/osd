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
#ifndef _OL_APP_H_
#define _OL_APP_H_

#include <glib.h>
#include "ol_music_info.h"

gboolean ol_app_download_lyric (OlMusicInfo *music_info);

struct OlPlayer;
struct OlLrc;

/** 
 * @brief Gets the current music
 * 
 * @return 
 */
OlMusicInfo* ol_app_get_current_music (void);

struct OlPlayer* ol_app_get_player (void);

struct OlLrc *ol_app_get_current_lyric (void);

gboolean ol_app_assign_lrcfile (const OlMusicInfo *info,
                                const char *filepath,
                                gboolean update);
/** 
 * @brief Adjust the offset of lyric by offset_ms
 *
 * The offset of the lyric will be original offset + offset_ms
 * @param offset_ms Incremental value of offset, in milliseconds
 */
void ol_app_adjust_lyric_offset (int offset_ms);
#endif /* _OL_APP_H_ */
