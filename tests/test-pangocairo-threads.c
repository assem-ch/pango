#include <stdlib.h>
#include <string.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#define WIDTH 100
#define HEIGHT 100
const char *text = "The quick brown fox jumped over the lazy dog!";
int num_iters = 100;

GMutex mutex;

static cairo_surface_t *
create_surface (void)
{
  return cairo_image_surface_create (CAIRO_FORMAT_A8, WIDTH, HEIGHT);
}

static PangoLayout *
create_layout (cairo_t *cr)
{
  PangoLayout *layout = pango_cairo_create_layout (cr);
  pango_layout_set_text (layout, text, -1);
  pango_layout_set_width (layout, WIDTH * PANGO_SCALE);
  return layout;
}

static void
draw (cairo_t *cr, PangoLayout *layout)
{
  cairo_set_source_rgba (cr, 1, 1, 1, 1);
  cairo_paint (cr);
  cairo_set_source_rgba (cr, 1, 1, 1, 0);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

  /* force a relayout */
  pango_layout_context_changed (layout);

  pango_cairo_show_layout (cr, layout);
}

static gpointer
thread_func (gpointer data)
{
  cairo_surface_t *surface = data;
  PangoLayout *layout;
  int i;

  cairo_t *cr = cairo_create (surface);

  layout = create_layout (cr);

  g_mutex_lock (&mutex);
  g_mutex_unlock (&mutex);

  for (i = 0; i < num_iters; i++)
    draw (cr, layout);

  return 0;
}

int
main (int argc, char **argv)
{
  int num_threads = 2;
  int i;
  GPtrArray *threads = g_ptr_array_new ();
  GPtrArray *surfaces = g_ptr_array_new ();

  if (argc > 1)
    num_threads = atoi (argv[1]);
  if (argc > 2)
    num_iters = atoi (argv[2]);

  g_mutex_lock (&mutex);

  for (i = 0; i < num_threads; i++)
    {
      cairo_surface_t *surface = create_surface ();
      g_ptr_array_add (surfaces, surface);
      g_ptr_array_add (threads,
		       g_thread_new (g_strdup_printf ("%d", i),
				     thread_func,
				     surface));
    }

  /* Let them loose! */
  g_mutex_unlock (&mutex);

  for (i = 0; i < num_threads; i++)
    g_thread_join (g_ptr_array_index (threads, i));

  /* Now, draw a reference image and check results. */
  {
    cairo_surface_t *ref_surface = create_surface ();
    cairo_t *cr = cairo_create (ref_surface);
    PangoLayout *layout = create_layout (cr);
    unsigned char *ref_data = cairo_image_surface_get_data (ref_surface);
    unsigned int len = WIDTH * HEIGHT;

    draw (cr, layout);

    /* cairo_surface_write_to_png (ref_surface, "test-pangocairo-threads-reference.png"); */

    g_assert (WIDTH == cairo_format_stride_for_width (CAIRO_FORMAT_A8, WIDTH));

    for (i = 0; i < num_threads; i++)
      {
	cairo_surface_t *surface = g_ptr_array_index (surfaces, i);
	unsigned char *data = cairo_image_surface_get_data (surface);
	if (memcmp (ref_data, data, len))
	  {
	    fprintf (stderr, "image for thread %d different from reference image.\n", i);
	    cairo_surface_write_to_png (ref_surface, "test-pangocairo-threads-reference.png");
	    cairo_surface_write_to_png (surface, "test-pangocairo-threads-failed.png");
	    return 1;
	  }
      }
  }

  return 0;
}