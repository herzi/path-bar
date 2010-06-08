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
        {NULL, N_("Programming")},
#if 0
        {NULL, N_("GTK+")},
        {NULL, N_("Path Bar")}
#endif
    };
  cairo_t* cr = gdk_cairo_create (event->window);
  guint i;

  gdk_cairo_region (cr, event->region);
  cairo_clip (cr);

  cairo_set_line_width (cr, 1.0);
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.5);
  cairo_save (cr);
  cairo_arc (cr, 4.5, 20.5, 4.0, 0.5 * G_PI, G_PI);
  cairo_arc (cr, 4.5, 4.5, 4.0, G_PI, 1.5 * G_PI);
  for (i = 0; i < G_N_ELEMENTS (elements); i++)
    {
      if (i > 0)
        {
          cairo_move_to (cr, 0.5, 24.5);
          cairo_line_to (cr, 12.5, 12.5);
          cairo_line_to (cr, 0.5, 0.5);
          cairo_translate (cr, 12.0, 0.0);
        }
      cairo_translate (cr, 4.0, 0.0);

      if (elements[i].icon_name)
        {
          /* FIXME: cache the pixbuf after realize() and update on every "style-set" */
          GdkPixbuf* buf = gtk_widget_render_icon (widget, elements[i].icon_name, GTK_ICON_SIZE_MENU, NULL);
          cairo_save (cr);
          cairo_translate (cr, 0.0, 4.0);
          gdk_cairo_set_source_pixbuf (cr, buf, 0.0, 0.0);
          cairo_paint (cr);
          cairo_restore (cr);
          cairo_translate (cr, gdk_pixbuf_get_width (buf) + 4.0, 0.0);
          g_object_unref (buf);
        }
      if (elements[i].label)
        {
          /* FIXME: cache the layout after realize() and update on every "style-set" */
          PangoLayout* layout = gtk_widget_create_pango_layout (widget, elements[i].label);
          PangoRectangle  logical;
          cairo_path_t* path = cairo_copy_path (cr);

          pango_layout_get_extents (layout, NULL, &logical);

          cairo_save (cr);
          cairo_new_path (cr);
          cairo_translate (cr, 0.0, 18.0 - pango_layout_get_baseline (layout) / PANGO_SCALE); //20 - pango_layout_get_baseline (layout) + 0.5);
          pango_cairo_show_layout (cr, layout);
          cairo_restore (cr);

          cairo_new_path (cr);
          cairo_append_path (cr, path);
          cairo_translate (cr, logical.width + 4.0, 0.0);
          cairo_path_destroy (path);
          g_object_unref (layout);
        }

      if (i < G_N_ELEMENTS (elements) - 1)
        {
          /* FIXME: restore original coordinates */
          cairo_line_to (cr, 0.5, 0.5);
          cairo_line_to (cr, 12.5, 12.5);
          cairo_line_to (cr, 0.5, 24.5);

          cairo_close_path (cr);

          cairo_stroke (cr);
          cairo_translate (cr, 1.0, 0.0);
        }
    }
  cairo_restore (cr);
  cairo_arc (cr, widget->allocation.width - 4.5, 4.5, 4.0, 1.5 * G_PI, 2.0 * G_PI);
  cairo_arc (cr, widget->allocation.width - 4.5, 20.5, 4.0, 0.0, 0.5 * G_PI);
  cairo_close_path (cr);
  cairo_stroke (cr);

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
