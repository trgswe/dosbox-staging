/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2020-2021  The DOSBox Staging Team
 *  Copyright (C) 2002-2021  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "midi_alsa.h"

#if C_ALSA

#include <string>
#include <sstream>

#include "programs.h"
#include "string_utils.h"

#define ADDR_DELIM ".:"

#if (SND_LIB_MAJOR >= 1) || ((SND_LIB_MAJOR == 0) && (SND_LIB_MINOR >= 6))
#define snd_seq_flush_output(x)           snd_seq_drain_output(x)
#define snd_seq_set_client_group(x, name) /* nop */
#define my_snd_seq_open(seqp)             snd_seq_open(seqp, "hw", SND_SEQ_OPEN_OUTPUT, 0)
#else
/* SND_SEQ_OPEN_OUT causes oops on early version of ALSA */
#define my_snd_seq_open(seqp) snd_seq_open(seqp, SND_SEQ_OPEN)
#endif

void MidiHandler_alsa::send_event(int do_flush)
{
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, my_port);
	snd_seq_ev_set_dest(&ev, seq_client, seq_port);

	snd_seq_event_output(seq_handle, &ev);
	if (do_flush)
		snd_seq_flush_output(seq_handle);
}

bool MidiHandler_alsa::parse_addr(const char *arg, int *client, int *port)
{
	std::string in(arg);
	if (in.empty())
		return false;

	if (in[0] == 's' || in[0] == 'S') {
		*client = SND_SEQ_ADDRESS_SUBSCRIBERS;
		*port = 0;
		return true;
	}

	if (in.find_first_of(ADDR_DELIM) == std::string::npos)
		return false;

	std::istringstream inp(in);
	int val1;
	int val2;
	char c;
	if (!(inp >> val1))
		return false;
	if (!(inp >> c))
		return false;
	if (!(inp >> val2))
		return false;
	*client = val1;
	*port = val2;
	return true;
}

void MidiHandler_alsa::PlaySysex(uint8_t *sysex, size_t len)
{
	snd_seq_ev_set_sysex(&ev, len, sysex);
	send_event(1);
}

void MidiHandler_alsa::PlayMsg(const uint8_t *msg)
{
	ev.type = SND_SEQ_EVENT_OSS;
	ev.data.raw32.d[0] = msg[0];
	ev.data.raw32.d[1] = msg[1];
	ev.data.raw32.d[2] = msg[2];

	unsigned char chanID = msg[0] & 0x0F;
	switch (msg[0] & 0xF0) {
	case 0x80:
		snd_seq_ev_set_noteoff(&ev, chanID, msg[1], msg[2]);
		send_event(1);
		break;
	case 0x90:
		snd_seq_ev_set_noteon(&ev, chanID, msg[1], msg[2]);
		send_event(1);
		break;
	case 0xA0:
		snd_seq_ev_set_keypress(&ev, chanID, msg[1], msg[2]);
		send_event(1);
		break;
	case 0xB0:
		snd_seq_ev_set_controller(&ev, chanID, msg[1], msg[2]);
		send_event(1);
		break;
	case 0xC0:
		snd_seq_ev_set_pgmchange(&ev, chanID, msg[1]);
		send_event(0);
		break;
	case 0xD0:
		snd_seq_ev_set_chanpress(&ev, chanID, msg[1]);
		send_event(0);
		break;
	case 0xE0: {
		long theBend = ((long)msg[1] + (long)(msg[2] << 7)) - 0x2000;
		snd_seq_ev_set_pitchbend(&ev, chanID, theBend);
		send_event(1);
		break;
	}
	default:
		// Maybe filter out FC as it leads for at least one user to
		// crash, but the entire midi stream has not yet been checked.
		LOG(LOG_MISC, LOG_WARN)("ALSA: Unknown Command: %02X %02X %02X",
		                        msg[0], msg[1], msg[2]);
		send_event(1);
		break;
	}
}

void MidiHandler_alsa::Close()
{
	if (seq_handle) {
		HaltSequence();
		snd_seq_close(seq_handle);
	}
}

