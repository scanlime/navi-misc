#include <gtk/gtk.h>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>

typedef boost::numeric::ublas::vector<double> twov;

twov point(2);
GtkWidget *window, *drawing_area, *iterl;
GtkWidget *as, *bs, *cs, *ds, *start, *stop, *save;
GdkPixmap *backb;
int iterations;
GdkGC *grey[65536], *gc;
int data[800][800];
double a, b, c, d;
guint idler;

void draw();
void flip();
static int draw_more(void *data);
gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
void spinnerchanged(GtkWidget *widget, gpointer user_data);
void startclick(GtkWidget *widget, gpointer user_data);
void stopclick(GtkWidget *widget, gpointer user_data);
void saveclick(GtkWidget *widget, gpointer user_data);
gboolean deletee(GtkWidget *widget, GdkEvent *event, gpointer user_data);
twov dejong(float a, float b, float c, float d, boost::numeric::ublas::vector<double> xyn);
GtkWidget *build_sidebar();

int main(int argc, char ** argv) {
  std::srand(std::time(NULL));
  gtk_init(&argc, &argv);

  GtkWidget *hbox, *vsep;

  a = 1.4191403;
  b = -2.2841323;
  c = 2.4275403;
  d = -2.177196;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(deletee), NULL);
  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area, 800, 800);
  vsep = gtk_vseparator_new();
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), build_sidebar(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), vsep, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), drawing_area, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), hbox);
  g_signal_connect(G_OBJECT(drawing_area), "expose-event", G_CALLBACK(expose), NULL);
  gtk_widget_show_all(window);

  backb = gdk_pixmap_new(NULL, 800, 800, 24);

  GdkColor c;
  for(int i = 0; i < 65536; i++) {
    c.red = c.green = c.blue = i;
    grey[i] = gdk_gc_new(backb);
    gdk_gc_set_rgb_bg_color(grey[i], &c);
    gdk_gc_set_rgb_fg_color(grey[i], &c);
  }
  gc = gdk_gc_new(drawing_area->window);

  bzero(data, 800 * 800 * sizeof(int));

  draw();

  point(0) = std::rand() / RAND_MAX;
  point(1) = std::rand() / RAND_MAX;

  idler = g_idle_add(draw_more, NULL);

  iterations = 0;

  gtk_main();
}

