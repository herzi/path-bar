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

#ifndef PATH_ELEMENT_H
#define PATH_ELEMENT_H

#include <simple-widgets.h>

G_BEGIN_DECLS

typedef struct _ProgressPathElement        ProgressPathElement;
typedef struct _ProgressPathElementClass   ProgressPathElementClass;
typedef struct _ProgressPathElementPrivate ProgressPathElementPrivate;

#define PROGRESS_TYPE_PATH_ELEMENT         (progress_path_element_get_type ())
#define PROGRESS_PATH_ELEMENT(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), PROGRESS_TYPE_PATH_ELEMENT, ProgressPathElement))

#define PROGRESS_IS_PATH_ELEMENT(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), PROGRESS_TYPE_PATH_ELEMENT))

GType      progress_path_element_get_type  (void);
GtkWidget* progress_path_element_new       (gchar const        * icon,
                                            gchar const        * label);
void       progress_path_element_select    (ProgressPathElement* self);
void       progress_path_element_set_first (ProgressPathElement* self,
                                            gboolean             first);
void       progress_path_element_set_last  (ProgressPathElement* self,
                                            gboolean             last);
void       progress_path_element_unselect  (ProgressPathElement* self);

struct _ProgressPathElement
{
  ProgressSimpleWidgetImpl       base_instance;
  guint32       first : 1;
  guint32       last : 1;
  guint32       button_press_time;
  cairo_path_t* path;
  gchar      * icon_name;
  GdkPixbuf  * icon;
  gchar      * label;
  PangoLayout* layout;
  ProgressPathElementPrivate   * _private;
};

struct _ProgressPathElementClass
{
  ProgressSimpleWidgetImplClass  base_class;
};

G_END_DECLS

#endif /* !PATH_ELEMENT_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
