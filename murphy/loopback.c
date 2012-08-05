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
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <pulsecore/pulsecore-config.h>

#include <pulse/def.h>
#include <pulsecore/thread.h>
#include <pulsecore/strlist.h>
#include <pulsecore/time-smoother.h>
#include <pulsecore/sink.h>
#include <pulsecore/sink-input.h>

#include <combine/userdata.h>

#include "loopback.h"
#include "utils.h"


pa_loopback *pa_loopback_init(void)
{
    pa_loopback *loopback = pa_xnew0(pa_loopback, 1);

    return loopback;
}


void pa_loopback_done(pa_loopback *loopback, pa_core *core)
{
    pa_loopnode *loop, *n;

    PA_LLIST_FOREACH_SAFE(loop,n, loopback->loopnodes) {
        pa_module_unload_by_index(core, loop->module_index, FALSE);
    }
}



pa_loopnode *pa_loopback_create(pa_loopback   *loopback,
                                pa_core       *core,
                                uint32_t       source_index,
                                uint32_t       sink_index)
{
    static char *modnam = "module-loopback";

    pa_loopnode     *loop;
    pa_source       *source;
    pa_sink         *sink;
    pa_module       *module;
    pa_sink_input   *sink_input;
    char             args[512];
    uint32_t         idx;

    pa_assert(core);

    if (!(source = pa_idxset_get_by_index(core->sources, source_index))) {
        pa_log_debug("can't find source (index %u) for loopback",source_index);
        return NULL;
    }

    if (!(sink = pa_idxset_get_by_index(core->sinks, sink_index))) {
        pa_log_debug("can't find the primary sink (index %u) for loopback",
                     sink_index);
        return NULL;
    }

    snprintf(args, sizeof(args), "source=\"%s\" sink=\"%s\"",
             source->name, sink->name);

    if (!(module = pa_module_load(core, modnam, args))) {
        pa_log("failed to load module '%s %s'. can't loopback", modnam, args);
        return NULL;
    }

    PA_IDXSET_FOREACH(sink_input, core->sink_inputs, idx) {
        if (sink_input->module == module)
            break;
    }

    if (!sink_input) {
        pa_log("can't find output stream of loopback module (index %u)",
               module->index);
        pa_module_unload(core, module, FALSE);
        return NULL;
    }

    pa_assert(sink_input->index != PA_IDXSET_INVALID);

    loop = pa_xnew0(pa_loopnode, 1);
    loop->module_index = module->index;
    loop->sink_input_index = sink_input->index;

    PA_LLIST_PREPEND(pa_loopnode, loopback->loopnodes, loop);

    pa_log_debug("loopback succesfully loaded. Module index %u",module->index);

    return loop;
}


void pa_loopback_destroy(pa_loopback *loopback,
                         pa_core     *core,
                         pa_loopnode *loop)
{
    pa_assert(loopback);
    pa_assert(core);

    if (loop) {
        PA_LLIST_REMOVE(pa_loopnode, loopback->loopnodes, loop);
        pa_module_unload_by_index(core, loop->module_index, FALSE);
        pa_xfree(loop);
    }
}



int pa_loopback_print(pa_loopnode *loop, char *buf, int len)
{
    char *p, *e;

    pa_assert(buf);
    pa_assert(len > 0);

    e = (p = buf) + len;

    if (!loop)
        p += snprintf(p, e-p, "<not set>");
    else {
        p += snprintf(p, e-p, "module %u, sink_input %u",
                      loop->module_index, loop->sink_input_index);
    }
    
    return p - buf;
}


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 *
 */
