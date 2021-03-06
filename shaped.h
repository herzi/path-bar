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

#ifndef SHAPED_H
#define SHAPED_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ProgressShaped      ProgressShaped;
typedef struct _ProgressShapedIface ProgressShapedIface;

#define PROGRESS_TYPE_SHAPED         (progress_shaped_get_type ())
#define PROGRESS_SHAPED(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), PROGRESS_TYPE_SHAPED, ProgressShaped))
#define PROGRESS_IS_SHAPED(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), PROGRESS_TYPE_SHAPED))
#define PROGRESS_SHAPED_GET_IFACE(i) (G_TYPE_INSTANCE_GET_INTERFACE ((i), PROGRESS_TYPE_SHAPED, ProgressShapedIface))

GType     progress_shaped_get_type (void);
gboolean  progress_shaped_test_hit (ProgressShaped* self,
                                    GdkEventMotion* event);

struct _ProgressShapedIface
{
  GTypeInterface  base_interface;

  /* vtable */
  gboolean (*test_hit) (ProgressShaped* self,
                        GdkEventMotion* event);
};

G_END_DECLS

#endif /* !SHAPED_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
