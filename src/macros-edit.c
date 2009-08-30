#include <macros-edit.h>
#include "gtk-all.h"
#include "macros.h"

typedef struct MacrosEditWindow_ {
  GtkWidget *win;
  GtkWidget *macrolist;
  GtkWidget *toolbar;
  GtkWidget *name, *value;

  GtkListStore *store;
} MacrosEditWindow;

void macros_edit_buttonpressed_add_cb(GtkButton *widget, MacrosEditWindow *mew);
void macros_edit_selchanged_cb(GtkTreeSelection *treeselection, MacrosEditWindow *mew);

void
macros_edit_window_run(GtkWindow *parent, Macros *macros)
{
  MacrosEditWindow mew_actual = { 0 };
  MacrosEditWindow *mew = &mew_actual;
  GtkPaned *paned;
  GtkVBox *box_left, *box_right;
  GtkToolbar *toolbar;
  GtkWidget *b;
  GtkTreeIter iter;
  GList *list_it;

  GtkCellRenderer *cell_renderer;
  GtkWidget *column;
  GtkTreeSelection *sel;

  mew->win = gtk_dialog_new_with_buttons(_("LogJam - Edit Macros"),
                                         parent,
                                         GTK_DIALOG_MODAL,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_OK,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_CANCEL,
                                         NULL);
  gtk_dialog_set_default_response(GTK_DIALOG(mew->win), GTK_RESPONSE_OK);

  paned = GTK_PANED(gtk_hpaned_new());
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(mew->win)->vbox), GTK_WIDGET(paned), TRUE, TRUE, 0);

  mew->store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
  for (list_it = macros->list; list_it != NULL; list_it = list_it->next) {
    gtk_list_store_append(GTK_LIST_STORE(mew->store), &iter);
    gtk_list_store_set(GTK_LIST_STORE(mew->store),
                       &iter,
                       0, ((MacroItem*)(list_it->data))->name,
                       1, list_it->data,
                       -1);
  }

  /* Left pane */

  mew->macrolist = gtk_tree_view_new_with_model(GTK_TREE_MODEL(mew->store));
  sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(mew->macrolist));

  cell_renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(column, _("Name"));
  gtk_tree_view_column_pack_start(column, cell_renderer, TRUE);
  gtk_tree_view_column_add_attribute(column, cell_renderer, "text", 0);
  gtk_tree_view_append_column(GTK_TREE_VIEW(mew->macrolist), column);


  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(mew->macrolist), TRUE);

  mew->toolbar = GTK_TOOLBAR(gtk_toolbar_new());
  gtk_toolbar_set_style(GTK_TOOLBAR(mew->toolbar), GTK_TOOLBAR_ICONS);
  b = gtk_tool_button_new_from_stock(GTK_STOCK_ADD);
  gtk_container_add(GTK_CONTAINER(mew->toolbar), GTK_WIDGET(b));
  g_signal_connect(G_OBJECT(b), "pressed", G_CALLBACK(macros_edit_buttonpressed_add_cb), NULL);
  b = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_container_add(GTK_CONTAINER(mew->toolbar), GTK_WIDGET(b));
  b = gtk_tool_button_new_from_stock(GTK_STOCK_GO_UP);
  gtk_container_add(GTK_CONTAINER(mew->toolbar), GTK_WIDGET(b));
  b = gtk_tool_button_new_from_stock(GTK_STOCK_GO_DOWN);
  gtk_container_add(GTK_CONTAINER(mew->toolbar), GTK_WIDGET(b));

  box_left = gtk_vbox_new(FALSE, 3);
  gtk_box_pack_start(GTK_BOX(box_left), GTK_WIDGET(mew->toolbar), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box_left), GTK_WIDGET(mew->macrolist), TRUE, TRUE, 0);

  gtk_widget_set_size_request(box_left, 300, -1);
  gtk_paned_add1(GTK_PANED(paned), GTK_WIDGET(box_left));

  /* Right pane */

  box_right = gtk_vbox_new(FALSE, 3);

  mew->name = gtk_entry_new();
  mew->value = gtk_text_view_new();
  g_assert(GTK_IS_ENTRY(mew->name));
  g_assert(GTK_IS_TEXT_VIEW(mew->value));

  g_signal_connect(G_OBJECT(sel), "changed", G_CALLBACK(macros_edit_selchanged_cb), mew);

  gtk_box_pack_start(GTK_BOX(box_right), gtk_label_new_with_mnemonic(_("_Name")), FALSE, FALSE, 0);
  gtk_box_pack_start(
                     GTK_BOX(box_right), mew->name, FALSE, FALSE, 0
  );
  gtk_box_pack_start(GTK_BOX(box_right), gtk_label_new_with_mnemonic(_("Macro _text")), FALSE, FALSE, 0);
  b = gtk_scrolled_window_new(FALSE, FALSE);
  gtk_container_add(GTK_CONTAINER(b), mew->value);
  gtk_box_pack_start(GTK_BOX(box_right), b , TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box_right), gtk_label_new_with_mnemonic(_("_Shortcut key")), FALSE, FALSE, 0);

  gtk_paned_add2(GTK_PANED(paned), GTK_WIDGET(box_right));

  /* end */
  gtk_widget_set_size_request(mew->win, 500, 480);

  gtk_widget_show_all(mew->win);

  if (gtk_dialog_run(GTK_DIALOG(mew->win)) == GTK_RESPONSE_OK )
    {
      g_debug("OK");
    }
  else
    {
      g_debug("Cancel");
    }

  gtk_widget_destroy(mew->win);
}

void
macros_edit_buttonpressed_add_cb(GtkButton *widget, MacrosEditWindow *mew)
{
  return;
}

void
macros_edit_selchanged_cb(GtkTreeSelection *treeselection, MacrosEditWindow *mew)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  MacroItem *m;

  if (!gtk_tree_selection_get_selected(treeselection, &model, &iter)) {
    return;
  }
  gtk_tree_model_get(model, &iter, 1, &m, -1);

  g_assert(GTK_IS_ENTRY(mew->name));
  g_assert(GTK_IS_TEXT_VIEW(mew->value));

  gtk_entry_set_text(GTK_ENTRY(mew->name), m->name);
  gtk_text_buffer_set_text
    (gtk_text_view_get_buffer(GTK_TEXT_VIEW(mew->value)), m->value, -1);
  g_object_set(G_OBJECT(mew->name), "bound_macroitem", m, NULL);
  g_object_set(G_OBJECT(mew->value), "bound_macroitem", m, NULL);
}
