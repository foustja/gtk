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
gcc `pkg-config --cflags gtk+-3.0` -o fractal7 fractal7.c \
`pkg-config --libs gtk+-3.0` -lm

Additional comments describing program and references below following code
*/

//Include files
#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>

//Constant definitions
#define WINWIDTH 1100
#define WINHEIGHT 600

#define DAWIDTH 1000
#define DAHEIGHT 600

//Global variables
static cairo_surface_t *surface = NULL;
static gdouble parameter_a = -0.5;
static gdouble parameter_b = -0.99998;

static gboolean closing = FALSE;

//Functions
static void henon(GtkWidget* drawing_area);
static void lorenz_xy(GtkWidget* drawing_area);
static void lorenz_yz(GtkWidget* drawing_area);
static void lorenz_xz(GtkWidget* drawing_area);
static void julia(GtkWidget* drawing_area);
static void juliasin(GtkWidget* drawing_area);
static void mandel(GtkWidget* drawing_area);
static void clear_surface (void);
static void do_drawing(cairo_t *cr);
static void stop_function(void);

//Callbacks
static void activate (GtkApplication *app, gpointer user_data);
static gboolean configure_event (GtkWidget *widget,
                                 GdkEventConfigure *event,
                                 gpointer data);
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
                              gpointer user_data);
static gboolean delete_event(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data );
static void henondraw (GtkWidget *drawing_area, GtkButton* button);
static void lorenz_xydraw(GtkWidget* drawing_area, GtkButton* button);
static void lorenz_yzdraw(GtkWidget* drawing_area, GtkButton* button);
static void lorenz_xzdraw(GtkWidget* drawing_area, GtkButton* button);
static void juliadraw (GtkWidget *drawing_area, GtkButton* button);
static void juliasindraw(GtkWidget* drawing_area, GtkButton* button);
static void mandeldraw (GtkWidget *drawing_area, GtkButton* button);
static void clear_drawing_area (GtkWidget* drawing_area);
static void enter_button_a_clicked(GtkWidget *button, gpointer data);
static void enter_button_b_clicked(GtkWidget *button, gpointer data);
static gboolean close_app(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data );
static void show_about(GtkWidget *widget, gpointer data);
static void show_reference(GtkWidget *widget, gpointer data);
static void show_parameters(GtkWidget *widget, gpointer window);
static void save_function(GtkButton* button, gpointer user_data);
static void open_function(GtkButton* button, gpointer user_data);


//Function definitions

//Generates Henon map
static void henon(GtkWidget* drawing_area)
{
  closing = FALSE;

  cairo_t *cr;

  cr = cairo_create (surface);

  int width;
  int height;
  long double x;
  long double y;
  long double init_x;
  long double init_y;
  long double x_new;
  long double y_new;
  long double a;
  long double b;
  long double d_screen_x;
  long double d_screen_y;
  int screen_x;
  int screen_y;
  int counter;
  int max_count;

  counter = 0;
  max_count = 40000;

  width = DAWIDTH;
  height = DAHEIGHT;

  init_x = 0.1;
  init_y = 0.1;

  //a = 1.4;
  //b = 0.3;

  a = (long double)parameter_a;
  b = (long double)parameter_b;

  x = init_x;
  y = init_y;

  while (counter < max_count && closing == FALSE)
  {
    x_new = 1 - a*x*x + y;
    y_new = b*x;

    x = x_new;
    y = y_new;

    //x = ((long double)screen_x/(1000/6.67)) - 3.335;
    d_screen_x = ((x + 3.335)*(width/6.67));
    screen_x = (int)d_screen_x;

    //y = -(((long double)screen_y/(600/4)) - 2.0);
    d_screen_y = ((-y + 2.0)*height/4);
    screen_y = (int)d_screen_y;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, 0.5);
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

  cairo_destroy (cr);
}

//Generates lorenz_xy attractor and displays in 2D

/*
Equations and code borrowed from Paul Bourke
http://paulbourke.net/fractals/lorenz_xy/

3 coupled non-linear differential equations

dx / dt = a (y - x)
dy / dt = x (b - z) - y
dz / dt = xy - c z

One commonly used set of constants is a = 10, b = 28, c = 8 / 3.
Another is a = 28, b = 46.92, c = 4.
"a" is sometimes known as the Prandtl number and "b" the Rayleigh number.

*/

