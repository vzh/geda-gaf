/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2011 gEDA Contributors (see ChangeLog for details)
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

#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gschem.h"

/*! \brief */
struct gsubr_t {
  const char* name;
  int req;
  int opt;
  int rst;
  SCM (*fnc)();
};

/*! \brief */
static struct gsubr_t gschem_funcs[] = {
  /* rc file */
  { "gschem-version",               1, 0, 0, (SCM (*) ()) g_rc_gschem_version },

  { "display-color-map",            0, 1, 0, (SCM (*) ()) g_rc_display_color_map },
  { "display-outline-color-map",    0, 1, 0, (SCM (*) ()) g_rc_display_outline_color_map },

  { "net-direction-mode",           1, 0, 0, (SCM (*) ()) g_rc_net_direction_mode },
  { "net-selection-mode",           1, 0, 0, (SCM (*) ()) g_rc_net_selection_mode },
  { "zoom-with-pan",                1, 0, 0, (SCM (*) ()) g_rc_zoom_with_pan },
  { "action-feedback-mode",         1, 0, 0, (SCM (*) ()) g_rc_action_feedback_mode },
  { "scrollbars",                   1, 0, 0, (SCM (*) ()) g_rc_scrollbars },
  { "embed-components",             1, 0, 0, (SCM (*) ()) g_rc_embed_components },
  { "logging",                      1, 0, 0, (SCM (*) ()) g_rc_logging },
  { "text-size",                    1, 0, 0, (SCM (*) ()) g_rc_text_size },
  { "snap-size",                    1, 0, 0, (SCM (*) ()) g_rc_snap_size },

  { "text-caps-style",              1, 0, 0, (SCM (*) ()) g_rc_text_caps_style },
  { "logging-destination",          1, 0, 0, (SCM (*) ()) g_rc_logging_destination },

  { "attribute-name",               1, 0, 0, (SCM (*) ()) g_rc_attribute_name },

  { "image-color",                  1, 0, 0, (SCM (*) ()) g_rc_image_color },
  { "image-size",                   2, 0, 0, (SCM (*) ()) g_rc_image_size },
  { "log-window",                   1, 0, 0, (SCM (*) ()) g_rc_log_window },
  { "log-window-type",              1, 0, 0, (SCM (*) ()) g_rc_log_window_type },
  { "third-button",                 1, 0, 0, (SCM (*) ()) g_rc_third_button },
  { "third-button-cancel",          1, 0, 0, (SCM (*) ()) g_rc_third_button_cancel },
  { "middle-button",                1, 0, 0, (SCM (*) ()) g_rc_middle_button },
  { "scroll-wheel",                 1, 0, 0, (SCM (*) ()) g_rc_scroll_wheel },
  { "net-consolidate",              1, 0, 0, (SCM (*) ()) g_rc_net_consolidate },
  { "file-preview",                 1, 0, 0, (SCM (*) ()) g_rc_file_preview },
  { "enforce-hierarchy",            1, 0, 0, (SCM (*) ()) g_rc_enforce_hierarchy },
  { "fast-mousepan",                1, 0, 0, (SCM (*) ()) g_rc_fast_mousepan },
  { "raise-dialog-boxes-on-expose", 1, 0, 0, (SCM (*) ()) g_rc_raise_dialog_boxes_on_expose },
  { "continue-component-place",     1, 0, 0, (SCM (*) ()) g_rc_continue_component_place },
  { "undo-levels",                  1, 0, 0, (SCM (*) ()) g_rc_undo_levels },
  { "undo-control",                 1, 0, 0, (SCM (*) ()) g_rc_undo_control },
  { "undo-type",                    1, 0, 0, (SCM (*) ()) g_rc_undo_type },
  { "undo-panzoom",                 1, 0, 0, (SCM (*) ()) g_rc_undo_panzoom },

