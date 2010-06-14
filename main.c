/* This file is part of herzi's playground
 *
 * Copyright (C) 2010  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <gtk/gtk.h>
#include <path-bar.h>

#include <glib/gi18n.h>

#include <execinfo.h> /* backtrace() */
#include <stdlib.h>   /* free() */

static void
my_logger (gchar const   * domain,
           GLogLevelFlags  flags,
           gchar const   * message,
           gpointer        user_data)
{
  gchar const* skip[] = {G_STRFUNC, "g_logv", "g_log"};
  GLogFunc* logger = user_data;
  GString * new_message = g_string_new (message);
  gpointer  buffer[100];
  char    **strings;
  int       i;
  int       limit;

  limit = backtrace (buffer, G_N_ELEMENTS (buffer));
  strings = backtrace_symbols (buffer, limit);

  g_string_append_printf (new_message, "\nBacktrace:");
  for (i = G_N_ELEMENTS (skip); i < limit; i++)
    {
      g_string_append_printf (new_message, "\n    %s", strings[i]);
    }
  free (strings);

  (*logger) (domain, flags, new_message->str, NULL);

  g_string_free (new_message, TRUE);
}

int
main (int   argc,
      char**argv)
{
  GLogFunc   logger = NULL;
  GtkWidget* box;
  GtkWidget* path;
  GtkWidget* text;
  GtkWidget* scrolled;
  GtkWidget* window;

  logger = g_log_set_default_handler (my_logger, &logger);
  if (G_UNLIKELY (!logger))
    {
      logger = g_log_default_handler;
    }

  gtk_init (&argc, &argv);

  window   = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  box      = gtk_vbox_new (FALSE, 0);
  path     = progress_path_bar_new ();
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  text     = gtk_text_view_new ();

  gtk_window_set_default_size (GTK_WINDOW (window), 400, 300);
  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  progress_path_bar_append (PROGRESS_PATH_BAR (path), GTK_STOCK_HOME, NULL);
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Programming"));
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, "GTK+");
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Path Bar"));
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Code"));
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Tests"));
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Automation"));
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Test Suite"));

  gtk_widget_modify_font (text,
                          pango_font_description_from_string ("Mono"));

  gtk_box_pack_start (GTK_BOX (box), path, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (scrolled), text);
  gtk_container_add (GTK_CONTAINER (box), scrolled);
  gtk_container_add (GTK_CONTAINER (window), box);

  gtk_widget_show_all (window);
  gtk_main ();

  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
