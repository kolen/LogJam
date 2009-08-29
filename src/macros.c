#include "macros.h"

#include <libxml/parser.h>
#include <libxml/tree.h>


enum {
  CHANGED_SIGNAL,
  LAST_SIGNAL
};

static guint macros_signals[LAST_SIGNAL] = { 0 };

static MacroItem macros_defaults[] = {
  {"Pre", "<pre>$0</pre>", 1},
  {"Image", "<img src=\"$0\" alt=\"\" border=\"\">", 1},
  {"Red", "<span style='font-color: #f00;'>$0</span>", 1},
  {"Lilowaya(tm)", "<span style='font-color: #f0f;'>$0</span>", 1},
  {NULL}
};

static void
macros_class_init (MacrosClass *klass)
{
  macros_signals[CHANGED_SIGNAL] =
    g_signal_new ("changed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (MacrosClass, macros),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0
		  );
}


static void
macros_init (Macros *self)
{  
  self->list = NULL;
  self->textbuffer = NULL;
  self->xml_file_path = NULL;
}

void
macros_attach_text_buffer(Macros *self, GtkTextBuffer *textbuffer)
{
  g_assert(GTK_IS_TEXT_BUFFER(textbuffer));
  self->textbuffer = textbuffer;
}

void
macros_execute_macro (Macros *self, MacroItem *mi)
{
  GtkTextIter start, end;
  GtkTextBuffer *tb = self->textbuffer;
  gboolean has_selection;
  gchar *selection, *pos, *lastpos, *m;
  GString *out = g_string_new("");

  m = mi->value;

  has_selection = 
    gtk_text_buffer_get_selection_bounds(tb, &start, &end);

  gtk_text_buffer_begin_user_action(tb);

  selection = gtk_text_buffer_get_text(tb, &start, &end, FALSE);

  if (has_selection) {    
    gtk_text_buffer_delete(tb, &start, &end);
  }

  pos = lastpos = m;
  while (pos = g_strrstr(pos, "$"))
    {
      g_string_append_len(out, lastpos, (pos-lastpos));
      if (g_str_has_prefix(pos, "$0"))
	{
	  g_string_append(out, selection);
	  pos += 2;
	}
      else 
	{
	  g_string_append_c(out, '$');
	  pos += 1;
	}
      lastpos = pos;
    }
  g_string_append_len(out, lastpos, (pos-lastpos));  

  gtk_text_buffer_insert(tb, &start, out->str, -1);

  gtk_text_buffer_end_user_action(tb);
  g_free(selection);
  g_string_free(out, TRUE);
}

GType
macros_get_type (void)
{
  static GType ttt_type = 0;

  if (!ttt_type)
    {
      static const GTypeInfo ttt_info =
      {
	sizeof (MacrosClass),
	NULL, /* base_init */
	NULL, /* base_finalize */
	(GClassInitFunc) macros_class_init,
	NULL, /* class_finalize */
	NULL, /* class_data */
	sizeof (Macros),
	0,    /* n_preallocs */
	(GInstanceInitFunc) macros_init,
      };

      ttt_type = g_type_register_static (G_TYPE_OBJECT,
                                         "Macros",
                                         &ttt_info,
                                         0);
    }

  return ttt_type;
}

xmlDocPtr
macros_parse_xml (Macros *self, gchar *dirname)
{
  xmlDocPtr doc;
  xmlNodePtr node;
  void *ret;
  char *path;
  
  path = g_build_filename(dirname, "macros.xml", NULL);

  g_free(self->xml_file_path);
  self->xml_file_path = g_strdup(path);
  
  if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
    g_free(path);
    return NULL;
  }
  
  doc = xmlParseFile(path);
  g_free(path);
  if (doc == NULL) {
    fprintf(stderr, _("error parsing configuration file.\n"));
    return NULL;
  }

  node = xmlDocGetRootElement(doc);
  if (node == NULL) {
    fprintf(stderr, _("empty document.\n"));
    xmlFreeDoc(doc);
    return NULL;
  }

  return doc;
}

void
macros_fill_defaults(Macros *self)
{
  MacroItem *i, *i1;
  
  for (i=macros_defaults; i->name; i++)
    {
      i1 = malloc(sizeof(MacroItem));
      memcpy(i1, i, sizeof(MacroItem));
      self->list = g_list_append(self->list, i1);  
    }

  macros_save_xml(self);
}

