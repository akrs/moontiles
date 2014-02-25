/* Application:   	Pebble Moontiles Watchface - Ed's Edition

   Filename: 	  	moontiles.c

   Version:       	1.0
   
   Version History:	1.0 Forked from Moontiles
   
   Purpose:	  		Main implementation source                                                        

   Authors:        	Brian K. Holman (me@brianholman.com), Andrew Akers (andrew@akrs.me)

   Date:	  		24 Feburary 2014
*/                                               

#include <pebble.h>
#include <moonphase.h>

#define REVERSE 1
#ifdef REVERSE
#define COLOR_FOREGROUND GColorBlack
#define COLOR_BACKGROUND GColorWhite
#else
#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack
#endif

Window *window;

TextLayer *time_text;
TextLayer *day_text;
TextLayer *moon_text;
TextLayer *date_text;
TextLayer *month_text;
TextLayer *year_text;
TextLayer *ampm_text;
Layer *background;

/* Moon Phase (0-14), Waxing Character, Waning Character */
static char MoonPhaseCharLookup[15][4] =
{
	{'0','0','0','0'},  /* 0 */
	{'A','Z','a','z'},  /* 1 */
	{'B','Y','b','y'},  /* 2 */
	{'C','X','c','x'},  /* 3 */
	{'D','W','d','w'},  /* 4 */
	{'E','V','e','v'},  /* 5 */
	{'F','U','f','u'},  /* 6 */
	{'G','T','g','t'},  /* 7 */
	{'H','S','h','s'},  /* 8 */
	{'I','R','i','r'},  /* 9 */
	{'J','Q','j','q'}, /* 10 */
	{'K','P','k','p'}, /* 11 */
	{'L','O','l','o'}, /* 12 */
	{'M','N','m','n'}, /* 13 */
	{'1','1','1','1'}  /* 14 */
};

/*  JDATE  --  Convert internal date to Julian day.  */

long jdate(struct tm *t)
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

// callback function for minute tick events that update the time and date display
void handle_tick(struct tm *tick_time, TimeUnits units_changed)
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
		strftime(time, sizeof(time), "%R", tick_time);
		ampm[0] = '\0';
	}
	else
	{
		strftime(time, sizeof(time), "%I:%M", tick_time);
		strftime(ampm, sizeof(ampm), "%p", tick_time);
	}
	text_layer_set_text(time_text, strip(time));
	text_layer_set_text(ampm_text, ampm);
	
	/* Set day of the week */
	if (tm_wday != tick_time->tm_wday)
	{
		strftime(day, sizeof(day), "%a", tick_time);
		text_layer_set_text(day_text, day);
		tm_wday = tick_time->tm_wday;
	}
	
	/* Set day of the month and associated moon phase for day of the month */
	if (tm_mday != tick_time->tm_mday)
	{
		strftime(date, sizeof(date), "%e", tick_time);
		text_layer_set_text(date_text, strip(date));
		tm_mday = tick_time->tm_mday;
		
		/* Set Moon Phase */
	
		/* Find the offset of today's julian date in the lookup table */
		arypos = jdate(tick_time) - JULIAN_MOON_EPIC;
		if (arypos >= 0 && arypos < MOONPHASE_ARRAY_SIZE)
		{
			if (!REVERSE)
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
				if (MoonPhaseDateLookup[arypos][1])
				{
					/* Waxing Moon */
					moon[0] = MoonPhaseCharLookup[MoonPhaseDateLookup[arypos][0]][2];
				}
				else 
				{
					/* Waning Moon */
					moon[0] = MoonPhaseCharLookup[MoonPhaseDateLookup[arypos][0]][3];
				}
			}
		}
		else
		{
			moon[0] = '\0';
		}
		text_layer_set_text(moon_text, moon);
	}
	
	/* Set name of the month */
	if (tm_mon != tick_time->tm_mon)
	{
		strftime(month, sizeof(month), "%b", tick_time);
		text_layer_set_text(month_text, strip(month));	
		tm_mon = tick_time->tm_mon;
	}
	
	/* Set year */
	if (tm_year != tick_time->tm_year)
	{
		strftime(year, sizeof(year), "%y", tick_time);
		text_layer_set_text(year_text, year);
		tm_year = tick_time->tm_year;
	}
}

// utility function for initializing a text layer
TextLayer* init_text(int x, int y, int width, int height, ResourceId font, GColor TextColor)
{
	TextLayer* textlayer;
	textlayer = text_layer_create(GRect(x, y, width, height));
	text_layer_set_text_alignment(textlayer, GTextAlignmentCenter);
	text_layer_set_text_color(textlayer, TextColor);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_font(textlayer, fonts_load_custom_font(resource_get_handle(font)));
	return textlayer;
}

// callback function for the app initialization
void handle_init()
{
	window = window_create();
	window_stack_push(window, true /* Animated */);
	window_set_background_color(window, COLOR_FOREGROUND);
	
	time_text = init_text(2, 8 + 4, 140, 68 - 4, RESOURCE_ID_FONT_WW_DIGITAL_SUBSET_52, COLOR_BACKGROUND);
	day_text = init_text(2, 81 + 2, 68, 43 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DOW_SUBSET_33, COLOR_BACKGROUND);
	date_text = init_text(2, 128 + 2, 32, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
	month_text = init_text(38, 128 + 2, 68, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
	year_text= init_text(110, 128 + 2, 32, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
	ampm_text = init_text(4, 63, 16, 12, RESOURCE_ID_FONT_WW_DIGITAL_SUBSET_10,COLOR_BACKGROUND);
	moon_text = init_text(74, 85 + 2, 68, 43 - 2, RESOURCE_ID_FONT_MOONPHASE_33, COLOR_BACKGROUND);
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_text));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(day_text));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_text));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(month_text));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(year_text));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(ampm_text));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(moon_text));
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void handle_deinit(void)
{
	tick_timer_service_unsubscribe();
}

// main entry point of this Pebble watchface
int main(void)
{
	handle_init();
	app_event_loop();
	handle_deinit();
}
