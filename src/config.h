//##############################################################################
// volumeicon
//
// config.h - a singleton providing configuration values/functions
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

//##############################################################################
// Getter functions
//##############################################################################

// Alsa
const gchar *config_get_card(void);
const gchar *config_get_channel(void);

// Status icon
int config_get_stepsize(void);
const gchar *config_get_helper(void);

//##############################################################################
// Miscellaneous functions
//##############################################################################

void config_initialize(gchar *config_name);

#endif
