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
#include "shaped.h"

struct _ProgressPathBarPrivate
{
  ProgressPathElement* active;

  /* coordinates of the last hit testing and the widgets we hit */
  gdouble x;
  gdouble y;
  GList* hits;
};

#define PRIV(i) (((ProgressPathBar*)(i))->_private)

static void update_hits (GtkWidget* widget,
                         GdkEvent * event);

G_DEFINE_TYPE (ProgressPathBar, progress_path_bar, PROGRESS_TYPE_SIMPLE_CONTAINER);

static void
progress_path_bar_init (ProgressPathBar* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, PROGRESS_TYPE_PATH_BAR, ProgressPathBarPrivate);

  /* FIXME: also enable GDK_POINTER_MOTION_HINT_MASK */
  gtk_widget_add_events (GTK_WIDGET (self),
                         GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                         GDK_POINTER_MOTION_MASK | GDK_LEAVE_NOTIFY_MASK);
  progress_simple_widget_set_use_input_window (PROGRESS_SIMPLE_WIDGET (self), TRUE);
}

static void
finalize (GObject* object)
{
  G_OBJECT_CLASS (progress_path_bar_parent_class)->finalize (object);
}

static gboolean
button_press_event (GtkWidget     * widget,
                    GdkEventButton* event)
{
  GList* iter;

  update_hits (widget, (GdkEvent*) event);

  for (iter = PRIV (widget)->hits; iter; iter = iter->next)
    {
      if (gtk_widget_event (iter->data, (GdkEvent*) event))
        {
          return TRUE;
        }
    }

  if (GTK_WIDGET_CLASS (progress_path_bar_parent_class)->button_press_event)
    {
      return GTK_WIDGET_CLASS (progress_path_bar_parent_class)->button_press_event (widget, event);
    }

  return FALSE;
}

static gboolean
button_release_event (GtkWidget     * widget,
                      GdkEventButton* event)
{
  GList* iter;

  update_hits (widget, (GdkEvent*) event);

  for (iter = PRIV (widget)->hits; iter; iter = iter->next)
    {
      if (gtk_widget_event (iter->data, (GdkEvent*) event))
        {
          return TRUE;
        }
    }

  if (GTK_WIDGET_CLASS (progress_path_bar_parent_class)->button_release_event)
    {
      return GTK_WIDGET_CLASS (progress_path_bar_parent_class)->button_release_event (widget, event);
    }

  return FALSE;
}

static void
recreate_hits (GtkWidget* widget,
               gpointer   user_data)
{
  GdkEventMotion* event;
  gpointer        container;
  GList         **hits;
  gdouble         x;
  gdouble         y;
  gint            x_i;
  gint            y_i;

  hits = user_data;
  event = (*hits)->data;

  x = event->x;
  y = event->y;

  gdk_window_get_user_data (event->window, &container);
  gtk_widget_translate_coordinates (container,
                                    widget,
                                    x - GTK_WIDGET (container)->allocation.x, y - GTK_WIDGET (container)->allocation.y,
                                    &x_i,
                                    &y_i);
  event->x = x_i + widget->allocation.x;
  event->y = y_i + widget->allocation.y;

  if (progress_shaped_test_hit (PROGRESS_SHAPED (widget), event))
    {
      if (G_UNLIKELY ((*hits)->next))
        {
          *hits = g_list_insert_before (*hits, (*hits)->next, g_object_ref (widget));
        }
      else
        {
          *hits = g_list_append (*hits, g_object_ref (widget));
        }
    }

  event->x = x;
  event->y = y;
}

static int
compare_hits (gconstpointer a, gconstpointer b)
{
  return a - b;
}

static inline void
emit_enter (GtkWidget* parent G_GNUC_UNUSED,
            GtkWidget* child,
            GdkEvent * event)
{
  GdkEvent* e = gdk_event_new (GDK_ENTER_NOTIFY);

  switch (event->type)
    {
      GEnumClass* enums;
      GEnumValue* value;
    case GDK_MOTION_NOTIFY:
      e->crossing.window = g_object_ref (event->motion.window);
      e->crossing.send_event = FALSE;
      e->crossing.subwindow = NULL;
      e->crossing.time = event->motion.time;
      e->crossing.x = event->motion.x;
      e->crossing.y = event->motion.y;
      e->crossing.x_root = event->motion.x_root;
      e->crossing.y_root = event->motion.y_root;
      e->crossing.mode = GDK_CROSSING_NORMAL;
      e->crossing.detail = GDK_NOTIFY_UNKNOWN;
      e->crossing.focus = FALSE;
      e->crossing.state = event->motion.state;
      break;
    default:
      enums = g_type_class_ref (GDK_TYPE_EVENT_TYPE);
      value = g_enum_get_value (enums, event->type);
      if (!value)
        {
          g_warning ("%s(%s): unknown GdkEvent type value %d",
                     G_STRFUNC, G_STRLOC,
                     event->type);
        }
      else
        {
          g_warning ("%s(%s): unhandled GdkEvent type %s",
                     G_STRFUNC, G_STRLOC,
                     value->value_nick);
        }
      g_type_class_unref (enums);
      gdk_event_free (e);
      return;
    }

  gtk_widget_event (child, e);

  gdk_event_free (e);
}