bool MidiHandler_alsa::Open(const char *conf)
{
	char var[10];
	unsigned int caps;
	bool defaultport = true; // try 17:0. Seems to be default nowadays

	// try to use port specified in config file
	if (conf && conf[0]) {
		safe_strcpy(var, conf);
		if (!parse_addr(var, &seq_client, &seq_port)) {
			LOG_MSG("ALSA: Invalid alsa port %s", var);
			return false;
		}
		defaultport = false;
	}
	// default port if none specified
	else if (!parse_addr("65:0", &seq_client, &seq_port)) {
		LOG_MSG("ALSA: Invalid alsa port 65:0");
		return false;
	}

	if (my_snd_seq_open(&seq_handle)) {
		LOG_MSG("ALSA: Can't open sequencer");
		return false;
	}

	my_client = snd_seq_client_id(seq_handle);
	snd_seq_set_client_name(seq_handle, "DOSBOX");
	snd_seq_set_client_group(seq_handle, "input");

	caps = SND_SEQ_PORT_CAP_READ;
	if (seq_client == SND_SEQ_ADDRESS_SUBSCRIBERS)
		caps = ~SND_SEQ_PORT_CAP_SUBS_READ;

	my_port = snd_seq_create_simple_port(seq_handle, "DOSBOX", caps,
	                                     SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
	if (my_port < 0) {
		snd_seq_close(seq_handle);
		LOG_MSG("ALSA: Can't create ALSA port");
		return false;
	}

	if (seq_client != SND_SEQ_ADDRESS_SUBSCRIBERS) {
		/* subscribe to MIDI port */
		if (snd_seq_connect_to(seq_handle, my_port, seq_client, seq_port) < 0) {
			if (defaultport) {
				// if port "65:0" (default) try "17:0" as well
				seq_client = 17;
				seq_port = 0; // Update reported values
				if (snd_seq_connect_to(seq_handle, my_port, seq_client, seq_port) < 0) {
					// Try 128:0 Timidity port as well
					// seq_client = 128; seq_port = 0; //Update reported values
					// if(snd_seq_connect_to(seq_handle,my_port,seq_client,seq_port) < 0) {
					snd_seq_close(seq_handle);
					LOG_MSG("ALSA: Can't subscribe to MIDI port (65:0) nor (17:0)");
					return false;
					// }
				}
			} else {
				snd_seq_close(seq_handle);
				LOG_MSG("ALSA: Can't subscribe to MIDI port (%d:%d)",
				        seq_client, seq_port);
				return false;
			}
		}
	}

	LOG_MSG("ALSA: Client initialised [%d:%d]", seq_client, seq_port);
	return true;
}

int MidiHandler_alsa::ListAll(Program *out)
{
	snd_seq_t *seq;
	if (snd_seq_open(&seq, "hw", SND_SEQ_OPEN_OUTPUT, 0) != 0)
		return -1; // TODO 

	// Iterate over all ALSA sequencers:
	snd_seq_client_info_t *client_info;
	snd_seq_client_info_malloc(&client_info);
	snd_seq_client_info_set_client(client_info, -1);
	while (snd_seq_query_next_client(seq, client_info) >= 0) {
		// Iterate over all ports in this sequencer:
		const int client_id = snd_seq_client_info_get_client(client_info);
		const char *client_name = snd_seq_client_info_get_name(client_info);
		snd_seq_port_info_t *port_info;
		snd_seq_port_info_malloc(&port_info);
		snd_seq_port_info_set_client(port_info, client_id);
		snd_seq_port_info_set_port(port_info, -1);
		while (snd_seq_query_next_port(seq, port_info) >= 0) {
			const auto *addr = snd_seq_port_info_get_addr(port_info);
			const char *name = snd_seq_port_info_get_name(port_info);

			out->WriteOut("  %3d:%d %s - %s\n", addr->client,
			              addr->port, client_name, name);
		}
		snd_seq_port_info_free(port_info);
	}
	snd_seq_client_info_free(client_info);
	snd_seq_close(seq);
	return 0;
}

#endif // C_ALSA
