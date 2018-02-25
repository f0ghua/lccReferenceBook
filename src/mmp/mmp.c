/*
	Master Music Program (MMP) by Kenneth Lantrip
	Version 0.2.01
	This program is used as a music file catalog and player.
	List is best viewed with tabs set to 4
*/

#include <io.h>
#include <stdarg.h>
#include <MyStuff\KenLib.c>
#include <fmod/fmod.h>  /* these two headers come from FMOD dev package */

const int MaxRecords = 50000; /* set this to the max number of records */
const char *Val1 = "0123456789?=<(_*$&#^~@!`-+')>%ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz.,[]{}:;\\";

const int NormColor = 10;		/* Normal list color */
const int QuedColor = 14;		/* Selected list color */
const int PlayColor = 12;		/* Track in que to play color */
const int DetlColor = 7;		/* Detail color at bottom of screen */
const int HeadColor = 11;		/* Header color at top of screen */
const int LineColor = 13;		/* Line seperator color */
const int AttnColor = 15;		/* Ask the user a question color */
const int BackColor = 0;		/* Background color */
const int HighLight = 0x40;		/* This is used to set the edit area color */

typedef struct {
	unsigned char VolLabel[13];
	unsigned char Class[7];
	unsigned char Path[70];
	unsigned char FileName[70];
	unsigned char Artist[70];
	unsigned char Title[70];
	unsigned char FDate[11];
	DWORD Size;
	int LenInSec;
	int Que;
} TrackInfo;

static TrackInfo Item[MaxRecords];
static FSOUND_STREAM *stream;
static FMUSIC_MODULE *mod;
static unsigned char t1[384], t2[384], ts[256], ps[256], lss[70];
static unsigned char OldPath[70], OldLabel[13];
static char *data1, *data2;
static int HiPlay, NumberItems, TopItem, channel;
static int CurSort, CurPlay, CurLine, CurField;
static long sfs;

int StrCompare(const void *arg1, const void *arg2) {
	char *v1 = (char*)arg1;
 	char *v2 = (char*)arg2;
	char *a1 = v1 + 160;
	char *a2 = v2 + 160;
	int x;

	if (strcmpi(left(a1,4),"The ") == 0) a1 += 4;	/* don't include the word "The " */
	if (strcmpi(left(a2,4),"The ") == 0) a2 += 4;	/* in the search */

	switch (CurSort) {
	case 0:		/* Sort on Artist, Title */
		if (x = strcmpi(a1,a2)) return x;
		return strcmpi(v1 + 230,v2 + 230);
	case 1:		/* Sort on Volume, Artist, Title */
		if (x = strcmpi(v1,v2)) return x;
		if (x = strcmpi(a1,a2)) return x;
		return strcmpi(v1 + 230,v2 + 230);
	case 2:		/* Sort on Title, Artist */
		if (x = strcmpi(v1 + 230,v2 + 230)) return x;
		return strcmpi(a1,a2);
	case 3:		/* Sort on Path, FileName */
		if (x = strcmpi(v1 + 20,v2 + 20)) return x;
		return strcmpi(v1 + 90,v2 + 90);
	case 4:		/* Sort on Type, Artist, Title */
		if (x = strcmpi(v1 + 13,v2 + 13)) return x;
		if (x = strcmpi(a1,a2)) return x;
		return strcmpi(v1 + 230,v2 + 230);
	}
 	return 0;	/* Just in case  ;)  */
}

void ShowDetail(int n) {
	locate (23,1); printf("%s",string(239,32));
	locate (23,1); color(DetlColor,BackColor);
	printf("%10s : %s\n",Item[n].FDate,Item[n].Artist);
	printf("%10d : %s\n",Item[n].Size,Item[n].FileName);
	printf("%4d:%02d:%02d : %s",Item[n].LenInSec / 3600,(Item[n].LenInSec / 60) % 60,
			Item[n].LenInSec % 60,Item[n].Path);
}

