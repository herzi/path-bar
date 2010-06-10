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

static void
size_allocate (GtkWidget    * widget,
               GtkAllocation* allocation)
{
  GtkAllocation  child_allocation;
  GList        * children;
  GList        * iter;

  GTK_WIDGET_CLASS (progress_path_bar_parent_class)->size_allocate (widget, allocation);

  child_allocation.x = allocation->x;
  child_allocation.y = allocation->y;
  child_allocation.height = allocation->height;

  children = gtk_container_get_children (GTK_CONTAINER (widget));
  for (iter = children; iter; iter = iter->next)
    {
      ProgressPathElement* element = iter->data;

      child_allocation.width = GTK_WIDGET (element)->requisition.width;

      gtk_widget_size_allocate (iter->data, &child_allocation);

      child_allocation.x += child_allocation.width - 12;
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

      req_width += child_requisition.width;
      if (iter->prev)
        {
          req_width -= 12;
        }

      req->height = MAX (req->height, child_requisition.height);
    }
  g_list_free (children);

  req->width = MAX (req->width, req_width);

  GTK_WIDGET_CLASS (progress_path_bar_parent_class)->size_request (widget, req);
}

static void
add (GtkContainer* container,
     GtkWidget   * child)
{
  GList* children = gtk_container_get_children (container);
  if (!children && PROGRESS_IS_PATH_ELEMENT (child))
    {
      /* TODO: consider moving this into an extra storage space on every widget (e.g. PackingInfo)
       * then update the packing info from a vfunc in the container */
      progress_path_element_set_first (PROGRESS_PATH_ELEMENT (child), TRUE);
      progress_path_element_set_last (PROGRESS_PATH_ELEMENT (child), TRUE);
    }
  if (children)
    {
      GList* last = g_list_last (children);
      if (PROGRESS_IS_PATH_ELEMENT (last->data))
        {
          progress_path_element_set_last (PROGRESS_PATH_ELEMENT (last->data), FALSE);
        }
      if (PROGRESS_IS_PATH_ELEMENT (child))
        {
          progress_path_element_set_last (PROGRESS_PATH_ELEMENT (child), TRUE);
        }
    }
  g_list_free (children);

  GTK_CONTAINER_CLASS (progress_path_bar_parent_class)->add (container, child);
}

static void
progress_path_bar_class_init (ProgressPathBarClass* self_class)
{
  GObjectClass     * object_class    = G_OBJECT_CLASS (self_class);
  GtkWidgetClass   * widget_class    = GTK_WIDGET_CLASS (self_class);
  GtkContainerClass* container_class = GTK_CONTAINER_CLASS (self_class);

  object_class->finalize = finalize;

  widget_class->size_allocate = size_allocate;
  widget_class->size_request  = size_request;

  container_class->add = add;

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
