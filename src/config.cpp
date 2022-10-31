//##############################################################################
// volumeicon
//
// config.c - a singleton providing configuration values/functions
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

#include "config.h"

//##############################################################################
// Definitions
//##############################################################################
#define CONFIG_DIRNAME "volumeicon"
#define CONFIG_FILENAME "volumeicon"

//##############################################################################
// Static variables
//##############################################################################
static struct config {
	gchar *path;

	// Alsa
	gchar *card; // TODO: Rename this to device.
	gchar *channel;

	// Status icon
	int stepsize; // TODO: Rename this to volume_stepsize.
	gchar *helper_program;

} m_config = {.path = NULL,

              // Alsa
              .card = NULL,
              .channel = NULL,

              // Status icon
              .stepsize = 0,
              .helper_program = NULL};

//##############################################################################
// Static functions
//##############################################################################
static void config_load_default(void)
{
	if(!m_config.helper_program)
		m_config.helper_program = g_strdup(DEFAULT_MIXERAPP);
	if(!m_config.card)
		m_config.card = g_strdup("default");
	if(!m_config.stepsize)
		m_config.stepsize = 5;
}

static void config_read(void)
{
	// Clean up previously loaded configuration values
	g_free(m_config.card);
	g_free(m_config.channel);
	g_free(m_config.helper_program);

	// Load keys from keyfile
	GKeyFile *kf = g_key_file_new();
	g_key_file_load_from_file(kf, m_config.path, G_KEY_FILE_NONE, NULL);

#define GET_VALUE(type, section, key)                                         \
	g_key_file_get_##type(kf, section, key, NULL)
#define GET_STRING(s, k) GET_VALUE(value, s, k)
#define GET_BOOL(s, k) GET_VALUE(boolean, s, k)
#define GET_INT(s, k) GET_VALUE(integer, s, k)

	// Alsa
	m_config.card = GET_STRING("Alsa", "card");
	m_config.channel = GET_STRING("Alsa", "channel");

	// Status icon
	m_config.stepsize = GET_INT("StatusIcon", "stepsize");
	m_config.helper_program = GET_STRING("StatusIcon", "onclick");

	g_key_file_free(kf);

#undef GET_VALUE
#undef GET_STRING
#undef GET_BOOL
#undef GET_INT

	// Load default values for unset keys
	config_load_default();
}

//##############################################################################
// Exported getter functions
//##############################################################################

// Alsa
const gchar *config_get_card(void) { return m_config.card; }

const gchar *config_get_channel(void) { return m_config.channel; }

// Status icon
int config_get_stepsize(void) { return m_config.stepsize; }

const gchar *config_get_helper(void) { return m_config.helper_program; }

//##############################################################################
// Exported miscellaneous functions
//##############################################################################
void config_initialize(gchar *config_name)
{
	// Build config directory name
	gchar *config_dir =
	    g_build_filename(g_get_user_config_dir(), CONFIG_DIRNAME, NULL);
	m_config.path = g_build_filename(
	    config_dir, config_name ? config_name : CONFIG_FILENAME, NULL);

	// If a config file doesn't exist, load defaults
	if(!g_file_test(m_config.path, G_FILE_TEST_EXISTS)) {
		config_load_default();
	}
	else {
		config_read();
	}

	g_free(config_dir);
}