static void lorenz_xy(GtkWidget* drawing_area)
{
  closing = FALSE;

  cairo_t *cr;

  cr = cairo_create (surface);

  int width;
  int height;
  long double x;
  long double y;
  long double z;
  long double init_x;
  long double init_y;
  long double init_z;
  long double x_new;
  long double y_new;
  long double z_new;
  long double d_screen_x;
  long double d_screen_y;
  int screen_x;
  int screen_y;
  int counter;
  int max_count;

  long double h = 0.01;
  long double a = 10.0;
  long double b = 28.0;
  long double c = 8.0 / 3.0;

  counter = 0;
  max_count = 400000;

  width = DAWIDTH;
  height = DAHEIGHT;

  init_x = 0.1;
  init_y = 0.0;
  init_z = 0.0;

  x = init_x;
  y = init_y;
  z = init_z;

  while (counter < max_count && closing == FALSE)
  {
    x_new = x + h * a * (y - x);
    y_new = y + h * (x * (b - z) - y);
    z_new = z + h * (x * y - c * z);

    x = x_new;
    y = y_new;
    z = z_new;

    //x = (screen_x/(1000/5) - 2.5);
    //d_screen_x = (x + 2.5)*(width/5);

    //x = d_screen_x/10 - 50.0
    d_screen_x = (x + 50.0)*10;
    screen_x = (int)d_screen_x;

    //y = -(d_screen_y/(600/3) - 1.5);
    //d_screen_y = (-y + 1.5)*(height/3);

    //y = -(d_screen_y/6 - 50.0);
    d_screen_y = (-y + 50.0)*6;
    screen_y = (int)d_screen_y;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, 0.5);
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

  cairo_destroy (cr);
}

static void lorenz_yz(GtkWidget* drawing_area)
{
  closing = FALSE;

  cairo_t *cr;

  cr = cairo_create (surface);

  int width;
  int height;
  long double x;
  long double y;
  long double z;
  long double init_x;
  long double init_y;
  long double init_z;
  long double x_new;
  long double y_new;
  long double z_new;
  long double d_screen_x;
  long double d_screen_y;
  long double d_screen_z;
  int screen_x;
  int screen_y;
  int screen_z;
  int counter;
  int max_count;

  long double h = 0.01;
  long double a = 10.0;
  long double b = 28.0;
  long double c = 8.0 / 3.0;

  counter = 0;
  max_count = 100000;

  width = DAWIDTH;
  height = DAHEIGHT;

  init_x = 0.1;
  init_y = 0.0;
  init_z = 0.0;

  x = init_x;
  y = init_y;
  z = init_z;

  while (counter < max_count && closing == FALSE)
  {
    x_new = x + h * a * (y - x);
    y_new = y + h * (x * (b - z) - y);
    z_new = z + h * (x * y - c * z);

    x = x_new;
    y = y_new;
    z = z_new;

    //x = (screen_x/(1000/5) - 2.5);
    //d_screen_x = (x + 2.5)*(width/5);

    //x = d_screen_x/10 - 50.0
    d_screen_y = (y + 50.0)*10;
    screen_y = (int)d_screen_y;

    //y = -(d_screen_y/(600/3) - 1.5);
    //d_screen_y = (-y + 1.5)*(height/3);

    //y = -(d_screen_y/6 - 50.0);
    d_screen_z = (-z + 50.0)*6;
    screen_z = (int)d_screen_z;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, 0.5);
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    cairo_move_to (cr, screen_y, screen_z);
    cairo_close_path (cr);
    cairo_stroke (cr);

    gtk_widget_queue_draw_area(drawing_area, screen_y, screen_z, 1, 1);

    while (gtk_events_pending())
    {
      gtk_main_iteration();
    }

    counter++;
  }

  cairo_destroy (cr);
}

