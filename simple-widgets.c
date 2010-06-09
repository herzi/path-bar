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

#include "simple-widgets.h"

struct _ProgressSimpleContainerPrivate
{
  GList* children;
};

G_DEFINE_TYPE (ProgressSimpleWidget, progress_simple_widget, GTK_TYPE_WIDGET);

static void
progress_simple_widget_init (ProgressSimpleWidget* self)
{
  gtk_widget_set_has_window (GTK_WIDGET (self), FALSE);
}

static void
progress_simple_widget_class_init (ProgressSimpleWidgetClass* self_class G_GNUC_UNUSED)
{}

#define PRIV(i) (((ProgressSimpleContainer*)(i))->_private)

G_DEFINE_TYPE (ProgressSimpleContainer, progress_simple_container, GTK_TYPE_CONTAINER);

static void
progress_simple_container_init (ProgressSimpleContainer* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, PROGRESS_TYPE_SIMPLE_CONTAINER, ProgressSimpleContainerPrivate);

  gtk_widget_set_has_window (GTK_WIDGET (self), FALSE);
}

static void
dispose (GObject* object)
{
  g_list_foreach (PRIV (object)->children, (GFunc)g_object_unref, NULL);
  g_list_free (PRIV (object)->children);
  PRIV (object)->children = NULL;

  G_OBJECT_CLASS (progress_simple_container_parent_class)->dispose (object);
}

static void
add (GtkContainer* container,
     GtkWidget   * child)
{
  /* TODO: consider using a GQueue or GSequence for the children */
  PRIV (container)->children = g_list_append (PRIV (container)->children, g_object_ref_sink (child));

  gtk_widget_set_parent (child, GTK_WIDGET (container));
}

static void
forall (GtkContainer* container,
        gboolean      include_internal G_GNUC_UNUSED,
        GtkCallback   callback,
        gpointer      user_data)
{
  GList* iter;

  for (iter = PRIV (container)->children; iter; iter = iter->next)
    {
      callback (iter->data, user_data);
    }
}

static void
progress_simple_container_class_init (ProgressSimpleContainerClass* self_class)
{
  GObjectClass     * object_class = G_OBJECT_CLASS (self_class);
  GtkContainerClass* container_class = GTK_CONTAINER_CLASS (self_class);

  object_class->dispose = dispose;

  container_class->add    = add;
  container_class->forall = forall;

  g_type_class_add_private (self_class, sizeof (ProgressSimpleContainerPrivate));
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
