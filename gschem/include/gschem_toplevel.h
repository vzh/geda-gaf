/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2013 gEDA Contributors (see ChangeLog for details)
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


/* Temporary cast until GschemToplevel becomes a class */
#define GSCHEM_TOPLEVEL(ptr) ((GschemToplevel*)(ptr))


struct st_gschem_toplevel {

  TOPLEVEL *toplevel;

  /* ------------------- */
  /* main window widgets */
  /* ------------------- */
  GtkWidget *main_window;

  GtkWidget *drawing_area;
  GtkWidget *menubar;
  GtkWidget *popup_menu;

  /* Widgets at inside the bottom of the main view */
  GtkWidget *find_text_widget;
  GtkWidget *find_patch_widget;
  GtkWidget *hide_text_widget;
  GtkWidget *show_text_widget;
  GtkWidget *macro_widget;
  GtkWidget *translate_widget;

  GtkWidget *bottom_widget;

  GtkWidget *toolbar_select;
  GtkWidget *toolbar_net;
  GtkWidget *toolbar_bus;

  GtkWidget *bottom_notebook;

  /* Widgets inside notebook at bottom of window */
  GschemFindTextState *find_text_state;
  GschemLogWidget     *log_widget;

  GtkWidget *right_notebook;

  /* Widgets inside the notebook at the right of the window */
  GtkWidget *object_properties;
  GtkWidget *options_widget;
  GtkWidget *text_properties;


  gchar *keyaccel_string;               /* visual feedback when pressing
                                           keyboard accelerators */
  gboolean keyaccel_string_source_id;   /* event source ID used by above */

  /* ------------ */
  /* Dialog boxes */
  /* ------------ */
  GtkWidget *sowindow;                  /* Script open */
  GtkWidget *pfswindow;                 /* Picture File Selection window */
  GtkWidget *cswindow;                  /* component select */
  GtkWidget *pswindow;                  /* page select */
  GtkWidget *tiwindow;                  /* text input */
  GtkWidget *sewindow;                  /* slot edit */
  GtkWidget *aawindow;                  /* arc attribs */
  GtkWidget *mawindow;                  /* multi attribute */
  GtkWidget *aewindow;                  /* attribute edit */
  GtkWidget *hkwindow;                  /* Help/Hotkeys... dialog*/
  GtkWidget *cowindow;                  /* Coordinate window */
  GtkWidget *coord_world;                 /* World coordinate label */
  GtkWidget *coord_screen;                /* Screen coordinate window */

  /* -------------------------------------- */
  /* Models for widgets inside dialog boxes */
  /* -------------------------------------- */
  GtkListStore *dash_length_list_store;
  GtkListStore *dash_space_list_store;
  GtkListStore *fill_angle_list_store;
  GtkListStore *fill_pitch_list_store;
  GtkListStore *fill_width_list_store;
  GtkListStore *line_width_list_store;
  GtkListStore *text_size_list_store;

  /* ----------------------------------------- */
  /* An adapter for manipulating the selection */
  /* ----------------------------------------- */
  GschemSelectionAdapter *selection_adapter;

  /* --------------------------------- */
  /* Manager for "Open Recent..." menu */
  /* --------------------------------- */
  GtkRecentManager *recent_manager;

  /* ----------------- */
  /* Picture placement */
  /* ----------------- */
  GdkPixbuf *current_pixbuf;            /* used by add picture dialog */
  double pixbuf_wh_ratio;               /* width/height ratio of the pixbuf */
  char *pixbuf_filename;

  /* ------------- */
  /* Drawing state */
  /* ------------- */
  EdaRenderer *renderer;
  int first_wx;
  int first_wy;
  int second_wx;
  int second_wy;
  int third_wx;
  int third_wy;
  int magnetic_wx, magnetic_wy;         /* Position of the magnetic marker*/
  int distance;
  int inside_action;                    /* Are we doing an action? */
  int rubber_visible;                   /* Are there any rubber lines on
					   the screen? */
  int net_direction;                    /* bit field to guess the best net direction */
  int which_grip;                       /* Which grip is being manipulated.
                                           Its range of values depends on the
                                           type of object being manipulated. */
  OBJECT *which_object;                 /* Object being manipulated */
  PATH *temp_path;                      /* Path being created */
  gboolean pathcontrol;                 /* Set path control point while path creating */ /* FIXME: can we do without it? */

  /* ------------------ */
  /* Rubberbanding nets */
  /* ------------------ */
  GList *stretch_list;

  /* --------------------- */
  /* Gschem internal state */
  /* --------------------- */
  int num_untitled;                     /* keep track of untitled wins */
  int event_state;                      /* Current event state */
  int image_width, image_height;        /* h, w of image write */
  int min_zoom;                         /* minimum zoom factor */
  int max_zoom;                         /* maximum zoom factor */
  int inside_redraw;                    /* complex vs list redrawing */
  int drawbounding_action_mode;         /* outline vs bounding box */
  int last_drawb_mode;                  /* last above mode */
  int CONTROLKEY;                       /* control key pressed? */
  int SHIFTKEY;                         /* shift key pressed? */
  int ALTKEY;                           /* alt key pressed? */
  int buffer_number;                    /* current paste buffer in use */