static void lorenz_xz(GtkWidget* drawing_area)
{
  closing = FALSE;

  cairo_t *cr;

  cr = cairo_create (surface);

  int width;
  int height;
  long double x;
  long double y;
  long double z;
  long double init_x;
  long double init_y;
  long double init_z;
  long double x_new;
  long double y_new;
  long double z_new;
  long double d_screen_x;
  long double d_screen_y;
  long double d_screen_z;
  int screen_x;
  int screen_y;
  int screen_z;
  int counter;
  int max_count;

  long double h = 0.01;
  long double a = 10.0;
  long double b = 28.0;
  long double c = 8.0 / 3.0;

  counter = 0;
  max_count = 100000;

  width = DAWIDTH;
  height = DAHEIGHT;

  init_x = 0.1;
  init_y = 0.0;
  init_z = 0.0;

  x = init_x;
  y = init_y;
  z = init_z;

  while (counter < max_count && closing == FALSE)
  {
    x_new = x + h * a * (y - x);
    y_new = y + h * (x * (b - z) - y);
    z_new = z + h * (x * y - c * z);

    x = x_new;
    y = y_new;
    z = z_new;

    //x = (screen_x/(1000/5) - 2.5);
    //d_screen_x = (x + 2.5)*(width/5);

    //x = d_screen_x/10 - 50.0
    d_screen_x = (x + 50.0)*10;
    screen_x = (int)d_screen_x;

    //y = -(d_screen_y/(600/3) - 1.5);
    //d_screen_y = (-y + 1.5)*(height/3);

    //y = -(d_screen_y/6 - 50.0);
    d_screen_z = (-z + 50.0)*6;
    screen_z = (int)d_screen_z;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, 0.5);
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    cairo_move_to (cr, screen_x, screen_z);
    cairo_close_path (cr);
    cairo_stroke (cr);

    gtk_widget_queue_draw_area(drawing_area, screen_x, screen_z, 1, 1);

    while (gtk_events_pending())
    {
      gtk_main_iteration();
    }

    counter++;
  }

  cairo_destroy (cr);
}

