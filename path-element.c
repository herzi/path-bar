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

#include "path-element.h"

G_DEFINE_TYPE (ProgressPathElement, progress_path_element, PROGRESS_TYPE_SIMPLE_WIDGET);

static void
progress_path_element_init (ProgressPathElement* self G_GNUC_UNUSED)
{}

static void
progress_path_element_class_init (ProgressPathElementClass* self_class G_GNUC_UNUSED)
{}

GtkWidget*
progress_path_element_new (void)
{
  return g_object_new (PROGRESS_TYPE_PATH_ELEMENT,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