  i_callback_func last_callback;        /* Last i_call* cmd executed */

  GList *clipboard_buffer;              /* buffer for system clipboard integration */

  /* ------------------ */
  /* rc/user parameters */
  /* ------------------ */
  GschemOptions *options;

  int text_caps;
  int text_size;

  int zoom_with_pan;

  int actionfeedback_mode;      /* can be either OUTLINE or BOUNDINGBOX */
  int net_direction_mode; /* controls if the net direction mode is used */
  int net_selection_mode;  /* controls the behaviour when selecting a net */
  int net_selection_state;  /* current status of the net selecting mode */
  int embed_complex;      /* controls if complex objects are embedded */
  int include_complex;    /* controls if complex objects are included */
  int scrollbars_flag;    /* controls if scrollbars are displayed */
  int log_window;         /* controls if the log windows mapped on startup */
  int log_window_type;    /* controls if the log window is decorated or not */
  int third_button;       /* controls what the third mouse button does */
  int third_button_cancel;/* controls if the third mouse button cancels actions */
  int middle_button;      /* controls what the third mouse button does */
  int scroll_wheel;       /* controls what the mouse scroll wheel does */
  int file_preview;       /* controls if the preview area is enabled or not */
  int enforce_hierarchy;  /* controls how much freedom user has when traversing the hierarchy */
  int fast_mousepan;      /* controls if text is completely drawn during mouse pan */
  int raise_dialog_boxes; /*controls if expose events raise dialog boxes*/

  /* controls if after doing a place the same component can be placed again */
  int continue_component_place;

  int undo_levels;        /* number of undo levels stored on disk */
  int undo_control;       /* sets if undo is enabled or not */
  int undo_type;          /* type of undo (disk/memory) */
  int undo_panzoom;       /* sets if pan / zoom info is saved in undo */
  int draw_grips;         /* sets if grips are enabled or not */

  int warp_cursor;        /* warp the cursor when zooming */
  int toolbars;           /* sets if the toolbar(s) are enabled or disabled */
  int handleboxes;        /* sets if the handleboxes are enabled or disabled */
  int bus_ripper_size;    /* sets size of the bus rippers */
  int bus_ripper_type;    /* sets type of the bus ripper (component or net) */
  int bus_ripper_rotation;  /* sets if the the bus ripper is symmetric or not */

  int grid_mode;          /* sets the mode of the grid (no grid, dots or mesh) */
  /* sets the mininum number of pixels necessary for the grid to be */
  /* displayed */
  int dots_grid_fixed_threshold;
  int dots_grid_dot_size; /* sets the grid dot size */
  int dots_grid_mode;     /* sets the mode of the dotted grid (either variable or fixed) */

  /* Minimum grid line pitch to display. Applies to major and minor lines. */
  int mesh_grid_display_threshold;

  /* sets the offset (in world coordinates) that are added to netname */
  /* attributes when they are attached to vertical or horizontal nets */
  int add_attribute_offset;

  int mousepan_gain;      /* Controls the gain of the mouse pan */
  int keyboardpan_gain;   /* Controls the gain of the keyboard pan */
  int select_slack_pixels; /* Number of pixels around an object we can still select it with */
  int zoom_gain;          /* Percentage increase in size for a zoom-in operation */
  int scrollpan_steps;    /* Number of scroll pan events required to traverse the viewed area */

  gboolean dont_invalidate;

  SCM smob;               /* The Scheme representation of this window */
};


void
gschem_toplevel_free (GschemToplevel *w_current);

GschemPageView*
gschem_toplevel_get_current_page_view (GschemToplevel *w_current);

GtkListStore*
gschem_toplevel_get_dash_length_list_store (GschemToplevel *w_current);

GtkListStore*
gschem_toplevel_get_dash_space_list_store (GschemToplevel *w_current);

GtkListStore*
gschem_toplevel_get_fill_angle_list_store (GschemToplevel *w_current);

GtkListStore*
gschem_toplevel_get_fill_pitch_list_store (GschemToplevel *w_current);

GtkListStore*
gschem_toplevel_get_fill_width_list_store (GschemToplevel *w_current);

GtkListStore*
gschem_toplevel_get_line_width_list_store (GschemToplevel *w_current);

GschemSelectionAdapter*
gschem_toplevel_get_selection_adapter (GschemToplevel *w_current);

GtkListStore*
gschem_toplevel_get_text_size_list_store (GschemToplevel *w_current);

TOPLEVEL*
gschem_toplevel_get_toplevel (GschemToplevel *w_current);

GschemToplevel*
gschem_toplevel_new ();

void
gschem_toplevel_notify_page_callback (GschemPageView *page_view, GParamSpec *pspec, GschemToplevel *w_current);

void
gschem_toplevel_page_changed (GschemToplevel *w_current);

void
gschem_toplevel_set_toplevel (GschemToplevel *w_current, TOPLEVEL *toplevel);

void
gschem_toplevel_page_content_changed (GschemToplevel *w_current, PAGE *page);
