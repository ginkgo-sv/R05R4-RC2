/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *  Copyright (C) 1999,2000  H�vard Kv�len
 *  Copyright (C) 2002  Daisuke Shimamura
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <gtk/gtk.h>

#include "FLAC/metadata.h"
#include "charset.h"
#include "configure.h"
#include "plugin_common/vorbiscomment.h"
#include "plugin_common/locale_hack.h"

static GtkWidget *window = NULL;
static GList *genre_list = NULL;
static GtkWidget *filename_entry, *tag_frame;
static GtkWidget *title_entry, *artist_entry, *album_entry, *date_entry, *tracknum_entry, *comment_entry;
static GtkWidget *genre_combo;
static GtkWidget *flac_samplerate, *flac_channels, *flac_bits_per_sample, *flac_blocksize, *flac_filesize, *flac_samples, *flac_bitrate;

static gchar *current_filename = NULL;
static FLAC_Plugin__CanonicalTag *canonical_tag = NULL;

static const gchar *vorbis_genres[] =
{
	N_("Blues"), N_("Classic Rock"), N_("Country"), N_("Dance"),
	N_("Disco"), N_("Funk"), N_("Grunge"), N_("Hip-Hop"),
	N_("Jazz"), N_("Metal"), N_("New Age"), N_("Oldies"),
	N_("Other"), N_("Pop"), N_("R&B"), N_("Rap"), N_("Reggae"),
	N_("Rock"), N_("Techno"), N_("Industrial"), N_("Alternative"),
	N_("Ska"), N_("Death Metal"), N_("Pranks"), N_("Soundtrack"),
	N_("Euro-Techno"), N_("Ambient"), N_("Trip-Hop"), N_("Vocal"),
	N_("Jazz+Funk"), N_("Fusion"), N_("Trance"), N_("Classical"),
	N_("Instrumental"), N_("Acid"), N_("House"), N_("Game"),
	N_("Sound Clip"), N_("Gospel"), N_("Noise"), N_("Alt"),
	N_("Bass"), N_("Soul"), N_("Punk"), N_("Space"),
	N_("Meditative"), N_("Instrumental Pop"),
	N_("Instrumental Rock"), N_("Ethnic"), N_("Gothic"),
	N_("Darkwave"), N_("Techno-Industrial"), N_("Electronic"),
	N_("Pop-Folk"), N_("Eurodance"), N_("Dream"),
	N_("Southern Rock"), N_("Comedy"), N_("Cult"),
	N_("Gangsta Rap"), N_("Top 40"), N_("Christian Rap"),
	N_("Pop/Funk"), N_("Jungle"), N_("Native American"),
	N_("Cabaret"), N_("New Wave"), N_("Psychedelic"), N_("Rave"),
	N_("Showtunes"), N_("Trailer"), N_("Lo-Fi"), N_("Tribal"),
	N_("Acid Punk"), N_("Acid Jazz"), N_("Polka"), N_("Retro"),
	N_("Musical"), N_("Rock & Roll"), N_("Hard Rock"), N_("Folk"),
	N_("Folk/Rock"), N_("National Folk"), N_("Swing"),
	N_("Fast-Fusion"), N_("Bebob"), N_("Latin"), N_("Revival"),
	N_("Celtic"), N_("Bluegrass"), N_("Avantgarde"),
	N_("Gothic Rock"), N_("Progressive Rock"),
	N_("Psychedelic Rock"), N_("Symphonic Rock"), N_("Slow Rock"),
	N_("Big Band"), N_("Chorus"), N_("Easy Listening"),
	N_("Acoustic"), N_("Humour"), N_("Speech"), N_("Chanson"),
	N_("Opera"), N_("Chamber Music"), N_("Sonata"), N_("Symphony"),
	N_("Booty Bass"), N_("Primus"), N_("Porn Groove"),
	N_("Satire"), N_("Slow Jam"), N_("Club"), N_("Tango"),
	N_("Samba"), N_("Folklore"), N_("Ballad"), N_("Power Ballad"),
	N_("Rhythmic Soul"), N_("Freestyle"), N_("Duet"),
	N_("Punk Rock"), N_("Drum Solo"), N_("A Cappella"),
	N_("Euro-House"), N_("Dance Hall"), N_("Goa"),
	N_("Drum & Bass"), N_("Club-House"), N_("Hardcore"),
	N_("Terror"), N_("Indie"), N_("BritPop"), N_("Negerpunk"),
	N_("Polsk Punk"), N_("Beat"), N_("Christian Gangsta Rap"),
	N_("Heavy Metal"), N_("Black Metal"), N_("Crossover"),
	N_("Contemporary Christian"), N_("Christian Rock"),
	N_("Merengue"), N_("Salsa"), N_("Thrash Metal"),
	N_("Anime"), N_("JPop"), N_("Synthpop")
};

