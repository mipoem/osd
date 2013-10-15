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
#include "ol_search_dialog.h"
#include "ol_gui.h"
#include "ol_lrc_fetch_module.h"
#include "ol_app.h"
#include "ol_lyric_manage.h"
#include "ol_config.h"
#include "ol_lrc_candidate_list.h"
#include "ol_lrc_engine_list.h"
#include "ol_intl.h"
#include "ol_debug.h"

const char *MSG_SEARCHING = N_("Searching lyrics from %s...");
const char *MSG_NOT_FOUND = N_("Ooops, no lyric found :(");
const char *MSG_FOUND = N_("%d lyrics found :)");

struct
{
  GtkWidget *window;
  GtkWidget *candidates_panel;
  GtkEntry *title;
  GtkEntry *artist;
  GtkTreeView *list;
  GtkLabel *msg;
  GtkWidget *download;
  GtkWidget *engine;
} widgets = {0};

static OlLrcFetchEngine *engine = NULL;
static int search_id = 0;
static OlMusicInfo music_info = {0};

gboolean ol_search_dialog_search_click (GtkWidget *widget, 
                                        gpointer data);
gboolean ol_search_dialog_download_click (GtkWidget *widget, 
                                        gpointer data);
gboolean ol_search_dialog_cancel_click (GtkWidget *widget,
                                        gpointer data);
static void internal_select_changed (GtkTreeSelection *selection, 
                                     gpointer data);

static void
internal_select_changed (GtkTreeSelection *selection, gpointer data)
{
  if (widgets.download != NULL)
    gtk_widget_set_sensitive (GTK_WIDGET (widgets.download),
                              gtk_tree_selection_get_selected (selection, 
                                                               NULL, 
                                                               NULL));
}

gboolean
ol_search_dialog_download_click (GtkWidget *widget, 
                                 gpointer data)
{
  ol_log_func ();
  ol_assert_ret (widgets.list != NULL, FALSE);
  OlLrcCandidate *candidate = ol_lrc_candidate_new ();
  ol_lrc_candidate_list_get_selected (widgets.list,
                                      candidate);
  char *filename = ol_lyric_download_path (&music_info);
  if (filename != NULL)
  {
    ol_lrc_fetch_begin_download (engine, candidate,
                                 &music_info, filename,
                                 NULL);
    g_free (filename);
  }
  ol_lrc_candidate_free (candidate);
  return TRUE;
}

gboolean
ol_search_dialog_cancel_click (GtkWidget *widget,
                               gpointer data)
{
  if (widgets.window != NULL)
    gtk_widget_hide (widgets.window);
  return TRUE;
}

static gboolean internal_init ();
static void internal_search_msg_callback (int search_id,
                                          enum OlLrcSearchMsgType msg_type,
                                          const char *message,
                                          void *userdata);
static void internal_search_callback (struct OlLrcFetchResult *result,
                                      void *userdata);

gboolean
ol_search_dialog_search_click (GtkWidget *widget,
                               gpointer data)
{
  ol_log_func ();
  OlMusicInfo *info = ol_music_info_new ();
  ol_music_info_copy (info, 
                      ol_app_get_current_music ());
  if (widgets.title != NULL)
    ol_music_info_set_title (info,
                             gtk_entry_get_text (widgets.title));  
  if (widgets.artist != NULL)
    ol_music_info_set_artist (info,
                              gtk_entry_get_text (widgets.artist));
  gtk_widget_set_sensitive (GTK_WIDGET (widgets.list),
                            FALSE);
  gtk_widget_set_sensitive (widgets.download,
                            FALSE);
  char **engine_list = (char**)ol_lrc_engine_list_get_engine_names (GTK_TREE_VIEW(widgets.engine));
  search_id = ol_lrc_fetch_begin_search (engine_list,
                                         info,
                                         internal_search_msg_callback,
                                         internal_search_callback,
                                         NULL);
  g_strfreev (engine_list);
  return TRUE;
}

static void
internal_search_msg_callback (int search_id,
                              enum OlLrcSearchMsgType msg_type,
                              const char *message,
                              void *userdata)
{
  switch (msg_type)
  {
  case OL_LRC_SEARCH_MSG_ENGINE:
    if (widgets.msg != NULL)
    {
      char *msg = g_strdup_printf (_(MSG_SEARCHING),
                                   _(message));
      gtk_label_set_text (widgets.msg, msg);
    }
    break;
  default:
    ol_errorf ("Unknown search message type %d: %s\n",
               (int)msg_type, message);
  }
}

static void
internal_search_callback (struct OlLrcFetchResult *result,
                          void *userdata)
{
  ol_log_func ();
  ol_assert (result != NULL);
  if (search_id != result->id)
    return;
  gtk_widget_set_sensitive (GTK_WIDGET (widgets.list),
                            TRUE);
  ol_lrc_candidate_list_set_list (widgets.list,
                                  result->candidates,
                                  result->count);
  gtk_widget_show (widgets.candidates_panel);
  if (widgets.msg != NULL)
  {
    if (result->count > 0)
    {
      engine = result->engine;
      char *msg = g_strdup_printf (_(MSG_FOUND), result->count);
      gtk_label_set_text (widgets.msg, msg);
    }
    else
    {
      gtk_label_set_text (widgets.msg, _(MSG_NOT_FOUND));
    }
  }
}

static gboolean
internal_init ()
{
  if (widgets.window == NULL)
  {
    widgets.window = ol_gui_get_widget ("search-dialog");
    if (widgets.window == NULL)
    {
      ol_error ("Search Dialog not found");
      return FALSE;
    }
    g_signal_connect (G_OBJECT (widgets.window),
                      "delete-event",
                      G_CALLBACK (gtk_widget_hide_on_delete),
                      NULL);
    widgets.title = GTK_ENTRY (ol_gui_get_widget ("search-title"));
    widgets.artist = GTK_ENTRY (ol_gui_get_widget ("search-artist"));
    widgets.candidates_panel = ol_gui_get_widget ("search-candidates");
    widgets.list = GTK_TREE_VIEW (ol_gui_get_widget ("search-candidates-list"));
    widgets.msg = GTK_LABEL (ol_gui_get_widget ("search-msg"));
    widgets.download = ol_gui_get_widget ("search-download");
    ol_lrc_candidate_list_init (widgets.list, 
                                G_CALLBACK (internal_select_changed));
    widgets.engine = ol_gui_get_widget ("search-engine");
    ol_lrc_engine_list_init (GTK_TREE_VIEW (widgets.engine));
  }
  return widgets.window != NULL;
}

void
ol_search_dialog_show ()
{
  if (!internal_init ())
    return;
  if (GTK_WIDGET_VISIBLE (widgets.window))
    return;
  ol_lrc_candidate_list_clear (widgets.list);
  
  ol_music_info_copy (&music_info, ol_app_get_current_music ());
  gtk_entry_set_text (widgets.title, 
                      ol_music_info_get_title (&music_info));
  gtk_entry_set_text (widgets.artist,
                      ol_music_info_get_artist (&music_info));
  gtk_widget_set_sensitive (widgets.download,
                            FALSE);
  OlConfig *config = ol_config_get_instance ();
  char **engine_names = ol_config_get_str_list (config, 
                                                "Download",
                                                "download-engine",
                                                NULL);
  ol_lrc_engine_list_set_engine_names (GTK_TREE_VIEW (widgets.engine),
                                       engine_names);
  g_strfreev (engine_names);

  gtk_widget_show (widgets.window);
}