  { "draw-grips",                   1, 0, 0, (SCM (*) ()) g_rc_draw_grips },
  { "netconn-rubberband",           1, 0, 0, (SCM (*) ()) g_rc_netconn_rubberband },
  { "magnetic-net-mode",            1, 0, 0, (SCM (*) ()) g_rc_magnetic_net_mode },
  { "add-menu",                     2, 0, 0, (SCM (*) ()) g_rc_add_menu },
  { "window-size",                  2, 0, 0, (SCM (*) ()) g_rc_window_size },
  { "warp-cursor",                  1, 0, 0, (SCM (*) ()) g_rc_warp_cursor },
  { "toolbars",                     1, 0, 0, (SCM (*) ()) g_rc_toolbars },
  { "handleboxes",                  1, 0, 0, (SCM (*) ()) g_rc_handleboxes },
  { "bus-ripper-size",              1, 0, 0, (SCM (*) ()) g_rc_bus_ripper_size },
  { "bus-ripper-type",              1, 0, 0, (SCM (*) ()) g_rc_bus_ripper_type },
  { "bus-ripper-rotation",          1, 0, 0, (SCM (*) ()) g_rc_bus_ripper_rotation },
  { "force-boundingbox",            1, 0, 0, (SCM (*) ()) g_rc_force_boundingbox },
  { "grid-mode",                    1, 0, 0, (SCM (*) ()) g_rc_grid_mode },
  { "dots-grid-dot-size",           1, 0, 0, (SCM (*) ()) g_rc_dots_grid_dot_size },
  { "dots-grid-mode",               1, 0, 0, (SCM (*) ()) g_rc_dots_grid_mode },
  { "dots-grid-fixed-threshold",    1, 0, 0, (SCM (*) ()) g_rc_dots_grid_fixed_threshold },
  { "mesh-grid-display-threshold",  1, 0, 0, (SCM (*) ()) g_rc_mesh_grid_display_threshold },
  { "add-attribute-offset",         1, 0, 0, (SCM (*) ()) g_rc_add_attribute_offset },
  { "mousepan-gain",                1, 0, 0, (SCM (*) ()) g_rc_mousepan_gain },
  { "keyboardpan-gain",             1, 0, 0, (SCM (*) ()) g_rc_keyboardpan_gain },
  { "select-slack-pixels",          1, 0, 0, (SCM (*) ()) g_rc_select_slack_pixels },
  { "zoom-gain",                    1, 0, 0, (SCM (*) ()) g_rc_zoom_gain },
  { "scrollpan-steps",              1, 0, 0, (SCM (*) ()) g_rc_scrollpan_steps },

  /* backup functions */
  { "auto-save-interval",           1, 0, 0, (SCM (*) ()) g_rc_auto_save_interval },

  /* general guile functions */
  { "gschem-pdf",                   1, 0, 0, (SCM (*) ()) g_funcs_pdf },
  { "gschem-image",                 1, 0, 0, (SCM (*) ()) g_funcs_image },
  { "gschem-use-rc-values",         0, 0, 0, (SCM (*) ()) g_funcs_use_rc_values },
  { "gschem-exit",                  0, 0, 0, (SCM (*) ()) g_funcs_exit },
  { "gschem-log",                   1, 0, 0, (SCM (*) ()) g_funcs_log },
  { "gschem-msg",                   1, 0, 0, (SCM (*) ()) g_funcs_msg },
  { "gschem-confirm",               1, 0, 0, (SCM (*) ()) g_funcs_confirm },
  { "gschem-filesel",               2, 0, 1, (SCM (*) ()) g_funcs_filesel },

