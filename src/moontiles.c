/* Application:   	Pebble Moontiles Watchface

   Filename: 	  	moontiles.c

   Version:       	1.1 
   
   Version History:	1.0 Initial Version
   			1.1 Combined hour and minute tiles and increased typeface size, support reverse of black and white
   
   Purpose:	  Main implementation source                                                        

   Author:        Brian K. Holman (me@brianholman.com)      

   Date:	  19 Apr 2013
*/                                               

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "moonphase.h"

#define MY_UUID { 0x52, 0x82, 0x1B, 0x97, 0xF2, 0xE8, 0x4B, 0xA0, 0x8D, 0x69, 0xB3, 0x95, 0x86, 0x6D, 0xA2, 0x85 }

PBL_APP_INFO(MY_UUID, "Moontiles", "Brian Holman", 1, 1, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

/* #define REVERSE 1 */
#ifdef REVERSE
#define COLOR_FOREGROUND GColorBlack
#define COLOR_BACKGROUND GColorWhite
#else
#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack
#endif

Window window;

Layer background;
TextLayer time_text;
TextLayer day_text;
TextLayer moon_text;
TextLayer date_text;
TextLayer month_text;
TextLayer year_text;
TextLayer ampm_text;

/* Moon Phase (0-14), Waxing Character, Waning Character */
static char MoonPhaseCharLookup[15][2] =
{
	{'0','0'},  /* 0 */
	{'A','Z'},  /* 1 */
	{'B','Y'},  /* 2 */
	{'C','X'},  /* 3 */
	{'D','W'},  /* 4 */
	{'E','V'},  /* 5 */
	{'F','U'},  /* 6 */
	{'G','T'},  /* 7 */
	{'H','S'},  /* 8 */
	{'I','R'},  /* 9 */
	{'J','Q'}, /* 10 */
	{'K','P'}, /* 11 */
	{'L','O'}, /* 12 */
	{'M','N'}, /* 13 */
	{'1','1'}  /* 14 */
};

/*  JDATE  --  Convert internal date to Julian day.  */

long jdate(PblTm *t)
{
	long c, m, y;

	y = t->tm_year + 1900;
	m = t->tm_mon + 1;
	if (m > 2)
	   m = m - 3;
	else {
	   m = m + 9;
	   y--;
	}
	c = y / 100L;		   /* Compute century */
	y -= 100L * c;
	return t->tm_mday + (c * 146097L) / 4 + (y * 1461L) / 4 +
	    (m * 153L + 2) / 5 + 1721119L;
}

// utility function to strip a leading space or zero from a string.
char* strip(char* input)
{
	if (strlen(input) > 1 && (input[0] == ' ' || input[0] == '0'))
	{
		return input + 1;
	}

	return input;
}

// callback function for rendering the background layer
void background_update_callback(Layer *me, GContext* ctx)
{
	(void) me;

	graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);
	graphics_fill_rect(ctx, GRect(2,8,140,68), 4, GCornersAll); /* Time Box */
	graphics_fill_rect(ctx, GRect(2,81,68,43), 4, GCornersAll); /* Day Box */
	graphics_fill_rect(ctx, GRect(74,81,68,43), 4, GCornersAll); /* Moon Box */
	graphics_fill_rect(ctx, GRect(2,128,32,32), 4, GCornersAll); /* Date Box */
	graphics_fill_rect(ctx, GRect(38,128,68,32), 4, GCornersAll); /* Month Box */
	graphics_fill_rect(ctx, GRect(110,128,32,32), 4, GCornersAll); /* Year Box */

}

