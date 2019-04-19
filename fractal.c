/*
fractal.c
4/18/2019
M. Foust
foustja@gmail.com
github.com/foustja

This code draws an images of the Mandelbrot set and Henon map.
It also uses gtk widgets to provide a UI.

Ubuntu Linux 18.04
gcc version 7.3.0
GTK+ version: 3.22.30
Glib version: 2.56.3

Compilation:
gcc `pkg-config --cflags gtk+-3.0` -o fractal fractal.c \
`pkg-config --libs gtk+-3.0`

Additional comments describing program and references below following code
*/

//Include files
#include <cairo.h>
#include <gtk/gtk.h>

//Constant definitions
#define WINWIDTH 1200
#define WINHEIGHT 600

#define DAWIDTH 1000
#define DAHEIGHT 600

//Global variables
static cairo_surface_t *surface = NULL;
static gdouble henon_parameter_a = 1.4;
static gdouble henon_parameter_b = 0.3;

//Functions
static void henon(GtkWidget* drawing_area);
static void mandel(GtkWidget* drawing_area);
static void clear_surface (void);
static void do_drawing(cairo_t *cr);

//Callbacks
static void activate (GtkApplication *app, gpointer user_data);
static gboolean configure_event (GtkWidget *widget,
                                 GdkEventConfigure *event,
                                 gpointer data);
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
                              gpointer user_data);
static void mandeldraw (GtkWidget *drawing_area, GtkButton* button);
static void henondraw (GtkWidget *drawing_area, GtkButton* button);
static void clear_drawing_area (GtkWidget* drawing_area);
static void enter_button_a_clicked(GtkWidget *button, gpointer data);
static void enter_button_b_clicked(GtkWidget *button, gpointer data);

//Function definitions

//Generates Henon map
static void henon(GtkWidget* drawing_area)
{
  cairo_t *cr;

  cr = cairo_create (surface);

  int width;
  int height;
  double x;
  double y;
  double init_x;
  double init_y;
  double x_new;
  double y_new;
  double a;
  double b;
  double d_screen_x;
  double d_screen_y;
  int screen_x;
  int screen_y;
  int counter;
  int max_count;

  counter = 0;
  max_count = 100000;

  width = DAWIDTH;
  height = DAHEIGHT;

  init_x = 0.1;
  init_y = 0.1;

  //a = 1.4;
  //b = 0.3;

  a = henon_parameter_a;
  b = henon_parameter_b;

  x = init_x;
  y = init_y;

  while (counter < max_count)
  {
    x_new = 1 - a*x*x + y;
    y_new = b*x;

    x = x_new;
    y = y_new;

    //x = ((double)screen_x/(800/3)) - 1.5;
    d_screen_x = ((x + 2.0)*(width/5));
    screen_x = (int)d_screen_x;

    //y = -(((double)screen_y/600) - 0.5);
    d_screen_y = ((-y + 1.5)*height/3);
    screen_y = (int)d_screen_y;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, 2.0);
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    cairo_move_to (cr, screen_x, screen_y);
    cairo_close_path (cr);
    cairo_stroke (cr);

    gtk_widget_queue_draw_area(drawing_area, screen_x, screen_y, 1, 1);

    while (gtk_events_pending())
    {
      gtk_main_iteration();
    }

    counter++;
  }
}

