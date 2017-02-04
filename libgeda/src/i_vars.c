/* gEDA - GPL Electronic Design Automation
 * libgeda - gEDA's library
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
#include <stdio.h>

#include "libgeda_priv.h"

/*! \def INIT_STR(w, name, str) */
#define INIT_STR(w, name, str) {                                        \
        g_free((w)->name);                                              \
        (w)->name = g_strdup(((default_ ## name) != NULL) ?             \
                             (default_ ## name) : (str));               \
}

/* \note 
 * Kazu Hirata <kazu@seul.org> on July 16, 1999 - Added these absolute
 * defaults used when default_... is NULL.
 */
#define DEFAULT_BITMAP_DIRECTORY "../lib/bitmaps"
#define DEFAULT_BUS_RIPPER_SYMNAME "busripper-1.sym"

/* These values are the default extents of the schematic drawing area.
 *
 * The negative values allow symbols, residing at the origin, to be edited
 * without translation to other coordinates.
 */
int   default_init_left = -60500;
int   default_init_right = 121000;
int   default_init_top = -45375;
int   default_init_bottom = 90750;

char *default_bitmap_directory = NULL;
char *default_bus_ripper_symname = NULL;
GPtrArray *default_always_promote_attributes = NULL;

int   default_attribute_promotion = TRUE;
int   default_promote_invisible = FALSE;
int   default_keep_invisible = TRUE;

int   default_make_backup_files = TRUE;

/*! \brief Initialize variables in TOPLEVEL object
 *  \par Function Description
 *  This function will initialize variables to default values.
 *
 *  \param [out] toplevel  The TOPLEVEL object to be updated.
 *
 */
void i_vars_libgeda_set(TOPLEVEL *toplevel)
{
  toplevel->init_left    = default_init_left;
  toplevel->init_right   = default_init_right;
  toplevel->init_top     = default_init_top;
  toplevel->init_bottom  = default_init_bottom;

  toplevel->attribute_promotion = default_attribute_promotion;
  toplevel->promote_invisible = default_promote_invisible;
  toplevel->keep_invisible = default_keep_invisible;

  toplevel->make_backup_files = default_make_backup_files;

  /* copy the always_promote_attributes list from the default */
  if (toplevel->always_promote_attributes) {
    g_ptr_array_unref (toplevel->always_promote_attributes);
    toplevel->always_promote_attributes = NULL;
  }
  if (default_always_promote_attributes) {
    toplevel->always_promote_attributes =
      g_ptr_array_ref (default_always_promote_attributes);
  }

  /* you cannot free the default* strings here since new windows */
  /* need them */
  INIT_STR(toplevel, bitmap_directory, DEFAULT_BITMAP_DIRECTORY);
  INIT_STR(toplevel, bus_ripper_symname, DEFAULT_BUS_RIPPER_SYMNAME);
}


/*! \brief Free default names
 *  \par Function Description
 *  This function will free all of the default variables for libgeda.
 *
 */
void i_vars_libgeda_freenames()
{
  g_free(default_bitmap_directory);
  g_free(default_bus_ripper_symname);

  g_ptr_array_unref (default_always_promote_attributes);
  default_always_promote_attributes = NULL;
}
