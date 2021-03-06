/* $Id: ao_polyp.c,v 1.1.1.1 2005/11/30 08:50:45 jerry Exp $ */

/***
  This file is part of libao-polyp.
 
  libao-polyp is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 2 of the License,
  or (at your option) any later version.
 
  libao-polyp is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with libao-polyp; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <string.h>
#include <signal.h>
#include <limits.h>

#include <polyp/polyplib-simple.h>
#include <ao/ao.h>
#include <ao/plugin.h>

/* Unfortunately libao doesn't allow "const" for these structures... */
static char * ao_polyp_options[] = {
    "server",
    "sink"
};

static ao_info ao_polyp_info = {
    AO_TYPE_LIVE,
    "polypaudio output",
    "polyp",
    PACKAGE_BUGREPORT,
    "Outputs to the Polypaudio Sound Server",
    AO_FMT_NATIVE,
    41,
    ao_polyp_options,
    2
};

typedef struct ao_polyp_internal {
    struct pa_simple *simple;
    char *server, *sink;
} ao_polyp_internal;

/* Dirty trick: import these two functions from polyplib */
char *pa_get_binary_name(char *s, size_t l);
char *pa_path_get_filename(const char *p);

/* Yes, this is very ugly, but required nonetheless... */
static void disable_sigpipe(void) {
    struct sigaction sa;

    sigaction(SIGPIPE, NULL, &sa);
    if (sa.sa_handler != SIG_IGN) {
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = SIG_IGN;
        sa.sa_flags = SA_RESTART;
        sigaction(SIGPIPE, &sa, NULL);
    }
}

int ao_plugin_test(void) {
    char p[PATH_MAX], t[256], t2[256], *fn = NULL;
    struct pa_simple *s;
    static const struct pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    disable_sigpipe();
    
    if (getenv("POLYP_SERVER") || getenv("POLYP_SINK"))
        return 1;

    if (pa_get_binary_name(p, sizeof(p))) {
        fn = pa_path_get_filename(p);
        snprintf(t, sizeof(t), "libao[%s]", fn);
        snprintf(t2, sizeof(t2), "libao[%s] test", fn);
    }

    if (!(s = pa_simple_new(NULL, fn ? t : "libao", PA_STREAM_PLAYBACK, NULL, fn ? t2 : "libao test", &ss, NULL, PA_VOLUME_NORM, NULL)))
        return 0;

    pa_simple_free(s);
    return 1;
}

ao_info *ao_plugin_driver_info(void) {
    return &ao_polyp_info;
}

int ao_plugin_device_init(ao_device *device) {
    ao_polyp_internal *internal;
    assert(device);

    internal = (ao_polyp_internal *) malloc(sizeof(ao_polyp_internal));
    
    if (internal == NULL)	
        return 0; 

    internal->simple = NULL;
    internal->server = NULL;
    internal->sink = NULL;
    device->internal = internal;
    
    return 1;
}

int ao_plugin_set_option(ao_device *device, const char *key, const char *value) {
    ao_polyp_internal *internal;
    assert(device && device->internal && key && value);
    internal = (ao_polyp_internal *) device->internal;
    
    if (!strcmp(key, "server")) {
        free(internal->server);
        internal->server = strdup(value);
    } else if (!strcmp(key, "sink")) {
        free(internal->sink);
        internal->sink = strdup(value);
    } else
        return 0;
    
    return 1;
}

int ao_plugin_open(ao_device *device, ao_sample_format *format) {
    char p[PATH_MAX], t[256], t2[256], *fn = NULL;
    ao_polyp_internal *internal;
    struct pa_sample_spec ss;

    assert(device && device->internal && format);

    internal = (ao_polyp_internal *) device->internal;

    if (format->bits == 8)
        ss.format = PA_SAMPLE_U8;
    else if (format->bits == 16)
        ss.format = PA_SAMPLE_S16NE;
    else
        return 0;

    if (format->channels <= 0)
        return 0;

    ss.channels = format->channels;
    ss.rate = format->rate;

    disable_sigpipe();

    if (pa_get_binary_name(p, sizeof(p))) {
        fn = pa_path_get_filename(p);
        snprintf(t, sizeof(t), "libao[%s]", fn);
        snprintf(t2, sizeof(t2), "libao[%s] playback stream", fn);
    }
    
    if (!(internal->simple = pa_simple_new(internal->server, fn ? t : "libao", PA_STREAM_PLAYBACK, internal->sink, fn ? t2 : "libao playback stream", &ss, NULL, PA_VOLUME_NORM, NULL)))
        return 0;

    device->driver_byte_format = AO_FMT_NATIVE;
    return 1;
}

int ao_plugin_play(ao_device *device, const char* output_samples, uint_32 num_bytes) {
    assert(device && device->internal);
    ao_polyp_internal *internal = (ao_polyp_internal *) device->internal;

    return pa_simple_write(internal->simple, output_samples, num_bytes, NULL) >= 0;
}


int ao_plugin_close(ao_device *device) {
    assert(device && device->internal);
    ao_polyp_internal *internal = (ao_polyp_internal *) device->internal;

    pa_simple_drain(internal->simple, NULL);
    pa_simple_free(internal->simple);
    internal->simple = NULL;

    return 1;
}

void ao_plugin_device_clear(ao_device *device) {
    assert(device && device->internal);
    ao_polyp_internal *internal = (ao_polyp_internal *) device->internal;

    free(internal->server);
    free(internal->sink);
    free(internal);
    device->internal = NULL;
}
