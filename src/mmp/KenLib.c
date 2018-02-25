/*----------------------------------------------------------------------------
 Accept is a generic input routine to accept input from the user

  Parameters: Row, Column, Color, Prompt, Length of return, Validate,
              Return string

   Set length of return to negative to hide input
   Set color to negative to prevent altering default on ESC
     color contains forground and background (bg * 16 + fg)
   Set return string to provide default
   Return Codes:         0: ESC
                         1: Enter
                         2: Up arrow
                         3: Down arrow
                         4: Page up
                         5: Page down
                   59 - 68:      Function Keys 1 - 10
                 104 - 113: Alt  Function Keys 1 - 10
                  94 - 103: Ctrl Function Keys 1 - 10

 Example: z = Accept(5,30,-14,"Prompt -> ",6,"ABCwz#-+{}0123456789",ret)
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Procedure:     RunExe ID:1
 Purpose:       Execute a program in another thread.
                This function is generally portalbe over Win32.
 Input:         szApp: The string can specify the full path and filename
                of the module to execute. The string can specify a
                partial name. In that case, the function uses the current
                drive and current directory to complete the specification.
 Output:        Run program.
 Errors:        Return function exit code, return false if error.
----------------------------------------------------------------------------*/

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

extern float Abs(float);
extern int Asc(char *,int = 0);
extern int instr(char *,char *,int = 0);
extern int LastInstr(char *,char *,int = 0);
extern char *space(int);
extern char *ucase(char *);
extern char *lcase(char *);
extern char *mid(char *,int,int = 1000000);
extern char *left(char *,int);
extern char *right (char *,int);
extern char *string(int,int);
extern void cls(void);
extern void color(int,int);
extern void locate(int,int);
extern int exist(char *);
extern BOOL ConsoleScreen(int,int);
extern void ScreenGetCursor(int *,int *);
extern BOOL ScrollConsoleVertical(short,short,short,short,short);
extern int Accept(int,int,int,char *,int,char *,char *);

static COORD cursor;
static HANDLE hConsole;
static int StrCnt,color_fg = 7,color_bg = 0;
static CONSOLE_SCREEN_BUFFER_INFO binfo;
static LPSTR StrFunc[64];

char *ucase(char *S) {
	if (++StrCnt == 64) StrCnt = 0;
	if (StrFunc[StrCnt]) free(StrFunc[StrCnt]);
	StrFunc[StrCnt] = (char *)calloc(strlen(S) + 256,sizeof(char));

	return strupr(strcpy(StrFunc[StrCnt],S));
}

char *lcase(char *S) {
	if (++StrCnt == 64) StrCnt = 0;
	if (StrFunc[StrCnt]) free(StrFunc[StrCnt]);
	StrFunc[StrCnt] = (char *)calloc(strlen(S) + 256,sizeof(char));

	return strlwr(strcpy(StrFunc[StrCnt],S));
}

extern char *left(char *S,int length) {
    if (++StrCnt == 64) StrCnt = 0;
    if (StrFunc[StrCnt]) free(StrFunc[StrCnt]);
    StrFunc[StrCnt] = (char *)malloc(strlen(S) + 256);
    memset (StrFunc[StrCnt],0,strlen(S) + 256);

    strncpy (StrFunc[StrCnt],&S[0],length);
    StrFunc[StrCnt][length] = 0;
    return StrFunc[StrCnt];
}

extern char *mid(char *S,int start,int length) {
    if (++StrCnt == 64) StrCnt = 0;
    if (StrFunc[StrCnt]) free(StrFunc[StrCnt]);
    StrFunc[StrCnt] = (char*)malloc(strlen(S) + length + 256);

    memset(StrFunc[StrCnt],0,strlen(S) + length + 256);
    if(start > strlen(S))
        {StrFunc[StrCnt][0] = 0;}
    else
        {strncpy(StrFunc[StrCnt],&S[start - 1],length);
    }
    StrFunc[StrCnt][length] = 0;
    return StrFunc[StrCnt];
}

char *right (char *S,int length) {
	int start;
	if (++StrCnt == 64) StrCnt = 0;
	if (StrFunc[StrCnt]) free (StrFunc[StrCnt]);
	StrFunc[StrCnt]=(char*)calloc(strlen(S) + 256,sizeof(char));
	start = strlen(S) - length;
	if (start<0) start = 0;
	strncpy (StrFunc[StrCnt],&S[start],length);
	StrFunc[StrCnt][length] = 0;
	return StrFunc[StrCnt];
}

