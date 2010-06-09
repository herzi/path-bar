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

#include "gdk-window.h"

struct _ProgressSimpleWidgetPrivate
{
  guint32    use_input_window : 1;
  GdkWindow* input_window;
};

struct _ProgressSimpleContainerPrivate
{
  GList* children;
};

#define PRIV(i) (((ProgressSimpleWidget*)(i))->_private)

G_DEFINE_TYPE (ProgressSimpleWidget, progress_simple_widget, GTK_TYPE_WIDGET);

static void
progress_simple_widget_init (ProgressSimpleWidget* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, PROGRESS_TYPE_SIMPLE_WIDGET, ProgressSimpleWidgetPrivate);

  gtk_widget_set_has_window (GTK_WIDGET (self), FALSE);
}

static void
widget_map (GtkWidget* widget)
{
  GTK_WIDGET_CLASS (progress_simple_widget_parent_class)->map (widget);

  if (PRIV (widget)->input_window)
    {
      gdk_window_show_unraised (PRIV (widget)->input_window);
    }
}

static void
widget_unmap (GtkWidget* widget)
{
  if (PRIV (widget)->input_window)
    {
      gdk_window_hide (PRIV (widget)->input_window);
    }

  GTK_WIDGET_CLASS (progress_simple_widget_parent_class)->unmap (widget);
}

static void
widget_size_allocate (GtkWidget    * widget,
                      GtkAllocation* allocation)
{
  GTK_WIDGET_CLASS (progress_simple_widget_parent_class)->size_allocate (widget, allocation);

  if (PRIV (widget)->input_window)
    {
      gdk_window_move_resize (PRIV (widget)->input_window,
                              allocation->x,
                              allocation->y,
                              allocation->width,
                              allocation->height);
    }
}

static void
widget_state_changed (GtkWidget   * widget,
                      GtkStateType  old_state)
{
  gtk_widget_queue_draw (widget);

  if (GTK_WIDGET_CLASS (progress_simple_widget_parent_class)->state_changed)
    {
      GTK_WIDGET_CLASS (progress_simple_widget_parent_class)->state_changed (widget, old_state);
    }
}

static void
widget_realize (GtkWidget* widget)
{
  GTK_WIDGET_CLASS (progress_simple_widget_parent_class)->realize (widget);

  if (PRIV (widget)->use_input_window)
    {
      PRIV (widget)->input_window = gfc_gdk_window_new (gtk_widget_get_parent_window (widget),
                                                        "class", GDK_INPUT_ONLY,
                                                        "x", widget->allocation.x,
                                                        "y", widget->allocation.y,
                                                        "width", widget->allocation.width,
                                                        "height", widget->allocation.height,
                                                        "event-mask", gtk_widget_get_events (widget) & (GDK_ALL_EVENTS_MASK & ~GDK_EXPOSURE_MASK),
                                                        NULL);

      gdk_window_set_user_data (PRIV (widget)->input_window, widget);
    }
}

static void
widget_unrealize (GtkWidget* widget)
{
  if (PRIV (widget)->input_window)
    {
      gdk_window_set_user_data (PRIV (widget)->input_window, NULL);
      gdk_window_destroy (PRIV (widget)->input_window);
      PRIV (widget)->input_window = NULL;
    }

  GTK_WIDGET_CLASS (progress_simple_widget_parent_class)->unrealize (widget);
}

static void
progress_simple_widget_class_init (ProgressSimpleWidgetClass* self_class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  widget_class->map           = widget_map;
  widget_class->unmap         = widget_unmap;
  widget_class->size_allocate = widget_size_allocate;
  widget_class->state_changed = widget_state_changed;
  widget_class->realize       = widget_realize;
  widget_class->unrealize     = widget_unrealize;

  g_type_class_add_private (self_class, sizeof (ProgressSimpleWidgetPrivate));
}

void
progress_simple_widget_set_use_input_window (ProgressSimpleWidget* self,
                                             gboolean              use_input_window)
{
  g_return_if_fail (PROGRESS_IS_SIMPLE_WIDGET (self));
  g_return_if_fail (use_input_window == TRUE || use_input_window == FALSE);

  if (GTK_WIDGET_REALIZED (self))
    {
      g_warning ("%s(%s): you cannot change \"use_input_window\" after the widget is realized",
                 G_STRFUNC, G_STRLOC);
      return;
    }

  if (use_input_window == PRIV (self)->use_input_window)
    {
      return;
    }

  PRIV (self)->use_input_window = use_input_window;
}

#undef PRIV
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
container_remove (GtkContainer* container,
                  GtkWidget   * widget)
{
  GList* iter;
  for (iter = PRIV (container)->children; iter; iter = iter->next)
    {
      if (iter->data == widget)
        {
          break;
        }
    }

  if (!iter)
    {
      g_warning ("trying to remove widget of type %s from container of type %s (but the widget wasn't in the container)",
                 G_OBJECT_TYPE_NAME (widget), G_OBJECT_TYPE_NAME (container));
      return;
    }

  PRIV (container)->children = g_list_remove_link (PRIV (container)->children, iter);
  gtk_widget_unparent (iter->data);
  gtk_widget_destroy (iter->data);
  g_object_unref (iter->data);
  g_list_free (iter);
}

static void
progress_simple_container_class_init (ProgressSimpleContainerClass* self_class)
{
  GObjectClass     * object_class = G_OBJECT_CLASS (self_class);
  GtkContainerClass* container_class = GTK_CONTAINER_CLASS (self_class);

  object_class->dispose = dispose;

  container_class->add    = add;
  container_class->forall = forall;
  container_class->remove = container_remove;

  g_type_class_add_private (self_class, sizeof (ProgressSimpleContainerPrivate));
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
