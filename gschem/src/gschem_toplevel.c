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
#include <config.h>

#include "gschem.h"

static void
handle_undo (GschemToplevel *w_current);

static void
notify_options (GschemToplevel *w_current);


/* A list of common values for the dash length drop down menu
 */
static const char *routine_dash_length[] =
{
   "50",
   "75",
   "100",
   "125",
   "150"
};

#define ROUTINE_DASH_LENGTH_COUNT (sizeof(routine_dash_length)/sizeof(char*))



/* A list of common values for the dash space drop down menu
 */
static const char *routine_dash_space[] =
{
   "50",
   "75",
   "100",
   "125",
   "150"
};

#define ROUTINE_DASH_SPACE_COUNT (sizeof(routine_dash_space)/sizeof(char*))



/* A list of common values for the fill angle drop down menu
 */
static const char *routine_fill_angle[] =
{
   "0",
   "30",
   "45",
   "60",
   "90",
   "120",
   "135",
   "150"
};

#define ROUTINE_FILL_ANGLE_COUNT (sizeof(routine_fill_angle)/sizeof(char*))



/* A list of common values for the fill pitch drop down menu
 */
static const char *routine_fill_pitch[] =
{
   "50",
   "100",
   "150",
   "200"
};

#define ROUTINE_FILL_PITCH_COUNT (sizeof(routine_fill_pitch)/sizeof(char*))



/* A list of common values for the fill line width drop down menu
 */
static const char *routine_fill_width[] =
{
   "0",
   "5",
   "10",
   "15",
   "20"
};

#define ROUTINE_FILL_WIDTH_COUNT (sizeof(routine_fill_width)/sizeof(char*))



/* A list of common values for the line width drop down menu
 */
static const char *routine_line_width[] =
{
   "0",
   "5",
   "10",
   "15",
   "20"
};

#define ROUTINE_LINE_WIDTH_COUNT (sizeof(routine_line_width)/sizeof(char*))



/* A list of common sizes for the drop down menu
 */
static const char *routine_text_size[] =
{
   "8",
   "9",
   "10",
   "11",
   "12",
   "14",
   "16",
   "18",
   "20",
   "22",
   "24",
   "26"
};

#define ROUTINE_TEXT_SIZE_COUNT (sizeof(routine_text_size)/sizeof(char*))



