//##############################################################################
// volumeicon
//
// volumeicon.c - implements the gtk status icon and preferences window
//
// Copyright 2011 Maato
//
// Authors:
//    Maato <maato@softwarebakery.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//##############################################################################

#include <assert.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libnotify/notify.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "alsa_backend.h"
#include "config.h"

#define _(x) (x)
#define N_(x) (x)

//##############################################################################
// Definitions
//##############################################################################
// About
#define APPNAME "Volume Icon"

// Setup retry
#define SETUP_RETRY_INTERVAL 1000

//##############################################################################
// Static variables
//##############################################################################
static gboolean m_backend_is_setup = FALSE;
static gchar *m_commandline_device_name = NULL;
static NotifyNotification *m_notification = NULL;
static GtkStatusIcon *m_status_icon = NULL;

// Backend Interface
static gboolean (*backend_setup)(const gchar *card, const gchar *channel,
                                 void (*volume_changed)(int, gboolean)) = NULL;
static void (*backend_set_channel)(const gchar *channel) = NULL;
static void (*backend_set_volume)(int volume) = NULL;
static void (*backend_set_mute)(gboolean mute) = NULL;
static int (*backend_get_volume)(void) = NULL;
static gboolean (*backend_get_mute)(void) = NULL;
static const gchar *(*backend_get_channel)(void) = NULL;
static const GList *(*backend_get_channel_names)(void) = NULL;
static const gchar *(*backend_get_device)(void) = NULL;
static const GList *(*backend_get_device_names)(void) = NULL;

// Status
static int m_volume = 0;
static gboolean m_mute = FALSE;

//##############################################################################
// Function prototypes
//##############################################################################
static void volume_icon_on_volume_changed(int volume, gboolean mute);
static void status_icon_update(gboolean mute, gboolean force);
static void notification_show();

//##############################################################################
// Static functions
//##############################################################################
// Helper
static inline int clamp_volume(int value)
{
	if(value < 0)
		return 0;
	if(value > 100)
		return 100;
	return value;
}

static void volume_icon_launch_helper()
{
	assert(config_get_helper() != NULL);
	pid_t pid = fork();
	if(pid == 0) {
		execl("/bin/sh", "/bin/sh", "-c", config_get_helper(), (char *)NULL);
		_exit(0);
	}
}

// StatusIcon handlers
static gboolean status_icon_on_button_press(GtkStatusIcon *status_icon,
                                            GdkEventButton *event,
                                            gpointer user_data)
{
	if(event->button == 1) {
		m_mute = !m_mute;
		backend_set_volume(m_volume);
		backend_set_mute(m_mute);
		status_icon_update(m_mute, FALSE);
		notification_show();
	}
	else if(event->button == 2) {
		volume_icon_launch_helper();
	}
	else {
		return FALSE;
	}

	return TRUE;
}

static void status_icon_on_scroll_event(GtkStatusIcon *status_icon,
                                        GdkEventScroll *event,
                                        gpointer user_data)
{
	int stepsize = config_get_stepsize();

	switch(event->direction) {
	case(GDK_SCROLL_UP):
	case(GDK_SCROLL_RIGHT):
		m_volume = clamp_volume(m_volume + stepsize);
		break;
	case(GDK_SCROLL_DOWN):
	case(GDK_SCROLL_LEFT):
		m_volume = clamp_volume(m_volume - stepsize);
		break;
	default:
		break;
	}

	backend_set_volume(m_volume);
	if(m_mute) {
		m_mute = FALSE;
		backend_set_mute(m_mute);
	}
	status_icon_update(m_mute, FALSE);
	notification_show();
}

static int status_icon_get_number(int volume, gboolean mute)
{
	if(mute)
		return 1;
	if(volume <= 0)
		return 1;
	if(volume <= 16)
		return 2;
	if(volume <= 33)
		return 3;
	if(volume <= 50)
		return 4;
	if(volume <= 67)
		return 5;
	if(volume <= 84)
		return 6;
	if(volume <= 99)
		return 7;
	return 8;
}

// Use the ignore_cache parameter to force the status icon to be loaded
// from file, for example after a theme change.
static void status_icon_update(gboolean mute, gboolean ignore_cache)
{
	static int volume_cache = -1;
	static int icon_cache = -1;
	int volume = backend_get_volume();

	int icon_number = status_icon_get_number(volume, mute);
	if(icon_number != icon_cache || ignore_cache) {
		const gchar *icon_name;

		if(icon_number == 1)
			icon_name = "audio-volume-muted";
		else if(icon_number <= 3)
			icon_name = "audio-volume-low";
		else if(icon_number <= 6)
			icon_name = "audio-volume-medium";
		else
			icon_name = "audio-volume-high";

		gtk_status_icon_set_from_icon_name(m_status_icon, icon_name);

		// Always use the current GTK icon theme for notifications.
		notify_notification_update(m_notification, APPNAME, NULL, icon_name);
		icon_cache = icon_number;
	}

	if((volume != volume_cache || ignore_cache) && backend_get_channel()) {
		gchar buffer[32];
		g_sprintf(buffer, "%s: %d%%", backend_get_channel(), volume);
		gtk_status_icon_set_tooltip_text(m_status_icon, buffer);

		notify_notification_set_hint_int32(m_notification, "value",
		                                   (gint)volume);
		volume_cache = volume;
	}
}

