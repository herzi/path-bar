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
test_widget_windows (gpointer  data)
{
  GType      type = GPOINTER_TO_SIZE (data);
  GtkWidget* window = gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);
  GtkWidget* widget = gtk_test_create_widget (type, NULL);
  GList    * windows;

  gtk_container_add (GTK_CONTAINER (window), widget);

  gtk_widget_show_all (window);

  windows = g_list_prepend (NULL, gtk_widget_get_window (window));
  while (windows)
    {
      gpointer  user_data = NULL;
      GList   * children = gdk_window_get_children (windows->data);

      if (!gdk_window_is_visible (windows->data))
        {
          g_error ("the GdkWindow %p wasn't shown in map()", windows->data);
        }

      gdk_window_get_user_data (windows->data, &user_data);
      if (!user_data)
        {
          /* at some point provide a better message (e.g. with the window tree and the data) */
          g_error ("the GdkWindow %p doesn't have user data set", windows->data);
        }

      windows = g_list_delete_link (windows, windows);
      windows = g_list_concat (windows, children);
    }
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
  g_string_set_size (path, 0);

  g_string_append_printf (path, "/%s/<<GtkWidget>>/windows", name);
  g_test_add_data_func (path->str, GSIZE_TO_POINTER (type), (void (*) ())test_widget_windows);

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

static gboolean
test_bar_life_cycle_idle (gpointer  user_data)
{
  gtk_widget_destroy (user_data);
  gtk_main_quit ();
  return FALSE;
}

static gboolean
test_bar_life_cycle_cb (GtkWidget     * widget    G_GNUC_UNUSED,
                        GdkEventExpose* event     G_GNUC_UNUSED,
                        gpointer        user_data)
{
  g_idle_add (test_bar_life_cycle_idle, user_data);
  return FALSE;
}

static void
test_bar_life_cycle (void)
{
  GtkWidget* window = gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);
  GtkWidget* bar    = progress_path_bar_new ();

  progress_path_bar_append (PROGRESS_PATH_BAR (bar), GTK_STOCK_INFO, "Information");

  gtk_container_add (GTK_CONTAINER (window), bar);

  g_signal_connect (bar, "expose-event",
                    G_CALLBACK (test_bar_life_cycle_cb), window);
  gtk_widget_show_all (window);

  gtk_main ();
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
  add_tests_for_path_bar ();
  add_gtk_widget_tests_for_type (PROGRESS_TYPE_PATH_ELEMENT);

  return g_test_run ();
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
