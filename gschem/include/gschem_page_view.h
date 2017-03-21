/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2010 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*!
 * \file gschem_page_view.h
 *
 * \brief A widget for viewing a schematic page
 */

#define GSCHEM_TYPE_PAGE_VIEW           (gschem_page_view_get_type())
#define GSCHEM_PAGE_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSCHEM_TYPE_PAGE_VIEW, GschemPageView))
#define GSCHEM_PAGE_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  GSCHEM_TYPE_PAGE_VIEW, GschemPageViewClass))
#define GSCHEM_IS_PAGE_VIEW(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSCHEM_TYPE_PAGE_VIEW))
#define GSCHEM_PAGE_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSCHEM_TYPE_PAGE_VIEW, GschemPageViewClass))

typedef struct _GschemPageViewClass GschemPageViewClass;
typedef struct _GschemPageView GschemPageView;

struct _GschemPageViewClass
{
  GtkWindowClass parent_class;
};

struct _GschemPageView
{
  GtkWindow parent;

  GtkAllocation previous_allocation;

  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;

  gboolean configured;

  gboolean doing_pan;  /* mouse pan status flag */
  int pan_x;
  int pan_y;
  int throttle;

  /*< private >*/
  PAGE *_page;

  GHashTable *_geometry_cache;
};



GtkAdjustment*
gschem_page_view_get_hadjustment (GschemPageView *view);

PAGE*
gschem_page_view_get_page (GschemPageView *view);

GschemPageGeometry*
gschem_page_view_get_page_geometry (GschemPageView *view);

GType
gschem_page_view_get_type ();

GtkAdjustment*
gschem_page_view_get_vadjustment (GschemPageView *view);

void
gschem_page_view_invalidate_all (GschemPageView *view);

void
gschem_page_view_invalidate_object (GschemPageView *view, OBJECT *object);

void
gschem_page_view_invalidate_screen_rect (GschemPageView *view, int left, int top, int right, int bottom);

void
gschem_page_view_invalidate_world_rect (GschemPageView *view, int left, int top, int right, int bottom);

GschemPageView*
gschem_page_view_new_with_page (PAGE *page);

void
gschem_page_view_pan_general(GschemPageView *page_view, int x, int y, double relativ_zoom_factor);

void
gschem_page_view_pan(GschemPageView *page_view, int x, int y);

void
gschem_page_view_pan_mouse(GschemPageView *page_view, int diff_x, int diff_y);

void
gschem_page_view_pan_start(GschemPageView *page_view, int x, int y);

void
gschem_page_view_pan_motion (GschemPageView *view, int mousepan_gain, int x, int y);

gboolean
gschem_page_view_pan_end(GschemPageView *page_view);

void
gschem_page_view_redraw (GschemPageView *view, GdkEventExpose *event, GschemToplevel *w_current);

int
gschem_page_view_SCREENabs(GschemPageView *view, int val);

void
gschem_page_view_SCREENtoWORLD (GschemPageView *view, int mx, int my, int *x, int *y);

void
gschem_page_view_set_hadjustment (GschemPageView *view, GtkAdjustment *hadjustment);

void
gschem_page_view_set_page (GschemPageView *view, PAGE *page);

void
gschem_page_view_set_vadjustment (GschemPageView *view, GtkAdjustment *vadjustment);

int
gschem_page_view_WORLDabs(GschemPageView *view, int val);

void
gschem_page_view_WORLDtoSCREEN (GschemPageView *view, int x, int y, int *px, int *py);

void
gschem_page_view_zoom_extents (GschemPageView *view, const GList *list);

OBJECT*
gschem_page_get_page_object(OBJECT *object);

void
gschem_page_view_zoom_text (GschemPageView *view, OBJECT *object);