void ShowLine(int row, int fc, int bc, int n, int sd) {
	locate(row,1); color(fc,bc);
	printf("%-5s %-12s %-60s",Item[n].Class,Item[n].VolLabel,Item[n].Title);
	if (sd) ShowDetail(n);
}

int PrepNextTrack(int w, int *length) {
	int i;
	unsigned char fln[256];		/* This routine's function is to load the next track */
	FILE *fp;					/* while the current track is still playing          */

	for (i = 0; i < NumberItems; i++) {
		if (Item[i].Que > 1) if (Item[i].Que == CurPlay + 1) {
			sprintf(fln,"%s%s",Item[i].Path,Item[i].FileName);
			if (strcmpi(right(fln,4),".wma"))  /* If next file is any of these */
			if (strcmpi(right(fln,4),".asf"))  /* we don't need to pre-load    */
			if (strcmpi(right(fln,4),".mod"))
			if (strcmpi(right(fln,4),".s3m"))
			if (strcmpi(right(fln,4),".mid"))
			if (strcmpi(right(fln,4),".rmi"))
			if (strcmpi(right(fln,4),".sgt"))
			if (strcmpi(right(fln,3),".xm"))
			if (strcmpi(right(fln,3),".it"))
			if ((fp = fopen(fln,"rb")) != NULL) {
				fseek(fp,0,SEEK_END); *length = ftell(fp); fseek(fp,0,SEEK_SET);
				if (w == 1) {
					data2 = (char *)malloc(*length); fread(data2,*length,1,fp); fclose(fp);
					return 2;
				} else {
					data1 = (char *)malloc(*length); fread(data1,*length,1,fp); fclose(fp);
					return 1;
				}
			}
			return 0;
		}
	}
	return 0;
}

signed char endcallback(FSOUND_STREAM *stm,void *buff,int len,int param) {
	/* end of stream callback doesnt have a 'buff' value, if it doesnt it could be
   		a sync point. */
	if (!buff) SetConsoleTitle("Master Music Program");
	return TRUE;
}

