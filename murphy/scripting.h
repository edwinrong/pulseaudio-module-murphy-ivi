/*
 * module-murphy-ivi -- PulseAudio module for providing audio routing support
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St - Fifth Floor, Boston,
 * MA 02110-1301 USA.
 *
 */
#ifndef fooscriptingfoo
#define fooscriptingfoo

#include "userdata.h"


pa_scripting *pa_scripting_init(struct userdata *);
void pa_scripting_done(struct userdata *);

pa_bool_t pa_scripting_dofile(struct userdata *, const char *);

scripting_node *pa_scripting_node_create(struct userdata *, mir_node *);
void pa_scripting_node_destroy(struct userdata *, mir_node *);

#endif /* fooscriptingfoo */

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 *
 */