//Generates and displays Julia set
static void julia(GtkWidget* drawing_area)
{
  closing = FALSE;

  cairo_t *cr;

  cr = cairo_create (surface);

  int width = DAWIDTH;
  int height = DAHEIGHT;
  int screen_x;
  int screen_y;
  long double d_screen_x;
  long double a;
  long double d_screen_y;
  long double b;
  long double mzsq;
  int counter;
  long double x_new;
  long double y_new;
  long double x;
  long double y;
  long double z_re;
  long double z_im;

  a = (long double)parameter_a;
  b = (long double)parameter_b;

  for (screen_x = 1; screen_x < width && closing == FALSE; screen_x++)
  {
    //transforms int screen_x to a long double value in the complex plane
    d_screen_x = (long double)screen_x;
    z_re = d_screen_x/(width/5) - 2.0;
    //x = d_screen_x/(width/5) - 2.0;

    for (screen_y = 1; screen_y < height && closing == FALSE; screen_y++)
    {
      //transforms int screen_y to a long double value in the complex plane
      d_screen_y = (long double)screen_y;
      z_im = -(d_screen_y/(height/3) - 1.5);
      //y = -(d_screen_y/(height/3) - 1.5);


      x = z_re;
      y = z_im;
      mzsq = 0.0;
      counter = 0;

      while (counter < 100 && closing == FALSE)
      {
      //loop to iterate function F(z) = z*z + c

        x_new = x*x - y*y + a;
        y_new = 2.00*x*y + b;

        //calculate square of the modulus of z
        mzsq = x_new*x_new + y_new*y_new;

        x = x_new;
        y = y_new;

        counter++;
      }

      //Test to determine if c is a member of the Julia
      //set and color the screen pixels according to whether
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

  //gtk_widget_queue_draw(drawing_area);
}

//Generates and displays Julia/Sine set

/*
xk+1 = sin(xk) cosh(yk)
yk+1 = cos(xk) sinh(yk)
*/
static void juliasin(GtkWidget* drawing_area)
{
  closing = FALSE;

  cairo_t *cr;

  cr = cairo_create (surface);

  int width = DAWIDTH;
  int height = DAHEIGHT;
  int screen_x;
  int screen_y;
  long double d_screen_x;
  long double a;
  long double d_screen_y;
  long double b;
  long double mzsq;
  int counter;
  long double x_new;
  long double y_new;
  long double x;
  long double y;
  long double z_re;
  long double z_im;

  a = (long double)parameter_a;
  b = (long double)parameter_b;

  for (screen_x = 1; screen_x < width && closing == FALSE; screen_x++)
  {
    //transforms int screen_x to a long double value in the complex plane
    d_screen_x = (long double)screen_x;
    z_re = d_screen_x/(width/5) - 2.0;
    //x = d_screen_x/(width/5) - 2.0;

    while (gtk_events_pending())
    {
      gtk_main_iteration();
    }

    for (screen_y = 1; screen_y < height && closing == FALSE; screen_y++)
    {
      //transforms int screen_y to a long double value in the complex plane
      d_screen_y = (long double)screen_y;
      z_im = -(d_screen_y/(height/3) - 1.5);
      //y = -(d_screen_y/(height/3) - 1.5);

      x = z_re;
      y = z_im;

      mzsq = 0.0;
      counter = 0;

      while (gtk_events_pending())
      {
        gtk_main_iteration();
      }

      while (counter < 100 && closing == FALSE)
      {
        //loop to iterate function F(z) = z*z + c

        /*
        x_new = x*x - y*y + a;
        y_new = 2.00*x*y + b;
        */

        x_new = sin(x)*cosh(y) + a;

        y_new = cos(x)*sinh(y) + b;


        //calculate square of the modulus of z
        mzsq = x_new*x_new + y_new*y_new;

        x = x_new;
        y = y_new;

        counter++;
      }

      //Test to determine if c is a member of the Julia
      //set and color the screen pixels according to whether
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

  //gtk_widget_queue_draw(drawing_area);
}

//Generates and displays Mandelbrot set
static void mandel(GtkWidget* drawing_area)
{
  closing = FALSE;

  cairo_t *cr;

  cr = cairo_create (surface);

  int width = DAWIDTH;
  int height = DAHEIGHT;
  int screen_x;
  int screen_y;
  long double d_screen_x;
  long double a;
  long double d_screen_y;
  long double b;
  long double mzsq;
  int counter;
  long double x_new;
  long double y_new;
  long double x;
  long double y;

  for (screen_x = 1; screen_x < width && closing == FALSE; screen_x++)
  {
    d_screen_x = (long double)screen_x;
    a = d_screen_x/(width/5) - 2.5;

    //transforms m to a value in the complex plane with the origin placed
    //somewhat ofset from the center of the window, to provide a good image
    //of the set

    for (screen_y = 1; screen_y < height && closing == FALSE; screen_y++)
    {
      d_screen_y = (long double)screen_y;
      b = -(d_screen_y/(height/3) - 1.5);
      //transforms n to a value in the complex plane

      x = 0.0;
      y = 0.0;
      mzsq = 0.0;
      counter = 0;

      while (counter < 100 && closing == FALSE)
      {
      //loop to iterate function z = z*z + c

        x_new = x*x - y*y + a;
        y_new = 2.00*x*y + b;
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

  //gtk_widget_queue_draw(drawing_area);
}

//Makes a neutral surface to draw on
static void clear_surface (void)
{
  cairo_t *cr;

  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 0.85, 0.85, 0.85);
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

  //Initialize the surface
  clear_surface ();

  //Returns TRUE so no additional processing by system takes place
  return TRUE;
}

//callback function for delete event
static gboolean delete_event(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
    */
  closing = TRUE;

  return FALSE;
}


//Calls henon(drawing_area) and includes GtkButton* button parameter
static void henondraw (GtkWidget *drawing_area, GtkButton* button)
{
  henon(drawing_area);
}

//Calls lorenz_xy(drawing area) and includes GtkButton* button parameter
static void lorenz_xydraw(GtkWidget* drawing_area, GtkButton* button)
{
  lorenz_xy(drawing_area);
}

static void lorenz_yzdraw(GtkWidget* drawing_area, GtkButton* button)
{
  lorenz_yz(drawing_area);
}

static void lorenz_xzdraw(GtkWidget* drawing_area, GtkButton* button)
{
  lorenz_xz(drawing_area);
}

//Calls julia(drawing_area) and includes GtkButton* button parameter
static void juliadraw (GtkWidget *drawing_area, GtkButton* button)
{
  julia(drawing_area);
}

//Calls juliasin(drawing_area) and includes GtkButton* button parameter
static void juliasindraw (GtkWidget *drawing_area, GtkButton* button)
{
  juliasin(drawing_area);
}

//Calls mandel(drawing_area) and includes GtkButton* button parameter
static void mandeldraw (GtkWidget *drawing_area, GtkButton* button)
{
  mandel(drawing_area);
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

//Callback for data entry - parameter a for Henon and Julia func
static void enter_button_a_clicked(GtkWidget *button, gpointer data)
{
  gpointer parameter_a_spin;

  parameter_a_spin = data;
  parameter_a = gtk_spin_button_get_value(parameter_a_spin);
}

//Callback for data entry - parameter b for Henon and Julia func
static void enter_button_b_clicked(GtkWidget *button, gpointer data)
{
  gpointer parameter_b_spin;

  parameter_b_spin = data;
  parameter_b = gtk_spin_button_get_value(parameter_b_spin);
}

static void stop_function(void)
{
  closing = TRUE;
}

//callback function for quit_menu_item
static gboolean close_app(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data )
{
  closing = TRUE;

  gtk_widget_destroy(data);

  return FALSE;
}

static void open_function(GtkButton* button, gpointer user_data)
{

  cairo_t *cr;
  cr = cairo_create (surface);
  GdkPixbuf *pixbuf = NULL;
	GtkWidget *toplevel = gtk_widget_get_toplevel (user_data);
	GtkFileFilter *filter = gtk_file_filter_new ();
	GtkWidget *dialog =
  gtk_file_chooser_dialog_new ("Open image",
	                             GTK_WINDOW (toplevel),
	                             GTK_FILE_CHOOSER_ACTION_OPEN,
	                             GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
	                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                             NULL);

	switch (gtk_dialog_run (GTK_DIALOG (dialog)))
	{
		case GTK_RESPONSE_ACCEPT:
		{
			gchar *filename =
			gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
      gdk_cairo_set_source_pixbuf (cr, pixbuf, 0,0);
      cairo_paint (cr);
      //gtk_widget_queue_draw(toplevel);
			break;
		}
		default:
			break;
	}
	gtk_widget_destroy (dialog);
}

static void save_function(GtkButton* button, gpointer user_data)
{
  gchar *filename;
  cairo_t *cr;
  GtkWidget *image;
	GtkWidget *toplevel;
	GtkFileFilter *filter;

  cr = cairo_create (surface);
  image = GTK_WIDGET (user_data);
  toplevel = gtk_widget_get_toplevel (image);
  GtkWidget *dialog =
  gtk_file_chooser_dialog_new("Save image",
                              GTK_WINDOW(toplevel),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                              GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                              NULL);

  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

  filter = gtk_file_filter_new ();
  gtk_file_filter_add_pattern(filter,"*.png");
  gtk_file_filter_set_name(filter,"PNG (Portable Network Graphics)");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);


	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
	  filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(dialog));

    if (strncmp(gtk_file_filter_get_name(filter),"PNG", 3) == 0)
    {
      if (strcmp(filename+strlen(filename)-4, ".png") != 0)
      {
        filename = (gchar *) g_realloc(filename,sizeof(gchar)*(strlen(filename)+4));
        strcat(filename,".png");
      }
		}
    cairo_surface_write_to_png(surface,filename);
    g_free(filename);
	}
  cairo_destroy(cr);
	gtk_widget_destroy (dialog);
}


void show_about(GtkWidget *widget, gpointer window)
{

  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("/home/foustja/C/lorenz.png", NULL);

  GtkWidget *dialog = gtk_about_dialog_new();

  char *comments;

  const gchar *people[] = {"M. Foust<foustja@gmail.com>", NULL};

  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Fractal");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.0");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"(c) M. Foust, 2019");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
                                "C/GTK+ program for generating fractal images");

  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
     "https://github.com/foustja");
  gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),
     "https://github.com/foustja");
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);

  gtk_about_dialog_add_credit_section (GTK_ABOUT_DIALOG(dialog),
                                       "Credits", people);
  //gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG(dialog),
                                //people);

  g_object_unref(pixbuf), pixbuf = NULL;

  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
}

