// findfileres.h

#ifndef _FINDFILERES_H_
#define _FINDFILERES_H_

#define ID_LNK     21
//------------------------------------------
// Main dialog
//------------------------------------------
#define IDDIALOG_MAIN       100
#define IDDIALOG_CONFIRM    101
#define IDDIALOG_CONFIRMALL 102
#define IDDIALOG_MESBOX     103
#define IDDIALOG_SENDTO     104
#define IDDIALOG_EDITSENDTO 105
#define IDDIALOG_EXTENDED   106
#define IDDIALOG_ABOUT      107

#define IDMENU_MAIN      110
#define IDMENU_POPUP     111

#define IDB_SEARCH    1
#define IDB_CANCEL    2
#define IDB_RESET     130
#define IDB_CLEAR     131

#define IDS_PICBOX    132 // subclassed to paint images
#define IDS_BACK      133 // back panel
#define IDS_EXTBACK   134
#define IDS_FILE      135 // "File Search"
#define IDS_TSEARCH   136 // "Text Search"
#define IDS_DATE_AFTER 137
#define IDS_DATE_BEFORE 138
#define IDS_SIZE_MORE   139
#define IDS_SIZE_LESS   140
#define IDS_DATE_AFTER_TXT 141
#define IDS_DATE_BEFORE_TXT 142
#define IDS_SIZE_MORE_TXT  143
#define IDS_SIZE_LESS_TXT  144

#define IDCHK_ENABLE_EXT 145
#define IDCHK_CASE        146
#define IDCHK_SUBFOLDER   147
#define IDCHK_AUTOWILDC   148
#define IDCHK_OCCUR       149

#define IDC_COMBO      150
#define IDC_STATUSBAR  151
#define IDC_LISTVIEW   152

#define IDE_FILTER    153
#define IDE_SEARCH    154
#define IDE_RENAME    155
#define IDE_HEADER    156

//------------------------------------------
// Confirm Dialog
//------------------------------------------
#define IDS_TEXTCONFIRM   165
#define IDB_CONFIRMYES    166
#define IDB_CONFIRMCANCEL 167
#define IDB_CONFIRMYESALL 168
#define IDI_CONFIRMICONDEL 169
#define IDI_CONFIRMICONMOV 170

//------------------------------------------
// MessageBox Dialog
//------------------------------------------
#define IDS_MESSAGETEXT 180

//------------------------------------------
// Menu items
//------------------------------------------
#define IDM_BANNER      198
#define IDM_FOLDER      200
#define IDM_DEPENDS     201
#define IDM_OPEN        202
#define IDM_RENAME      203
#define IDM_COPYTO      204
#define IDM_MOVETO      205
#define IDM_DELETE      206
#define IDM_RECYCLE     207
#define IDM_PROPERTIES  208
#define IDM_EXIT        209
#define IDM_EDITSENDTO  210
#define IDM_EXTENDEDSEARCH 211
#define IDM_ABOUT       212
#define IDM_UP          213 // these three are not used
#define IDM_DOWN        214 // as menu items but ID's
#define IDM_NOTDEPENDS  215 // for loading bitmaps

#define IDM_VSEARCH     216
#define IDM_SENDTOFIRST 217
#define IDM_SENDTO      225

//------------------------------------------
// Choose SendTo Dialog
//------------------------------------------
#define IDE_1     301
#define IDE_2     302
#define IDE_3     303
#define IDE_4     304
#define IDE_5     305
#define IDE_6     306

#define IDB_ED1 311
#define IDB_ED2 312
#define IDB_ED3 313
#define IDB_ED4 314
#define IDB_ED5 315
#define IDB_ED6 316

#define IDB_PROG_1 321
#define IDB_PROG_2 322
#define IDB_PROG_3 323
#define IDB_PROG_4 324
#define IDB_PROG_5 325
#define IDB_PROG_6 326

#define IDB_FOLD_1 327
#define IDE_DISP   328

//------------------------------------------
// Extended Pararmeters Dialog
//------------------------------------------
#define IDE_SIZE_GREATER    351
#define IDE_SIZE_LESS       352
#define IDC_DAY_BEFORE      353
#define IDC_MONTH_BEFORE    354
#define IDC_YEAR_BEFORE     355
#define IDC_DAY_AFTER       356
#define IDC_MONTH_AFTER     357
#define IDC_YEAR_AFTER      358

//------------------------------------------
// About Dlg
//------------------------------------------
#define IDS_VERSION    5000

#endif // _FINDFILERES_H_