// callback function for minute tick events that update the time and date display
void handle_tick(AppContextRef ctx, PebbleTickEvent *event)
{
	long arypos;
	static char time[]   = "     ";
	static char day[]    = "   ";
	static char moon[]   = " ";
	static char date[]   = "  ";
	static char month[]  = "   ";
	static char year[]   = "  ";
	static char ampm[]   = "  ";
	static int tm_mday = -1;
	static int tm_mon = -1;
	static int tm_year = -1;
	static int tm_wday = -1;

	/* Set time and AM/PM if not 24-hour*/
	if (clock_is_24h_style())
	{
		string_format_time(time, sizeof(time), "%R", event->tick_time);
		ampm[0] = '\0';
	}
	else
	{
		string_format_time(time, sizeof(time), "%I:%M", event->tick_time);
		string_format_time(ampm, sizeof(ampm), "%p", event->tick_time);
	}
	text_layer_set_text(&time_text, strip(time));
	text_layer_set_text(&ampm_text, ampm);
	
	/* Set day of the week */
	if (tm_wday != event->tick_time->tm_wday)
	{
		string_format_time(day, sizeof(day), "%a", event->tick_time);
		text_layer_set_text(&day_text, day);
		tm_wday = event->tick_time->tm_wday;
	}
	
	/* Set day of the month and associated moon phase for day of the month */
	if (tm_mday != event->tick_time->tm_mday)
	{
		string_format_time(date, sizeof(date), "%e", event->tick_time);
		text_layer_set_text(&date_text, strip(date));
		tm_mday = event->tick_time->tm_mday;
		
		/* Set Moon Phase */
	
		/* Find the offset of today's julian date in the lookup table */
		arypos = jdate(event->tick_time) - JULIAN_MOON_EPIC;
		if (arypos >= 0 && arypos < MOONPHASE_ARRAY_SIZE)
		{
			if (MoonPhaseDateLookup[arypos][1])
			{
				/* Waxing Moon */
				moon[0] = MoonPhaseCharLookup[MoonPhaseDateLookup[arypos][0]][0];
			}
			else 
			{
				/* Waning Moon */
				moon[0] = MoonPhaseCharLookup[MoonPhaseDateLookup[arypos][0]][1];
			}
		}
		else
		{
			moon[0] = '\0';
		}
		text_layer_set_text(&moon_text, moon);
	}
	
	/* Set name of the month */
	if (tm_mon != event->tick_time->tm_mon)
	{
		string_format_time(month, sizeof(month), "%b", event->tick_time);
		text_layer_set_text(&month_text, strip(month));	
		tm_mon = event->tick_time->tm_mon;
	}
	
	/* Set year */
	if (tm_year != event->tick_time->tm_year)
	{
		string_format_time(year, sizeof(year), "%y", event->tick_time);
		text_layer_set_text(&year_text, year);
		tm_year = event->tick_time->tm_year;
	}
}

// utility function for initializing a text layer
void init_text(TextLayer* textlayer, int x, int y, int width, int height, ResourceId font, GColor TextColor)
{
	text_layer_init(textlayer, GRect(x, y, width, height));
	text_layer_set_text_alignment(textlayer, GTextAlignmentCenter);
	text_layer_set_text_color(textlayer, COLOR_BACKGROUND);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_font(textlayer, fonts_load_custom_font(resource_get_handle(font)));
}

// callback function for the app initialization
void handle_init(AppContextRef ctx)
{
	(void)ctx;

	window_init(&window, "Moontiles");
	window_stack_push(&window, true /* Animated */);
	window_set_background_color(&window, COLOR_BACKGROUND);

	resource_init_current_app(&APP_RESOURCES);

	layer_init(&background, window.layer.frame);
	background.update_proc = &background_update_callback;
	layer_add_child(&window.layer, &background);

	init_text(&time_text, 2, 8 + 4, 140, 68 - 4, RESOURCE_ID_FONT_WW_DIGITAL_SUBSET_52, COLOR_BACKGROUND);
	init_text(&day_text, 2, 81 + 2, 68, 43 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DOW_SUBSET_33, COLOR_BACKGROUND);
	init_text(&moon_text, 74, 85 + 2, 68, 43 - 2, RESOURCE_ID_FONT_MOONPHASE_33, COLOR_BACKGROUND);
	init_text(&date_text, 2, 128 + 2, 32, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
	init_text(&month_text, 38, 128 + 2, 68, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
	init_text(&year_text, 110, 128 + 2, 32, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
	init_text(&ampm_text, 4, 63, 16, 12, RESOURCE_ID_FONT_WW_DIGITAL_SUBSET_10,COLOR_BACKGROUND);

	layer_add_child(&window.layer, &time_text.layer);
	layer_add_child(&window.layer, &day_text.layer);
	layer_add_child(&window.layer, &moon_text.layer);
	layer_add_child(&window.layer, &date_text.layer);
	layer_add_child(&window.layer, &month_text.layer);
	layer_add_child(&window.layer, &year_text.layer);
	layer_add_child(&window.layer, &ampm_text.layer);
}

// main entry point of this Pebble watchface
void pbl_main(void *params)
{
	PebbleAppHandlers handlers =
	{
		.init_handler = &handle_init,
		.tick_info =
		{
			.tick_handler = &handle_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	app_event_loop(params, &handlers);
}
