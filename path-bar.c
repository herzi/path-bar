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

typedef struct _ProgressPathBarElement
{
  gchar* icon_name;
  gchar* label;
} ProgressPathBarElement;

struct _ProgressPathBarPrivate
{
  GList* elements;
};

#define PRIV(i) (((ProgressPathBar*)(i))->_private)

G_DEFINE_TYPE (ProgressPathBar, progress_path_bar, PROGRESS_TYPE_SIMPLE_WIDGET);

static void
progress_path_bar_init (ProgressPathBar* self G_GNUC_UNUSED)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, PROGRESS_TYPE_PATH_BAR, ProgressPathBarPrivate);
}

static void
free_element (gpointer  data,
              gpointer  user_data G_GNUC_UNUSED)
{
  ProgressPathBarElement* element = data;
  g_free (element->icon_name);
  g_free (element->label);
  g_slice_free (ProgressPathBarElement, element);
}

static void
finalize (GObject* object)
{
  g_list_foreach (PRIV (object)->elements, free_element, NULL);
  g_list_free (PRIV (object)->elements);

  G_OBJECT_CLASS (progress_path_bar_parent_class)->finalize (object);
}

static gboolean
expose_event (GtkWidget     * widget,
              GdkEventExpose* event)
{
  cairo_t* cr = gdk_cairo_create (event->window);
  GList  * iter;

  gdk_cairo_region (cr, event->region);
  cairo_clip (cr);

  cairo_set_line_width (cr, 1.0);
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.5);
  cairo_save (cr);
  cairo_arc (cr, 4.5, 20.5, 4.0, 0.5 * G_PI, G_PI);
  cairo_arc (cr, 4.5, 4.5, 4.0, G_PI, 1.5 * G_PI);
  for (iter = PRIV (widget)->elements; iter; iter = iter->next)
    {
      ProgressPathBarElement* element = iter->data;
      double intern = 0.0;
      if (iter->prev)
        {
          cairo_new_path (cr);
          cairo_move_to (cr, 0.5, 24.5);
          cairo_line_to (cr, 12.5, 12.5);
          cairo_line_to (cr, 0.5, 0.5);
          cairo_translate (cr, 12.0, 0.0);
          intern += 12.0;
        }
      cairo_translate (cr, 4.0, 0.0);
      intern += 4.0;

      if (element->icon_name)
        {
          /* FIXME: cache the pixbuf after realize() and update on every "style-set" */
          GdkPixbuf* buf = gtk_widget_render_icon (widget, element->icon_name, GTK_ICON_SIZE_MENU, NULL);
          cairo_save (cr);
          cairo_translate (cr, 0.0, 4.0);
          gdk_cairo_set_source_pixbuf (cr, buf, 0.0, 0.0);
          cairo_paint (cr);
          cairo_restore (cr);
          cairo_translate (cr, gdk_pixbuf_get_width (buf) + 4.0, 0.0);
          intern += gdk_pixbuf_get_width (buf) + 4.0;
          g_object_unref (buf);
        }
      if (element->label)
        {
          /* FIXME: cache the layout after realize() and update on every "style-set" */
          PangoLayout* layout = gtk_widget_create_pango_layout (widget, element->label);
          PangoRectangle  logical;
          cairo_path_t* path = cairo_copy_path (cr);

          pango_layout_get_extents (layout, NULL, &logical);

          cairo_save (cr);
          cairo_new_path (cr);
          cairo_translate (cr, 0.0, 18.0 - pango_layout_get_baseline (layout) / PANGO_SCALE);
          cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.75);
          pango_cairo_show_layout (cr, layout);
          cairo_restore (cr);

          cairo_new_path (cr);
          cairo_append_path (cr, path);
          cairo_translate (cr, logical.width / PANGO_SCALE + 4.0, 0.0);
          intern += logical.width / PANGO_SCALE + 4.0;
          cairo_path_destroy (path);
          g_object_unref (layout);
        }

      if (iter->next)
        {
          cairo_pattern_t* pattern = cairo_pattern_create_linear (-intern, 0.0, 0.0, 24.0);
          cairo_pattern_add_color_stop_rgba (pattern, 0.0, 0.0, 0.0, 0.0, 0.15);
          cairo_pattern_add_color_stop_rgba (pattern, 1.0, 1.0, 1.0, 1.0, 0.15);

          cairo_line_to (cr, 0.5, 0.5);
          cairo_line_to (cr, 12.5, 12.5);
          cairo_line_to (cr, 0.5, 24.5);
          cairo_close_path (cr);

          cairo_save (cr);
          cairo_set_source (cr, pattern);
          cairo_fill_preserve (cr);
          cairo_pattern_destroy (pattern);
          cairo_restore (cr);

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
  GObjectClass  * object_class = G_OBJECT_CLASS (self_class);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  object_class->finalize = finalize;

  widget_class->expose_event = expose_event;

  g_type_class_add_private (self_class, sizeof (ProgressPathBarPrivate));
}

void
progress_path_bar_append (ProgressPathBar* self,
                          gchar const    * icon,
                          gchar const    * label)
{
  ProgressPathBarElement* element;

  g_return_if_fail (PROGRESS_IS_PATH_BAR (self));

  element = g_slice_new (ProgressPathBarElement);
  element->icon_name = g_strdup (icon);
  element->label     = g_strdup (label);

  PRIV (self)->elements = g_list_append (PRIV (self)->elements, element);
}

GtkWidget*
progress_path_bar_new (void)
{
  return g_object_new (PROGRESS_TYPE_PATH_BAR,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