//Generates and displays Mandelbrot set
static void mandel(GtkWidget* drawing_area)
{
  cairo_t *cr;

  cr = cairo_create (surface);

  int width = DAWIDTH;
  int height = DAHEIGHT;
  int screen_x;
  int screen_y;
  double d_screen_x;
  double a;
  double d_screen_y;
  double b;
  double mzsq;
  int counter;
  double x_new;
  double y_new;
  double x;
  double y;

  for (screen_x = 1; screen_x < width; screen_x++)
  {
    d_screen_x = (double)screen_x;
    a = d_screen_x/(width/5) - 2.0;

    //transforms m to a value in the complex plane with the origin placed
    //somewhat ofset from the center of the window, to provide a good image
    //of the set

    for (screen_y = 1; screen_y < height; screen_y++)
    {
      d_screen_y = (double)screen_y;
      b = -(d_screen_y/(height/3) - 1.5);
      //transforms n to a value in the complex plane

      x = 0.0;
      y = 0.0;
      mzsq = 0.0;
      counter = 0;

      while (counter < 20)
      {
      //loop to iterate function z = z*z - c

        x_new = x*x - y*y - a;
        y_new = 2.00*x*y - b;
        //calculate square of the modulus of z
        mzsq = x_new*x_new + y_new*y_new;

        x = x_new;
        y = y_new;

        counter++;
      }

      //Test to determine if c is a member of the Mandelbrot
      //set and color the screen pixels according to whether or not
      //the value of c is included in the set.

      if (mzsq < 4.0)
      {
        cairo_set_source_rgb (cr, 0, 0, 0);
        cairo_set_line_width (cr, 2.0);
        cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
        cairo_move_to (cr, screen_x, screen_y);
        cairo_close_path (cr);

        cairo_stroke (cr);
      }

      else
      {
        cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
        cairo_set_line_width (cr, 2.0);
        cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
        cairo_move_to (cr, screen_x, screen_y);
        cairo_close_path (cr);

        cairo_stroke (cr);
      }
      gtk_widget_queue_draw_area(drawing_area, screen_x, screen_y, 1, 1);

      while (gtk_events_pending())
      {
        gtk_main_iteration();
      }
    }
  }
  cairo_destroy (cr);

  gtk_widget_queue_draw(drawing_area);
}

//Makes a neutral grey surface to draw on
static void clear_surface (void)
{
  cairo_t *cr;

  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
  cairo_paint (cr);

  cairo_destroy (cr);
}

//Calls clear_surface() and gtk_widget_queue_draw(drawing_area) in order
//to clear and redraw surface
static void clear_drawing_area (GtkWidget* drawing_area)
{
  clear_surface();

  gtk_widget_queue_draw(drawing_area);
}

static gboolean configure_event (GtkWidget *widget,
                                 GdkEventConfigure *event,
                                 gpointer data)
{
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));

  //Initialize the surface to gray
  clear_surface ();

  //Returns TRUE so no additional processing by system takes place
  return TRUE;
}

//Calls mandel(drawing_area) and includes GtkButton* button parameter
static void mandeldraw (GtkWidget *drawing_area, GtkButton* button)
{
  mandel(drawing_area);
}

//Calls henon(drawing_area) and includes GtkButton* button parameter
static void henondraw (GtkWidget *drawing_area, GtkButton* button)
{
  henon(drawing_area);
}

//sets surface as source for cairo context cr and paints
static void do_drawing(cairo_t *cr)
{
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);
}

//Callback for draw event
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
    gpointer user_data)
{
  do_drawing(cr);

  return FALSE;
}

//Callback for data entry - Henon parameter a
static void enter_button_a_clicked(GtkWidget *button, gpointer data)
{
  gpointer parameter_a_spin;

  parameter_a_spin = data;
  henon_parameter_a = gtk_spin_button_get_value(parameter_a_spin);
}

//Callback for data entry - Henon parameter b
static void enter_button_b_clicked(GtkWidget *button, gpointer data)
{
  gpointer parameter_b_spin;

  parameter_b_spin = data;
  henon_parameter_b = gtk_spin_button_get_value(parameter_b_spin);
}