static void show_reference(GtkWidget *widget, gpointer window)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_OTHER,
            GTK_BUTTONS_OK,
            "\"Dynamical Systems and Fractals:\n"
            "Computer graphics experiments in Pascal,\"\n"
            "by Karl-Heinz Becker and Michael Doerfler,\n"
            "2nd edition, 1988");
  gtk_window_set_title(GTK_WINDOW(dialog), "Reference");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

static void show_parameters(GtkWidget *widget, gpointer window)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_OTHER,
            GTK_BUTTONS_OK,
            "Suggested parameters for\n"
            "Julia sets and Henon maps\n\n"
            "c = −0.122 + 0.745i\n"
            "c = −0.625 + 0.425i\n"
            "c = −0.29 − 0.695i\n"
            "a = 1.4, b = 0.3 (Henon)");

  gtk_window_set_title(GTK_WINDOW(dialog), "Parameters");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

//activate callback function - creates top level window and widgets
static void activate (GtkApplication *app, gpointer user_data)
{
  //Widgets for activate callback function
  GtkWidget *window;
  GtkWidget *drawing_area;

  GtkWidget *button_henon;
  GtkWidget *button_lorenz_xy;
  GtkWidget *button_lorenz_yz;
  GtkWidget *button_lorenz_xz;
  GtkWidget *button_julia;
  GtkWidget *button_juliasin;
  GtkWidget *button_mandel;

  GtkWidget *button_clear;
  GtkWidget *button_stop;
  GtkWidget *button_save;
  GtkWidget *button_open;


  GtkAdjustment *adj_a;
  GtkAdjustment *adj_b;

  GtkWidget *parameter_a_label;
  GtkWidget *parameter_b_label;

  GtkWidget *parameter_a_spin;
  GtkWidget *parameter_b_spin;

  GtkWidget *enter_button_a;
  GtkWidget *enter_button_b;

  GtkWidget *empty_label1;
  GtkWidget *empty_label2;

  GtkWidget *parameter_label1;
  GtkWidget *parameter_label2;
  GtkWidget *parameter_label3;
  GtkWidget *parameter_label4;

  GtkWidget *vbox_top;
  GtkWidget *vbox_bottom;
    GtkWidget *vbox_outer;
  GtkWidget *vbox;
  GtkWidget *hbox;

  GdkPixbuf *icon;
  gchar *icon_filename;
  icon_filename = "/home/foustja/C/lorenz.png";

  GtkWidget *menubar;

  GtkWidget *formula_menu;
  GtkWidget *formula_menu_item;
  GtkWidget *julia_menu_item;
  GtkWidget *juliasin_menu_item;
  GtkWidget *henon_menu_item;
  GtkWidget *lorenz_xy_menu_item;
  GtkWidget *lorenz_yz_menu_item;
  GtkWidget *lorenz_xz_menu_item;
  GtkWidget *mandel_menu_item;
  GtkWidget *clear_menu_item;
  GtkWidget *stop_menu_item;
  GtkWidget *quit_menu_item;

  GtkWidget *info_menu;
  GtkWidget *info_menu_item;
  GtkWidget *about_menu_item;
  GtkWidget *reference_menu_item;
  GtkWidget *parameter_menu_item;

  GtkWidget *file_menu;
  GtkWidget *file_menu_item;
  GtkWidget *open_menu_item;
  GtkWidget *save_menu_item;



  window = gtk_application_window_new (app);

  icon = gdk_pixbuf_new_from_file(icon_filename, NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), WINWIDTH, WINHEIGHT);
  gtk_window_set_title(GTK_WINDOW(window),
                       "GTK window for fractal generation 1200X800");
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, DAWIDTH, DAHEIGHT);

  button_henon  =    gtk_button_new_with_label("Henon");
  button_lorenz_xy = gtk_button_new_with_label("lorenz - xy");
  button_lorenz_yz = gtk_button_new_with_label("lorenz - yz");
  button_lorenz_xz = gtk_button_new_with_label("lorenz - xz");
  button_julia  =    gtk_button_new_with_label("Julia");
  button_juliasin  = gtk_button_new_with_label("JuliaSine");
  button_mandel =    gtk_button_new_with_label("Mandelbrot");
  button_clear  =    gtk_button_new_with_label("Clear screen");
  button_stop  =     gtk_button_new_with_label("Stop");
  button_save  =     gtk_button_new_with_label("Save File");
  button_open  =     gtk_button_new_with_label("Open File");

  empty_label1 = gtk_label_new("");
  empty_label2 = gtk_label_new("");

  parameter_label1 = gtk_label_new("");
  parameter_label2 = gtk_label_new("");
  parameter_label3 = gtk_label_new("");
  parameter_label4 = gtk_label_new("");

  parameter_a_label = gtk_label_new("parameter a");
  parameter_b_label = gtk_label_new("parameter b");

  adj_a = (GtkAdjustment *) gtk_adjustment_new (-0.5, -99.0, 99.0, 0.00001,
					      0.00001, 0.0);
  adj_b = (GtkAdjustment *) gtk_adjustment_new (-0.99998, -99.0, 99.0, 0.00001,
                0.00001, 0.0);

  parameter_a_spin = gtk_spin_button_new (adj_a, 0.0, 5);
  parameter_b_spin = gtk_spin_button_new (adj_b, 0.0, 5);

  enter_button_a = gtk_button_new_with_label("Enter a");
  enter_button_b = gtk_button_new_with_label("Enter b");

