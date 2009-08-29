#include "htmltoolbar.h"

void jam_html_toolbar_foreach_remove(GtkWidget *w, gpointer data);
void jam_html_toolbar_foreach_macro(MacroItem *item, gpointer user_data);


enum {
  REBUILD_SIGNAL,
  LAST_SIGNAL
};

static guint jam_html_toolbar_signals[LAST_SIGNAL] = { 0 };

static void
jam_html_toolbar_class_init (JamHtmlToolbarClass *klass)
{
  jam_html_toolbar_signals[REBUILD_SIGNAL] =
    g_signal_new ("rebuild",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (JamHtmlToolbarClass, jam_html_toolbar),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0
		  );
}


static void
jam_html_toolbar_init (JamHtmlToolbar *self)
{
  self->macros = NULL;
}

void
jam_html_toolbar_build(JamHtmlToolbar *self)
{

  g_assert (self->macros != NULL);
  gtk_container_foreach(self, &jam_html_toolbar_foreach_remove, NULL);

  g_list_foreach(self->macros->list, jam_html_toolbar_foreach_macro, self);

  gtk_widget_show_all(self);
}

void
jam_html_toolbar_attach_macros(JamHtmlToolbar *self, Macros *m)
{
  self->macros = m;
  jam_html_toolbar_build(self);
}

void jam_html_toolbar_foreach_remove(GtkWidget *w, gpointer data)
{
  gtk_widget_destroy(w);
}

void
jam_html_toolbar_foreach_macro(MacroItem *item, gpointer user_data)
{
  GtkToolButton *b;
  if (item->is_on_toolbar) {
    b = gtk_tool_button_new(NULL, item->name);
    g_object_set_data(G_OBJECT(b), "macros", JAM_HTML_TOOLBAR(user_data)->macros);
    g_object_set_data(G_OBJECT(b), "macro_item", item);
    g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(jam_html_toolbar_button_clicked), NULL);

    //    gtk_label_set_markup(gtk_tool_button_get_label_widget(b), item->name);
    
    gtk_container_add(user_data, b);
  }
}

GType
jam_html_toolbar_get_type (void)
{
  static GType ttt_type = 0;

  if (!ttt_type)
    {
      static const GTypeInfo ttt_info =
      {
	sizeof (JamHtmlToolbarClass),
	NULL, /* base_init */
	NULL, /* base_finalize */
	(GClassInitFunc) jam_html_toolbar_class_init,
	NULL, /* class_finalize */
	NULL, /* class_data */
	sizeof (JamHtmlToolbar),
	0,    /* n_preallocs */
	(GInstanceInitFunc) jam_html_toolbar_init,
      };

      ttt_type = g_type_register_static (GTK_TYPE_TOOLBAR,
                                         "JamHtmlToolbar",
                                         &ttt_info,
                                         0);
    }

  return ttt_type;
}


GtkWidget*
jam_html_toolbar_new (void)
{
  return GTK_WIDGET ( g_object_new (JAM_HTML_TOOLBAR_TYPE, NULL));
}

static void
jam_html_toolbar_button_clicked(GtkWidget *widget, gpointer user_data)
{
  Macros *m;
  MacroItem *i;

  m = (Macros*)g_object_get_data(widget, "macros");
  i = (MacroItem*)g_object_get_data(widget, "macro_item");
  g_assert(IS_MACROS(m));

  macros_execute_macro(m, i);
}
