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
test_widget_life_cycle (gpointer  data)
{
  GType      type = GPOINTER_TO_SIZE (data);
  GtkWidget* window = gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);
  GtkWidget* widget = gtk_test_create_widget (type, NULL);

  gtk_container_add (GTK_CONTAINER (window), widget);

  gtk_widget_show_all (window);
}

static void
add_gtk_widget_tests_for_type (GType  type)
{
  gchar const* name = g_type_name (type);
  GString* path = g_string_new ("");

  g_string_append_printf (path, "/%s/<<GtkWidget>>/GType", name);
  g_test_add_data_func (path->str, GSIZE_TO_POINTER (type), (void (*) ())test_widget_type);
  g_string_set_size (path, 0);

  g_string_append_printf (path, "/%s/<<GtkWidget>>/life-cycle", name);
  g_test_add_data_func (path->str, GSIZE_TO_POINTER (type), (void (*) ())test_widget_life_cycle);

  g_string_free (path, TRUE);
}

static void
test_container_type (gpointer  data)
{
  GType  type = GPOINTER_TO_SIZE (data);

  g_assert (g_type_is_a (type, GTK_TYPE_CONTAINER));
}

static void
test_container_add (gpointer  data)
{
  GType      type = GPOINTER_TO_SIZE (data);
  GtkWidget* window = gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);
  GtkWidget* container = gtk_test_create_widget (type, NULL);
  int i;

  for (i = 0; i < 5; i++)
    {
      gchar    * text = g_strdup_printf ("%d", 1 + i);
      GtkWidget* child = gtk_label_new (text);

      gtk_container_add (GTK_CONTAINER (container), child);
    }

  gtk_container_add (GTK_CONTAINER (window), container);
  gtk_widget_show (window);
}

static void
test_container_remove (gpointer  data)
{
  GType      type = GPOINTER_TO_SIZE (data);
  GtkWidget* window = gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);
  GtkWidget* container = gtk_test_create_widget (type, NULL);
  int i;

  for (i = 0; i < 5; i++)
    {
      gchar    * text = g_strdup_printf ("%d", 1 + i);
      GtkWidget* child = gtk_label_new (text);

      gtk_container_add (GTK_CONTAINER (container), child);
    }

  gtk_container_add (GTK_CONTAINER (window), container);

  ;
}

static void
add_gtk_container_tests_for_type (GType  type)
{
  gchar const* name = g_type_name (type);
  GString    * path = g_string_new ("");

  g_string_append_printf (path, "/%s/<<GtkContainer>>/GType", name);
  g_test_add_data_func (path->str, GSIZE_TO_POINTER (type), (void (*) ())test_container_type);
  g_string_set_size (path, 0);

  g_string_append_printf (path, "/%s/<<GtkContainer>>/add", name);
  g_test_add_data_func (path->str, GSIZE_TO_POINTER (type), (void (*) ())test_container_add);
  g_string_set_size (path, 0);

  g_string_append_printf (path, "/%s/<<GtkContainer>>/remove", name);
  g_test_add_data_func (path->str, GSIZE_TO_POINTER (type), (void (*) ())test_container_remove);

  g_string_free (path, TRUE);
}

static void
test_bar_life_cycle (void)
{
  GtkWidget* window = gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);
  GtkWidget* bar    = progress_path_bar_new ();

  progress_path_bar_append (PROGRESS_PATH_BAR (bar), GTK_STOCK_INFO, "Information");

  gtk_container_add (GTK_CONTAINER (window), bar);

  gtk_widget_show_all (window);
}

static void
add_tests_for_path_bar (void)
{
  add_gtk_widget_tests_for_type (PROGRESS_TYPE_PATH_BAR);
  add_gtk_container_tests_for_type (PROGRESS_TYPE_PATH_BAR);
  g_test_add_func ("/ProgressPathBar/life-cycle", test_bar_life_cycle);
}

int
main (int   argc,
      char**argv)
{
  gtk_test_init (&argc, &argv, NULL);

  add_gtk_widget_tests_for_type (PROGRESS_TYPE_SIMPLE_WIDGET);
  add_gtk_widget_tests_for_type (PROGRESS_TYPE_SIMPLE_CONTAINER);
  add_gtk_container_tests_for_type (PROGRESS_TYPE_SIMPLE_CONTAINER);
  add_gtk_widget_tests_for_type (PROGRESS_TYPE_PATH_ELEMENT);
  add_tests_for_path_bar ();

  return g_test_run ();
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