GschemToplevel *gschem_toplevel_new ()
{
  GschemToplevel *w_current;

  w_current = g_new0 (GschemToplevel, 1);

  w_current->toplevel = NULL;

  w_current->dont_invalidate = FALSE;

  /* ------------------- */
  /* main window widgets */
  /* ------------------- */
  w_current->main_window  = NULL;
  w_current->drawing_area = NULL;
  w_current->menubar      = NULL;
  w_current->popup_menu   = NULL;
  w_current->find_text_widget = NULL;
  w_current->find_patch_widget = NULL;
  w_current->macro_widget  = NULL;
  w_current->bottom_widget = NULL;
  w_current->translate_widget = NULL;

  w_current->toolbar_select = NULL;
  w_current->toolbar_net    = NULL;
  w_current->toolbar_bus    = NULL;

  w_current->bottom_notebook = NULL;

  w_current->find_text_state = NULL;
  w_current->log_widget      = NULL;

  w_current->keyaccel_string = NULL;
  w_current->keyaccel_string_source_id = FALSE;

  /* ------------ */
  /* Dialog boxes */
  /* ------------ */
  w_current->sowindow     = NULL;
  w_current->pfswindow    = NULL;
  w_current->cswindow     = NULL;
  w_current->pswindow     = NULL;
  w_current->tiwindow     = NULL;
  w_current->sewindow     = NULL;
  w_current->aawindow     = NULL;
  w_current->mawindow     = NULL;
  w_current->aewindow     = NULL;
  w_current->hkwindow     = NULL;
  w_current->cowindow     = NULL;
  w_current->coord_world  = NULL;
  w_current->coord_screen = NULL;

  /* ------------------------------- */
  /* Manager for recently used files */
  /* ------------------------------- */
  w_current->recent_manager = NULL;

  /* -------------------------------------- */
  /* Models for widgets inside dialog boxes */
  /* -------------------------------------- */
  w_current->dash_length_list_store = NULL;
  w_current->dash_space_list_store = NULL;
  w_current->fill_angle_list_store = NULL;
  w_current->fill_pitch_list_store = NULL;
  w_current->fill_width_list_store = NULL;
  w_current->line_width_list_store = NULL;
  w_current->text_size_list_store = NULL;

  /* ----------------------------------------- */
  /* An adapter for manipulating the selection */
  /* ----------------------------------------- */
  w_current->selection_adapter = NULL;

  /* ----------------- */
  /* Picture placement */
  /* ----------------- */
  w_current->current_pixbuf = NULL;
  w_current->pixbuf_filename = NULL;
  w_current->pixbuf_wh_ratio = 0;

  /* ------------- */
  /* Drawing state */
  /* ------------- */
  w_current->renderer = EDA_RENDERER (g_object_new (EDA_TYPE_RENDERER, NULL));
  w_current->first_wx = -1;
  w_current->first_wy = -1;
  w_current->second_wx = -1;
  w_current->second_wy = -1;
  w_current->third_wx = -1;
  w_current->third_wy = -1;
  w_current->distance = 0;
  w_current->magnetic_wx = -1;
  w_current->magnetic_wy = -1;
  w_current->inside_action = 0;
  w_current->rubber_visible = 0;
  w_current->net_direction = 0;
  w_current->which_grip = -1;
  w_current->which_object = NULL;
  w_current->temp_path = NULL;
  w_current->pathcontrol = TRUE;

  /* ------------------ */
  /* Rubberbanding nets */
  /* ------------------ */
  w_current->stretch_list = NULL;

  /* --------------------- */
  /* Gschem internal state */
  /* --------------------- */
  w_current->num_untitled = 0;
  w_current->event_state = SELECT;
  w_current->image_width  = 0;
  w_current->image_height = 0;
  w_current->min_zoom = 0;
  w_current->max_zoom = 8;
  w_current->drawbounding_action_mode = FREE;
  w_current->last_drawb_mode = LAST_DRAWB_MODE_NONE;
  w_current->CONTROLKEY = 0;
  w_current->SHIFTKEY   = 0;
  w_current->ALTKEY     = 0;
  w_current->buffer_number = 0;
  w_current->last_callback = NULL;
  w_current->clipboard_buffer = NULL;

  /* ------------------ */
  /* rc/user parameters */
  /* ------------------ */
  w_current->options = gschem_options_new();

  g_signal_connect_swapped (G_OBJECT (w_current->options),
                            "notify",
                            G_CALLBACK (notify_options),
                            w_current);

  w_current->text_caps = 0;
  w_current->text_size = 0;

  w_current->zoom_with_pan = 0;
  w_current->actionfeedback_mode = OUTLINE;
  w_current->net_direction_mode = TRUE;
  w_current->net_selection_mode = 0;
  w_current->net_selection_state = 0;
  w_current->embed_complex = 0;
  w_current->include_complex = 0;
  w_current->scrollbars_flag = 0;
  w_current->log_window = 0;
  w_current->log_window_type = 0;
  w_current->third_button = 0;
  w_current->third_button_cancel = TRUE;
  w_current->middle_button = 0;
  w_current->file_preview = 0;
  w_current->enforce_hierarchy = 0;
  w_current->fast_mousepan = 0;
  w_current->raise_dialog_boxes = 0;
  w_current->continue_component_place = 0;
  w_current->undo_levels = 0;
  w_current->undo_control = 0;
  w_current->undo_type = 0;
  w_current->undo_panzoom = 0;
  w_current->draw_grips = 0;
  w_current->warp_cursor = 0;
  w_current->toolbars = 0;
  w_current->handleboxes = 0;
  w_current->bus_ripper_size = 0;
  w_current->bus_ripper_type = 0;
  w_current->bus_ripper_rotation = 0;
  w_current->grid_mode = GRID_MODE_NONE;
  w_current->dots_grid_fixed_threshold = 10;
  w_current->dots_grid_dot_size = 1;
  w_current->dots_grid_mode = DOTS_GRID_VARIABLE_MODE;
  w_current->mesh_grid_display_threshold = 3;
  w_current->add_attribute_offset = 50;
  w_current->mousepan_gain = 5;
  w_current->keyboardpan_gain = 10;
  w_current->select_slack_pixels = 4;
  w_current->zoom_gain = 20;
  w_current->scrollpan_steps = 8;

  w_current->smob = SCM_UNDEFINED;

  return w_current;
}



/*! \brief Free the gschem toplevel
 *
 *  \param [in] w_current The gschem toplevel
 */