  /* keymapping callbacks */
  { "file-new-window",              0, 0, 0, (SCM (*) ()) g_keys_file_new_window },
  { "file-new",                     0, 0, 0, (SCM (*) ()) g_keys_file_new },
  { "file-open",                    0, 0, 0, (SCM (*) ()) g_keys_file_open },
  { "file-script",                  0, 0, 0, (SCM (*) ()) g_keys_file_script },
  { "file-save",                    0, 0, 0, (SCM (*) ()) g_keys_file_save },
  { "file-save-as",                 0, 0, 0, (SCM (*) ()) g_keys_file_save_as },
  { "file-save-all",                0, 0, 0, (SCM (*) ()) g_keys_file_save_all },
  { "file-print",                   0, 0, 0, (SCM (*) ()) g_keys_file_print },
  { "file-image",                   0, 0, 0, (SCM (*) ()) g_keys_file_write_png },
  { "file-close-window",            0, 0, 0, (SCM (*) ()) g_keys_file_close },
  { "file-quit",                    0, 0, 0, (SCM (*) ()) g_keys_file_quit },
  { "edit-undo",                    0, 0, 0, (SCM (*) ()) g_keys_edit_undo },
  { "edit-redo",                    0, 0, 0, (SCM (*) ()) g_keys_edit_redo },
  { "edit-select",                  0, 0, 0, (SCM (*) ()) g_keys_edit_select },
  { "edit-select-all",              0, 0, 0, (SCM (*) ()) g_keys_edit_select_all },
  { "edit-deselect",                0, 0, 0, (SCM (*) ()) g_keys_edit_deselect },
  { "edit-copy",                    0, 0, 0, (SCM (*) ()) g_keys_edit_copy },
  { "edit-mcopy",                   0, 0, 0, (SCM (*) ()) g_keys_edit_mcopy },
  { "edit-move",                    0, 0, 0, (SCM (*) ()) g_keys_edit_move },
  { "edit-delete",                  0, 0, 0, (SCM (*) ()) g_keys_edit_delete },
  { "edit-rotate-90",               0, 0, 0, (SCM (*) ()) g_keys_edit_rotate_90 },
  { "edit-mirror",                  0, 0, 0, (SCM (*) ()) g_keys_edit_mirror },
  { "edit-slot",                    0, 0, 0, (SCM (*) ()) g_keys_edit_slot },
  { "edit-color",                   0, 0, 0, (SCM (*) ()) g_keys_edit_color },
  { "edit-edit",                    0, 0, 0, (SCM (*) ()) g_keys_edit_edit },
  { "edit-text",                    0, 0, 0, (SCM (*) ()) g_keys_edit_text },
  { "edit-lock",                    0, 0, 0, (SCM (*) ()) g_keys_edit_lock },
  { "edit-unlock",                  0, 0, 0, (SCM (*) ()) g_keys_edit_unlock },
  { "edit-linetype",                0, 0, 0, (SCM (*) ()) g_keys_edit_linetype },
  { "edit-filltype",                0, 0, 0, (SCM (*) ()) g_keys_edit_filltype },
  { "edit-pin-type",                0, 0, 0, (SCM (*) ()) g_keys_edit_pin_type },
  { "edit-translate",               0, 0, 0, (SCM (*) ()) g_keys_edit_translate },
  { "edit-invoke-macro",            0, 0, 0, (SCM (*) ()) g_keys_edit_invoke_macro },
  { "edit-embed",                   0, 0, 0, (SCM (*) ()) g_keys_edit_embed },
  { "edit-unembed",                 0, 0, 0, (SCM (*) ()) g_keys_edit_unembed },
  { "edit-update",                  0, 0, 0, (SCM (*) ()) g_keys_edit_update },
  { "edit-show-hidden",             0, 0, 0, (SCM (*) ()) g_keys_edit_show_hidden },
  { "edit-find-text",               0, 0, 0, (SCM (*) ()) g_keys_edit_find },
  { "edit-find-patch",              0, 0, 0, (SCM (*) ()) g_keys_edit_find_patch },
  { "edit-show-text",               0, 0, 0, (SCM (*) ()) g_keys_edit_show_text },
  { "edit-hide-text",               0, 0, 0, (SCM (*) ()) g_keys_edit_hide_text },
  { "edit-autonumber",              0, 0, 0, (SCM (*) ()) g_keys_edit_autonumber_text },

  { "clipboard-copy",               0, 0, 0, (SCM (*) ()) g_keys_clipboard_copy },
  { "clipboard-cut",                0, 0, 0, (SCM (*) ()) g_keys_clipboard_cut },
  { "clipboard-paste",              0, 0, 0, (SCM (*) ()) g_keys_clipboard_paste },

