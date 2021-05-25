#include "2048.h"

#include <ZApplication.h>
#include <ZMainWidget.h>
#include <ZMessageBox.h>

#include <ezxres.h>
#include <ezxutilcst.h>

#include <qfileinfo.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qtimer.h>

#include <ctime>

const int FIELD_OFFSET_SCALE = 32;
const int TILE_SIZE          = 48;
const int TILE_MARGIN        = 5;

enum { M_SCREEN, M_SAVE, M_LOAD, M_RESET, M_SOUND, M_ROUNDED, M_RECTANGLE, M_TILES, M_ABOUT, M_EXIT };

class Widget : public QWidget {
	Q_OBJECT

	QPixmap *fb;
	QFont *font_large, *font_middle, *font_small;
	int ww, hh;
	int score;
	bool rounded;

	inline int offsetCoords(int coord) { return coord * (TILE_MARGIN + TILE_SIZE) + TILE_MARGIN * 2; }
	void drawTile(QPainter &painter, int value, int x, int y) {
		const int xOffset = offsetCoords(x) + ww / FIELD_OFFSET_SCALE, yOffset = offsetCoords(y) - TILE_MARGIN - 1;
		painter.setPen(QPen::NoPen);
		painter.setBrush(QColor(e_background(value)));
		if (rounded)
			painter.drawRoundRect(xOffset, yOffset, TILE_SIZE, TILE_SIZE, 36, 36);
		else
			painter.drawRect(xOffset, yOffset, TILE_SIZE, TILE_SIZE);
		if (value) {
			const int size = (value < 100) ? 18 : (value < 1000) ? 14 : 10;
			const QFont &font = *((value < 100) ? font_large : (value < 1000) ? font_middle : font_small);
			const QString strValue = QString("%1").arg(value);
			painter.setPen(QColor(e_foreground(value)));
			painter.setFont(font);
			const int wo = (value < 100) ? 1 : (value < 1000) ? 0 : (-1), ho = (value > 1000) ? 0 : 3;
			const int w = QFontMetrics(font).width(strValue) - wo, h = size - ho;
			painter.drawText(xOffset + (TILE_SIZE - w) / 2, yOffset + TILE_SIZE - (TILE_SIZE - h) / 2 - 2, strValue);
		}
	}
	void drawFinal(QPainter &painter) {
		if (e_win || e_lose) {
			painter.fillRect(0, 0, ww, hh, QBrush(COLOR_OVERLAY, Dense6Pattern));
			painter.setPen(QColor(COLOR_FINAL));
			painter.setFont(*font_large);
			const QString center = (e_win) ? "You won!" : "Game Over!";
			const int w = QFontMetrics(*font_large).width(center);
			painter.drawText(ww / 2 - w / 2, hh / 2, center);
		}
		if (score != e_score) {
			score = e_score;
			emit scoreChanged(score);
		}
	}
signals:
	void scoreChanged(int);
public:
	Widget(QWidget *parent = 0, const char *name = 0) : QWidget(parent, name, WRepaintNoErase | WResizeNoErase) {
		e_init(Qt::Key_PageUp, Qt::Key_Left, Qt::Key_Right, Qt::Key_Up, Qt::Key_Down);
		fb = NULL; score = 0; rounded = true;
		font_large = new QFont("Sans", 20, QFont::Bold);
		font_middle = new QFont("Sans", 18, QFont::Bold);
		font_small = new QFont("Sans", 16, QFont::Bold);
		QPixmap::setDefaultOptimization(QPixmap::NormalOptim);
		setBackgroundMode(NoBackground);
		setFocusPolicy(QWidget::StrongFocus);
	}
	~Widget() { delete fb; delete font_large; delete font_middle; delete font_small; }
	void setRounded(bool aRounded) {
		rounded = aRounded;
		update();
	}
public slots:
	void screenShotTimer() { QTimer::singleShot(500, this, SLOT(screenShot())); }
	void reset() {
		e_key(Qt::Key_PageUp);
		update();
	}
	void screenShot() {
		const QWidget *parent = static_cast<QWidget *>(this->parent());
		const int wp = parent->width(), hp = parent->height();
		QPixmap pixmap(wp, hp);
		bitBlt(&pixmap, 0, 0, parent, 0, 0, wp, hp, Qt::CopyROP, true);
		const QString path = QString("%1/%2.png").arg(QFileInfo(qApp->argv()[0]).dirPath(true)).arg(time(NULL));
		const QString pathReplaced = QString(path).replace(QString("/"), " / ");
		ZMessageBox *msgDlg = new ZMessageBox(this, NULL, QString::null, "OK");
		if (pixmap.save(path, "PNG")) {
			msgDlg->setText(QString("<h3>Saved!</h3><br>"
				"Screenshot Saved!<br>Path:<br><font size=\"2\">%1</font>").arg(pathReplaced));
			msgDlg->setIconPixmap(RES_ICON_Reader().getIcon("Dialog_Complete", true));
		} else {
			msgDlg->setText(QString("<h3>Error!</h3><br>"
				"Error: Cannot Save Screenshot!<br>Path:<br><font size=\"2\">%1</font>").arg(pathReplaced));
			msgDlg->setIconPixmap(RES_ICON_Reader().getIcon("Dialog_Error", true));
		}
		msgDlg->exec();
		delete msgDlg;
	}
	void save() {
		ZMessageBox *msgDlg = new ZMessageBox(this, NULL, QString::null, "OK");
		QFile save(QString("%1/save.dat").arg(QFileInfo(qApp->argv()[0]).dirPath(true)));
		if (save.open(IO_WriteOnly)) {
			QDateTime saveDateTime = QDateTime::currentDateTime();
			QDataStream dataStream(&save);
			dataStream << saveDateTime;
			for (int i = 0; i < BOARD_SIZE; ++i)
				dataStream << (Q_INT32) e_board[i];
			dataStream << e_score; dataStream << e_win; dataStream << e_lose;
			msgDlg->setText(QString("<h3>Game Saved!</h3><br>State on:\n%1").arg(saveDateTime.toString()));
			msgDlg->setIconPixmap(RES_ICON_Reader().getIcon("Dialog_Complete", true));
		} else {
			msgDlg->setText(QString("<h3>Save Error!</h3><br>Cannot create save.dat file."));
			msgDlg->setIconPixmap(RES_ICON_Reader().getIcon("Dialog_Error", true));
		}
		msgDlg->exec();
		delete msgDlg;
	}
	void load() {
		ZMessageBox *msgDlg = new ZMessageBox(this, NULL, QString::null, "OK");
		QFile save(QString("%1/save.dat").arg(QFileInfo(qApp->argv()[0]).dirPath(true)));
		if (save.open(IO_ReadOnly)) {
			QDateTime loadDateTime;
			QDataStream dataStream(&save);
			dataStream >> loadDateTime;
			Q_INT32 value, score, win, lose;
			for (int i = 0; i < BOARD_SIZE; ++i) {
				dataStream >> value;
				e_board[i] = value;
			}
			dataStream >> score; dataStream >> win; dataStream >> lose;
			e_score = score; e_win = win; e_lose = lose;
			msgDlg->setText(QString("<h3>Game Loaded!</h3><br>State on:\n%1").arg(loadDateTime.toString()));
			msgDlg->setIconPixmap(RES_ICON_Reader().getIcon("Dialog_Complete", true));
			update();
		} else {
			msgDlg->setText(QString("<h3>Load Error!</h3><br>Cannot find save.dat file."));
			msgDlg->setIconPixmap(RES_ICON_Reader().getIcon("Dialog_Error", true));
		}
		msgDlg->exec();
		delete msgDlg;
	}
protected:
	virtual void keyPressEvent(QKeyEvent *keyEvent) {
		QWidget::keyPressEvent(keyEvent);
		int key = keyEvent->key();
		if (key == Qt::Key_PageDown) // Vol "-" button.
			e_key(Qt::Key_PageUp);   // Vol "+" button.
		else
			e_key(key);
		repaint();
	}
	virtual void mousePressEvent(QMouseEvent *mouseEvent) {
		QWidget::mousePressEvent(mouseEvent);
		const int w4 = ww / 4, h4 = hh / 4;
		const QRect up(w4, 0, w4 * 2, h4);
		const QRect down(w4, h4 * 3, w4 * 2, h4);
		const QRect left(0, h4, w4, h4 * 2);
		const QRect right(w4 * 3, h4, w4, h4 * 2);
		const QPoint click(mouseEvent->pos().x(), mouseEvent->pos().y());
		if (up.contains(click))
			e_key(Qt::Key_Up);
		else if (down.contains(click))
			e_key(Qt::Key_Down);
		else if (left.contains(click))
			e_key(Qt::Key_Left);
		else if (right.contains(click))
			e_key(Qt::Key_Right);
		repaint();
	}
	virtual void paintEvent(QPaintEvent *) {
		ww = width(); hh = height();
		if (!fb)
			fb = new QPixmap(ww, hh);
		QPainter painter;
		painter.begin(fb, this);
		painter.fillRect(0, 0, ww, hh, QColor(COLOR_BOARD));
		for (int y = 0; y < LINE_SIZE; ++y)
			for (int x = 0; x < LINE_SIZE; ++x)
				drawTile(painter, e_board[x + y * LINE_SIZE], x, y);
		drawFinal(painter);
		painter.end();
		bitBlt(this, 0, 0, fb, 0, 0, ww, hh, Qt::CopyROP, true);
	}
};

