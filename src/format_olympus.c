/*
 *  GQView
 *  (C) 2005 John Ellis
 *
 * This software is released under the GNU General Public License (GNU GPL).
 * Please read the included file COPYING for more information.
 * This software comes with no warranty of any kind, use at your own risk!
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "intl.h"

#include "format_olympus.h"
#include "format_raw.h"

#include "exif.h"


/*
 *-----------------------------------------------------------------------------
 * EXIF Makernote for Olympus
 *-----------------------------------------------------------------------------
 */

static ExifTextList KonMinTagColorMode[]= {
	{ 0,	"natural" },
	{ 1,	"black and white" },
	{ 2,	"vivid" },
	{ 3,	"solarization" },
	{ 4,	"Adobe RGB" },
	EXIF_TEXT_LIST_END
};

static ExifTextList KonMinTagQuality[]= {
	{ 0,	"raw" },
	{ 1,	"super fine" },
	{ 2,	"find" },
	{ 3,	"standard" },
	{ 4,	"extra fine" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusTagJpegQuality[]= {
	{ 1,	"standard" },
	{ 2,	"high" },
	{ 3,	"super high" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusTagMacro[]= {
	{ 0,	"off" },
	{ 1,	"on" },
	{ 2,	"view" },
	{ 3,	"manual" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusTagFlashMode[]= {
	{ 0,	"auto" },
	{ 1,	"red-eye reduction" },
	{ 2,	"fill" },
	{ 3,	"off" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusTagFocusMode[]= {
	{ 0,	"auto" },
	{ 1,	"manual" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusTagSharpness[]= {
	{ 0,	"normal" },
	{ 1,	"hard" },
	{ 2,	"soft" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusTagContrast[]= {
	{ 0,	"hard" },
	{ 1,	"normal" },
	{ 2,	"soft" },
	EXIF_TEXT_LIST_END
};

#if 0
static ExifTextList OlympusTag[]= {
	{ ,	"" },
	{ ,	"" },
	EXIF_TEXT_LIST_END
};
#endif


static ExifMarker OlympusExifMarkersList[] = {
{ 0x0001, EXIF_FORMAT_LONG_UNSIGNED, -1, "Konica/MinoltaSettings", "Konica / Minolta settings", NULL },
{ 0x0003, EXIF_FORMAT_LONG_UNSIGNED, -1, "Konica/MinoltaSettings", "Konica / Minolta settings", NULL },
{ 0x0040, EXIF_FORMAT_LONG_UNSIGNED, -1, "CompressedImageSize",	"Compressed image size", NULL },
{ 0x0081, EXIF_FORMAT_LONG_UNSIGNED, 1,  "ThumbnailOffset",	"Thumbnail offset",	NULL },
{ 0x0088, EXIF_FORMAT_LONG_UNSIGNED, 1,  "ThumbnailOffset",	"Thumbnail offset",	NULL },
{ 0x0089, EXIF_FORMAT_LONG_UNSIGNED, 1,  "ThumbnailLength",	"Thumbnail length",	NULL },
{ 0x0101, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Konica/Minolta.ColorMode", "Color mode",	KonMinTagColorMode },
{ 0x0102, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Konica/Minolta.Quality", "Quality",		KonMinTagQuality },
{ 0x0103, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Konica/Minolta.Quality", "Quality",		KonMinTagQuality },
{ 0x0200, EXIF_FORMAT_LONG_UNSIGNED, 3,  "Olympus.SpecialMode",	"Special mode",		NULL },
{ 0x0201, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.JpegQuality",	"Jpeg quality",		OlympusTagJpegQuality },
{ 0x0202, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.Macro",	"Macro",		OlympusTagMacro },
{ 0x0204, EXIF_FORMAT_RATIONAL_UNSIGNED, 1, "Olympus.DigitalZoom", "Digital zoom",	NULL },
{ 0x0207, EXIF_FORMAT_STRING, -1,	 "Olympus.Firmware",	"Firmware version",	NULL },
{ 0x0208, EXIF_FORMAT_STRING, -1,	 "Olympus.PictureInfo",	"Picture info",		NULL },
{ 0x0209, EXIF_FORMAT_UNDEFINED, -1,	 "Olympus.CameraID",	"Camera ID",		NULL },
{ 0x020b, EXIF_FORMAT_LONG_UNSIGNED, 1,	 "Epson.ImageWidth",	"Image width",		NULL },
{ 0x020c, EXIF_FORMAT_LONG_UNSIGNED, 1,  "Epson.ImageHeight",	"Image height",		NULL },
{ 0x020d, EXIF_FORMAT_STRING, -1,	 "Epson.Manufacturer",	"Manufacturer",		NULL },
{ 0x0e00, EXIF_FORMAT_BYTE, -1,		 "Olympus.PrintImageMatching", "Print image matching", NULL },
{ 0x1004, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.FlashMode",	"Flash mode",		OlympusTagFlashMode },
{ 0x1006, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.Bracket",	"Bracket",		NULL },
{ 0x100b, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.FocusMode",	"Focus mode",		OlympusTagFocusMode },
{ 0x100c, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.FocusDistance", "Focus distance",	NULL },
{ 0x100d, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.Zoom",	"Zoom",			NULL },
{ 0x1006, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.MacroFocus",	"Macro focus",		NULL },
{ 0x100f, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.Sharpness",	"Sharpness",		OlympusTagSharpness },
{ 0x1011, EXIF_FORMAT_SHORT_UNSIGNED, 9, "Olympus.ColorMatrix",	"Color matrix",		NULL },
{ 0x1012, EXIF_FORMAT_SHORT_UNSIGNED, 4, "Olympus.BlackLevel",	"Black level",		NULL },
{ 0x1015, EXIF_FORMAT_SHORT_UNSIGNED, 2, "Olympus.WhiteBalance", "White balance",	NULL },
{ 0x1017, EXIF_FORMAT_SHORT_UNSIGNED, 2, "Olympus.RedBias",	"Red bias",		NULL },
{ 0x1018, EXIF_FORMAT_SHORT_UNSIGNED, 2, "Olympus.BlueBias",	"Blue bias",		NULL },
{ 0x101a, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.SerialNumber", "Serial number",	NULL },
{ 0x1023, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.FlashBias",	"Flash bias",		NULL },
{ 0x1029, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.Contrast",	"Contrast",		OlympusTagContrast },
{ 0x102a, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.SharpnessFactor", "Sharpness factor",	NULL },
{ 0x102b, EXIF_FORMAT_SHORT_UNSIGNED, 6, "Olympus.ColorControl", "Color control",	NULL },
{ 0x102c, EXIF_FORMAT_SHORT_UNSIGNED, 2, "Olympus.ValidBits",	"Valid bits",		NULL },
{ 0x102d, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.CoringFilter", "Coring filter",	NULL },
{ 0x102e, EXIF_FORMAT_LONG_UNSIGNED, 1,  "Olympus.FinalWidth",	"Final width",		NULL },
{ 0x102f, EXIF_FORMAT_LONG_UNSIGNED, 1,  "Olympus.FinalHeight",	"Final height",		NULL },
{ 0x1034, EXIF_FORMAT_SHORT_UNSIGNED, 1, "Olympus.CompressionRatio", "Compression ratio", NULL },
EXIF_MARKER_LIST_END
};

static ExifTextList OlympusShootingMode[]= {
	{ 0,	"normal" },
	{ 1,	"unknown" },
	{ 2,	"fast" },
	{ 3,	"panorama" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusPanoramaDirection[]= {
	{ 1,	"left to right" },
	{ 2,	"right to left" },
	{ 3,	"bottom to top" },
	{ 4,	"top to bottom" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusWB[]= {
	{ 1,	"auto" },
	{ 2,	"manual" },
	{ 3,	"one-touch" },
	EXIF_TEXT_LIST_END
};

static ExifTextList OlympusWBColorTemp[]= {
	{ 2,	"3000" },
	{ 3,	"3700" },
	{ 4,	"4000" },
	{ 5,	"4500" },
	{ 6,	"5500" },
	{ 7,	"6500" },
	{ 8,	"7500" },
	EXIF_TEXT_LIST_END
};

gint format_olympus_makernote(ExifData *exif, unsigned char *tiff, guint offset,
			      guint size, ExifByteOrder bo)
{
	unsigned char *data;
	ExifItem *item;

	if (offset + 8 + 4 >= size) return FALSE;

	data = tiff + offset;

	/* Olympus tag format starts with "OLYMP\x00\x01" or "OLYMP\x00\x02",
	 * plus an unknown byte,
	 * followed by IFD data using Olympus tags.
	 */
	if (memcmp(data, "OLYMP\x00\x01", 7) != 0 &&
	    memcmp(data, "OLYMP\x00\x02", 7) != 0) return FALSE;

	if (exif_parse_IFD_table(exif, tiff, offset + 8, size,
				 bo, 0, OlympusExifMarkersList) != 0)
		{
		return FALSE;
		}

	item = exif_get_item(exif, "Olympus.SpecialMode");
	if (item && item->data_len == 3 * sizeof(guint32))
		{
		static ExifMarker marker = { 0x0200, EXIF_FORMAT_STRING, -1,
					     "Olympus.ShootingMode", "Shooting mode", NULL };
		guint32 *array = item->data;
		const gchar *mode;
		const gchar *pdir = NULL;
		gchar *text;
		gint l;

		mode = exif_text_list_find_value(OlympusShootingMode, array[0]);
		if (array[0] == 3)
			{
			pdir = exif_text_list_find_value(OlympusPanoramaDirection, array[2]);
			}

		text = g_strdup_printf("%s%s%s, seq %d", (mode) ? mode : "unknown",
				       (pdir) ? " " : "", (pdir) ? pdir : "",
				       array[1] + 1);
		l = strlen(text) + 1;
		item = exif_item_new(marker.format, marker.tag, l, &marker);
		memcpy(item->data, text, l);
		g_free(text);

		exif->items = g_list_prepend(exif->items, item);
		}

	item = exif_get_item(exif, "Olympus.WhiteBalance");
	if (item && item->data_len == 2 * sizeof(guint16))
		{
		static ExifMarker marker = { 0x1015, EXIF_FORMAT_STRING, -1,
					     "Olympus.WhiteBalance", "White balance", NULL };
		guint16 *array = item->data;
		const gchar *mode;
		const gchar *color = NULL;
		gchar *text;
		gint l;

		mode = exif_text_list_find_value(OlympusWB, array[0]);
		if (array[0] == 2)
			{
			color = exif_text_list_find_value(OlympusWBColorTemp, array[1]);
			}

		text = g_strdup_printf("%s%s%s", (mode) ? mode : "unknown",
				       (color) ? " " : "", (color) ? color : "");
		l = strlen(text) + 1;
		item = exif_item_new(marker.format, marker.tag, l, &marker);
		memcpy(item->data, text, l);
		g_free(text);

		exif->items = g_list_prepend(exif->items, item);
		}

	return TRUE;
}