extern char *string (int count, int a) {
    if (++StrCnt == 64) StrCnt = 0;
    if (StrFunc[StrCnt]) free (StrFunc[StrCnt]);
    StrFunc[StrCnt] = (char*)malloc(2048);
    memset (StrFunc[StrCnt],0,2048);
    memset (StrFunc[StrCnt],a,count);
    return StrFunc[StrCnt];
}

extern char *space(int a) {return string(a,32);}

extern int instr(char *mane,char *match,int offset) {
    char *s1, *s2;
    int c;

    if (!*match) return 0;
    if (offset > strlen(mane)) return 0;
    if (offset < 0) offset = 0;
    c = offset;
    while (c > 0) {mane++; c--;}
    while (*mane) {
        s1 = mane;
        s2 = (char*)match;
        while(*s1 && *s2 && !(*s1 - *s2)) s1++,s2++;
        if (!*s2) break;
        c++; mane++;
    }
    offset += c + 1;
    if (!*mane || offset < 0) return 0;
    return offset;
}

extern int LastInstr(char *f,char *s,int o) {
	int x;

	o = instr(f,s,o); x = o;
	if (o) do {
		o = instr(f,s,o);
		if (o) x = o;
	} while (o);
	return x;
}

extern float Abs(float a) {if (a < 0) {return -a;} else {return a;}}

extern int Asc(char *z,int index) {
    int q = 0;

    memmove(&q, z + index,1);
    return q;
}

extern void locate(int row,int col) {
    cursor.X = col - 1;
    cursor.Y = row - 1;
    SetConsoleCursorPosition(hConsole,cursor);
}

extern void cls(void) {
    DWORD ret;
    int attr, nc;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole,&binfo);
    cursor.X = 0;
    cursor.Y = 0;
    nc = binfo.dwSize.X * binfo.dwSize.Y;
    FillConsoleOutputCharacter(hConsole,32,nc,cursor,&ret);
    attr = color_fg + color_bg * 16;
    FillConsoleOutputAttribute(hConsole,attr,nc,cursor,&ret);
    locate(1,1);
}

extern void color(int fg,int bg) {
    SetConsoleTextAttribute(hConsole,fg + bg * 16);
    color_fg = fg; color_bg = bg;
}

extern BOOL ScrollConsoleVertical(short tr,short tc,short br,short bc,short n) {
    SMALL_RECT w1, w2;
	COORD d;
	CHAR_INFO ci;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	ci.Attributes = 0;

	d.Y  = (int)tr - 1;
	d.X  = (int)tc - 1;
	w1.Left   = tc - 1;
	w1.Top    = tr - 1;
	w1.Right  = bc - 1;
	w1.Bottom = br - 1;
	w2 = w1;

	if (n > 0) w1.Top    += n;
	if (n < 0) {w1.Bottom += n; d.Y -= (int)n;}
	return ScrollConsoleScreenBuffer(hConsole,&w1,&w2,d,&ci);
}

extern BOOL RunExe(LPCTSTR lpApp,LPTSTR cla) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si,sizeof(si));
    ZeroMemory(&pi,sizeof(pi));
    si.cb = sizeof(si);
    return CreateProcess(lpApp,cla,NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE +
						 DETACHED_PROCESS,NULL,NULL,&si,&pi);
}

extern BOOL exist(char *FileName) {
    WIN32_FIND_DATA FindData;
    HANDLE rc;
    rc = FindFirstFile(FileName,&FindData);
    if(rc == INVALID_HANDLE_VALUE) return FALSE; else return TRUE;
}

extern BOOL ConsoleScreen(int row,int col) {
    cursor.X = col;
    cursor.Y = row;
    return SetConsoleScreenBufferSize(hConsole,cursor);
}

extern void ScreenGetCursor(int *row,int *col) {
    GetConsoleScreenBufferInfo(hConsole,&binfo);
    *col = binfo.dwCursorPosition.X + 1;
    *row = binfo.dwCursorPosition.Y + 1;
}