void
gschem_toplevel_free (GschemToplevel *w_current)
{
  if (w_current->toplevel != NULL) {
    s_toplevel_delete (w_current->toplevel);
    w_current->toplevel = NULL;
  }

  if (w_current->dash_length_list_store != NULL) {
    g_object_unref (w_current->dash_length_list_store);
    w_current->dash_length_list_store = NULL;
  }

  if (w_current->dash_space_list_store != NULL) {
    g_object_unref (w_current->dash_space_list_store);
    w_current->dash_space_list_store = NULL;
  }

  if (w_current->fill_angle_list_store != NULL) {
    g_object_unref (w_current->fill_angle_list_store);
    w_current->fill_angle_list_store = NULL;
  }

  if (w_current->fill_pitch_list_store != NULL) {
    g_object_unref (w_current->fill_pitch_list_store);
    w_current->fill_pitch_list_store = NULL;
  }

  if (w_current->fill_width_list_store != NULL) {
    g_object_unref (w_current->fill_width_list_store);
    w_current->fill_width_list_store = NULL;
  }

  if (w_current->line_width_list_store != NULL) {
    g_object_unref (w_current->line_width_list_store);
    w_current->line_width_list_store = NULL;
  }

  if (w_current->text_size_list_store != NULL) {
    g_object_unref (w_current->text_size_list_store);
    w_current->text_size_list_store = NULL;
  }

  if (w_current->options != NULL) {
    g_object_unref (w_current->options);
    w_current->options = NULL;
  }

  if (w_current->renderer != NULL) {
    g_object_unref (w_current->renderer);
    w_current->renderer = NULL;
  }

  g_free (w_current);
}



/*! \brief Get the selection adapter
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return The selection adapter
 */
GschemPageView*
gschem_toplevel_get_current_page_view (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  return GSCHEM_PAGE_VIEW (w_current->drawing_area);
}



/*! \brief Get the selection adapter
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return The selection adapter
 */
GschemSelectionAdapter*
gschem_toplevel_get_selection_adapter (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);
  g_return_val_if_fail (w_current->toplevel != NULL, NULL);

  if (w_current->selection_adapter == NULL) {
    w_current->selection_adapter = gschem_selection_adapter_new ();

    g_signal_connect_swapped (w_current->selection_adapter,
                              "handle-undo",
                              G_CALLBACK (handle_undo),
                              w_current);


    gschem_selection_adapter_set_toplevel (w_current->selection_adapter, w_current->toplevel);

    if (w_current->toplevel->page_current != NULL) {
      gschem_selection_adapter_set_selection (w_current->selection_adapter,
                                              w_current->toplevel->page_current->selection_list);
    } else {
      gschem_selection_adapter_set_selection (w_current->selection_adapter,
                                              NULL);
    }
  }

  return w_current->selection_adapter;
}



/*! \brief Get a list of the commonly used dash lengths
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return A list of the commonly used dash_lengths
 */
GtkListStore*
gschem_toplevel_get_dash_length_list_store (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  if (w_current->dash_length_list_store == NULL) {
    w_current->dash_length_list_store = x_integerls_new_with_values (routine_dash_length, ROUTINE_DASH_LENGTH_COUNT);
  }

  return w_current->dash_length_list_store;
}



/*! \brief Get a list of the commonly used dash spacing
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return A list of the commonly used dash spacing
 */
GtkListStore*
gschem_toplevel_get_dash_space_list_store (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  if (w_current->dash_space_list_store == NULL) {
    w_current->dash_space_list_store = x_integerls_new_with_values (routine_dash_space, ROUTINE_DASH_SPACE_COUNT);
  }

  return w_current->dash_space_list_store;
}



/*! \brief Get a list of the commonly used fill angles
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return A list of the commonly used fill angles
 */
GtkListStore*
gschem_toplevel_get_fill_angle_list_store (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  if (w_current->fill_angle_list_store == NULL) {
    w_current->fill_angle_list_store = x_integerls_new_with_values (routine_fill_angle, ROUTINE_FILL_ANGLE_COUNT);
  }

  return w_current->fill_angle_list_store;
}



/*! \brief Get a list of the commonly used fill pitches
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return A list of the commonly used fill pitches
 */
GtkListStore*
gschem_toplevel_get_fill_pitch_list_store (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  if (w_current->fill_pitch_list_store == NULL) {
    w_current->fill_pitch_list_store = x_integerls_new_with_values (routine_fill_pitch, ROUTINE_FILL_PITCH_COUNT);
  }

  return w_current->fill_pitch_list_store;
}



/*! \brief Get a list of the commonly used fill line widths
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return A list of the commonly used fill line widths
 */