static void local__safe_free(void *object)
{
	if(0 != object)
		free(object);
}

static void label_set_text(GtkWidget * label, char *str, ...)
{
	va_list args;
	gchar *tempstr;

	va_start(args, str);
	tempstr = g_strdup_vprintf(str, args);
	va_end(args);

	gtk_label_set_text(GTK_LABEL(label), tempstr);
	g_free(tempstr);
}

static void set_entry_tag(GtkEntry * entry, gchar * tag)
{
	char *text;

	if(tag) {
		if(flac_cfg.title.convert_char_set) {
			text = convert_from_file_to_user(tag);
			gtk_entry_set_text(entry, text);
			free(text);
		}
		else
			gtk_entry_set_text(entry, tag);
	}
	else
		gtk_entry_set_text(entry, "");
}

static char *get_entry_tag(GtkEntry * entry)
{
	gchar *text;

	text = gtk_entry_get_text(entry);
	if (!text || strlen(text) == 0)
		return 0;
	if(flac_cfg.title.convert_char_set)
		return convert_from_user_to_file(text);
	else
		return strdup(text);
}

static void show_tag()
{
	set_entry_tag(GTK_ENTRY(title_entry), canonical_tag->title);
	set_entry_tag(GTK_ENTRY(artist_entry), canonical_tag->composer);
	set_entry_tag(GTK_ENTRY(album_entry), canonical_tag->album);
	set_entry_tag(GTK_ENTRY(date_entry), canonical_tag->year_recorded);
	set_entry_tag(GTK_ENTRY(tracknum_entry), canonical_tag->track_number);
	set_entry_tag(GTK_ENTRY(comment_entry), canonical_tag->comment);
	set_entry_tag(GTK_ENTRY(GTK_COMBO(genre_combo)->entry), canonical_tag->genre);
}

static void save_tag(GtkWidget * w, gpointer data)
{
	(void)w;
	(void)data;

	local__safe_free(canonical_tag->title);
	local__safe_free(canonical_tag->composer);
	local__safe_free(canonical_tag->album);
	local__safe_free(canonical_tag->year_recorded);
	local__safe_free(canonical_tag->track_number);
	local__safe_free(canonical_tag->comment);
	local__safe_free(canonical_tag->genre);
	canonical_tag->title = get_entry_tag(GTK_ENTRY(title_entry));
	canonical_tag->composer = get_entry_tag(GTK_ENTRY(artist_entry));
	canonical_tag->album = get_entry_tag(GTK_ENTRY(album_entry));
	canonical_tag->year_recorded = get_entry_tag(GTK_ENTRY(date_entry));
	canonical_tag->track_number = get_entry_tag(GTK_ENTRY(tracknum_entry));
	canonical_tag->comment = get_entry_tag(GTK_ENTRY(comment_entry));
	canonical_tag->genre = get_entry_tag(GTK_ENTRY(GTK_COMBO(genre_combo)->entry));
	
	FLAC_plugin__vorbiscomment_set(current_filename, canonical_tag);
	gtk_widget_destroy(window);
}