static inline void
emit_leave (GtkWidget* parent G_GNUC_UNUSED,
            GtkWidget* child,
            GdkEvent * event)
{
  GdkEvent* e = gdk_event_new (GDK_LEAVE_NOTIFY);

  switch (event->type)
    {
    case GDK_MOTION_NOTIFY:
      e->crossing.window = g_object_ref (event->motion.window);
      e->crossing.time = event->motion.time;
      e->crossing.x = event->motion.x;
      e->crossing.y = event->motion.y;
      e->crossing.x_root = event->motion.x_root;
      e->crossing.y_root = event->motion.y_root;
      e->crossing.state = event->motion.state;
      break;
    case GDK_LEAVE_NOTIFY:
      *e = *event;
      g_object_ref (e->crossing.window);
      break;
    default:
      g_warning ("%s(%s): unexpected event %d",
                 G_STRFUNC, G_STRLOC,
                 event->type);
      gdk_event_free (e);
      return;
    }
  e->crossing.send_event = FALSE;
  e->crossing.subwindow = NULL;
  e->crossing.mode = GDK_CROSSING_NORMAL;
  e->crossing.detail = GDK_NOTIFY_UNKNOWN;
  e->crossing.focus = FALSE;


  gtk_widget_event (child, e);

  gdk_event_free (e);
}

static void
update_hits (GtkWidget* widget,
             GdkEvent * event)
{
  GList* hits = g_list_prepend (NULL, event);
  GList* new;
  GList* old;

  gdouble x;
  gdouble y;

  gdk_event_get_coords (event, &x, &y);

  gtk_container_forall (GTK_CONTAINER (widget), recreate_hits, &hits);

  hits = g_list_delete_link (hits, hits);
  hits = g_list_sort (hits, compare_hits);

  for (new = hits, old = PRIV (widget)->hits; new || old; )
    {
      if (!new)
        {
          emit_leave (widget, old->data, (GdkEvent*) event);

          old = old->next;
        }
      else if (!old)
        {
          emit_enter (widget, new->data, (GdkEvent*) event);

          new = new->next;
        }
      else /* no one of the two is NULL */
        {
          if (old->data < new->data)
            {
              emit_leave (widget, old->data, (GdkEvent*) event);
              old = old->next;
            }
          else if (old->data > new->data)
            {
              emit_enter (widget, new->data, event);
              new = new->next;
            }
          else /* both are equal */
            {
              new = new->next;
              old = old->next;
            }
        }
    }

  g_list_foreach (PRIV (widget)->hits, (GFunc)g_object_unref, NULL);
  g_list_free (PRIV (widget)->hits);
  PRIV (widget)->hits = hits;
}

static gboolean
motion_notify_event (GtkWidget     * widget,
                     GdkEventMotion* event)
{
  update_hits (widget, (GdkEvent*) event);

  if (GTK_WIDGET_CLASS (progress_path_bar_parent_class)->motion_notify_event)
    {
      return GTK_WIDGET_CLASS (progress_path_bar_parent_class)->motion_notify_event (widget, event);
    }

  return FALSE;
}

static gboolean
leave_notify_event (GtkWidget       * widget,
                    GdkEventCrossing* event)
{
  GList* iter;
  for (iter = PRIV (widget)->hits; iter; iter = iter->next)
    {
      emit_leave (widget, iter->data, (GdkEvent*) event);
      g_object_unref (iter->data);
    }
  g_list_free (PRIV (widget)->hits);
  PRIV (widget)->hits = NULL;

  if (GTK_WIDGET_CLASS (progress_path_bar_parent_class)->leave_notify_event)
    {
      return GTK_WIDGET_CLASS (progress_path_bar_parent_class)->leave_notify_event (widget, event);
    }

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
element_clicked (ProgressPathElement* element,
                 ProgressPathBar    * self)
{
  /* FIXME: refactor this into a ProgressPathGroup */
  if (PRIV (self)->active == element)
    {
      return;
    }

  if (PRIV (self)->active)
    {
      progress_path_element_unselect (PRIV (self)->active);
      g_object_unref (PRIV (self)->active);
    }
  if (element)
    {
      PRIV (self)->active = g_object_ref (element);
      progress_path_element_select (PRIV (self)->active);
    }
}

static void
add (GtkContainer* container,
     GtkWidget   * child)
{
  GList* children = gtk_container_get_children (container);

  GTK_CONTAINER_CLASS (progress_path_bar_parent_class)->add (container, child);

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
          element_clicked (PROGRESS_PATH_ELEMENT (child), PROGRESS_PATH_BAR (container));
        }
    }
  g_list_free (children);

  if (PROGRESS_IS_PATH_ELEMENT (child))
    {
      g_signal_connect (child, "clicked",
                        G_CALLBACK (element_clicked), container);
    }
}

static void
progress_path_bar_class_init (ProgressPathBarClass* self_class)
{
  GObjectClass     * object_class    = G_OBJECT_CLASS (self_class);
  GtkWidgetClass   * widget_class    = GTK_WIDGET_CLASS (self_class);
  GtkContainerClass* container_class = GTK_CONTAINER_CLASS (self_class);

  object_class->finalize = finalize;

  widget_class->button_press_event   = button_press_event;
  widget_class->button_release_event = button_release_event;
  widget_class->motion_notify_event  = motion_notify_event;
  widget_class->leave_notify_event   = leave_notify_event;
  widget_class->size_allocate        = size_allocate;
  widget_class->size_request         = size_request;

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