  { "buffer-copy1",                 0, 0, 0, (SCM (*) ()) g_keys_buffer_copy1 },
  { "buffer-copy2",                 0, 0, 0, (SCM (*) ()) g_keys_buffer_copy2 },
  { "buffer-copy3",                 0, 0, 0, (SCM (*) ()) g_keys_buffer_copy3 },
  { "buffer-copy4",                 0, 0, 0, (SCM (*) ()) g_keys_buffer_copy4 },
  { "buffer-copy5",                 0, 0, 0, (SCM (*) ()) g_keys_buffer_copy5 },
  { "buffer-cut1",                  0, 0, 0, (SCM (*) ()) g_keys_buffer_cut1 },
  { "buffer-cut2",                  0, 0, 0, (SCM (*) ()) g_keys_buffer_cut2 },
  { "buffer-cut3",                  0, 0, 0, (SCM (*) ()) g_keys_buffer_cut3 },
  { "buffer-cut4",                  0, 0, 0, (SCM (*) ()) g_keys_buffer_cut4 },
  { "buffer-cut5",                  0, 0, 0, (SCM (*) ()) g_keys_buffer_cut5 },
  { "buffer-paste1",                0, 0, 0, (SCM (*) ()) g_keys_buffer_paste1 },
  { "buffer-paste2",                0, 0, 0, (SCM (*) ()) g_keys_buffer_paste2 },
  { "buffer-paste3",                0, 0, 0, (SCM (*) ()) g_keys_buffer_paste3 },
  { "buffer-paste4",                0, 0, 0, (SCM (*) ()) g_keys_buffer_paste4 },
  { "buffer-paste5",                0, 0, 0, (SCM (*) ()) g_keys_buffer_paste5 },