class MainWidget : public ZMainWidget {
	Q_OBJECT

	QLabel *titleBar;
	Widget *widget;
	QPopupMenu *menu, *tileMenu;
	bool sound;

public slots:
	void about() {
		ZMessageBox *msgDlg = new ZMessageBox(this, NULL, QString::null, "Close");
		msgDlg->setText(QTextCodec::codecForName("UTF-8")->toUnicode("<h3>About 2048-EZX</h3><br><font size=\"2\">"
			"2048 Game implementation especially for Motorola EZX platform.<br><br>Version: 1.0, %1<br>"
			"© EXL (exl@bk.ru)<br>"
			"<u>https://github.com/EXL/2048</u></font>").arg(__DATE__));
		const QString iconPath = QString("%1/ezx_dia_50x50.png").arg(QFileInfo(qApp->argv()[0]).dirPath(true));
		if (QFile::exists(iconPath)) {
			QPixmap icon(50, 50);
			icon.load(iconPath);
			msgDlg->setIconPixmap(icon);
		} else
			msgDlg->setIconPixmap(RES_ICON_Reader().getIcon("Dialog_Exclamatory_Mark", true));
		msgDlg->exec();
		delete msgDlg;
	}
	void setTitleScore(int score) { titleBar->setText(QString(" 2048-EZX | Score: %1 ").arg(score)); }
	void roundedOn() {
		widget->setRounded(true);
		tileMenu->setItemChecked(M_ROUNDED, true);
		tileMenu->setItemChecked(M_RECTANGLE, false);
	}
	void roundedOff() {
		widget->setRounded(false);
		tileMenu->setItemChecked(M_ROUNDED, false);
		tileMenu->setItemChecked(M_RECTANGLE, true);
	}
	void soundOnOff() {
		sound = !sound;
		menu->setItemChecked(M_SOUND, sound);
		static_cast<ZApplication *>(qApp)->enableTouchSound(sound);
	}
public:
	MainWidget() : ZMainWidget(false, NULL, NULL, 0), sound(true) {
		titleBar = new QLabel(this, NULL);
		titleBar->setText(" 2048-EZX | Score: 0 ");
		setTitleBarWidget(titleBar);

		widget = new Widget(this, NULL);
		connect(widget, SIGNAL(scoreChanged(int)), this, SLOT(setTitleScore(int)));
		setContentWidget(widget);

		UTIL_CST *cst = new UTIL_CST(this, "Reset");
		setCSTWidget(cst);
		connect(cst->getMidBtn(), SIGNAL(clicked()), widget, SLOT(reset()));
		connect(cst->getRightBtn(), SIGNAL(clicked()), qApp, SLOT(quit()));

		menu = new QPopupMenu(this);
		menu->insertItem("Take Screenshot", widget, SLOT(screenShotTimer()), 0, M_SCREEN);
		menu->insertSeparator();
		menu->insertItem("Save Game", widget, SLOT(save()), 0, M_SAVE);
		menu->insertItem("Load Game", widget, SLOT(load()), 0, M_LOAD);
		menu->insertItem("Reset Game", widget, SLOT(reset()), 0, M_RESET);
		menu->insertSeparator();
		menu->insertItem("Touch Sounds", this, SLOT(soundOnOff()), 0, M_SOUND);
		menu->setItemChecked(M_SOUND, true);
		tileMenu = new QPopupMenu(this);
		tileMenu->insertItem("Rounded", this, SLOT(roundedOn()), 0, M_ROUNDED);
		tileMenu->insertItem("Rectangle", this, SLOT(roundedOff()), 0, M_RECTANGLE);
		tileMenu->setItemChecked(M_ROUNDED, true);
		menu->insertItem("Tiles", tileMenu, 0, M_TILES);
		menu->insertSeparator();
		menu->insertItem("About...", this, SLOT(about()), 0, M_ABOUT);
		menu->insertItem("Exit", qApp, SLOT(quit()), 0, M_EXIT);
		cst->getLeftBtn()->setPopup(menu);
	}
};

int main(int argc, char *argv[]) {
	ZApplication application(argc, argv);
	MainWidget mainWidget;
	application.showMainWidget(&mainWidget);
	return application.exec();
}

#include "2048-EZX.moc"