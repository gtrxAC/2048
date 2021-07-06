#include <Types.h>
#include <Locator.h>
#include <Memory.h>
#include <Misctool.h>
#include <QuickDraw.h>
#include <Event.h>
#include <Window.h>
#include <Control.h>
#include <Menu.h>
#include <Lineedit.h>
#include <Dialog.h>
#include <Desk.h>

#include "Src.2048.h"
#include "Src.2048.c"

#define WINDOW_HEIGHT     160
#define WINDOW_WIDTH      280

#define MENU_APPLE_ABOUT  257
#define MENU_GAME_RESET   258
#define MENU_GAME_QUIT    259
#define MENU_TILES_RECT   260
#define MENU_TILES_ROUND  261

#define MODE mode640
#define MaxX 640
#define dpAttr attrLocked+attrFixed+attrBank

static char *gAppleMenu = ">>@\\XN1\r--About 2048-IIGS...\\N257\r---\\N512D\r.";
static char *gGameMenu  = ">>  Game  \\N2\r--Reset\\N258*Rr\r---\\N513D\r--Quit\\N259*Qq\r.";
static char *gTilesMenu = ">>  Tiles  \\N3\r--Rectangle\\N260\r--Rounded\\N261\r.";

static ParamList gWindowParams = {
	sizeof(ParamList),
	fQContent + fMove + fClose + fTitle, /* 0xC0C0 */
	"\p2048-IIGS",
	0L,
	0, 0, 0, 0,
	NULL,
	0, 0, WINDOW_HEIGHT, WINDOW_WIDTH,
	WINDOW_HEIGHT, WINDOW_WIDTH,
	0, 0, 0, 0, 0L, 0,
	NULL, NULL, NULL,
	32, 32, 32 + WINDOW_HEIGHT, 32 + WINDOW_WIDTH,
	(GrafPortPtr) topMost, /* -1L */
	NULL
};

int myID;
Handle zp;
int ToolTable[] = {
	7,
	4, 0x0100, /* QiuckDraw */
	6, 0x0100, /* Event */
	14, 0x0100, /* Window */
	15, 0x0100, /* Control */
	16, 0x0100, /* Menu */
	20, 0x0100, /* Line Edit */
	21, 0x0100  /* Dialog */
};

StartTools() {
	TLStartUp();
	myID = MMStartUp();
	MTStartUp();
	LoadTools(ToolTable);
	ToolInit();
}

ToolInit() {
	zp = NewHandle(0x700L, myID, dpAttr, 0L);
	QDStartUp((int) *zp, MODE, 160, myID);
	EMStartUp((int) (*zp + 0x300), 20, 0, MaxX, 0, 200, myID);
	WindStartUp(myID);
	RefreshDesktop(NULL);
	CtlStartUp(myID, (int)  (*zp + 0x400));
	MenuStartUp(myID, (int) (*zp + 0x500));
	LEStartUp(myID, (int) (*zp + 0x600));
	DialogStartUp(myID);
	ShowCursor();
}

ShutDown() {
	GrafOff();
	DialogShutDown();
	LEShutDown();
	MenuShutDown();
	CtlShutDown();
	WindShutDown();
	EMShutDown();
	QDShutDown();
	MTShutDown();
	DisposeHandle(zp);
	MMShutDown(myID);
	TLShutDown();
}

BuildMenu() {
	InsertMenu(NewMenu(gTilesMenu), 0);
	InsertMenu(NewMenu(gGameMenu), 0);
	InsertMenu(NewMenu(gAppleMenu), 0);
	FixAppleMenu(1);
	FixMenuBar();
	DrawMenuBar();
	CheckMItem(true, MENU_TILES_RECT);
}

LocInfo picOLocInfo = {
	mode640,
	NULL,
	160,
	0, 0, 200, 640
};

Rect screenRect = { 0, 0, 200, 640 };
GrafPort picOPort;

#define IMAGE_ATTR (attrLocked + attrFixed + attrNoCross + attrNoSpec + attrPage)

PicOSetup() {
	GrafPortPtr thePortPtr;
	picOLocInfo.ptrToPixImage = *(NewHandle(0x8000L, myID, IMAGE_ATTR, NULL));
	thePortPtr = GetPort();
	OpenPort(&picOPort);
	SetPort(&picOPort);
	SetPortLoc(&picOLocInfo);
	ClipRect(&screenRect);
	EraseRect(&screenRect);
	SetPort(thePortPtr);
}