GtkWidget *build_sidebar() {
  GtkWidget *table;
  GtkWidget *al, *bl, *cl, *dl;

  table = gtk_table_new(8, 2, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 6);
  gtk_table_set_col_spacings(GTK_TABLE(table), 6);
  al = gtk_label_new("a:");
  gtk_table_attach(GTK_TABLE(table), al, 0, 1, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  bl = gtk_label_new("b:");
  gtk_table_attach(GTK_TABLE(table), bl, 0, 1, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  cl = gtk_label_new("c:");
  gtk_table_attach(GTK_TABLE(table), cl, 0, 1, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  dl = gtk_label_new("d:");
  gtk_table_attach(GTK_TABLE(table), dl, 0, 1, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

  as = gtk_spin_button_new_with_range(-4.0, 4.0, 0.001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(as), a);
  gtk_table_attach(GTK_TABLE(table), as, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 6);
  bs = gtk_spin_button_new_with_range(-4.0, 4.0, 0.001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(bs), b);
  gtk_table_attach(GTK_TABLE(table), bs, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  cs = gtk_spin_button_new_with_range(-4.0, 4.0, 0.001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cs), c);
  gtk_table_attach(GTK_TABLE(table), cs, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  ds = gtk_spin_button_new_with_range(-4.0, 4.0, 0.001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ds), d);
  gtk_table_attach(GTK_TABLE(table), ds, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  g_signal_connect(G_OBJECT(as), "changed", G_CALLBACK(spinnerchanged), NULL);
  g_signal_connect(G_OBJECT(bs), "changed", G_CALLBACK(spinnerchanged), NULL);
  g_signal_connect(G_OBJECT(cs), "changed", G_CALLBACK(spinnerchanged), NULL);
  g_signal_connect(G_OBJECT(ds), "changed", G_CALLBACK(spinnerchanged), NULL);

  iterl = gtk_label_new("");
  gtk_table_attach(GTK_TABLE(table), iterl, 0, 2, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  start = gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_widget_set_sensitive(start, FALSE);
  g_signal_connect(G_OBJECT(start), "clicked", G_CALLBACK(startclick), NULL);
  gtk_table_attach(GTK_TABLE(table), start, 0, 2, 5, 6, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  stop = gtk_button_new_from_stock(GTK_STOCK_STOP);
  g_signal_connect(G_OBJECT(stop), "clicked", G_CALLBACK(stopclick), NULL);
  gtk_table_attach(GTK_TABLE(table), stop, 0, 2, 6, 7, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  save = gtk_button_new_from_stock(GTK_STOCK_SAVE);
  gtk_widget_set_sensitive(save, FALSE);
  g_signal_connect(G_OBJECT(save), "clicked", G_CALLBACK(saveclick), NULL);
  gtk_table_attach(GTK_TABLE(table), save, 0, 2, 7, 8, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

  return table;
}

void draw() {
  gdk_draw_rectangle(backb, grey[65535], TRUE, 0, 0, 800, 800);
  flip();
}

void flip() {
  gdk_draw_drawable(drawing_area->window, gc, backb, 0, 0, 0, 0, 800, 800);
}

static void plop(int x, int y) {
  if(data[x][y] < 64880) {
    data[x][y] += 655;
  }
  gdk_draw_point(backb, grey[65535 - data[x][y]], x, y);
}

static int draw_more(void *data) {
  double x, y;
  const int iterationsAtOnce = 10000;

  for(int i = iterationsAtOnce; i; --i) {
    point = dejong(a, b, c, d, point);
    plop(int(point(0) * 160 + 400), int(point(1) * 160 + 400));
  }

  flip();
  iterations += iterationsAtOnce;
  gchar *iters = g_strdup_printf("%d", iterations);
  gtk_label_set_text(GTK_LABEL(iterl), iters);
  g_free(iters);
  return 1;
}

twov dejong(float a, float b, float c, float d, boost::numeric::ublas::vector<double> xyn) {
  twov r(2);
  r(0) = std::sin(a * xyn(1)) - std::cos(b * xyn(0));
  r(1) = std::sin(c * xyn(0)) - std::cos(c * xyn(1));
  return r;
}

gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
  flip();
  return TRUE;
}

gboolean deletee(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
  g_source_remove(idler);
  gtk_main_quit();
}

void startclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(stop, TRUE);
  gtk_widget_set_sensitive(start, FALSE);
  gtk_widget_set_sensitive(save, FALSE);
  bzero(data, 800 * 800 * sizeof(int));
  draw();
  a = gtk_spin_button_get_value(GTK_SPIN_BUTTON(as));
  b = gtk_spin_button_get_value(GTK_SPIN_BUTTON(bs));
  c = gtk_spin_button_get_value(GTK_SPIN_BUTTON(cs));
  d = gtk_spin_button_get_value(GTK_SPIN_BUTTON(ds));
  iterations = 0;
  idler = g_idle_add(draw_more, NULL);
}

void stopclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(stop, FALSE);
  gtk_widget_set_sensitive(start, TRUE);
  gtk_widget_set_sensitive(save, TRUE);
  g_source_remove(idler);
}

void spinnerchanged(GtkWidget *widget, gpointer user_data) {
  stopclick(widget, user_data);
  startclick(widget, user_data);
}

void saveclick(GtkWidget *widget, gpointer user_data) {
  GtkWidget *dialog;

  dialog = gtk_file_chooser_dialog_new("Save", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE,
  				       GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				       NULL);
  if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    GdkPixbuf *buf = gdk_pixbuf_get_from_drawable(NULL, backb, NULL, 0, 0, 0, 0, 800, 800);
    gdk_pixbuf_save(buf, filename, "png", NULL, NULL);
    g_free(filename);
  }
  gtk_widget_destroy(dialog);
}
