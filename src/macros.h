#ifndef MACROS_H_GPEC86F4NM44O
#define MACROS_H_GPEC86F4NM44O

#include "gtk-all.h"

G_BEGIN_DECLS

#define MACROS_TYPE            (macros_get_type ())
#define MACROS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MACROS_TYPE, Macros))
#define MACROS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MACROS_TYPE, MacrosClass))
#define IS_MACROS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MACROS_TYPE))
#define IS_MACROS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MACROS_TYPE))

typedef struct {
  GObject parent_instance;

  GList *list;
  GtkTextBuffer *textbuffer;
  gchar *xml_file_path;
} Macros;

typedef struct {
  GObjectClass parent_class;
  void (* macros) (Macros *ttt);
} MacrosClass;

typedef struct {
  gchar *name;
  gchar *value;
  gboolean is_on_toolbar;
} MacroItem;

GType macros_get_type (void);

GObject* macros_new (void);
void macros_attach_text_buffer(Macros *self, GtkTextBuffer *textbuffer);
gboolean macros_load (Macros *m, char *dirname);
gboolean macros_save_xml (Macros *self);

G_END_DECLS

#endif
