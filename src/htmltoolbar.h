#ifndef HTMLTOOLBAR_H
#define HTMLTOOLBAR_H

#include "gtk-all.h"
#include "macros.h"

G_BEGIN_DECLS

#define JAM_HTML_TOOLBAR_TYPE            (jam_html_toolbar_get_type ())
#define JAM_HTML_TOOLBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), JAM_HTML_TOOLBAR_TYPE, JamHtmlToolbar))
#define JAM_HTML_TOOLBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), JAM_HTML_TOOLBAR_TYPE, JamHtmlToolbarClass))
#define IS_JAM_HTML_TOOLBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), JAM_HTML_TOOLBAR_TYPE))
#define IS_JAM_HTML_TOOLBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), JAM_HTML_TOOLBAR_TYPE))

typedef struct {
  GtkToolbar parent_instance;

  Macros *macros;
} JamHtmlToolbar;

typedef struct {
  GtkToolbarClass parent_class;
  void (* jam_html_toolbar) (JamHtmlToolbar *ttt);
} JamHtmlToolbarClass;

GType jam_html_toolbar_get_type (void);

GtkWidget* jam_html_toolbar_new (void);
void jam_html_toolbar_build(JamHtmlToolbar *self);
void jam_html_toolbar_attach_macros(JamHtmlToolbar *self, Macros * m);
static void jam_html_toolbar_button_clicked(GtkWidget *widget, gpointer user_data);
G_END_DECLS

#endif