//activate callback function - creates top level window and widgets
static void activate (GtkApplication *app, gpointer user_data)
{
  //Widgets for activate callback function
  GtkWidget *window;
  GtkWidget *drawing_area;

  GtkWidget *button_mandel;
  GtkWidget *button_henon;
  GtkWidget *button_clear;

  GtkAdjustment *adj_a;
  GtkAdjustment *adj_b;

  GtkWidget *parameter_a_label;
  GtkWidget *parameter_b_label;

  GtkWidget *empty_label1;
  GtkWidget *empty_label2;
  GtkWidget *empty_label3;

  GtkWidget *parameter_a_spin;
  GtkWidget *parameter_b_spin;

  GtkWidget *enter_button_a;
  GtkWidget *enter_button_b;

  GtkWidget *vbox;
  GtkWidget *hbox;

  window = gtk_application_window_new (app);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), WINWIDTH, WINHEIGHT);
  gtk_window_set_title(GTK_WINDOW(window), "GTK window");

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, DAWIDTH, DAHEIGHT);

  button_mandel = gtk_button_new_with_label("Mandelbrot");
  button_henon = gtk_button_new_with_label("Henon");
  button_clear = gtk_button_new_with_label("Clear screen");

  empty_label1 = gtk_label_new("");
  empty_label2 = gtk_label_new("");
  empty_label3 = gtk_label_new("");

  parameter_a_label = gtk_label_new("Henon parameter a");
  parameter_b_label = gtk_label_new("Henon parameter b");

  adj_a = (GtkAdjustment *) gtk_adjustment_new (-0.50000, -99.0, 99.0, 0.00001,
					      0.00001, 0.0);
  adj_b = (GtkAdjustment *) gtk_adjustment_new (-0.99998, -99.0, 99.0, 0.00001,
                0.00001, 0.0);

  parameter_a_spin = gtk_spin_button_new (adj_a, 0.0, 5);
  parameter_b_spin = gtk_spin_button_new (adj_b, 0.0, 5);

  enter_button_a = gtk_button_new_with_label("Enter a");
  enter_button_b = gtk_button_new_with_label("Enter b");

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

  gtk_box_pack_start (GTK_BOX (vbox), button_mandel, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), empty_label1, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), parameter_a_label, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), parameter_a_spin, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), enter_button_a, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), parameter_b_label, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), parameter_b_spin, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), enter_button_b, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), empty_label2, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), button_henon, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), empty_label3, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), button_clear, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), drawing_area, TRUE, TRUE, 5);

  gtk_container_add (GTK_CONTAINER (window), hbox);

  g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event), NULL);

  g_signal_connect_swapped (button_mandel, "clicked",
      G_CALLBACK (mandeldraw), drawing_area);

  g_signal_connect(G_OBJECT(enter_button_a), "clicked",
      G_CALLBACK(enter_button_a_clicked), parameter_a_spin);

  g_signal_connect(G_OBJECT(enter_button_b), "clicked",
      G_CALLBACK(enter_button_b_clicked), parameter_b_spin);

  g_signal_connect_swapped (button_henon, "clicked",
      G_CALLBACK (henondraw), drawing_area);

  g_signal_connect_swapped (button_clear, "clicked",
      G_CALLBACK (clear_drawing_area), drawing_area);

  g_signal_connect (drawing_area, "draw",
      G_CALLBACK(on_draw_event), NULL);

//  g_signal_connect (window, "destroy",
    //  G_CALLBACK(g_application_quit), app);

  gtk_widget_show_all (window);
}

//Main - creates and runs application configured and initialized by activate
int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("io.github.foustja.drawmandel", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

/*
Additional information regarding code, algorithms, and references

The function used to generate the Mandelbrot set is the iterated function:

z_new = z*z - c

where z and c are complex numbers of the form:

z = x + i*y

and

c = a + i*b

The initial value of z is set to 0, and the function is iterated 20 times for
each value of c in the subset of the complex plane represented by the values
-1.5 to 2.5 for a and -1.5 to 1.5 for b. Horizontal values represent the real
values a, and values on the vertical axis represent imaginary values b.

(The computer sets a value of (0, 0) to the upper left hand corner of the
screen, and so the screen values (screen_x, screen_y) are transformed to
(x, y) plane values.

The results of the iterated function can be represented thus:

z_new = z*z - c

z_new = (x*x - y*y -a) + i*(2*x*y - b)

where

x_new = x*x - y*y - a

y_new = 2*x*y - b

The square of the modulus of z_new, mzsq = x_new*x_new + y_new*y_new

It is the square of the modulus (mzsq) which is tested, and if mzsq < 4.0 after
20 iterations, then the initial value of c for this series of iterations is
considered to belong to the Mandelbrot set, and the corresponding screen pixel
at (m, n) is colored black.

The iterated formulas for the Henon map are:
x_new = 1 - a*x*x + y
y_new = bx

Initial (x, y) = (0.1, 0.1)

Values for (a, b) include (1.4, 0.4) and (-0.5, -0.9998)

Some code and algorithms were adapted from "Dynamical Systems and Fractals:
Computer graphics experiments in Pascal," by Karl-Heinz Becker and
Michael Doerfler, 2nd edition, 1988.

Original source for a portion of code relating to Cairo graphics and Gtk:
http://zetcode.com/gfx/cairo/cairobackends/
*/
