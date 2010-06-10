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
  gpointer sliff;
};

#define PRIV(i) (((ProgressPathBar*)(i))->_private)

G_DEFINE_TYPE (ProgressPathBar, progress_path_bar, PROGRESS_TYPE_SIMPLE_CONTAINER);

static void
progress_path_bar_init (ProgressPathBar* self G_GNUC_UNUSED)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, PROGRESS_TYPE_PATH_BAR, ProgressPathBarPrivate);
}

static void
finalize (GObject* object)
{
  G_OBJECT_CLASS (progress_path_bar_parent_class)->finalize (object);
}

static gboolean
expose_event (GtkWidget     * widget,
              GdkEventExpose* event)
{
  cairo_t* cr = gdk_cairo_create (event->window);
  GList  * children;
  GList  * iter;
  gdouble  x = 0.0;

  gdk_cairo_region (cr, event->region);
  cairo_clip (cr);

  /* FIXME: respect allocation.x */
  /* FIXME: respect alloaction.y */
  /* FIXME: respect allocation.height */
  /* FIXME: respect allocation.width */

  cairo_set_line_width (cr, 1.0);
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.5);

  children = gtk_container_get_children (GTK_CONTAINER (widget));
  for (iter = children; iter; iter = iter->next)
    {
      ProgressPathElement* element = iter->data;
      cairo_pattern_t    * pattern;
      double               intern = 0.0;

      if (iter->prev)
        {
          cairo_new_path (cr);
          cairo_move_to (cr, x + 0.5, 23.5);
          cairo_line_to (cr, x + 12.5, 12.0);
          cairo_line_to (cr, x + 0.5, 0.5);
          intern += 12.0;
        }
      else
        {
          cairo_arc (cr, x + 4.5, 19.5, 4.0, 0.5 * G_PI, G_PI);
          cairo_arc (cr, x + 4.5, 4.5, 4.0, G_PI, 1.5 * G_PI);
        }
      intern += 4.0 + GTK_WIDGET (element)->allocation.width + 4.0;

      pattern = cairo_pattern_create_linear (x, 0.0, x + intern, 24.0);
      cairo_pattern_add_color_stop_rgba (pattern, 0.0, 0.0, 0.0, 0.0, 0.15);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 1.0, 1.0, 1.0, 0.15);

      if (iter->next)
        {
          cairo_line_to (cr, x + intern + 0.5, 0.5);
          cairo_line_to (cr, x + intern + 12.5, 12.0);
          cairo_line_to (cr, x + intern + 0.5, 23.5);

          x += intern + 1;
        }
      else
        {
          cairo_arc (cr, widget->allocation.width - 4.5, 4.5, 4.0, 1.5 * G_PI, 2.0 * G_PI);
          cairo_arc (cr, widget->allocation.width - 4.5, 19.5, 4.0, 0.0, 0.5 * G_PI);
        }

      cairo_close_path (cr);

      cairo_save (cr);
      cairo_set_source (cr, pattern);
      cairo_fill_preserve (cr);
      cairo_pattern_destroy (pattern);
      cairo_restore (cr);

      gtk_container_propagate_expose (GTK_CONTAINER (widget), GTK_WIDGET (element), event);

      cairo_stroke (cr);
    }
  g_list_free (children);

  cairo_destroy (cr);

  return FALSE;
}

static void
size_allocate (GtkWidget    * widget,
               GtkAllocation* allocation)
{
  GtkAllocation  child_allocation;
  GList        * children;
  GList        * iter;

  GTK_WIDGET_CLASS (progress_path_bar_parent_class)->size_allocate (widget, allocation);

  child_allocation.x = allocation->x + 4;
  child_allocation.y = allocation->y;
  child_allocation.height = allocation->height;

  children = gtk_container_get_children (GTK_CONTAINER (widget));
  for (iter = children; iter; iter = iter->next)
    {
      ProgressPathElement* element = iter->data;
      gboolean             has_icon = FALSE;

      child_allocation.width = 0;

      if (iter->prev)
        {
          child_allocation.x += 12;
        }

      child_allocation.width = GTK_WIDGET (element)->requisition.width;

      gtk_widget_size_allocate (iter->data, &child_allocation);

      child_allocation.x += child_allocation.width + 8 + 1;
    }
  g_list_free (children);
}

static void
size_request (GtkWidget     * widget,
              GtkRequisition* req)
{
  GList* children;
  GList* iter;
  int    req_width = 0;

  /* FIXME: turn the padding into a #define'd contant, later into a style property */

  children = gtk_container_get_children (GTK_CONTAINER (widget));
  for (iter = children; iter; iter = iter->next)
    {
      GtkRequisition  child_requisition;

      gtk_widget_size_request (iter->data, &child_requisition);
      ProgressPathElement* element = iter->data;

      if (iter->prev)
        {
          req_width += 12;
        }
      req_width += 4;

      req_width += child_requisition.width;

      if (iter->next)
        {
          req_width += 1;
        }
      req->height = MAX (req->height, 2 * 4 + child_requisition.height);
    }
  g_list_free (children);

  req->width = MAX (req->width, req_width);

  GTK_WIDGET_CLASS (progress_path_bar_parent_class)->size_request (widget, req);
}

static void
progress_path_bar_class_init (ProgressPathBarClass* self_class)
{
  GObjectClass  * object_class = G_OBJECT_CLASS (self_class);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  object_class->finalize = finalize;

  widget_class->expose_event  = expose_event;
  widget_class->size_allocate = size_allocate;
  widget_class->size_request  = size_request;

  g_type_class_add_private (self_class, sizeof (ProgressPathBarPrivate));
}

void
progress_path_bar_append (ProgressPathBar* self,
                          gchar const    * icon,
                          gchar const    * label)
{
  ProgressPathElement* element;

  g_return_if_fail (PROGRESS_IS_PATH_BAR (self));

  element = PROGRESS_PATH_ELEMENT (progress_path_element_new (icon, label));
  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (element));
}

GtkWidget*
progress_path_bar_new (void)
{
  return g_object_new (PROGRESS_TYPE_PATH_BAR,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
