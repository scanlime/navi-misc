#include <gtk/gtk.h>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>

typedef boost::numeric::ublas::vector<double> twov;

twov point(2);
GtkWidget *drawing_area;
GtkWidget *as, *bs, *cs, *ds, *start, *stop, *save;
GdkPixmap *backb;
int iterations;
GdkGC *grey[65536], *gc;
int data[800][800];
double a, b, c, d;

void draw();
void flip();
static int draw_more(void *data);
gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
void startclick(GtkWidget *widget, gpointer user_data);
void stopclick(GtkWidget *widget, gpointer user_data);
twov dejong(float a, float b, float c, float d, boost::numeric::ublas::vector<double> xyn);
GtkWidget *build_sidebar();

int main(int argc, char ** argv) {
  std::srand(std::time(NULL));
  gtk_init(&argc, &argv);

  GtkWidget *window, *hbox, *vsep;

  a = 1.4191403;
  b = -2.2841323;
  c = 2.4275403;
  d = -2.177196;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area, 800, 800);
  vsep = gtk_vseparator_new();
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), build_sidebar(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), vsep, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), drawing_area, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), hbox);
  gtk_signal_connect(GTK_OBJECT(drawing_area), "expose-event", GTK_SIGNAL_FUNC(expose), NULL);
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

  g_idle_add(draw_more, NULL);

  iterations = 0;

  gtk_main();

  std::cout << "completed " << iterations << " iterations\n";
}

GtkWidget *build_sidebar() {
  GtkWidget *table;
  GtkWidget *al, *bl, *cl, *dl;

  table = gtk_table_new(7, 2, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 6);
  gtk_table_set_col_spacings(GTK_TABLE(table), 6);
  al = gtk_label_new("a:");
  gtk_table_attach(GTK_TABLE(table), al, 0, 1, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 6);
  bl = gtk_label_new("b:");
  gtk_table_attach(GTK_TABLE(table), bl, 0, 1, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  cl = gtk_label_new("c:");
  gtk_table_attach(GTK_TABLE(table), cl, 0, 1, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  dl = gtk_label_new("d:");
  gtk_table_attach(GTK_TABLE(table), dl, 0, 1, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

  as = gtk_spin_button_new_with_range(-4.0, 4.0, 0.00000001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(as), a);
  gtk_widget_set_sensitive(as, FALSE);
  gtk_table_attach(GTK_TABLE(table), as, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 6);
  bs = gtk_spin_button_new_with_range(-4.0, 4.0, 0.00000001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(bs), b);
  gtk_widget_set_sensitive(bs, FALSE);
  gtk_table_attach(GTK_TABLE(table), bs, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  cs = gtk_spin_button_new_with_range(-4.0, 4.0, 0.00000001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cs), c);
  gtk_widget_set_sensitive(cs, FALSE);
  gtk_table_attach(GTK_TABLE(table), cs, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  ds = gtk_spin_button_new_with_range(-4.0, 4.0, 0.00000001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ds), d);
  gtk_widget_set_sensitive(ds, FALSE);
  gtk_table_attach(GTK_TABLE(table), ds, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

  start = gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_widget_set_sensitive(start, FALSE);
  g_signal_connect(G_OBJECT(start), "clicked", G_CALLBACK(startclick), NULL);
  gtk_table_attach(GTK_TABLE(table), start, 0, 2, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  stop = gtk_button_new_from_stock(GTK_STOCK_STOP);
  g_signal_connect(G_OBJECT(stop), "clicked", G_CALLBACK(stopclick), NULL);
  gtk_table_attach(GTK_TABLE(table), stop, 0, 2, 5, 6, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  save = gtk_button_new_from_stock(GTK_STOCK_SAVE);
  gtk_widget_set_sensitive(save, FALSE);
//  g_signal_connect(G_OBJECT(stop), "clicked", G_CALLBACK(stopclick), NULL);
  gtk_table_attach(GTK_TABLE(table), save, 0, 2, 6, 7, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

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

  std::vector<twov> points;
  points.push_back(point);
  twov newpoint;

  for(int i = 0; i < 500; i++) {
    newpoint = dejong(a, b, c, d, points[i]);
    points.push_back(newpoint);
    point = newpoint;
  }
  for(std::vector<twov>::iterator i = ++(points.begin()); i != points.end(); i++) {

    plop(int((*i)(0) * 160 + 400), int((*i)(1) * 160 + 400));
  }
  flip();
  iterations += 50;
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

void startclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(stop, TRUE);
  gtk_widget_set_sensitive(start, FALSE);
  gtk_widget_set_sensitive(save, FALSE);
  gtk_widget_set_sensitive(as, FALSE);
  gtk_widget_set_sensitive(bs, FALSE);
  gtk_widget_set_sensitive(cs, FALSE);
  gtk_widget_set_sensitive(ds, FALSE);
  bzero(data, 800 * 800 * sizeof(int));
  draw();
  a = gtk_spin_button_get_value(GTK_SPIN_BUTTON(as));
  b = gtk_spin_button_get_value(GTK_SPIN_BUTTON(bs));
  c = gtk_spin_button_get_value(GTK_SPIN_BUTTON(cs));
  d = gtk_spin_button_get_value(GTK_SPIN_BUTTON(ds));
  g_idle_add(draw_more, (gpointer) draw_more);
}

void stopclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(stop, FALSE);
  gtk_widget_set_sensitive(start, TRUE);
  gtk_widget_set_sensitive(save, TRUE);
  gtk_widget_set_sensitive(as, TRUE);
  gtk_widget_set_sensitive(bs, TRUE);
  gtk_widget_set_sensitive(cs, TRUE);
  gtk_widget_set_sensitive(ds, TRUE);
  g_idle_remove_by_data((gpointer) draw_more);
}
