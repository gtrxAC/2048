/* Link header for application - AppBuilder 2.03  */

extern ApContext_t AbContext;

ApWindowLink_t GmWindow = {
	"GmWindow.wgtw",
	&AbContext,
	AbLinks_GmWindow, 0, 11
	};

ApDialogLink_t GmDialogAbout = {
	"GmDialogAbout.wgtd",
	&AbContext,
	AbLinks_GmDialogAbout, 6, 2
	};

static ApItem_t ApItems_GmMenuGame[ 4 ] = {
	{ 1, 1, 0,"ESC", 0, "GmMenuItemReset", "Reset", NULL },
	{ 1, 16, 0, NULL, 4, "", "", NULL },
	{ 1, 1, 4,"X", 0, "GmMenuItemQuit", "Quit", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t GmMenuGame = {
	"GmMenuGame",
	"",
	NULL,
	NULL,
	2,
	ApItems_GmMenuGame,
	& AbContext,
	AbLinks_GmMenuGame,
	9, 2, 3
	};

static ApItem_t ApItems_GmMenuView[ 4 ] = {
	{ 1, 4, 0, NULL, 0, "GmMenuItemBackground", "Background", NULL },
	{ 1, 4, 0, NULL, 0, "GmMenuItemTiles", "Round Tiles", NULL },
	{ 1, 4, 0, NULL, 0, "GmMenuItemFonts", "Smooth Fonts", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t GmMenuView = {
	"GmMenuView",
	"",
	NULL,
	NULL,
	2,
	ApItems_GmMenuView,
	& AbContext,
	AbLinks_GmMenuView,
	13, 3, 3
	};

static const char ab_GmMenuItemAbout_imgd[ 14 ][ 64 ] = {
		{ 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 199, 227, 255, 0, 176, 200, 255, 0, 176, 200, 255, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 186, 212, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 199, 227, 255, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 38, 178, 201, 255, 46, 179, 202, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 176, 200, 255, 0, 176, 200, 255, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 38, 178, 201, 255, 188, 220, 230, 255, 200, 226, 234, 255, 86, 185, 206, 255, 0, 176, 200, 255, 0, 176, 200, 255, 204, 228, 235, 255, 255, 255, 255, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 199, 227, 255, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 206, 229, 236, 255, 192, 222, 231, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 135, 199, 215, 255, 223, 238, 242, 255, 255, 255, 255, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 69, 182, 204, 255, 255, 255, 255, 255, 205, 229, 236, 255, 214, 233, 239, 255, 152, 205, 219, 255, 46, 179, 202, 255, 223, 238, 242, 255, 130, 197, 214, 255, 255, 255, 255, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 88, 186, 206, 255, 255, 255, 255, 255, 64, 181, 203, 255, 144, 202, 217, 255, 246, 250, 251, 255, 155, 206, 220, 255, 219, 236, 241, 255, 213, 232, 238, 255, 255, 255, 255, 255, 180, 217, 227, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 193, 223, 231, 255, 215, 234, 239, 255, 220, 236, 241, 255, 182, 218, 228, 255, 0, 176, 200, 255, 0, 176, 200, 255, 174, 214, 225, 255, 255, 255, 255, 255, 131, 198, 214, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 176, 200, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 186, 212, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 135, 154, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 199, 227, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 119, 135, 255, 0, 0, 0, 0, 0, 0, 0, 0, },
		};
static const PhImage_t ab_GmMenuItemAbout_img = {
		32, // type: image type ( multi, pseudo, direct )
		     // bitmap type (backfilled, transparent fill)
		0x9CA67F34, // image_tag: image data tag
		      64, //bpl:  image bytes per line
		{ 14, 14 }, // size:  image size
		0x0, // palette_tag:  palette data tag
		0, // colors:  number of colors in image
		NULL, // image alpha map used if src alpha map option enabled 
		0, // transparent
		0, // format:  graphic format
		0, // flags:  image flags
		0,  // ghost_bpl:  bpl for ghosting bitmap ghost_bitmap
		0,  // spare1:  reserved
		NULL, // ghost_bitmap: transparency bitmap for ghosting a image
		0, // mask_bpl:  bytes per line for transparency mask
		NULL, // mask_bm: pointer to transparency mask bitmap
		NULL, // palette:  image palette
		(char*) ab_GmMenuItemAbout_imgd //image:  image bitmap data
		};

static ApItem_t ApItems_GmMenuHelp[ 2 ] = {
	{ 1, 1, 0, NULL, 0, "GmMenuItemAbout", "About 2048-Photon...", &ab_GmMenuItemAbout_img },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t GmMenuHelp = {
	"GmMenuHelp",
	"",
	NULL,
	NULL,
	2,
	ApItems_GmMenuHelp,
	& AbContext,
	AbLinks_GmMenuHelp,
	17, 1, 1
	};