DWORD PlayQuedTracks(void) {
	static unsigned char wtitle[160];
	int h, i, j, k, CurHiPlay, cx, cy, ocf, ocb;
	int length, ntr = 0;
	FILE *fp;

	FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
	FSOUND_SetBufferSize(2000);
	if (!FSOUND_Init(44100,16,FSOUND_INIT_GLOBALFOCUS)) return 0;

	do {
		CurHiPlay = HiPlay; j = CurHiPlay;
		for (i = 0; i < NumberItems; i++) if (Item[i].Que > 1 && Item[i].Que < j) j = Item[i].Que;
		for (CurPlay = j; CurPlay < CurHiPlay; CurPlay++)
			for (i = 0; i < NumberItems; i++) if (Item[i].Que == CurPlay) {
				sprintf(ps,"%s%s",Item[i].Path,Item[i].FileName);
				sprintf(wtitle,"%s - %s",Item[i].Artist,Item[i].Title);
				SetConsoleTitle(wtitle);
				mod = FMUSIC_LoadSong(ps);
				if (mod == NULL) {
					if (ntr == 0) if ((fp = fopen(ps,"rb")) != NULL) {
						fseek(fp,0,SEEK_END); length = ftell(fp); fseek(fp,0,SEEK_SET);
						data1 = (char *)malloc(length); fread(data1,length,1,fp); fclose(fp);
						ntr = 1;
					} else ntr = 0;  /* this is set upon last if condition fail */
					if (ntr) {
						if (ntr == 1) stream = FSOUND_Stream_OpenFile(data1,0xE051,length);
						else stream = FSOUND_Stream_OpenFile(data2,0xE051,length);

						if (stream == NULL) stream = FSOUND_Stream_OpenFile(ps,0x6051,length);
						if (stream != NULL) {
							FSOUND_Stream_SetEndCallback(stream,endcallback,0);
							FSOUND_Stream_SetSynchCallback(stream,endcallback,0);

							if ((channel = FSOUND_Stream_PlayEx(FSOUND_FREE,stream,NULL,TRUE)) != -1) {
								Item[i].LenInSec = FSOUND_Stream_GetLengthMs(stream) / 1000;
								if (i == CurLine) {
									ScreenGetCursor(&cx,&cy); locate(25,1);
									ocf = color_fg; ocb = color_bg; color(DetlColor,BackColor);
									printf("%4d:%02d:%02d",Item[i].LenInSec / 3600,
											(Item[i].LenInSec / 60) % 60,Item[i].LenInSec % 60);
									locate(cx,cy); color(ocf,ocb);
								}
								FSOUND_SetPaused(channel,FALSE); h = -1;
								ntr = PrepNextTrack(ntr,&length);
								do {
									k = FSOUND_Stream_GetTime(stream);
									if (k > h || k < 2000 || FSOUND_GetPaused(channel)) {
										h = k; k = k / 1000;
									} else break;
									sprintf(wtitle,"%d:%02d:%02d / %s - %s",k / 3600,(k / 60) % 60,k % 60,
											Item[i].Artist,Item[i].Title);
									SetConsoleTitle(left(wtitle,80)); sleep(445);
								} while (FSOUND_IsPlaying(channel) || FSOUND_GetPaused(channel));
							}
							FSOUND_Stream_Close(stream);
						}
					}
				} else {
					FMUSIC_PlaySong(mod); h = -1;
					do {
						k = FMUSIC_GetTime(mod);
						if (k > h || k < 1000 || FMUSIC_GetPaused(mod)) {
							h = k; k = k / 1000;
						} else break;
						sprintf(wtitle,"%d:%02d:%02d / %s - %s",k / 3600,(k / 60) % 60,k % 60,
								Item[i].Artist,Item[i].Title);
						SetConsoleTitle(left(wtitle,80)); sleep(445);
					} while (FMUSIC_IsPlaying(mod));
					FMUSIC_FreeSong(mod);
				}
				if (Item[i].Que > 1) {
					Item[i].Que = 0;
					if (i == CurLine) {
						ScreenGetCursor(&cx,&cy); ocf = color_fg; ocb = color_bg;
						if (Item[i].Que) ShowLine(i - TopItem + 3,QuedColor,BackColor,i,0);
						else ShowLine(i - TopItem + 3,NormColor,BackColor,i,0);
						locate(cx,cy); color(color_fg,HighLight >> 4);

						switch(CurField) {
						case 0:
							if (cy == 1) printf("%-5s",Item[i].Class); break;
						case 1:
							if (cy == 7) printf("%-12s",Item[i].VolLabel); break;
						case 2:
							if (cy == 20) printf("%-60s",Item[i].Title); break;
						}
						locate(cx,cy); color(ocf,ocb);
					} else {
						if (i >= TopItem && i < TopItem + 19) {
							ScreenGetCursor(&cx,&cy); ocf = color_fg; ocb = color_bg;
							if (Item[i].Que) ShowLine(i - TopItem + 3,QuedColor,BackColor,i,0);
							else ShowLine(i - TopItem + 3,NormColor,BackColor,i,0);
							locate(cx,cy); color(ocf,ocb);
						}
					}
				}
		}
	} while (CurHiPlay < HiPlay);
	FSOUND_Close(); CurPlay = 0; HiPlay = 2;
	return 0;
}

void ShowList(int c,int t) {
	int l = 3, i, j = 0;

	cls(); color(HeadColor,BackColor);
	cputs("Type  Volume       Title"); sfs = 0;
	for (i = 0; i < c; i++) if (Item[i].Que == 1) {j++; sfs += Item[i].Size;}
	locate(1,58); color(DetlColor,BackColor); printf("%10d  %5d %5d",sfs,j,c);
	color(LineColor,BackColor); cputs(string(80,205));
	locate(22,1); cputs(string(80,205));
	if (t + 19 < c) j = t + 19; else j = c;
	for (i = t; i < j; i++)
		if (Item[i].Que == 1) ShowLine(l++,QuedColor,BackColor,i,0);
		else if (Item[i].Que == 0) ShowLine(l++,NormColor,BackColor,i,0);
		else ShowLine(l++,PlayColor,BackColor,i,0);
}

