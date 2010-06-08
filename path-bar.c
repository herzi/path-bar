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

#include "path-element.h"

struct _ProgressPathBarPrivate
{
  GList* elements;
};

#define PRIV(i) (((ProgressPathBar*)(i))->_private)

G_DEFINE_TYPE (ProgressPathBar, progress_path_bar, PROGRESS_TYPE_SIMPLE_CONTAINER);

static void
progress_path_bar_init (ProgressPathBar* self G_GNUC_UNUSED)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, PROGRESS_TYPE_PATH_BAR, ProgressPathBarPrivate);
}

static void
free_element (gpointer  data,
              gpointer  user_data G_GNUC_UNUSED)
{
  ProgressPathElement* element = data;
  g_free (element->icon_name);
  if (element->icon)
    {
      g_object_unref (element->icon);
    }
  g_free (element->label);
  if (element->layout)
    {
      g_object_unref (element->layout);
    }
  g_object_unref (element);
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

  /* FIXME: respect allocation.x */
  /* FIXME: respect alloaction.y */
  /* FIXME: respect allocation.height */
  /* FIXME: respect allocation.width */

  cairo_set_line_width (cr, 1.0);
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.5);
  cairo_save (cr);
  cairo_arc (cr, 4.5, 19.5, 4.0, 0.5 * G_PI, G_PI);
  cairo_arc (cr, 4.5, 4.5, 4.0, G_PI, 1.5 * G_PI);
  for (iter = PRIV (widget)->elements; iter; iter = iter->next)
    {
      ProgressPathElement* element = iter->data;
      double intern = 0.0;
      if (iter->prev)
        {
          cairo_new_path (cr);
          cairo_move_to (cr, 0.5, 23.5);
          cairo_line_to (cr, 12.5, 12.0);
          cairo_line_to (cr, 0.5, 0.5);
          cairo_translate (cr, 12.0, 0.0);
          intern += 12.0;
        }
      cairo_translate (cr, 4.0, 0.0);
      intern += 4.0;

      if (element->icon)
        {
          cairo_save (cr);
          cairo_translate (cr, 0.0, 4.0);
          gdk_cairo_set_source_pixbuf (cr, element->icon, 0.0, 0.0);
          cairo_paint (cr);
          cairo_restore (cr);
          cairo_translate (cr, gdk_pixbuf_get_width (element->icon) + 4.0, 0.0);
          intern += gdk_pixbuf_get_width (element->icon) + 4.0;
        }
      if (element->layout)
        {
          PangoRectangle  logical;
          cairo_path_t* path = cairo_copy_path (cr);

          pango_layout_get_extents (element->layout, NULL, &logical);

          cairo_save (cr);
          cairo_new_path (cr);
          cairo_translate (cr, 0.0, 18.0 - pango_layout_get_baseline (element->layout) / PANGO_SCALE);
          cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.75);
          pango_cairo_show_layout (cr, element->layout);
          cairo_restore (cr);

          cairo_new_path (cr);
          cairo_append_path (cr, path);
          cairo_translate (cr, logical.width / PANGO_SCALE + 4.0, 0.0);
          intern += logical.width / PANGO_SCALE + 4.0;
          cairo_path_destroy (path);
        }

      if (iter->next)
        {
          cairo_pattern_t* pattern = cairo_pattern_create_linear (-intern, 0.0, 0.0, 24.0);
          cairo_pattern_add_color_stop_rgba (pattern, 0.0, 0.0, 0.0, 0.0, 0.15);
          cairo_pattern_add_color_stop_rgba (pattern, 1.0, 1.0, 1.0, 1.0, 0.15);

          cairo_line_to (cr, 0.5, 0.5);
          cairo_line_to (cr, 12.5, 12.0);
          cairo_line_to (cr, 0.5, 23.5);
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
  cairo_arc (cr, widget->allocation.width - 4.5, 19.5, 4.0, 0.0, 0.5 * G_PI);
  cairo_close_path (cr);
  cairo_stroke (cr);

  cairo_destroy (cr);
  return FALSE;
}

static void
size_request (GtkWidget     * widget,
              GtkRequisition* req)
{
  GList* iter;
  int    req_width = 0;

  /* FIXME: turn the padding into a #define'd contant, later into a style property */

  for (iter = PRIV (widget)->elements; iter; iter = iter->next)
    {
      ProgressPathElement* element = iter->data;

      if (iter->prev)
        {
          req_width += 12;
        }
      req_width += 4;

      if (element->icon)
        {
          req->height = MAX (req->height, 2 * 4 + gdk_pixbuf_get_height (element->icon));
          req_width += gdk_pixbuf_get_width (element->icon) + 4;
        }
      if (element->layout)
        {
          PangoRectangle  rectangle;
          pango_layout_get_extents (element->layout, NULL, &rectangle);
          req->height = MAX (req->height, 2 * 4 + rectangle.height / PANGO_SCALE);
          req_width += PANGO_PIXELS_CEIL (rectangle.width) + 4;
        }
    }

  req->width = MAX (req->width, req_width);

  GTK_WIDGET_CLASS (progress_path_bar_parent_class)->size_request (widget, req);
}

static void
style_set (GtkWidget* widget,
           GtkStyle * old_style)
{
  GList* iter;

  if (GTK_WIDGET_CLASS (progress_path_bar_parent_class)->style_set)
    {
      GTK_WIDGET_CLASS (progress_path_bar_parent_class)->style_set (widget, old_style);
    }

  for (iter = PRIV (widget)->elements; iter; iter = iter->next)
    {
      ProgressPathElement* element = iter->data;

      if (element->icon)
        {
          g_object_unref (element->icon);
          element->icon = gtk_widget_render_icon (widget, element->icon_name, GTK_ICON_SIZE_MENU, NULL);
        }
      if (element->layout)
        {
          g_object_unref (element->layout);
          element->layout = gtk_widget_create_pango_layout (widget, element->label);
        }
    }
}

static void
progress_path_bar_class_init (ProgressPathBarClass* self_class)
{
  GObjectClass  * object_class = G_OBJECT_CLASS (self_class);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  object_class->finalize = finalize;

  widget_class->expose_event = expose_event;
  widget_class->size_request = size_request;
  widget_class->style_set    = style_set;

  g_type_class_add_private (self_class, sizeof (ProgressPathBarPrivate));
}

void
progress_path_bar_append (ProgressPathBar* self,
                          gchar const    * icon,
                          gchar const    * label)
{
  ProgressPathElement* element;

  g_return_if_fail (PROGRESS_IS_PATH_BAR (self));

  element = PROGRESS_PATH_ELEMENT (progress_path_element_new ());
  g_object_ref_sink (element);

  element->icon_name = g_strdup (icon);
  element->icon      = NULL;
  if (element->icon_name)
    {
      element->icon = gtk_widget_render_icon (GTK_WIDGET (self), element->icon_name, GTK_ICON_SIZE_MENU, NULL);
    }

  element->label  = g_strdup (label);
  element->layout = NULL;
  if (element->label)
    {
      element->layout = gtk_widget_create_pango_layout (GTK_WIDGET (self), element->label);
    }

  PRIV (self)->elements = g_list_append (PRIV (self)->elements, element);
}

GtkWidget*
progress_path_bar_new (void)
{
  return g_object_new (PROGRESS_TYPE_PATH_BAR,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