static void remove_tag(GtkWidget * w, gpointer data)
{
	(void)w;
	(void)data;
	
	local__safe_free(canonical_tag->title);
	local__safe_free(canonical_tag->composer);
	local__safe_free(canonical_tag->album);
	local__safe_free(canonical_tag->year_recorded);
	local__safe_free(canonical_tag->track_number);
	local__safe_free(canonical_tag->comment);
	local__safe_free(canonical_tag->genre);

	canonical_tag->title = canonical_tag->composer = canonical_tag->album = canonical_tag->year_recorded = canonical_tag->track_number = canonical_tag->comment = canonical_tag->genre = 0;

	FLAC_plugin__vorbiscomment_set(current_filename, canonical_tag);
	gtk_widget_destroy(window);
}

static void show_file_info()
{
	FLAC__StreamMetadata streaminfo;
	struct stat _stat;

	gtk_label_set_text(GTK_LABEL(flac_samplerate), "");
	gtk_label_set_text(GTK_LABEL(flac_channels), "");
	gtk_label_set_text(GTK_LABEL(flac_bits_per_sample), "");
	gtk_label_set_text(GTK_LABEL(flac_blocksize), "");
	gtk_label_set_text(GTK_LABEL(flac_filesize), "");
	gtk_label_set_text(GTK_LABEL(flac_samples), "");
	gtk_label_set_text(GTK_LABEL(flac_bitrate), "");

	if(!FLAC__metadata_get_streaminfo(current_filename, &streaminfo)) {
		return;
	}

	label_set_text(flac_samplerate, _("Samplerate: %d Hz"), streaminfo.data.stream_info.sample_rate);
	label_set_text(flac_channels, _("Channels: %d"), streaminfo.data.stream_info.channels);
	label_set_text(flac_bits_per_sample, _("Bits/Sample: %d"), streaminfo.data.stream_info.bits_per_sample);
	if(streaminfo.data.stream_info.min_blocksize == streaminfo.data.stream_info.max_blocksize)
		label_set_text(flac_blocksize, _("Blocksize: %d"), streaminfo.data.stream_info.min_blocksize);
	else
		label_set_text(flac_blocksize, _("Blocksize: variable\n  min/max: %d/%d"), streaminfo.data.stream_info.min_blocksize, streaminfo.data.stream_info.max_blocksize);

	if (streaminfo.data.stream_info.total_samples)
		label_set_text(flac_samples, _("Samples: %llu\nLength: %d:%.2d"),
				streaminfo.data.stream_info.total_samples,
				(int)(streaminfo.data.stream_info.total_samples / streaminfo.data.stream_info.sample_rate / 60),
				(int)(streaminfo.data.stream_info.total_samples / streaminfo.data.stream_info.sample_rate % 60));

	if(!stat(current_filename, &_stat) && S_ISREG(_stat.st_mode)) {
		label_set_text(flac_filesize, _("Filesize: %ld B"), _stat.st_size);
		if (streaminfo.data.stream_info.total_samples)
			label_set_text(flac_bitrate, _("Avg. bitrate: %.1f kb/s\nCompression ratio: %.1f%%"),
					8.0 * (float)(_stat.st_size) / (1000.0 * (float)streaminfo.data.stream_info.total_samples / (float)streaminfo.data.stream_info.sample_rate),
					100.0 * (float)_stat.st_size / (float)(streaminfo.data.stream_info.bits_per_sample / 8 * streaminfo.data.stream_info.channels * streaminfo.data.stream_info.total_samples));
	}
}