extern int Accept(int row,int col,int clr,char *prompt,int length,char *validate,char *rts) {
    static unsigned char orig[82],k;
    int hide,s,cp,ep,ins,rcd,cx,cy;
	CONSOLE_CURSOR_INFO cci,cco;

    if (length < 0) {length = -length; hide = 1; rts[0] = 0;} else {hide = 0;}

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole,&binfo);
	if (col < 1 || row < 1) return 0;
	if (col + strlen(prompt) + length > 80) return 0;
	if (row > binfo.dwSize.Y) return 0;

    sprintf(orig,"%s",left(rts,length)); sprintf(rts,"%s",orig);

    ScreenGetCursor(&cx,&cy); locate(row,col);
	GetConsoleCursorInfo(hConsole,&cci);
	cco = cci; cci.dwSize = 90; cci.bVisible = TRUE;
	SetConsoleCursorInfo(hConsole,&cci);
    printf("%s",prompt); color((int)Abs(clr) & 15,((int)Abs(clr) >> 4) & 15);
    printf("%s%s",rts,space(length - strlen(rts)));
    s = 1; cp = col + strlen(prompt); ep = cp; rcd = 1; ins = 0;

    do {
        locate(row,cp);
        k = getch();
        if (k == 224 || k == 0) {      /* A function type key was pressed */
            k = getch();
            if (hide == 0) {
                switch(k) {
                case 71:               /* Home key pressed        */
                    cp = ep; k = 128; break;
                case 79:               /* End key pressed         */
                    cp = ep + strlen(rts); k = 128; break;
                case 75:               /* Left arrow key pressed  */
					if (cp == ep) {rcd = 6; k = 13; break;}
                    if (cp > ep) cp--; k = 128; break;
                case 77:               /* Right arrow key pressed */
					if (cp == ep && s != 0) {rcd = 7; k = 13; break;}
                    if (cp < (ep + strlen(rts))) {
						cp++; k = 128;
					} else {
						rcd = 7; k = 13;
					} break;
                case 72:               /* Up arrow key pressed    */
                    rcd = 2; k = 13; break;
                case 80:               /* Down arrow key pressed  */
                    rcd = 3; k = 13; break;
                case 73:               /* Page Up key pressed     */
                    rcd = 4; k = 13; break;
                case 81:               /* Page Down key pressed   */
                    rcd = 5; k = 13; break;
                case 82:               /* Insert key pressed      */
                    ins = 1 - ins; k = 128;
					if (ins) cci.dwSize = 20; else cci.dwSize = 90;
					SetConsoleCursorInfo(hConsole,&cci);
					break;
                case 83:               /* Delete key pressed      */
                    if (s) {
                        cp = ep; rts[0] = 0; locate(row,cp);
                        printf("%s",space(length));
                    } else {
                        if ((cp - ep) < strlen(rts)) {
                            s = cp - ep;
                            sprintf(rts,"%s%s",left(rts,s),mid(rts,s + 2));
                            locate(row,ep);
                            printf("%s%s",rts,space(length - strlen(rts)));
                        }
                    }
                    k = 128; break;
                default:
                    rcd = k; k = 13; break;
                }
            } else {
                k = 128;
            }
            s = 0;
        }
        if (k == 8 && cp > ep) {        /* Backspace key pressed  */
            s = cp - ep; memcpy(rts + s - 1,rts + s,length - s + 1);
            cp--; s = 0;
            if (hide) {
                locate(row,cp); putch(32);
            } else {
                locate(row,ep); printf("%s%s",rts,space(length - strlen(rts)));
            }
        }
        if (k == 27) {rcd = 0; k = 13;} /* Escape key pressed    */
        if (instr(validate, &k)) {
            if (cp - ep < length) {
                if (s) {
                    rts[0] = 0; rts[1] = 0;
                    printf(space(length)); locate(row,cp);
                }
                s = cp - ep;
                if (s == strlen(rts)) {
                    rts[s++] = k; rts[s] = 0;
                } else {
                    if (ins) {
                        memcpy(rts + s + 1,rts + s,length - s + 1);
                        rts[s] = k; rts[length] = 0;
                        locate(row,ep); printf("%s",rts); locate(row,cp);
                    } else {
                        rts[s] = k;
                    }
                }
                s = 0; cp++;
                if (hide) {putch(42);} else {putch(k);}
            }
        }
    } while (k != 13);
    if (rcd == 0 && clr < 0) sprintf(rts,"%s",orig);
    locate(cx,cy); SetConsoleCursorInfo(hConsole,&cco);
    return rcd;
}