gchar *
macros_xml_get_element_string(xmlNodePtr n)
{
  if (n->children) {
    return g_strdup(n->children->content);
  } else {
    return g_strdup("");
  }
}

gboolean
macros_save_xml (Macros *self)
{
  xmlDocPtr doc;
  xmlNodePtr rootnode, itemnode, subitemnode, textnode;
  MacroItem *item;
  GList *iter;

  doc = xmlNewDoc("1.0");

  rootnode = xmlNewDocNode(doc, NULL, "macros", NULL);
  xmlAddChild((xmlNodePtr)doc, rootnode);

  for (iter = g_list_first(self->list); iter != NULL; iter = iter->next) {
    item = (MacroItem*)iter->data;
    itemnode = xmlNewDocNode(doc, NULL, "macro", NULL);

    subitemnode = xmlNewDocNode(doc, NULL, "name", NULL);
    textnode = xmlNewDocText(doc, item->name);
    xmlAddChild(subitemnode, textnode);
    xmlAddChild(itemnode, subitemnode);

    subitemnode = xmlNewDocNode(doc, NULL, "value", NULL);
    textnode = xmlNewDocText(doc, item->value);
    xmlAddChild(subitemnode, textnode);
    xmlAddChild(itemnode, subitemnode);

    subitemnode = xmlNewDocNode(doc, NULL, "is_on_toolbar", NULL);
    textnode = xmlNewDocText(doc, item->is_on_toolbar?"1":"0");
    xmlAddChild(subitemnode, textnode);
    xmlAddChild(itemnode, subitemnode);

    xmlAddChild(rootnode, itemnode);
  }

  xmlSaveFormatFile(self->xml_file_path, doc, 1);
  xmlFreeDoc(doc);

  return TRUE;
}

gboolean
macros_load_from_xml (Macros *self, xmlDocPtr doc)
{
  xmlNodePtr curr, sub;
  const xmlChar *n;
  MacroItem *m;
  gchar *temp;

  curr = xmlDocGetRootElement(doc);
  if (!curr) return FALSE;
  if (!g_str_equal(curr->name, "macros")) return FALSE;
  curr = curr->children;
  if (!curr) return FALSE;
  do {
    if (g_str_equal(curr->name, "macro"))
      {
	sub = curr->children;
	if (!sub) continue;

	m = (MacroItem*)malloc(sizeof(MacroItem));
	memset(m, 0, sizeof(m));
	m->name = NULL;
	m->value = NULL;
	m->is_on_toolbar = TRUE;


	do {
	  n = sub->name;
	  if (sub->type != XML_ELEMENT_NODE) continue;
	  
	  switch (n[0]) {
	  case 'n':
	    if (g_str_equal(n, "name")) {
	      m->name = macros_xml_get_element_string(sub);
	    }
	    break;
	  case 'v':
	    switch(n[1]) {
	    case 'i':
	      if (g_str_equal(n, "visible-on-toolbar")) {
		temp = macros_xml_get_element_string(sub);
		if (temp[0] == '1')
		  m->is_on_toolbar = TRUE;
		else
		  m->is_on_toolbar = FALSE;
	      }
	      break;
	    case 'a':
	      if (g_str_equal(n, "value")) {
		m->value = macros_xml_get_element_string(sub);
	      }
	      break;
	    }
	    break;
	  };
	} while (sub = sub->next);

	if (!m->name || !m->value) {
	  g_free(m);
	  continue;
	}
	self->list = g_list_append(self->list, m);
      }
  } while (curr = curr->next);

  g_signal_emit(G_OBJECT(self), macros_signals[CHANGED_SIGNAL], 0);

  return TRUE;
}

gboolean
macros_load (Macros *m, char *dirname)
{
  xmlDocPtr doc;
  doc = macros_parse_xml(m, dirname);
  if (!doc) {
    g_warning("No macros file -- using defaults");
    macros_fill_defaults(m);
    return FALSE;
  }
  if (!macros_load_from_xml(m, doc)) {
    g_warning("Corrupted macros.xml");
    macros_fill_defaults(m);
    xmlFreeDoc(doc);
    return FALSE;
  }  
  xmlFreeDoc(doc);
  return TRUE;
}

GObject*
macros_new (void)
{
  return G_OBJECT ( g_object_new (MACROS_TYPE, NULL));
}