  { "view-sidebar",                 0, 0, 0, (SCM (*) ()) g_keys_view_sidebar },
  { "view-status",                  0, 0, 0, (SCM (*) ()) g_keys_view_status },
  { "view-redraw",                  0, 0, 0, (SCM (*) ()) g_keys_view_redraw },
  { "view-zoom-full",               0, 0, 0, (SCM (*) ()) g_keys_view_zoom_full },
  { "view-zoom-extents",            0, 0, 0, (SCM (*) ()) g_keys_view_zoom_extents },
  { "view-zoom-in",                 0, 0, 0, (SCM (*) ()) g_keys_view_zoom_in },
  { "view-zoom-out",                0, 0, 0, (SCM (*) ()) g_keys_view_zoom_out },
  { "view-zoom-box",                0, 0, 0, (SCM (*) ()) g_keys_view_zoom_box },
  { "view-pan",                     0, 0, 0, (SCM (*) ()) g_keys_view_pan },
  { "view-pan-left",                0, 0, 0, (SCM (*) ()) g_keys_view_pan_left },
  { "view-pan-right",               0, 0, 0, (SCM (*) ()) g_keys_view_pan_right },
  { "view-pan-up",                  0, 0, 0, (SCM (*) ()) g_keys_view_pan_up },
  { "view-pan-down",                0, 0, 0, (SCM (*) ()) g_keys_view_pan_down },
  { "view-dark-colors",             0, 0, 0, (SCM (*) ()) g_keys_view_dark_colors },
  { "view-light-colors",            0, 0, 0, (SCM (*) ()) g_keys_view_light_colors },
  { "view-bw-colors",               0, 0, 0, (SCM (*) ()) g_keys_view_bw_colors },
  { "page-manager",                 0, 0, 0, (SCM (*) ()) g_keys_page_manager },
  { "page-next",                    0, 0, 0, (SCM (*) ()) g_keys_page_next },
  { "page-prev",                    0, 0, 0, (SCM (*) ()) g_keys_page_prev },
  { "page-close",                   0, 0, 0, (SCM (*) ()) g_keys_page_close },
  { "page-revert",                  0, 0, 0, (SCM (*) ()) g_keys_page_revert },
  { "page-print",                   0, 0, 0, (SCM (*) ()) g_keys_page_print },
  { "add-component",                0, 0, 0, (SCM (*) ()) g_keys_add_component },
  { "add-attribute",                0, 0, 0, (SCM (*) ()) g_keys_add_attribute },
  { "add-net",                      0, 0, 0, (SCM (*) ()) g_keys_add_net },
  { "add-bus",                      0, 0, 0, (SCM (*) ()) g_keys_add_bus },
  { "add-text",                     0, 0, 0, (SCM (*) ()) g_keys_add_text },
  { "add-path",                     0, 0, 0, (SCM (*) ()) g_keys_add_path },
  { "add-line",                     0, 0, 0, (SCM (*) ()) g_keys_add_line },
  { "add-box",                      0, 0, 0, (SCM (*) ()) g_keys_add_box },
  { "add-picture",                  0, 0, 0, (SCM (*) ()) g_keys_add_picture},
  { "add-circle",                   0, 0, 0, (SCM (*) ()) g_keys_add_circle },
  { "add-arc",                      0, 0, 0, (SCM (*) ()) g_keys_add_arc },
  { "add-pin",                      0, 0, 0, (SCM (*) ()) g_keys_add_pin },
  { "hierarchy-down-schematic",     0, 0, 0, (SCM (*) ()) g_keys_hierarchy_down_schematic },
  { "hierarchy-down-symbol",        0, 0, 0, (SCM (*) ()) g_keys_hierarchy_down_symbol },
  { "hierarchy-up",                 0, 0, 0, (SCM (*) ()) g_keys_hierarchy_up },
  { "attributes-attach",            0, 0, 0, (SCM (*) ()) g_keys_attributes_attach },
  { "attributes-detach",            0, 0, 0, (SCM (*) ()) g_keys_attributes_detach },
  { "attributes-show-name",         0, 0, 0, (SCM (*) ()) g_keys_attributes_show_name },
  { "attributes-show-value",        0, 0, 0, (SCM (*) ()) g_keys_attributes_show_value },
  { "attributes-show-both",         0, 0, 0, (SCM (*) ()) g_keys_attributes_show_both },
  { "attributes-visibility-toggle", 0, 0, 0, (SCM (*) ()) g_keys_attributes_visibility_toggle },
  { "options-text-size",            0, 0, 0, (SCM (*) ()) g_keys_options_text_size },
  { "options-snap-size",            0, 0, 0, (SCM (*) ()) g_keys_options_snap_size },
  { "options-scale-up-snap-size",   0, 0, 0, (SCM (*) ()) g_keys_options_scale_up_snap_size },
  { "options-scale-down-snap-size", 0, 0, 0, (SCM (*) ()) g_keys_options_scale_down_snap_size },
  { "options-action-feedback",      0, 0, 0, (SCM (*) ()) g_keys_options_afeedback },
  { "options-grid",                 0, 0, 0, (SCM (*) ()) g_keys_options_grid },
  { "options-snap",                 0, 0, 0, (SCM (*) ()) g_keys_options_snap },
  { "options-rubberband",           0, 0, 0, (SCM (*) ()) g_keys_options_rubberband },
  { "options-magneticnet",          0, 0, 0, (SCM (*) ()) g_keys_options_magneticnet },
  { "options-show-log-window",      0, 0, 0, (SCM (*) ()) g_keys_options_show_log_window },
  { "options-show-coord-window",    0, 0, 0, (SCM (*) ()) g_keys_options_show_coord_window },
  { "help-about",                   0, 0, 0, (SCM (*) ()) g_keys_help_about },
  { "help-hotkeys",                 0, 0, 0, (SCM (*) ()) g_keys_help_hotkeys },
  { "cancel",                       0, 0, 0, (SCM (*) ()) g_keys_cancel },

  { NULL,                           0, 0, 0, NULL } };

/*! \brief Define a hook.
 * \par Function Description
 * Creates a Guile new hook with \a n_args arguments, and binds it to
 * the variable \a name, returning the newly created hook.
 *
 * \param n_args Number of arguments the hook should take.
 * \param name   Name of variable to bind the hook to.
 *
 * \return the newly-created hook.
 */
static SCM
create_hook (const char *name, int n_args)
{
  SCM hook = scm_make_hook (scm_from_int (n_args));
  scm_c_define (name, hook);
  return scm_permanent_object (hook);
}

/*! \brief Register function with Scheme.
 *  \par Function Description
 *  Creates <B>subr</B> objects to make <B>g_rc_*</B> functions that are defined *  #g_rc.c, #g_keys.c and #g_funcs.c visible to Scheme.
 */
void g_register_funcs (void)
{
  struct gsubr_t *tmp = gschem_funcs;

  while (tmp->name != NULL) {
    scm_c_define_gsubr (tmp->name, tmp->req, tmp->opt, tmp->rst, (scm_t_subr) tmp->fnc);
    tmp++;
  }

  /* Hook stuff */
  complex_place_list_changed_hook = create_hook ("complex-place-list-changed-hook", 1);
}
