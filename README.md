WATCHFACE NAME:
===============
Moontiles Pebble Watchface 

WATCHFACE DESCRIPTION:
======================
Key information is found in 7 titles as follows:

Hour, Minute, Day of the Week, Moon Phase, Day of the Month, Month, Year

For the moon phase, an icon for one of 14 phases of the moon is displayed
with a different representation for waxing and waning moons.  I wanted to do
the actual calculations on the Pebble to determine moon phase but without a
floating point math library in the Pebble API, I chose to dynamically
generate 5 years of data and store it in the app.  If you change the date to
one outside of the table range, the Moon Phase won't show any icon.  So moon
phases prior to the watchfile compile date will be blank.

THIRD-PARTY ATTRIBUTION:
========================
I used two free fonts from DaFont.com:

WW Digital by Michelle Laura
Moon Phases by Curtis Clark

My source code for pre-generating the moon phases table is based heavily on
an old C-based GUI MoonTool for Sun workstations written by John Walker in
1987.  John is crazy smart and I really don't understand all of the
underlying seriously complex algorithms nor do I aspire to be a plantary
scientist.  The tile layout is based on Ronald van der Lingen's Retro Clock
watchface with different type treatment, a more readable font, no lines
through the font, and no odd white space between tiles.

The Watchface only imports the exact characters needed to render the
display.  It also only updates the portion of the display that has changed.
However, I'm not sure if this saves much battery power.
