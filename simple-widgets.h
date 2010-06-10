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

#ifndef SIMPLE_WIDGETS_H
#define SIMPLE_WIDGETS_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ProgressSimpleWidget        ProgressSimpleWidget;
typedef struct _ProgressSimpleWidgetIface   ProgressSimpleWidgetIface;

typedef struct _ProgressSimpleWidgetImpl        ProgressSimpleWidgetImpl;
typedef struct _ProgressSimpleWidgetImplClass   ProgressSimpleWidgetImplClass;
typedef struct _ProgressSimpleWidgetImplPrivate ProgressSimpleWidgetImplPrivate;

typedef struct _ProgressSimpleContainer        ProgressSimpleContainer;
typedef struct _ProgressSimpleContainerClass   ProgressSimpleContainerClass;
typedef struct _ProgressSimpleContainerPrivate ProgressSimpleContainerPrivate;

#define PROGRESS_TYPE_SIMPLE_WIDGET_IMPL         (progress_simple_widget_impl_get_type ())

#define PROGRESS_TYPE_SIMPLE_WIDGET         (progress_simple_widget_get_type ())
#define PROGRESS_SIMPLE_WIDGET(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), PROGRESS_TYPE_SIMPLE_WIDGET, ProgressSimpleWidget))
#define PROGRESS_IS_SIMPLE_WIDGET(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), PROGRESS_TYPE_SIMPLE_WIDGET))

#define PROGRESS_TYPE_SIMPLE_CONTAINER      (progress_simple_container_get_type ())

GType  progress_simple_widget_get_type             (void);
void   progress_simple_widget_set_use_input_window (ProgressSimpleWidget* widget,
                                                    gboolean              use_input_window);

GType  progress_simple_widget_impl_get_type (void);

GType  progress_simple_container_get_type (void);

struct _ProgressSimpleWidgetIface
{
  GTypeInterface  base_interface;
};

struct _ProgressSimpleWidgetImpl
{
  GtkWidget                        base_instance;
  ProgressSimpleWidgetImplPrivate* _private;
};

struct _ProgressSimpleWidgetImplClass
{
  GtkWidgetClass               base_class;
};

struct _ProgressSimpleContainer
{
  GtkContainer                    base_instance;
  ProgressSimpleContainerPrivate* _private;
};

struct _ProgressSimpleContainerClass
{
  GtkContainerClass               base_class;
};

G_END_DECLS

#endif /* !SIMPLE_WIDGETS_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
