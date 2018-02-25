Master Music Program (MMP) Version 0.2.01
Initial version by Kenneth Lantrip (bothersome@mindspring.com)

MMP is placed into the public domain so that others may improve upon
it and use its source code as a learning tool.  There are some very
useful routines in MMP that can be utilized very easily in other
programs that need them.

There is much room for improvement with MMP.  Hopefully someone will
be able to add some of these needed features.

*  Use and edit truly long filenames, paths, descriptions, artists.
*  Add a left/right scroll to the edit field.
*  Add copy and paste funtions compatible with the rest of Windows.

Even with MMP's shortcomings, it's still a great program and a delight
to use.  MMP no longer needs an outside player to function anymore.
I have added the ability to play these following formats:

.RAW/.WAV/.MP2/.MP3/.OGG/.WMA/.ASF
.MOD (protracker/fasttracker modules)
.S3M (screamtracker 3 modules)
.XM  (fasttracker 2 modules)
.IT  (impulse tracker modules)
.MID (MIDI files)
.RMI (MIDI files)
.SGT (DirectMusic segment files)

MMP now works like a jukebox.  You simply select files from the catalog
and press F2 (play).  As MMP plays these files you may select more and
add those to the play list (F2).  


== Setting UP MMP ==

Place the MMP.EXE and FMOD.DLL file into a directory of your choice.
Make a shortcut to MMP.EXE from your desktop or wherever.


== Using MMP ==

Basically MMP is a list type catalog that holds music file tracks.
The function keys used in MMP are:

       F1 - Select track
   Alt-F1 - Select all tracks in current volume and path
  Ctrl-F1 - Unselect all tracks
       F2 - Play or add selected tracks to play list
  Ctrl-F2 - Play next track in play list or stop play if last
       F3 - Pause or UnPause current playing track
       F4 - Stop playing all tracks
   Alt-F3 - Search within current field
  Ctrl-F4 - Copy selected tracks to destination path
   Alt-F4 - Copy selected tracks to destination and rebuild filenames

       F5 - Sort on Artist, Title
       F6 - Sort on Volume, Artist, Title
       F7 - Sort on Title, Artist
       F8 - Sort on Path, Filename

       F9 - Copy field of previous record into current field
      F12 - Add new records by scanning a disc or directory
 Ctrl-F12 - Delete current record

     Home - Move to beginning of field
Ctrl-Home - Move to top of list
     PgUp - Move to top of page or page up
     pgDn - Move to bottom of page or page down
      End - Move to end of field
 Ctrl-End - Move to bottom of list
      Del - Delete a field or character
      Ins - Toggle character insert mode
Arrow Key - Move edit box or cursor position


== Walk Through ==

Start MMP with the shortcut icon you set up.  Put one of your MP3
(or OGG) CD collections into your CD drive.  Press F12 and type in
the Drive and Directory (if needed) and the wildcard specs for
your files that you want to add.  For example, suppose you have a
MP3 collection on a CD disc and these files are located in the
"\Music" directory that is on the CD.  Type in:

   D:\Music\*.mp3

This would add all the MP3 files from the \Music directory.  You
could also use any other valid wildcard query like:

   D:\Music\Kid Rock*.mp3

This would add all the Kid Rock tracks into your catalog.

After you get those files added you can switch discs and add any
other discs the same way.

To play some of the tracks (put the appropriate disc in the drive
first) and press F1 to select some of the tracks you want to play.
Then press F2 to play them.  MMP will start playing the tracks you
selected.  Press F3 to pause.  Press F4 to stop.  While MMP is
playing you may select more tracks with F1 and add them to the play
list with F2.  You can re-sort the list by Pressing F5 through F8.

The track artist and title are pulled from the filenames.  The
seperator is a " - " character combination within the filename.

If you notice an error and would like to correct it, arrow over
to the field that needs fixing.  Just start typing to replace the
current field or press Ins or End to get into Field Edit mode.
Then arrow the cursor to what needs fixing.

If you mess up and wish to revert back to a previous save.  Just
click on the close program icon to exit without saving.  Any changes
to the catalog are only saved when you exit with the Escape key.

When you edit the Path, all matching path entries for all tracks on that
Volume are changed to the new path you just changed.  I know, that
sounds confusing.  It works like this...  Suppose you have some tracks
stored away in "C:\MyTunes".  You decided to move them to another drive
like "D:\Rock Tunes".  When you change the path for one of those
tracks, MMP searches and changes all tracks that were on that disk,
and in that directory (matching path).  MMP will change those entries
to the new location so you don't have to edit each one.  If for some
reason you want to only change one or some of the tracks, you could
change the Volume name for each track needing moved.  Then change the
path to a new location.  It may be easier to just delete the track
entries and re-add them from their new location.


== Credits and Plugs ==

Some code snippets for MMP were derived from the excellent tool BCX
(Basic to C Converter) Project from Kevin Diggins.
http://www.users.qwest.net/~sdiggins/bcx.htm

Some helpful advice on reading the Volume label from Robert Wishlaw.

Compiled on a great 32 bit Windows compiler LCC-Win32.
http://www.cs.virginia.edu/~lcc-win32/

Uses the FMOD (Music and Sound Effects System) libraries
http://www.fmod.org 

== Programmer Notes ==

My original intention for MMP was to write an application in BCX.  I
was so impressed with Kevin's program that it just HAD to have a neat
and nifty program to show off it's capabilities.  But I had only just
started using it and my not knowing what was allowed kept me putting
in-line C code into the program.  This cluttered the effect and I
really wanted the code to look clean for others.  So I converted the
whole project to C.

I used console mode because I like being able to control the size of
the characters and layout at a whim.  I guess someone should convert
it to a Grid-Control-GUI layout.

To compile MMP (with LCC-Win32) you will need to get the FMOD
developement package for the FMODLCC.LIB file and the header files.

== Ogg Vorbis Legaleze ==

Ogg Vorbis License
------------------
Portions Copyright (c) 2001, Xiphophorus

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of the Xiphophorus nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


== Release Notes ==

0.1
First Release

0.1.01
Corrected source code remarks
Added Alt-F1 track selection
Added Alt-F2 copy selected tracks

0.1.02
Added Alt-F3 copy selected tracks and rebuild filename
Fixed Alt-F1 track selection
Added custom colors so "color-blind" people can see the difference

0.1.03
Fixed routine to automatically assign Artist and Title on scan for
  longer lengths.
Increased the sort routine's efficiency and to exclude the word "The ".
Made the sort routine case insensitive.

0.2
Removed dependancy on external players.  MMP can now play files alone.
Fixed MMP to work like a jukebox.
Changed some function key functions.
Added playing versus selected highlighting.

0.2.01
Made a 2 second sound buffer for less re-sync problems under Win2000.
Removed auto-detect for WinMM or DirectSound (fixes re-sync) uses WinMM.
Minor changes to the way the sound system is initialized.
Added ability to pre-load next track while playing current track.