pascal void DrawContent() {
	PPToPort(&picOLocInfo, &(picOLocInfo.boundsRect), 0, 0, modeCopy);
}

GrafPortPtr winOPtr;

MakeWindow() {
	gWindowParams.wContDefProc = (ProcPtr) DrawContent;
	winOPtr = NewWindow(&gWindowParams);
	PicOSetup();
	ErasePicO();
	SelectWindow(winOPtr);
	ShowWindow(winOPtr);
}

Boolean done = false;

void DoAboutAlert() {
	static ItemTemplate lAlertButton = { 1, 52, 18, 0, 0, buttonItem, "\pOK", 0, 0, NULL };
	static ItemTemplate lAlertMessage = { 2, 4, 90, 90, 350, itemDisable + statText, NULL, 0, 0, NULL };
	static AlertTemplate lAlertRec = { 60, 150, 133, 485, 3, 0x80, 0x80, 0x80, 0x80, NULL, NULL, NULL };

	SetForeColor(0);
	SetBackColor(15);

	lAlertMessage.itemDescr = "\p            About 2048-IIGS\r\r   Port of \"2048\" puzzle game\r      for Apple IIGS platform\r\rVersion 1.0, 06-Jul-2021, (C) EXL\r   https://github.com/EXL/2048";
	lAlertRec.atItemList[0] = (ItemTempPtr) &lAlertButton;
	lAlertRec.atItemList[1] = (ItemTempPtr) &lAlertMessage;

	NoteAlert(&lAlertRec, NULL);
}

main() {
	e_init(0, 1, 2, 3, 4);
	StartTools();
	BuildMenu();
	MakeWindow();
	EventLoop();
	DisposeHandle(FindHandle(picOLocInfo.ptrToPixImage));
	ShutDown();
}

WmTaskRec myEvent;

EventLoop() {
	myEvent.wmTaskMask = 0x0FFF;
	while (!done)
		switch (TaskMaster(everyEvent, &myEvent)) {
			case wInMenuBar:
				DoMenus();
				break;
			case wInGoAway:
				HideWindow(winOPtr);
				break;
			case wInContent:
				Sketch();
				break;
		}
}

DoMenus() {
	Word *data = (Word *) &myEvent.wmTaskData;
	switch (*data) {
		case MENU_APPLE_ABOUT:
			DoAboutAlert();
			break;
		case MENU_GAME_RESET:
			e_key(0);
			ErasePicO();
			HideWindow(winOPtr);
			CloseWindow(winOPtr);
			winOPtr = NewWindow(&gWindowParams);
			SelectWindow(winOPtr);
			ShowWindow(winOPtr);
			break;
		case MENU_GAME_QUIT:
			done = true;
			break;
		case MENU_TILES_RECT:
			break;
		case MENU_TILES_ROUND:
			SelectWindow(winOPtr);
			ShowWindow(winOPtr);
			break;
	}
	HiliteMenu(false, *(data + 1));
}

ErasePicO() {
	GrafPortPtr oldPortPtr;
	oldPortPtr = GetPort();
	SetPort(&picOPort);
	ClipRect(&screenRect);
	EraseRect(&screenRect);
	SetPort(oldPortPtr);
}

Sketch() {
	int i;
	Point mouseLoc;
	GrafPortPtr thePortPtr = (GrafPortPtr) myEvent.wmTaskData;
	Rect theRect;

        	mouseLoc = myEvent.wmWhere;

	StartDrawing(thePortPtr);
	GetPortRect(&theRect);
	GlobalToLocal(&mouseLoc);

	MoveTo(mouseLoc);
	SetPort(&picOPort);
	ClipRect(&theRect);
	MoveTo(mouseLoc);
	SetPort(thePortPtr);

	for (i = 0; i < BOARD_SIZE; ++i)
		printf("%d ", e_board[i]);
	printf("\n");

	while (StillDown(0)) {
		GetMouse(&mouseLoc);

		LineTo(mouseLoc);
		SetPort(&picOPort);
		LineTo(mouseLoc);
		SetPort(thePortPtr);
	}
	SetOrigin(0, 0);
}
