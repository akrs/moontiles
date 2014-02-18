/* Application:     Pebble Moontiles Watchface

   Filename:        moontiles.c

   Version:         2.0 
   
   Version History: 1.0 Initial Version
                    1.1 Combined hour and minute tiles and increased typeface size, support reverse of black and white
                    2.0 Updated to Pebble SDK v2
   
   Purpose:         Main implementation source                                                        

   Authors:         Brian K. Holman (me@brianholman.com), Andrew Akers (andrew@akrs.me)

   Date:            9 January 2013
*/                                               

#include <pebble.h>
#include <moonphase.h>
    
#define REVERSED_KEY 0

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
    c = y / 100L;          /* Compute century */
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
void background_update_callback(Layer *me, GContext *ctx)
{
    bool not_reversed = persist_read_int(REVERSED_KEY) == 0;
    GColor COLOR_FOREGROUND;
    if (not_reversed) 
    {
        COLOR_FOREGROUND = GColorWhite;
    }
    else
    {
        COLOR_FOREGROUND = GColorBlack;
    }
    graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);
    graphics_fill_rect(ctx, GRect(2,8,140,68), 4, GCornersAll); /* Time Box */
    graphics_fill_rect(ctx, GRect(2,81,68,43), 4, GCornersAll); /* Day Box */
    graphics_fill_rect(ctx, GRect(74,81,68,43), 4, GCornersAll); /* Moon Box */
    graphics_fill_rect(ctx, GRect(2,128,32,32), 4, GCornersAll); /* Date Box */
    graphics_fill_rect(ctx, GRect(38,128,68,32), 4, GCornersAll); /* Month Box */
    graphics_fill_rect(ctx, GRect(110,128,32,32), 4, GCornersAll); /* Year Box */
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
    bool not_reversed = persist_read_int(REVERSED_KEY) == 0;
    GColor COLOR_BACKGROUND;
    if (not_reversed)
    {
        COLOR_BACKGROUND = GColorBlack;
    }
    else
    {
        COLOR_BACKGROUND = GColorWhite;
    }
    TextLayer* textlayer;
    textlayer = text_layer_create(GRect(x, y, width, height));
    text_layer_set_text_alignment(textlayer, GTextAlignmentCenter);
    text_layer_set_text_color(textlayer, COLOR_BACKGROUND);
    text_layer_set_background_color(textlayer, GColorClear);
    text_layer_set_font(textlayer, fonts_load_custom_font(resource_get_handle(font)));
    return textlayer;
}

void handle_inbox(DictionaryIterator *it, void *context)
{
	Tuple* tup = dict_read_first(it);
	persist_write_int(tup->key, tup->value->int32);
}

// callback function for the app initialization
void handle_init()
{
    if (!persist_exists(REVERSED_KEY)) 
    {
        persist_write_int(REVERSED_KEY, 0);
    }
    bool not_reversed = persist_read_int(REVERSED_KEY) == 0;
    
    GColor COLOR_BACKGROUND;
    if (not_reversed) 
    {
        COLOR_BACKGROUND = GColorBlack;
    }
    else
    {
        COLOR_BACKGROUND = GColorWhite;
    }
    window = window_create();
    window_stack_push(window, true /* Animated */);
    window_set_background_color(window, COLOR_BACKGROUND);
    
    background = layer_create(layer_get_bounds(window_get_root_layer(window)));
    layer_set_update_proc(background, &background_update_callback);
    layer_add_child(window_get_root_layer(window), background);
    
    time_text = init_text(2, 8 + 4, 140, 68 - 4, RESOURCE_ID_FONT_WW_DIGITAL_SUBSET_52, COLOR_BACKGROUND);
    day_text = init_text(2, 81 + 2, 68, 43 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DOW_SUBSET_33, COLOR_BACKGROUND);
    moon_text = init_text(74, 85 + 2, 68, 43 - 2, RESOURCE_ID_FONT_MOONPHASE_33, COLOR_BACKGROUND);
    date_text = init_text(2, 128 + 2, 32, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
    month_text = init_text(38, 128 + 2, 68, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
    year_text= init_text(110, 128 + 2, 32, 32 - 2, RESOURCE_ID_FONT_WW_DIGITAL_DATE_SUBSET_22,COLOR_BACKGROUND);
    ampm_text = init_text(4, 63, 16, 12, RESOURCE_ID_FONT_WW_DIGITAL_SUBSET_10,COLOR_BACKGROUND);
    
    layer_add_child(background, text_layer_get_layer(time_text));
    layer_add_child(background, text_layer_get_layer(day_text));
    layer_add_child(background, text_layer_get_layer(moon_text));
    layer_add_child(background, text_layer_get_layer(date_text));
    layer_add_child(background, text_layer_get_layer(month_text));
    layer_add_child(background, text_layer_get_layer(year_text));
    layer_add_child(background, text_layer_get_layer(ampm_text));
    
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
	app_message_register_inbox_received(handle_inbox);
}

void handle_deinit(void)
{
    tick_timer_service_unsubscribe();
	app_message_deregister_callbacks();
}



// main entry point of this Pebble watchface
int main(void)
{
    handle_init();
    app_event_loop();
    handle_deinit();
}