//Menubar and menu items
  menubar =      gtk_menu_bar_new();

  formula_menu = gtk_menu_new();
  info_menu =    gtk_menu_new();
  file_menu =    gtk_menu_new();

  formula_menu_item =    gtk_menu_item_new_with_label("Fractals");
  henon_menu_item =      gtk_menu_item_new_with_label("Henon");
  lorenz_xy_menu_item =  gtk_menu_item_new_with_label("lorenz - xy");
  lorenz_yz_menu_item =  gtk_menu_item_new_with_label("lorenz - yz");
  lorenz_xz_menu_item =  gtk_menu_item_new_with_label("lorenz - xz");
  julia_menu_item  =     gtk_menu_item_new_with_label("Julia");
  juliasin_menu_item =   gtk_menu_item_new_with_label("JuliaSine");
  mandel_menu_item =     gtk_menu_item_new_with_label("Mandelbrot");
  clear_menu_item  =     gtk_menu_item_new_with_label("Clear screen");
  stop_menu_item  =      gtk_menu_item_new_with_label("Stop");
  quit_menu_item =       gtk_menu_item_new_with_label("Quit");

  file_menu_item =       gtk_menu_item_new_with_label("File");
  open_menu_item =      gtk_menu_item_new_with_label("Open");
  save_menu_item =      gtk_menu_item_new_with_label("Save");

  info_menu_item =       gtk_menu_item_new_with_label("Info");
  about_menu_item =      gtk_menu_item_new_with_label("About");
  reference_menu_item =  gtk_menu_item_new_with_label("Reference");
  parameter_menu_item =  gtk_menu_item_new_with_label("Suggested parameters");



  gtk_menu_item_set_submenu(GTK_MENU_ITEM(formula_menu_item), formula_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), formula_menu_item);

  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), henon_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), lorenz_xy_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), lorenz_yz_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), lorenz_xz_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), julia_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), juliasin_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), mandel_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), clear_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), stop_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(formula_menu), quit_menu_item);

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_menu_item);

  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_menu_item);

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(info_menu_item), info_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), info_menu_item);

  gtk_menu_shell_append(GTK_MENU_SHELL(info_menu), about_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(info_menu), reference_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(info_menu), parameter_menu_item);

  vbox_top = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  vbox_bottom = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  vbox_outer = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

  gtk_box_pack_start(GTK_BOX(vbox_top), menubar, FALSE, FALSE, 0);

  gtk_box_pack_start (GTK_BOX (vbox), parameter_label1, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), parameter_label2, FALSE, TRUE, 5);
  //gtk_box_pack_start (GTK_BOX (vbox), parameter_label3, FALSE, TRUE, 5);
  //gtk_box_pack_start (GTK_BOX (vbox), parameter_label4, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), empty_label1, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), parameter_a_label, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), parameter_a_spin, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), enter_button_a, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), parameter_b_label, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), parameter_b_spin, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), enter_button_b, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), empty_label2, FALSE, TRUE, 5);


  //gtk_box_pack_start (GTK_BOX (vbox), button_henon, FALSE, TRUE, 5);
  //gtk_box_pack_start (GTK_BOX (vbox), button_lorenz_xy, FALSE, TRUE, 5);
  //gtk_box_pack_start (GTK_BOX (vbox), button_lorenz_yz, FALSE, TRUE, 5);
  //gtk_box_pack_start (GTK_BOX (vbox), button_julia, FALSE, TRUE, 5);
  //gtk_box_pack_start (GTK_BOX (vbox), button_juliasin, FALSE, TRUE, 5);
  //gtk_box_pack_start (GTK_BOX (vbox), button_mandel, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (vbox), button_clear, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), button_stop, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), button_save, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), button_open, FALSE, TRUE, 5);

  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), drawing_area, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox_bottom), hbox, TRUE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox_outer), vbox_top, FALSE, TRUE, 5);
  gtk_box_pack_start (GTK_BOX (vbox_outer), vbox_bottom, FALSE, TRUE, 5);

  gtk_container_add (GTK_CONTAINER (window), vbox_outer);

  g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event), NULL);

  g_signal_connect(G_OBJECT(enter_button_a), "clicked",
      G_CALLBACK(enter_button_a_clicked), parameter_a_spin);

  g_signal_connect(G_OBJECT(enter_button_b), "clicked",
      G_CALLBACK(enter_button_b_clicked), parameter_b_spin);

  g_signal_connect_swapped (button_henon, "clicked",
      G_CALLBACK (henondraw), drawing_area);

  g_signal_connect_swapped (button_lorenz_xy, "clicked",
      G_CALLBACK (lorenz_xydraw), drawing_area);

  g_signal_connect_swapped (button_lorenz_yz, "clicked",
      G_CALLBACK (lorenz_yzdraw), drawing_area);

  g_signal_connect_swapped (button_lorenz_xz, "clicked",
      G_CALLBACK (lorenz_xzdraw), drawing_area);

  g_signal_connect_swapped (button_julia, "clicked",
      G_CALLBACK (juliadraw), drawing_area);

  g_signal_connect_swapped (button_juliasin, "clicked",
      G_CALLBACK (juliasindraw), drawing_area);

  g_signal_connect_swapped (button_mandel, "clicked",
      G_CALLBACK (mandeldraw), drawing_area);

  g_signal_connect_swapped (button_clear, "clicked",
      G_CALLBACK (clear_drawing_area), drawing_area);

 g_signal_connect (G_OBJECT(button_stop), "clicked",
      G_CALLBACK (stop_function), NULL);

 g_signal_connect (G_OBJECT(button_save), "clicked",
      G_CALLBACK (save_function), drawing_area);

 g_signal_connect (G_OBJECT(button_open), "clicked",
      G_CALLBACK (open_function), drawing_area);

  g_signal_connect (drawing_area, "draw",
      G_CALLBACK(on_draw_event), NULL);

  g_signal_connect (window, "delete-event",
    	G_CALLBACK (delete_event), NULL);

