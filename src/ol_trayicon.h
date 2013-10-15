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
#ifndef __OL_TRAYICON_H__
#define __OL_TRAYICON_H__

enum OlPlayerStatus;

/** 
 * @brief give a Entrance to do trayicon job
 * 
 */
void ol_trayicon_inital ();

/** 
 * @brief Notifiy the trayicon that the playing status has changed
 * 
 */
void ol_trayicon_status_changed (enum OlPlayerStatus status);

void ol_trayicon_free ();
#endif