int GetAtField(int t,int n,int f) {
	int x, r, c, l;
	unsigned char *z;

	switch (f) {
	case 0:
		r = n - t + 3; c = 1; l = 5; z = Item[n].Class; break;
	case 1:
		r = n - t + 3; c = 7; l = 12; z = Item[n].VolLabel; break;
	case 2:
		r = n - t + 3; c = 20; l = 60; z = Item[n].Title; break;
	case 3:
		r = 23; c = 14; l = 66; z = Item[n].Artist; break;
	case 4:
		r = 24; c = 14; l = 66; z = Item[n].FileName; break;
	case 5:
		r = 25; c = 14; l = 66; z = Item[n].Path; break;
	}

	if (Item[n].Que == 1) x = Accept(r,c,-HighLight - QuedColor,"",l,Val1,z);
	else if (Item[n].Que == 0) x = Accept(r,c,-HighLight - NormColor,"",l,Val1,z);
	else x = Accept(r,c,-HighLight - PlayColor,"",l,Val1,z);

	if (Item[n].Que == 1) color(QuedColor,BackColor);
	else if (Item[n].Que == 0) color(NormColor,BackColor);
	else color(PlayColor,BackColor);

	locate(r,c); printf("%s%s",z,space(l - strlen(z)));
	return x;
}

int AddFiles(int n) {
	SYSTEMTIME st;
	DWORD attr;
	LPDWORD vsn, vmcl, vsf;
	WIN32_FIND_DATA FindData;
	HANDLE FileHandle;
	int i = 0, j, k;

	strcpy(ts,"x:\\path\\file*.ext");
	if (Accept(1,1,AttnColor,"Path\\File: ",68,Val1,ts) != 1) return 0;
	if (strlen(ts) < 2) return 0;
	if (ts[1] != ':') return 0;

	GetVolumeInformation(left(ts,2),t1,255,&vsn,&vmcl,&vsf,t2,255);
	FileHandle = FindFirstFile(ts,&FindData);
	if (FileHandle != INVALID_HANDLE_VALUE) do {
		attr = FindData.dwFileAttributes;
		if (attr & 0x21) {
			strcpy(Item[n].Path,left(ts,LastInstr(ts,"\\")));
			strcpy(Item[n].FileName,left(FindData.cFileName,66));
			FileTimeToSystemTime(&FindData.ftLastWriteTime,&st);
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,Item[n].FDate,255);
			Item[n].Size = FindData.nFileSizeLow;
			if (j = instr(FindData.cFileName," - ")) {
				strcpy(Item[n].Artist,left(left(FindData.cFileName,j - 1),68));
				if (k = LastInstr(FindData.cFileName,".")) {
					strcpy(Item[n].Class,mid(FindData.cFileName,k + 1,5));
					strcpy(Item[n].Title,left(mid(FindData.cFileName,j + 3,k - j - 3),60));
				} else strcpy(Item[n].Title,left(mid(FindData.cFileName,j + 3),60));
			}
			strcpy(Item[n].VolLabel,left(t1,12)); n++; i++;
			locate(1,76); color(DetlColor,BackColor); printf("%5d",i);
		}
	} while (FindNextFile(FileHandle,&FindData) != 0 && n < MaxRecords);
	FindClose(FileHandle); return i;
}