//Menu item callbacks

  g_signal_connect_swapped (henon_menu_item, "activate",
    G_CALLBACK (henondraw), drawing_area);

  g_signal_connect_swapped (lorenz_xy_menu_item, "activate",
    G_CALLBACK (lorenz_xydraw), drawing_area);

  g_signal_connect_swapped (lorenz_yz_menu_item, "activate",
    G_CALLBACK (lorenz_yzdraw), drawing_area);

  g_signal_connect_swapped (lorenz_xz_menu_item, "activate",
    G_CALLBACK (lorenz_xzdraw), drawing_area);

  g_signal_connect_swapped (julia_menu_item, "activate",
    G_CALLBACK (juliadraw), drawing_area);

  g_signal_connect_swapped (juliasin_menu_item, "activate",
    G_CALLBACK (juliasindraw), drawing_area);

  g_signal_connect_swapped (mandel_menu_item, "activate",
    G_CALLBACK (mandeldraw), drawing_area);

  g_signal_connect_swapped (clear_menu_item, "activate",
    G_CALLBACK (clear_drawing_area), drawing_area);

  g_signal_connect(G_OBJECT(stop_menu_item), "activate",
      G_CALLBACK(stop_function), NULL);

  g_signal_connect(G_OBJECT(quit_menu_item), "activate",
      G_CALLBACK(stop_function), NULL);

  g_signal_connect_swapped(G_OBJECT(quit_menu_item), "activate",
      G_CALLBACK(gtk_widget_destroy), G_OBJECT(window));

  g_signal_connect(G_OBJECT(about_menu_item), "activate",
      G_CALLBACK(show_about), (gpointer) window);

  g_signal_connect(G_OBJECT(reference_menu_item), "activate",
      G_CALLBACK(show_reference), (gpointer) window);

  g_signal_connect(G_OBJECT(parameter_menu_item), "activate",
      G_CALLBACK(show_parameters), (gpointer) window);

  g_signal_connect_swapped (open_menu_item, "activate",
      G_CALLBACK (open_function), drawing_area);

  g_signal_connect_swapped (save_menu_item, "activate",
      G_CALLBACK (save_function), drawing_area);


  gtk_widget_show_all (window);
}

//Main - creates and runs application initialized by activate
int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("io.github.foustja.testprogram_fractal7",
                             G_APPLICATION_FLAGS_NONE);
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
