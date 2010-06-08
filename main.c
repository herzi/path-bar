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

int
main (int   argc,
      char**argv)
{
  GtkWidget* window;
  GtkWidget* path;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  path   = progress_path_bar_new ();

  gtk_window_set_default_size (GTK_WINDOW (window), 400, 300);
  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  progress_path_bar_append (PROGRESS_PATH_BAR (path), GTK_STOCK_HOME, NULL);
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Programming"));
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, "GTK+");
  progress_path_bar_append (PROGRESS_PATH_BAR (path), NULL, N_("Path Bar"));

  gtk_container_add (GTK_CONTAINER (window), path);

  gtk_widget_show_all (window);
  gtk_main ();

  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
