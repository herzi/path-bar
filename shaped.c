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

#include "shaped.h"

static gboolean
widget_test_hit (ProgressShaped* shape,
                 GdkEventMotion* event)
{
  GtkAllocation  allocation;

  gtk_widget_get_allocation (GTK_WIDGET (shape), &allocation);

  if (event->x < allocation.x)
    {
      return FALSE;
    }

  if (event->y < allocation.y)
    {
      return FALSE;
    }

  if (event->x >= allocation.x + allocation.width)
    {
      return FALSE;
    }

  if (event->y >= allocation.y + allocation.height)
    {
      return FALSE;
    }

  return TRUE;
}

static void
implement_for_widget (ProgressShapedIface* iface)
{
  iface->test_hit = widget_test_hit;
}

GType
progress_shaped_get_type (void)
{
  static GType  stored = 0;

  if (g_once_init_enter (&stored))
    {
      GType  registered = g_type_register_static_simple (G_TYPE_INTERFACE,
                                                         "ProgressShaped",
                                                         sizeof (ProgressShapedIface),
                                                         NULL,
                                                         0, NULL,
                                                         0);
      GInterfaceInfo  info =
        {
          (GInterfaceInitFunc) implement_for_widget,
          NULL,
          NULL
        };

      g_type_add_interface_static (GTK_TYPE_WIDGET, registered,
                                   &info);

      g_once_init_leave (&stored, registered);
    }

  return stored;
}

gboolean
progress_shaped_test_hit (ProgressShaped* self,
                          GdkEventMotion* event)
{
  ProgressShapedIface* iface;

  g_return_val_if_fail (PROGRESS_IS_SHAPED (self), FALSE);
  g_return_val_if_fail (event, FALSE);

  iface = PROGRESS_SHAPED_GET_IFACE (self);

  if (!iface->test_hit)
    {
      g_warning ("%s(%s): %s doesn't implement ProgressShaped::hit_test()",
                 G_STRFUNC, G_STRLOC,
                 G_OBJECT_TYPE_NAME (self));
      return FALSE;
    }
  else
    {
      return iface->test_hit (self, event);
    }
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
