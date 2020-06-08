#include "2048.h"

#include <gtk/gtk.h>

#include <cairo.h>

#include <math.h>

static int* board = NULL;

const static int TILE_SIZE = 64;
const static int TILE_MARGIN = 16;

static inline int offsetCoords(int coord) { return coord * (TILE_MARGIN + TILE_SIZE) + TILE_MARGIN; }
static inline double R(int rgb) { return ((rgb >> 16) & 0xFF) / 255.0; }
static inline double G(int rgb) { return ((rgb >> 8) & 0xFF) / 255.0; }
static inline double B(int rgb) { return ((rgb >> 0) & 0xFF) / 255.0; }

// Source: https://www.cairographics.org/samples/rounded_rectangle/
static void draw_rounded_rectangle(cairo_t *cairo, double x, double y, double width, double height, double radius) {
	const double degrees = M_PI / 180.0;
	cairo_new_sub_path(cairo);
	cairo_arc(cairo, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc(cairo, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc(cairo, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cairo, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path(cairo);
	cairo_fill(cairo);
}

static void drawTile(cairo_t *cairo, int value, int x, int y) {
	const int xOffset = offsetCoords(x), yOffset = offsetCoords(y);
	const unsigned bkg = e_background(value), frg = e_foreground(value);
	cairo_set_source_rgb(cairo, R(bkg), G(bkg), B(bkg));
	draw_rounded_rectangle(cairo, xOffset, yOffset, TILE_SIZE, TILE_SIZE, 6);
	cairo_select_font_face(cairo, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	if (value) {
		const int size = (value < 100) ? 26 : (value < 1000) ? 22 : 16;
		gchar *strValue = g_strdup_printf("%d", value);
		cairo_set_source_rgb(cairo, R(frg), G(frg), B(frg));
		cairo_select_font_face(cairo, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cairo, size);
		cairo_text_extents_t extents;
		cairo_text_extents(cairo, strValue, &extents);
		const int w = (int) (extents.width) + 2, h = size;
		cairo_move_to(cairo, xOffset + (TILE_SIZE - w) / 2, yOffset + TILE_SIZE - (TILE_SIZE - h) / 2 - 2);
		cairo_show_text(cairo, strValue);
		g_free(strValue);
	}
}

static void drawFinal(GtkWidget *widget, cairo_t *cairo) {
	const int win = e_win(), lose = e_lose();
	int width = gtk_widget_get_allocated_width(widget), height = gtk_widget_get_allocated_height(widget);
	int size = 26;
	if (win || lose) {
		cairo_set_source_rgba(cairo, R(0x888888), G(0x888888), B(0x888888), 0.5);
		cairo_paint(cairo);
		cairo_set_source_rgb(cairo, R(0x800000), G(0x800000), B(0x800000));
		cairo_set_font_size(cairo, size);
		const gchar *center = ((win) ? "You won!" : (lose) ? "Game Over!" : "");
		cairo_text_extents_t extents;
		cairo_text_extents(cairo, center, &extents);
		cairo_move_to(cairo, width / 2 - (int) (extents.width) / 2, height / 2);
		cairo_show_text(cairo, center);
	}
	size = 18;
	cairo_select_font_face(cairo, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_source_rgb(cairo, R(0x776E65), G(0x776E65), B(0x776E65));
	cairo_set_font_size(cairo, size);
	gchar *strScore = g_strdup_printf("Score: %d", e_score());
	cairo_text_extents_t extents;
	cairo_text_extents(cairo, strScore, &extents);
	cairo_move_to(cairo, TILE_MARGIN, height - size);
	cairo_show_text(cairo, "ESC to Restart!");
	cairo_move_to(cairo, width - (int) (extents.width) - TILE_MARGIN, height - size);
	cairo_show_text(cairo, strScore);
	g_free(strScore);
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, G_GNUC_UNUSED gpointer data) {
	if (!GTK_IS_BIN(widget))
		return FALSE;
	e_key_event(event->keyval);
	gtk_widget_queue_draw(gtk_bin_get_child(GTK_BIN(widget)));
	return TRUE;
}

static gboolean on_draw(GtkWidget *widget, cairo_t *cairo, G_GNUC_UNUSED gpointer data) {
	cairo_set_source_rgb(cairo, R(0xBBADA0), G(0xBBADA0), B(0xBBADA0));
	cairo_paint(cairo);
	int y = 0;
	for (; y < VERTICAL; ++y) {
		int x = 0;
		for (; x < HORIZONTAL; ++x)
			drawTile(cairo, board[x + y * 4], x, y);
	}
	drawFinal(widget, cairo);
	return FALSE;
}

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);
	board = e_init_board(GDK_KEY_Escape, GDK_KEY_Left, GDK_KEY_Right, GDK_KEY_Up, GDK_KEY_Down);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 340, 400);
	GtkWidget *drawing = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), drawing);
	g_signal_connect(G_OBJECT(drawing), "draw", G_CALLBACK(on_draw), NULL);
	g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