GtkListStore*
gschem_toplevel_get_fill_width_list_store (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  if (w_current->fill_width_list_store == NULL) {
    w_current->fill_width_list_store = x_integerls_new_with_values (routine_fill_width, ROUTINE_FILL_WIDTH_COUNT);
  }

  return w_current->fill_width_list_store;
}



/*! \brief Get a list of the commonly used line widths
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return A list of the commonly used line widths
 */
GtkListStore*
gschem_toplevel_get_line_width_list_store (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  if (w_current->line_width_list_store == NULL) {
    w_current->line_width_list_store = x_integerls_new_with_values (routine_line_width, ROUTINE_LINE_WIDTH_COUNT);
  }

  return w_current->line_width_list_store;
}



/*! \brief Get a list of the commonly used text sizes
 *
 *  \param [in] w_current The current gschem toplevel
 *  \return A list of the commonly used text sizes
 */
GtkListStore*
gschem_toplevel_get_text_size_list_store (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  if (w_current->text_size_list_store == NULL) {
    w_current->text_size_list_store = x_integerls_new_with_values (routine_text_size, ROUTINE_TEXT_SIZE_COUNT);
  }

  return w_current->text_size_list_store;
}



/*! \brief Get the libgeda toplevel for this gschem toplevel
 *
 *  \param [in] w_current This gschem toplevel
 *  \return The libgeda toplevel
 */
TOPLEVEL*
gschem_toplevel_get_toplevel (GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, NULL);

  return w_current->toplevel;
}



/*! \brief Signal handler for a notify::page signal
 *
 *  \param [in] page_view The GschemPageView signal source
 *  \param [in] w_current The current gschem toplevel
 */
void
gschem_toplevel_notify_page_callback (GschemPageView *page_view, GParamSpec *pspec, GschemToplevel *w_current)
{
  g_return_if_fail (w_current != NULL);

  gschem_toplevel_page_changed (w_current);
}



/*! \brief Temporary function to notify dialogs of a page change
 *
 *  This function is temporary until the toplevel can emit a
 *  "notify::page_current" signal.
 *
 *  \param [in] w_current The current gschem toplevel
 */
void
gschem_toplevel_page_changed (GschemToplevel *w_current)
{
  g_return_if_fail (w_current != NULL);
  g_return_if_fail (w_current->toplevel != NULL);

  if ((w_current->selection_adapter != NULL) && (w_current->toplevel->page_current != NULL)) {
    gschem_selection_adapter_set_selection (w_current->selection_adapter, w_current->toplevel->page_current->selection_list);
  }
}



/*! \brief Temporary function to notify of page content change
 *
 *  This function is temporary until library functions can emit signals.
 *
 *  \param [in] w_current The current gschem toplevel.
 *  \param [in] page      The page that underwent changes.
 */
void
gschem_toplevel_page_content_changed (GschemToplevel *w_current, PAGE *page)
{
  g_return_if_fail (page != NULL);
  g_return_if_fail (w_current != NULL);

  page->CHANGED = 1;

  x_pagesel_update (w_current);
}


/*! \brief Set the libgeda toplevel for this gschem toplevel
 *
 *  \param [in] w_current This gschem toplevel
 *  \param [in] toplevel The libgeda toplevel
 */
void
gschem_toplevel_set_toplevel (GschemToplevel *w_current, TOPLEVEL *toplevel)
{
  g_return_if_fail (w_current != NULL);

  w_current->toplevel = toplevel;
}

/*! \brief Allow the undo manager to process changes
 *
 *  \param [in] w_current
 */
static void
handle_undo (GschemToplevel *w_current)
{
  TOPLEVEL *toplevel;

  g_return_if_fail (w_current != NULL);

  toplevel = gschem_toplevel_get_toplevel (w_current);
  g_return_if_fail (toplevel != NULL);

  gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
  o_undo_savestate_old (w_current, UNDO_ALL);
}



/*! \brief Property change notification for any/all settings
 *
 *  \param [in] w_current
 */
static void
notify_options (GschemToplevel *w_current)
{
  g_return_if_fail (w_current != NULL);

  /* Events can occur before the drawing area is created */

  if (w_current->drawing_area != NULL) {
    /* maybe remove those two lines
     * gschem_toplevel_get_toplevel (w_current)->page_current->CHANGED=1;
     * o_undo_savestate_old(w_current, UNDO_ALL);
     */

    i_update_grid_info (w_current);
    gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
  }
}
