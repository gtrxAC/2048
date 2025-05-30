#include "2048.h"

#include <cybiko.h>
#include <cywin.h>
#include <highscore.h>

#define STR_LEN_VALUE            (5)
#define STR_LEN_SCORE            (6)
#define STR_LEN_LABEL           (10)
#define STR_TIME_DATE           (18)
#define STR_SAVE                (46)

#define TILE_MARGIN              (3)
#define TILE_SIZE               (22)
#define X_END_FIELD             ((TILE_MARGIN * 4) + (TILE_SIZE * 4))
#define SCREEN_WIDTH_DIV_4      (SCREEN_WIDTH / 4)

typedef enum _ACTION_T {
	ACTION_EXIT,
	ACTION_RESET,
	ACTION_SAVE,
	ACTION_LOAD,
	ACTION_SCORE,
	ACTION_SCREENSHOT,
	ACTION_ABOUT
} ACTION_T;

typedef struct _GAME_SAVE_T {
	char date_time[STR_TIME_DATE]; /* 00:00:00 31-12-01 */
	int board[BOARD_SIZE];
	int score;
	int win;
	int lose;
} GAME_SAVE_T;

struct cMenuForm : public cCustomForm {
	struct cList *m_list;
	int m_index;
};

const char g_bmp_4bpp_head[] = {
	0x42, 0x4D, 0xB6, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00,
	0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x64, 0x00,
	0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x1F,
	0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xA3, 0xA3,
	0xA3, 0x00, 0x53, 0x53, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const char *menu_items[] = {
	"Exit",
	"Reset Game",
	"Save Game...",
	"Load Game...",
	"High Scores...",
	"Screenshot...",
	"About...",
};

struct module_t g_main_module;

static bool g_quit = FALSE;
static bool g_focus = FALSE;
static bool g_req_menu = FALSE;
static bool g_req_draw = FALSE;
static bool g_req_score = FALSE;
static bool g_req_screen_shot = FALSE;

static void cMessageBox_Info(const char *title, const char *text) {
	struct cDialog *p_dialog = (struct cDialog *) malloc(sizeof(struct cDialog));

	cDialog_ctor(p_dialog, title, text, mbOk | mbs1, 0, g_main_module.m_process);

	cDialog_ShowModal(p_dialog);

	cDialog_dtor(p_dialog, FREE_MEMORY);
}

static bool Action_Save(void) {
	int i;
	bool is_ok;
	char save_str[STR_SAVE];

	GAME_SAVE_T save;
	struct Time c_time;
	struct FileOutput *p_file_output;

	is_ok = FALSE;

	Time_decode(&c_time, time());
	memset(save.date_time, 0, STR_TIME_DATE);
	sprintf(
		save.date_time, "%02d:%02d:%02d %02d-%02d-%02d",
		c_time.hour, c_time.minute, c_time.second, c_time.day, c_time.month, c_time.year
	);
	for (i = 0; i < BOARD_SIZE; ++i)
		save.board[i] = e_board[i];
	save.score = e_score;
	save.win = e_win;
	save.lose = e_lose;

	p_file_output = (struct FileOutput *) malloc(sizeof(struct FileOutput));
	FileOutput_ctor_Ex(p_file_output, "2048-CyOS.sav", TRUE);
	if (FileOutput_is_good(p_file_output)) {
		FileOutput_write(p_file_output, &save, sizeof(GAME_SAVE_T));
		is_ok = TRUE;
	}
	FileOutput_dtor(p_file_output, FREE_MEMORY);

	if (is_ok) {
		sprintf(save_str, "\n%s\nOn: %s", "Saved '2048-CyOS.sav'", save.date_time);
		cMessageBox_Info("Game Saved...", save_str);
	} else {
		cMessageBox_Info("Error Saving Game...", "\nCannot save '2048-CyOS.sav' file!");
	}

	return is_ok;
}

static bool Action_Load(void) {
	int i;
	bool is_ok;
	char save_str[STR_SAVE];

	GAME_SAVE_T save;
	struct FileInput *p_file_input;

	is_ok = FALSE;

	p_file_input = (struct FileInput *) malloc(sizeof(struct FileInput));
	FileInput_ctor_Ex(p_file_input, "2048-CyOS.sav");
	if (FileInput_is_good(p_file_input)) {
		long file_size = FileInput_get_size(p_file_input);
		if (file_size > 0) {
			FileInput_read(p_file_input, &save, sizeof(GAME_SAVE_T));

			for (i = 0; i < BOARD_SIZE; ++i)
				e_board[i] = save.board[i];
			e_score = save.score;
			e_win = save.win;
			e_lose = save.lose;

			is_ok = TRUE;
		}
	}
	FileInput_dtor(p_file_input, FREE_MEMORY);

	if (is_ok) {
		g_req_score = FALSE;
		sprintf(save_str, "\n%s\nOn: %s", "Loaded '2048-CyOS.sav'", save.date_time);
		cMessageBox_Info("Game Loaded...", save_str);
	} else {
		cMessageBox_Info("Error Loading Game...", "\nCannot read '2048-CyOS.sav' file!");
	}

	return is_ok;
}

static void Action_HighScore(void) {
	highscore_enter(&g_main_module, e_score, HSM_SHOW);
}

static bool Action_Screenshot(void) {
	bool is_ok;
	char filename[STR_SAVE];

	struct Time c_time;
	struct FileOutput *p_file_output;

	is_ok = FALSE;

	Time_decode(&c_time, time());
	sprintf(filename, "2048-CyOS_%02d%02d%02d.bmp", c_time.hour, c_time.minute, c_time.second);

	p_file_output = (struct FileOutput *) malloc(sizeof(struct FileOutput));
	FileOutput_ctor_Ex(p_file_output, filename, TRUE);
	if (FileOutput_is_good(p_file_output)) {
		int i;
		int j;
		int out_i;
		char *bmp;
		int size = Graphics_get_bytes_total(g_main_module.m_gfx);
		char *vbuf = Graphics_get_buf_addr(g_main_module.m_gfx);

		FileOutput_write(p_file_output, g_bmp_4bpp_head, sizeof(g_bmp_4bpp_head));

		bmp = (char *) malloc((SCREEN_WIDTH * SCREEN_HEIGHT) / 2);

		out_i = 0;
		for (i = 0; i < SCREEN_HEIGHT; ++i) {
			int row_i = (SCREEN_HEIGHT - 1 - i) * SCREEN_WIDTH_DIV_4;
			for (j = 0; j < SCREEN_WIDTH_DIV_4; ++j) {
				int i_i = row_i + j;
				char b = vbuf[i_i];

				bmp[out_i++] = (b & 0xC0) >> 2 | (b & 0x30) >> 4;
				bmp[out_i++] = (b & 0x0C) << 2 | (b & 0x03);
			}
		}

		FileOutput_write(p_file_output, bmp, (SCREEN_WIDTH * SCREEN_HEIGHT) / 2);

		free(bmp);

		is_ok = TRUE;
	}
	FileOutput_dtor(p_file_output, FREE_MEMORY);

	if (is_ok) {
		cMessageBox_Info("Screen Saved...", filename);
	} else {
		cMessageBox_Info("Error Save Screen...", filename);
	}

	return is_ok;
}

static void Action_About(void) {
	cMessageBox_Info("About 2048-CyOS...", "2048 Game for Cybiko\n� EXL, MotoFan.Ru\n15-Mar-2025");
}

static struct cMenuForm *cMenuForm_ctor(
	struct cMenuForm *p_form,
	int x, int y, int w, int h,
	const char *title,
	bool round,
	struct cWinApp *p_win,
	const char **item_strs,
	int item_count
) {
	int i;
	struct rect_t menu_form_rect;

	rect_set(&menu_form_rect, x, y, w, h);

	p_form->m_list = (struct cList *) malloc(sizeof(struct cList));
	cList_ctor(p_form->m_list, w - 6);
	for (i = 0; i < item_count; ++i) {
		struct cItem *p_item = (struct cItem *) malloc(sizeof(struct cItem));
		cItem_ctor(p_item, w - 6, item_strs[i], FALSE, NULL, NULL);
		cList_AddItem(p_form->m_list, p_item);
	}

	cCustomForm_ctor(p_form, &menu_form_rect, title, round, p_win);

	cCustomForm_AddObj(p_form, p_form->m_list, 0, 0);

	p_form->HelpContext = 0;
	p_form->ModalResult = mrNone;

	p_form->m_index = -1;

	cCustomForm_Hide(p_form);

	return p_form;
}

static void cMenuForm_dtor(struct cMenuForm *p_form, int mem_flag) {
	cList_dtor(p_form->m_list, mem_flag);
	cCustomForm_dtor(p_form, mem_flag);
}

static bool cMenuForm_proc(struct cMenuForm *p_form, struct Message *p_msg) {
	switch (p_msg->msgid) {
		case MSG_SHUTUP:
		case MSG_QUIT:
			p_form->ModalResult = mrQuit;
			return TRUE;
			break;
		case MSG_KEYDOWN:
			switch (Message_get_key_param(p_msg)->scancode) {
				case KEY_HELP:
					return TRUE;
					break;
				case KEY_ESC:
					p_form->ModalResult = mrCancel;
					return TRUE;
					break;
				case KEY_ENTER:
					p_form->ModalResult = mrOk;
					p_form->m_index = cList_FindObj(p_form->m_list, cList_GetSelectedObject(p_form->m_list));
					return TRUE;
			}
			break;
	}
	return cCustomForm_proc(p_form, p_msg);
}

static int cMenuForm_ShowModal(struct cMenuForm *p_form) {
	cCustomForm_Show(p_form);

	cList_SelectFirst(p_form->m_list);

	p_form->ModalResult = mrNone;
	while (p_form->ModalResult == mrNone) {
		struct Message *p_msg = cWinApp_get_message(p_form->CurrApplication, 0, 1, MSG_USER);

		cMenuForm_proc(p_form, p_msg);

		cCustomForm_update(p_form);

		Message_delete(p_msg);
	}

	cCustomForm_Hide(p_form);

	return p_form->ModalResult;
}

static void cMenuForm_DoAction(struct cMenuForm *p_form) {
	switch (p_form->m_index) {
		case ACTION_EXIT:
			g_quit = TRUE;
			break;
		case ACTION_RESET:
			g_req_score = FALSE;
			e_key(KEY_DEL);
			break;
		case ACTION_SAVE:
			Action_Save();
			break;
		case ACTION_LOAD:
			Action_Load();
			break;
		case ACTION_SCORE:
			Action_HighScore();
			break;
		case ACTION_SCREENSHOT:
			g_req_screen_shot = TRUE;
			break;
		case ACTION_ABOUT:
			Action_About();
			break;
	}
}

static void draw_tile(int value, int x, int y) {
	int x_o;
	int y_o;
	struct rect_t tile;

	x_o = (x * (TILE_MARGIN + TILE_SIZE) + TILE_MARGIN);
	y_o = (y * (TILE_MARGIN + TILE_SIZE) + TILE_MARGIN) - 2;

	tile.x = x_o + 1;
	tile.y = y_o + 1;
	tile.w = TILE_SIZE;
	tile.h = TILE_SIZE;

	DisplayGraphics_set_color(g_main_module.m_gfx, CLR_DKGRAY);
	DisplayGraphics_fill_rect_Ex(g_main_module.m_gfx, &tile);

	if (value) {
		int w;
		int x_f;
		int y_f;
		struct Font *font;
		char num[STR_LEN_VALUE]; // "2048\0"

		tile.x = x_o;
		tile.y = y_o;

		sprintf(num, "%d", value);

		DisplayGraphics_set_color(g_main_module.m_gfx, CLR_WHITE);
		DisplayGraphics_fill_rect_Ex(g_main_module.m_gfx, &tile);
		DisplayGraphics_set_color(g_main_module.m_gfx, CLR_BLACK);
		DisplayGraphics_draw_rect_Ex(g_main_module.m_gfx, &tile);
		/* EXL, 14-Mar-2025: This works buggy on small rect sizes? Also slow. */
//		draw_roundrect(g_main_module.m_gfx, &tile, RR_ROUND, NULL);

		font = (value < 100) ? cool_bold_font : (value < 1000) ? mini_bold_font : mini_normal_font;
		DisplayGraphics_set_font(g_main_module.m_gfx, font);

		x_f = (value < 1000) ? 1 : 0;
		y_f = (value < 100) ? 0 : 1;
		w = DisplayGraphics_string_width(g_main_module.m_gfx, num);
		DisplayGraphics_draw_text(
			g_main_module.m_gfx, num,
			(x_o + ((TILE_SIZE - w) / 2)) + x_f, (y_o + (TILE_SIZE / 4)) + y_f
		);
	}
}

static void draw_final(void) {
	char score[STR_LEN_SCORE]; // "99999\0"
	const char *score_label = "Score:";
	const char *reset_label_1 = "Press <DEL>";
	const char *reset_label_2 = "to Reset!";

	sprintf(score, "%d", e_score);

	DisplayGraphics_set_color(g_main_module.m_gfx, CLR_BLACK);
	DisplayGraphics_set_font(g_main_module.m_gfx, cool_bold_font);
	DisplayGraphics_draw_text(g_main_module.m_gfx, score_label, X_END_FIELD + TILE_MARGIN, 2);
	DisplayGraphics_draw_text(g_main_module.m_gfx, score, X_END_FIELD + TILE_MARGIN, 18);

	DisplayGraphics_set_color(g_main_module.m_gfx, CLR_DKGRAY);
	DisplayGraphics_set_font(g_main_module.m_gfx, mini_normal_font);
	DisplayGraphics_draw_text(g_main_module.m_gfx, reset_label_1, X_END_FIELD + TILE_MARGIN, SCREEN_HEIGHT - 24);
	DisplayGraphics_draw_text(g_main_module.m_gfx, reset_label_2, X_END_FIELD + TILE_MARGIN + 6, SCREEN_HEIGHT - 12);

	if (e_win || e_lose) {
		int o_x = (e_win) ? 0 : 1;
		const char *game_over_label = (e_win) ? "You Won!" : "You Lose!";
		DisplayGraphics_set_draw_mode(g_main_module.m_gfx, DM_XOR);
		DisplayGraphics_set_color(g_main_module.m_gfx, CLR_BLACK);
		DisplayGraphics_fill_rect(g_main_module.m_gfx, 2, 0, X_END_FIELD, SCREEN_HEIGHT);

		DisplayGraphics_set_font(g_main_module.m_gfx, cool_bold_font);
		DisplayGraphics_draw_text(g_main_module.m_gfx, game_over_label, X_END_FIELD + TILE_MARGIN - o_x, 46);
	}
}

static void draw(void) {
	int x;
	int y;

	DisplayGraphics_set_draw_mode(g_main_module.m_gfx, DM_PUT);
	DisplayGraphics_fill_screen(g_main_module.m_gfx, CLR_WHITE);

	for (y = 0; y < LINE_SIZE; ++y)
		for (x = 0; x < LINE_SIZE; ++x)
			draw_tile(e_board[x + y * LINE_SIZE], x, y);
	draw_final();

	DisplayGraphics_show(g_main_module.m_gfx);
}

long main(int argc, char *argv[], bool start) {
	struct MSequence music;
	struct cMenuForm *p_menu;

	init_module(&g_main_module);

	highscore_init(&g_main_module);

	e_init(KEY_DEL, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN);

#if 0
	e_score = 15656;
	e_board[ 0] =  128;
	e_board[ 1] =   64;
	e_board[ 4] =  256;
	e_board[ 5] =   32;
	e_board[ 8] =  512;
	e_board[ 9] =   16;
	e_board[12] = 1024;
	e_board[13] =    8;
#endif

	MSequence_ctor(&music, "title.mus");
	if (MSequence_is_sane(&music)) {
		MSequence_play_background(&music);
	}

	AppGeneric_clear_screen();

	p_menu = (struct cMenuForm *) malloc(sizeof(struct cMenuForm));
	cMenuForm_ctor(
		p_menu,
		25, 20, 100, 60,
		"Main Menu", TRUE,
		g_main_module.m_process,
		menu_items, sizeof(menu_items) / sizeof(menu_items[0])
	);

	while (!g_quit) {
		struct Message *p_msg;
		struct KeyParam *p_key_param;

		p_msg = cWinApp_get_message(g_main_module.m_process, 0, 1, MSG_USER);

		g_req_menu = g_req_draw = FALSE;

		switch (p_msg->msgid) {
			case MSG_SHUTUP:
			case MSG_QUIT:
				g_quit = TRUE;
				g_focus = FALSE;
				break;
			case MSG_GOTFOCUS:
				g_focus = TRUE;
				g_req_draw = TRUE;
				break;
			case MSG_LOSTFOCUS:
				g_focus = FALSE;
				if (MSequence_is_playing(&music)) {
					MSequence_stop(&music);
				}
				break;
			case MSG_KEYDOWN:
				p_key_param = Message_get_key_param(p_msg);
				switch (p_key_param->scancode) {
					case KEY_DEL:
						g_req_score = FALSE;
						/* Fall-through. */
					case KEY_LEFT:
					case KEY_RIGHT:
					case KEY_UP:
					case KEY_DOWN:
						e_key(p_key_param->scancode);
						g_req_draw = TRUE;
						break;
					case KEY_R:
						e_key(KEY_DEL);
						g_req_score = FALSE;
						g_req_draw = TRUE;
						break;
					case KEY_ESC:
					case KEY_SPACE:
					case KEY_ENTER: /* EXL, 15-Mar-2025: Also "Menu" key on Cybiko Xtreme in apps & games but not OS. */
						/* Drop key auto-repeat. */
						if(!(p_key_param->mask & KEYMASK_AUTOREPEAT)) {
							if (g_focus) {
								g_req_menu = TRUE;
							}
						}
						break;
					default:
						/* Process other keys like "Help", etc. */
						cWinApp_defproc(g_main_module.m_process, p_msg);
						break;
				}
				break;
			default:
				cWinApp_defproc(g_main_module.m_process, p_msg);
				break;
		}

		Message_delete(p_msg);

		if (g_req_menu) {
			int mr = cMenuForm_ShowModal(p_menu);
			if (mr == mrOk) {
				cMenuForm_DoAction(p_menu);
			}
			g_req_draw = TRUE;
		}

		if (e_win || e_lose) {
			if (g_focus && g_req_draw) {
				draw();
			}
			if (!g_req_score) {
				int highscores_result;

				g_req_score = TRUE;
				highscores_result = highscore_enter(&g_main_module, e_score, HSM_CHECKSHOWALL);
				if (highscores_result == HSR_RESTART) {
					g_req_score = FALSE;
					e_key(KEY_DEL);
				}
			}
		}

		if (g_focus && g_req_draw) {
			draw();
		}

		if (g_req_screen_shot) {
			g_req_screen_shot = FALSE;
			Action_Screenshot();
			draw();
		}
	}

	cMenuForm_dtor(p_menu, FREE_MEMORY);

	highscore_cleanup(&g_main_module);

	MSequence_dtor(&music, LEAVE_MEMORY);

	return 0L;
}