void FLAC_XMMS__file_info_box(char *filename)
{
	unsigned i;
	gchar *title;

	if (!window)
	{
		GtkWidget *vbox, *hbox, *left_vbox, *table;
		GtkWidget *flac_frame, *flac_box;
		GtkWidget *label, *filename_hbox;
		GtkWidget *bbox, *save, *remove, *cancel;

		window = gtk_window_new(GTK_WINDOW_DIALOG);
		gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
		gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &window);
		gtk_container_set_border_width(GTK_CONTAINER(window), 10);

		vbox = gtk_vbox_new(FALSE, 10);
		gtk_container_add(GTK_CONTAINER(window), vbox);

		filename_hbox = gtk_hbox_new(FALSE, 5);
		gtk_box_pack_start(GTK_BOX(vbox), filename_hbox, FALSE, TRUE, 0);

		label = gtk_label_new(_("Filename:"));
		gtk_box_pack_start(GTK_BOX(filename_hbox), label, FALSE, TRUE, 0);
		filename_entry = gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(filename_entry), FALSE);
		gtk_box_pack_start(GTK_BOX(filename_hbox), filename_entry, TRUE, TRUE, 0);

		hbox = gtk_hbox_new(FALSE, 10);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

		left_vbox = gtk_vbox_new(FALSE, 10);
		gtk_box_pack_start(GTK_BOX(hbox), left_vbox, FALSE, FALSE, 0);

		tag_frame = gtk_frame_new(_("Tag:"));
		gtk_box_pack_start(GTK_BOX(left_vbox), tag_frame, FALSE, FALSE, 0);

		table = gtk_table_new(5, 5, FALSE);
		gtk_container_set_border_width(GTK_CONTAINER(table), 5);
		gtk_container_add(GTK_CONTAINER(tag_frame), table);

		label = gtk_label_new(_("Title:"));
		gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);

		title_entry = gtk_entry_new();
		gtk_table_attach(GTK_TABLE(table), title_entry, 1, 4, 0, 1, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 5);

		label = gtk_label_new(_("Artist:"));
		gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);

		artist_entry = gtk_entry_new();
		gtk_table_attach(GTK_TABLE(table), artist_entry, 1, 4, 1, 2, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 5);

		label = gtk_label_new(_("Album:"));
		gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);

		album_entry = gtk_entry_new();
		gtk_table_attach(GTK_TABLE(table), album_entry, 1, 4, 2, 3, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 5);

		label = gtk_label_new(_("Comment:"));
		gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);

		comment_entry = gtk_entry_new();
		gtk_table_attach(GTK_TABLE(table), comment_entry, 1, 4, 3, 4, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 5);

		label = gtk_label_new(_("Date:"));
		gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 5, 5);

		date_entry = gtk_entry_new();
		gtk_widget_set_usize(date_entry, 40, -1);
		gtk_table_attach(GTK_TABLE(table), date_entry, 1, 2, 4, 5, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 5);

		label = gtk_label_new(_("Track number:"));
		gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 2, 3, 4, 5, GTK_FILL, GTK_FILL, 5, 5);

		tracknum_entry = gtk_entry_new();
		gtk_widget_set_usize(tracknum_entry, 40, -1);
		gtk_table_attach(GTK_TABLE(table), tracknum_entry, 3, 4, 4, 5, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 5);

		label = gtk_label_new(_("Genre:"));
		gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 5, 6, GTK_FILL, GTK_FILL, 5, 5);

		genre_combo = gtk_combo_new();
		gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(genre_combo)->entry), TRUE);

		if (!genre_list)
		{
			for (i = 0; i < sizeof(vorbis_genres) / sizeof(*vorbis_genres) ; i++)
				genre_list = g_list_prepend(genre_list, (char *)vorbis_genres[i]);
			genre_list = g_list_prepend(genre_list, "");
			genre_list = g_list_sort(genre_list, (GCompareFunc)g_strcasecmp);
		}
		gtk_combo_set_popdown_strings(GTK_COMBO(genre_combo), genre_list);

		gtk_table_attach(GTK_TABLE(table), genre_combo, 1, 4, 5, 6, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 5);

		bbox = gtk_hbutton_box_new();
		gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
		gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);
		gtk_box_pack_start(GTK_BOX(left_vbox), bbox, FALSE, FALSE, 0);

		save = gtk_button_new_with_label(_("Save"));
		gtk_signal_connect(GTK_OBJECT(save), "clicked", GTK_SIGNAL_FUNC(save_tag), NULL);
		GTK_WIDGET_SET_FLAGS(save, GTK_CAN_DEFAULT);
		gtk_box_pack_start(GTK_BOX(bbox), save, TRUE, TRUE, 0);
		gtk_widget_grab_default(save);

		remove= gtk_button_new_with_label(_("Remove Tag"));
		gtk_signal_connect(GTK_OBJECT(remove), "clicked", GTK_SIGNAL_FUNC(remove_tag), NULL);
		GTK_WIDGET_SET_FLAGS(remove, GTK_CAN_DEFAULT);
		gtk_box_pack_start(GTK_BOX(bbox), remove, TRUE, TRUE, 0);

		cancel = gtk_button_new_with_label(_("Cancel"));
		gtk_signal_connect_object(GTK_OBJECT(cancel), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));
		GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);
		gtk_box_pack_start(GTK_BOX(bbox), cancel, TRUE, TRUE, 0);

		flac_frame = gtk_frame_new(_("FLAC Info:"));
		gtk_box_pack_start(GTK_BOX(hbox), flac_frame, FALSE, FALSE, 0);

		flac_box = gtk_vbox_new(FALSE, 5);
		gtk_container_add(GTK_CONTAINER(flac_frame), flac_box);
		gtk_container_set_border_width(GTK_CONTAINER(flac_box), 10);
		gtk_box_set_spacing(GTK_BOX(flac_box), 0);

		flac_samplerate = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(flac_samplerate), 0, 0);
		gtk_label_set_justify(GTK_LABEL(flac_samplerate), GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(flac_box), flac_samplerate, FALSE, FALSE, 0);

		flac_channels = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(flac_channels), 0, 0);
		gtk_label_set_justify(GTK_LABEL(flac_channels), GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(flac_box), flac_channels, FALSE, FALSE, 0);

		flac_bits_per_sample = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(flac_bits_per_sample), 0, 0);
		gtk_label_set_justify(GTK_LABEL(flac_bits_per_sample), GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(flac_box), flac_bits_per_sample, FALSE, FALSE, 0);

		flac_blocksize = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(flac_blocksize), 0, 0);
		gtk_label_set_justify(GTK_LABEL(flac_blocksize), GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(flac_box), flac_blocksize, FALSE, FALSE, 0);

		flac_filesize = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(flac_filesize), 0, 0);
		gtk_label_set_justify(GTK_LABEL(flac_filesize), GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(flac_box), flac_filesize, FALSE, FALSE, 0);

		flac_samples = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(flac_samples), 0, 0);
		gtk_label_set_justify(GTK_LABEL(flac_samples), GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(flac_box), flac_samples, FALSE, FALSE, 0);

		flac_bitrate = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(flac_bitrate), 0, 0);
		gtk_label_set_justify(GTK_LABEL(flac_bitrate), GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(flac_box), flac_bitrate, FALSE, FALSE, 0);

		gtk_widget_show_all(window);
	}

	if(current_filename)
		g_free(current_filename);
	if(!(current_filename = g_strdup(filename)))
		return;

	title = g_strdup_printf(_("File Info - %s"), g_basename(filename));
	gtk_window_set_title(GTK_WINDOW(window), title);
	g_free(title);

	gtk_entry_set_text(GTK_ENTRY(filename_entry), filename);
	gtk_editable_set_position(GTK_EDITABLE(filename_entry), -1);

	if(canonical_tag)
		FLAC_plugin__canonical_tag_clear(canonical_tag);
	else
		canonical_tag = FLAC_plugin__canonical_tag_new();

	FLAC_plugin__vorbiscomment_get(current_filename, canonical_tag);

	show_tag();
	show_file_info();

	gtk_widget_set_sensitive(tag_frame, TRUE);
}
