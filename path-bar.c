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

#include "path-bar.h"

#define GETTEXT_PACKAGE NULL
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (ProgressPathBar, progress_path_bar, PROGRESS_TYPE_SIMPLE_WIDGET);

static void
progress_path_bar_init (ProgressPathBar* self G_GNUC_UNUSED)
{
}

static gboolean
expose_event (GtkWidget     * widget,
              GdkEventExpose* event)
{
  struct
    {
      gchar const* icon_name;
      gchar const* label;
    }
  elements[] =
    {
        {GTK_STOCK_HOME, NULL},
#if 0
        {NULL, N_("Programming")},
        {NULL, N_("GTK+")},
        {NULL, N_("Path Bar")}
#endif
    };
  cairo_t* cr = gdk_cairo_create (event->window);
  guint i;

  gdk_cairo_region (cr, event->region);
  cairo_clip (cr);

  cairo_save (cr);
  cairo_set_line_width (cr, 1.0);
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.5);
  for (i = 0; i == 0 && i < G_N_ELEMENTS (elements); i++)
    {
      if (i > 0)
        {
          cairo_move_to (cr, 0.0, 16.0);
          cairo_line_to (cr, 8.0, 8.0);
          cairo_line_to (cr, 0.0, 0.0);
        }
      else
        {
          cairo_arc (cr, 4.5, 20.5, 4.0, 0.5 * G_PI, G_PI);
          cairo_arc (cr, 4.5, 4.5, 4.0, G_PI, 1.5 * G_PI);
        }

      if (i >= G_N_ELEMENTS (elements) - 1)
        {
          cairo_arc (cr, widget->allocation.width - 4.5, 4.5, 4.0, 1.5 * G_PI, 2.0 * G_PI);
          cairo_arc (cr, widget->allocation.width - 4.5, 20.5, 4.0, 0.0, 0.5 * G_PI);
        }

      cairo_close_path (cr);

      cairo_stroke (cr);
    }
  cairo_restore (cr);

  cairo_destroy (cr);
  return FALSE;
}

static void
progress_path_bar_class_init (ProgressPathBarClass* self_class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  widget_class->expose_event = expose_event;
}

GtkWidget*
progress_path_bar_new (void)
{
  return g_object_new (PROGRESS_TYPE_PATH_BAR,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
