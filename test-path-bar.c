/* This file is part of herzi's playground
 *
 * Copyright (C) 2010  Sven Herzberg
 *
 * This work is provided "as is"; redistribution and modification
 * in whole or in part, in any medium, physical or electronic is
 * permitted without restriction.
 *
 * This work is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * In no event shall the authors or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 */

#include "path-bar.h"
#include "path-element.h"

static void
test_widget_type (gpointer  data)
{
  GType  type = GPOINTER_TO_SIZE (data);

  g_assert (g_type_is_a (type, GTK_TYPE_WIDGET));
}

static void
add_gtk_widget_tests_for_type (GType  type)
{
  gchar const* name = g_type_name (type);
  GString* path = g_string_new ("");

  g_string_append_printf (path, "/%s/<<GtkWidget>>/GType", name);
  g_test_add_data_func (path->str, GSIZE_TO_POINTER (type), (void (*) ())test_widget_type);

  g_string_free (path, TRUE);
}

int
main (int   argc,
      char**argv)
{
  gtk_test_init (&argc, &argv, NULL);

  add_gtk_widget_tests_for_type (PROGRESS_TYPE_SIMPLE_WIDGET);
  add_gtk_widget_tests_for_type (PROGRESS_TYPE_SIMPLE_CONTAINER);
  add_gtk_widget_tests_for_type (PROGRESS_TYPE_PATH_BAR);
  add_gtk_widget_tests_for_type (PROGRESS_TYPE_PATH_ELEMENT);

  return g_test_run ();
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