int main(int argc,char *argv[]) {
	static LPDWORD PlayThread;
	HANDLE ThreadHandle;
	int i, j;
	unsigned char kp;
	FILE *fp;

	SetConsoleTitle("Master Music Program");
	TopItem = 0; CurLine = 0; CurField = 2; CurSort = 0; CurPlay = 0; HiPlay = 2;
	NumberItems = 0; channel = -1;
	fp = fopen("program.dat","rb");
	if (fp != NULL) {
		printf("loading items\n");
		fread(&NumberItems,sizeof(NumberItems),1,fp);
		fread(&Item,sizeof(Item[0]),NumberItems,fp);
		fclose(fp);
	} else {
		Item[0].Class[0] = 0;
		NumberItems = 0;
	}

	cls(); if (ConsoleScreen(25,80) == FALSE) return 99; /* set screen size */
	color(NormColor,BackColor); cls(); ShowList(NumberItems,TopItem);

	do {
		if (Item[CurLine].Que == 1) ShowLine(CurLine - TopItem + 3,QuedColor,BackColor,CurLine,1);
		else if (Item[CurLine].Que == 0) ShowLine(CurLine - TopItem + 3,NormColor,BackColor,CurLine,1);
		else ShowLine(CurLine - TopItem + 3,PlayColor,BackColor,CurLine,1);

		if (CurField == 5) {
			strcpy(OldLabel,Item[CurLine].VolLabel);
			strcpy(OldPath,Item[CurLine].Path);
		}

		kp = GetAtField(TopItem,CurLine,CurField);
		switch (kp) {
		case 1: /* change all items that have same volume label and path */
			if (CurField == 5)
				for (i = 0; i < NumberItems; i++)
					if (strcmp(Item[i].VolLabel,OldLabel) == 0)
						if (strcmp(Item[i].Path,OldPath) == 0)
							strcpy(Item[i].Path,Item[CurLine].Path);
			break;
		case 2:
			if (CurLine > 0) CurLine--;
			if (CurLine < TopItem) {
				TopItem = CurLine;
				ScrollConsoleVertical(3,1,21,80,-1);
			}
			break;
		case 3:
			if (CurLine < NumberItems - 1) CurLine++;
			if (CurLine - TopItem > 18) {
				TopItem = CurLine - 18;
				ScrollConsoleVertical(3,1,21,80,1);
			}
			break;
		case 4:
			if (CurLine == TopItem) {
				CurLine -= 19;
				if (CurLine < 0) CurLine = 0;
				TopItem = CurLine;
				ShowList(NumberItems,TopItem);
			} else {
				CurLine = TopItem;
			}
			break;
		case 5:
			if (CurLine == TopItem + 18) {
				CurLine += 19;
				if (CurLine >= NumberItems) CurLine = NumberItems - 1;
				TopItem = CurLine - 18;
				if (TopItem < 0) TopItem = 0;
				ShowList(NumberItems,TopItem);
			} else {
				CurLine = TopItem + 18;
				if (CurLine >= NumberItems) CurLine = NumberItems - 1;
			}
			break;
		case 6:  /*  Left arrow pressed */
			if (CurField > 0) CurField--; break;
		case 7:  /* Right arrow pressed */
			if (CurField < 5) CurField++; break;
		case 59:  /* F1 - Select current record */
			if (Item[CurLine].Que) Item[CurLine].Que = 0; else Item[CurLine].Que = 1;
			j = 0; sfs = 0;
			for (i = 0; i < NumberItems; i++) if (Item[i].Que == 1) {j++; sfs += Item[i].Size;}
			locate(1,58); color(DetlColor,BackColor); printf("%10d  %5d %5d",sfs,j,NumberItems);
			break;
		case 94:  /* Ctrl-F1 - Reset qued tracks */
			for (i = 0; i < NumberItems; i++) if (Item[i].Que == 1) Item[i].Que = 0;
			ShowList(NumberItems,TopItem);
			break;
		case 104: /* Alt-F1 - Select all tracks from current VolLabel and Path */
			for (i = 0; i < NumberItems; i++)
				if (strcmp(Item[i].VolLabel,Item[CurLine].VolLabel) == 0)
					if (strcmp(Item[i].Path,Item[CurLine].Path) == 0) Item[i].Que = 1;
			ShowList(NumberItems,TopItem);
			break;
		case 60:  /* F2 - Add tracks to play list */
			j = HiPlay; /* Get highest play number */
			for (i = 0; i < NumberItems; i++) if (Item[i].Que == 1) Item[i].Que = j++;
			HiPlay = j; /* Update highest play number */
			if (CurPlay == 0) {
				ThreadHandle = CreateThread(NULL,0,PlayQuedTracks,0,0,&PlayThread);
				if (ThreadHandle != NULL) SetThreadPriority(ThreadHandle,THREAD_PRIORITY_ABOVE_NORMAL);
			}
			ShowList(NumberItems,TopItem);
			break;
		case 95:  /* Ctrl-F2 - Play next track */
			if (FSOUND_IsPlaying(channel)) FSOUND_Stream_Stop(stream);
			if (FMUSIC_IsPlaying(mod)) FMUSIC_StopSong(mod);
			break;
		case 61:  /* F3 - Pause or UnPause playing track */
			if (channel != -1) if (!FSOUND_SetPaused(channel,!FSOUND_GetPaused(channel))) channel = -1;
			if (mod != NULL) if (!FMUSIC_SetPaused(mod,!FMUSIC_GetPaused(mod))) mod = NULL;
			break;
		case 62:  /* F4 - Stop playing all tracks */
			for (i = 0; i < NumberItems; i++) if (Item[i].Que > 1) Item[i].Que = 0;
			if (FSOUND_IsPlaying(channel)) FSOUND_Stream_Stop(stream);
			if (FMUSIC_IsPlaying(mod)) FMUSIC_StopSong(mod);
			ShowList(NumberItems,TopItem);
			break;
		case 97: /* Ctrl-F4 - Copy qued tracks to destination path */
			j = CurSort; CurSort = 1;
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ts[0] = 0;
			if (Accept(1,1,AttnColor,"Destination Path: ",61,Val1,ts) == 1) {
				if (strcmp(right(ts,1),"\\")) sprintf(ts,"%s\\",ts);
				for (i = 0; i < NumberItems; i++) {
					if (Item[i].Que == 1) {
						sprintf(t1,"%s%s",Item[i].Path,Item[i].FileName);
						sprintf(t2,"%s%s",ts,Item[i].FileName);
						if (CopyFile(t1,t2,FALSE)) Item[i].Que = 0;
					}
				}
			}
			CurSort = j;
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ShowList(NumberItems,TopItem);
			break;
		case 107: /* Alt-F4 - Copy qued tracks to destination path and rebuild filename */
			j = CurSort; CurSort = 1;
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ts[0] = 0;
			if (Accept(1,1,AttnColor,"Destination Path: ",61,Val1,ts) == 1) {
				if (strcmp(right(ts,1),"\\")) sprintf(ts,"%s\\",ts);
				for (i = 0; i < NumberItems; i++) {
					if (Item[i].Que == 1) {
						sprintf(t1,"%s%s",Item[i].Path,Item[i].FileName);
						sprintf(t2,"%s%s - %s%s",ts,Item[i].Artist,Item[i].Title,
								mid(Item[i].FileName,LastInstr(Item[i].FileName,"."),4));
						if (CopyFile(t1,t2,FALSE)) Item[i].Que = 0;
					}
				}
			}
			CurSort = j;
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ShowList(NumberItems,TopItem);
			break;
		case 106:  /* Alt-F3 - Search in current field */
			if (Accept(1,1,AttnColor,"Search: ",66,Val1,lss) == 1) {
				j = 0; strcpy(lss,ucase(lss));
				for (i = CurLine + 1; i < NumberItems; i++) {
					switch (CurField) {
					case 0:
						if (instr(ucase(Item[i].Class),lss)) j = i;
						break;
					case 1:
						if (instr(ucase(Item[i].VolLabel),lss)) j = i;
						break;
					case 2:
						if (instr(ucase(Item[i].Title),lss)) j = i;
						break;
					case 3:
						if (instr(ucase(Item[i].Artist),lss)) j = i;
						break;
					case 4:
						if (instr(ucase(Item[i].FileName),lss)) j = i;
						break;
					case 5:
						if (instr(ucase(Item[i].Path),lss)) j = i;
						break;
					}
					if (j > 0) {CurLine = j; break;}
				}
				if (CurLine > TopItem + 18) TopItem = CurLine - 18;
			}
			ShowList(NumberItems,TopItem);
			break;
		case 63:  /* F5 - Sort on Artist, Title  */
			locate(1,1); color(AttnColor,BackColor); CurSort = 0;
			cputs("Sorting on Artist, Title.  Please wait.          ");
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ShowList(NumberItems,TopItem);
			break;
		case 98:  /* Ctrl-F5 - Sort on Type, Artist, Title */
			locate(1,1); color(AttnColor,BackColor); CurSort = 4;
			cputs("Sorting on Type, Artist, Title.  Please wait.  ");
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ShowList(NumberItems,TopItem);
			break;
		case 64:  /* F6 - Sort on Volume, Artist, Title  */
			locate(1,1); color(AttnColor,BackColor); CurSort = 1;
			cputs("Sorting on Volume, Artist, Title.  Please wait.  ");
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ShowList(NumberItems,TopItem);
			break;
		case 65:  /* F7 - Sort on Title, Artist  */
			locate(1,1); color(AttnColor,BackColor); CurSort = 2;
			cputs("Sorting on Title, Artist.  Please wait.          ");
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ShowList(NumberItems,TopItem);
			break;
		case 66:  /* F8 - Sort on Path, Filename  */
			locate(1,1); color(AttnColor,BackColor); CurSort = 3;
			cputs("Sorting on Path, Filename.  Please wait.         ");
			qsort((void *)Item,NumberItems,sizeof(Item[0]),StrCompare);
			ShowList(NumberItems,TopItem);
			break;
		case 67:  /* F9 - Copy record above */
			if (CurLine == 0) break;
			switch (CurField) {
			case 0:
				strcpy(Item[CurLine].Class,Item[CurLine - 1].Class);
				break;
			case 1:
				strcpy(Item[CurLine].VolLabel,Item[CurLine - 1].VolLabel);
				break;
			case 2:
				strcpy(Item[CurLine].Title,Item[CurLine - 1].Title);
				break;
			case 3:
				strcpy(Item[CurLine].Artist,Item[CurLine - 1].Artist);
				break;
			case 4:
				strcpy(Item[CurLine].FileName,Item[CurLine - 1].FileName);
				break;
			case 5:
				strcpy(Item[CurLine].Path,Item[CurLine - 1].Path);
				break;
			}
			break;
		case 117: /* Ctrl-End - Go to bottom of list */
			CurLine = NumberItems - 1; TopItem = CurLine - 18;
			if (TopItem < 0) TopItem = 0;
			if (CurLine < 0) CurLine = 0;
			ShowList(NumberItems,TopItem);
			break;
		case 119: /* Ctrl-Home - Go to top of list */
			CurLine = 0; TopItem = 0;
			ShowList(NumberItems,TopItem);
			break;
		case 134: /* F12 - Scan and add new items */
			i = AddFiles(NumberItems);
			locate(1,1); printf("%4d  Files added%s",i,string(63,32));
			Sleep(2000); NumberItems += i;
			ShowList(NumberItems,TopItem);
			break;
		case 138: /* Ctrl-F12 - Delete current item */
			if (CurLine == NumberItems - 1) {
				if (CurLine) CurLine--;
				if (CurLine < TopItem) TopItem = CurLine;
			} else {
				for (i = CurLine; i < NumberItems - 1; i++) Item[i] = Item[i + 1];
			}
			if (NumberItems > 1) NumberItems--;  /* Do Not Delete Last Item */
			ShowList(NumberItems,TopItem);
			break;
		}

		locate(1,52); color(DetlColor,BackColor); printf("%3d",kp); /* temp line */
	} while (kp != 0);

	/* Exit program and save all settings */
	color(NormColor,BackColor); cls(); printf("Exiting Program\n");
	for (i = 0; i < NumberItems; i++) if (Item[i].Que > 1) Item[i].Que = 0;

	fp = fopen("program.dat","wb");
	if (fp != NULL) {
		printf("saving items\n");
		fwrite(&NumberItems, sizeof(NumberItems),1,fp);
		fwrite(&Item,sizeof(Item[0]),NumberItems,fp);
		fclose(fp);
	} return 0;
}