static void status_icon_setup(gboolean mute)
{
	m_status_icon = gtk_status_icon_new();
	g_signal_connect(G_OBJECT(m_status_icon), "button_press_event",
	                 G_CALLBACK(status_icon_on_button_press), NULL);
	g_signal_connect(G_OBJECT(m_status_icon), "scroll_event",
	                 G_CALLBACK(status_icon_on_scroll_event), NULL);
	status_icon_update(mute, FALSE);
	gtk_status_icon_set_visible(m_status_icon, TRUE);
}

static void volume_icon_on_volume_changed(int volume, gboolean mute)
{
	m_mute = mute;
	m_volume = clamp_volume(volume);
	status_icon_update(m_mute, FALSE);
}

static void notification_show()
{
	notify_notification_show(m_notification, NULL);
}

static gboolean retry_setup_cb(gpointer data)
{
	if(m_backend_is_setup) {
		return FALSE;
	}
	m_backend_is_setup =
	    backend_setup(m_commandline_device_name ? m_commandline_device_name :
	                                              config_get_card(),
	                  config_get_channel(), volume_icon_on_volume_changed);
	if(m_backend_is_setup) {
		m_volume = clamp_volume(backend_get_volume());
		m_mute = backend_get_mute();
		status_icon_update(m_mute, FALSE);
		return FALSE;
	}
	else {
		return TRUE;
	}
}

//##############################################################################
// Exported functions
//##############################################################################
int main(int argc, char *argv[])
{
	// Initialize gtk with arguments
	GError *error = 0;
	gchar *config_name = 0;
	gboolean print_version = FALSE;
	GOptionEntry options[] = {
	    {"config", 'c', 0, G_OPTION_ARG_FILENAME, &config_name,
	     N_("Alternate name to use for config file, default is volumeicon"),
	     "name"},
	    {"device", 'd', 0, G_OPTION_ARG_STRING, &m_commandline_device_name,
	     N_("Mixer device name"), "name"},
	    {"version", 'v', 0, G_OPTION_ARG_NONE, &print_version,
	     N_("Output version number and exit"), NULL},
	    {NULL}};
	GOptionContext *context = g_option_context_new(NULL);
	g_option_context_add_main_entries(context, options, NULL);
	g_option_context_add_group(context, gtk_get_option_group(TRUE));
	if(!g_option_context_parse(context, &argc, &argv, &error)) {
		if(error) {
			g_printerr("%s\n", error->message);
		}
		return EXIT_FAILURE;
	}
	signal(SIGCHLD, SIG_IGN);

	if(print_version) {
		g_fprintf(stdout, "%s %s\n", APPNAME, VERSION);
		return EXIT_SUCCESS;
	}

	// Setup OSD Notification
	if(notify_init(APPNAME)) {
		m_notification = notify_notification_new(APPNAME, NULL, NULL);
	}

	if(m_notification != NULL) {
		notify_notification_set_timeout(m_notification,
		                                NOTIFY_EXPIRES_DEFAULT);
		notify_notification_set_hint_string(m_notification, "synchronous",
		                                    "volume");
	}
	else {
		g_fprintf(stderr, "Failed to initialize notifications\n");
	}

	// Setup backend
	backend_setup = &asound_setup;
	backend_set_channel = &asound_set_channel;
	backend_set_volume = &asound_set_volume;
	backend_get_volume = &asound_get_volume;
	backend_get_mute = &asound_get_mute;
	backend_set_mute = &asound_set_mute;
	backend_get_channel = &asound_get_channel;
	backend_get_channel_names = &asound_get_channel_names;
	backend_get_device = &asound_get_device;
	backend_get_device_names = &asound_get_device_names;

	// Setup
	config_initialize(config_name);
	m_backend_is_setup =
	    backend_setup(m_commandline_device_name ? m_commandline_device_name :
	                                              config_get_card(),
	                  config_get_channel(), volume_icon_on_volume_changed);
	if(!m_backend_is_setup) {
		g_timeout_add(SETUP_RETRY_INTERVAL, retry_setup_cb, NULL);
	}
	else {
		m_volume = clamp_volume(backend_get_volume());
		m_mute = backend_get_mute();
	}
	status_icon_setup(m_mute);

	// Main Loop
	gtk_main();

	g_object_unref(G_OBJECT(m_notification));
	notify_uninit();

	return EXIT_SUCCESS;
}
