/*
	Skelton for retropc emulator
	Author : Takeda.Toshiya
        Port to Qt : K.Ohta <whatisthis.sowhat _at_ gmail.com>
	Date   : 2006.08.18 -
	License : GPLv2
	History : 2015.11.10 Split from qt_main.h
	[ win32 main ] -> [ Qt main ] -> [Emu Thread]
*/
#ifndef _CSP_QT_EMU_THREAD_TMPL_H
#define _CSP_QT_EMU_THREAD_TMPL_H

#include <QThread>
#include <QQueue>
#include <QString>
#include <QElapsedTimer>

#include "fifo.h"
#include "common.h"
#include "commonclasses.h"
#include "fileio.h"
#include "emu.h"

//#include "menuclasses.h"
#include "mainwidget_base.h"
#include "commonclasses.h"
#include "config.h"

class META_MainWindow;
class EMU;
class QWaitCondition;
class USING_FLAGS;

QT_BEGIN_NAMESPACE
//#include "../../romakana.h"

typedef struct {
	uint32_t code;
	uint32_t mod;
	bool repeat;
} key_queue_t;

class DLL_PREFIX EmuThreadClassBase : public QThread {
	Q_OBJECT
protected:
	bool calc_message;
	bool tape_play_flag;
	bool tape_rec_flag;
	int tape_pos;
	bool mouse_flag;
	int mouse_x;
	int mouse_y;
	
	//QQueue <key_queue_t>key_up_queue;
	//QQueue <key_queue_t>key_down_queue;
    FIFO *key_up_code_fifo;
	FIFO *key_up_mod_fifo;
	FIFO *key_up_repeat_fifo;
	
    FIFO *key_down_code_fifo;
	FIFO *key_down_mod_fifo;
	FIFO *key_down_repeat_fifo;

	
	uint32_t key_mod;
	bool romakana_conversion_mode;

	EMU *p_emu;
	USING_FLAGS *using_flags;
	config_t *p_config;
	
	QWaitCondition *drawCond;
	class META_MainWindow *MainWindow;
	QElapsedTimer tick_timer;
	
	bool bRunThread;
	bool bResetReq;
	bool bSpecialResetReq;
	bool bLoadStateReq;
	bool bSaveStateReq;
	bool bUpdateConfigReq;
	bool bStartRecordSoundReq;
	bool bStopRecordSoundReq;
	bool bStartRecordMovieReq;
	QString sStateFile;

//	bool draw_timing;
	bool doing_debug_command;
	bool bUpdateVolumeReq[32];
	int volume_balance[32];
	int volume_avg[32];
	int record_fps;

	qint64 next_time;
	qint64 update_fps_time;
	bool prev_skip;
	int total_frames;
	int draw_frames;
	int skip_frames;
	QString qd_text[4];
	QString fd_text[8];
	QString fd_lamp[8];
	QString cmt_text[4];
	QString cdrom_text;
	QString laserdisc_text;
	QString bubble_text[16];
	QString clipBoardText;

	_TCHAR roma_kana_buffer[8];
	uint32_t roma_kana_shadow[8];
	int roma_kana_ptr;
	void calc_volume_from_balance(int num, int balance);
	void calc_volume_from_level(int num, int level);
	
	virtual void button_pressed_mouse_sub(Qt::MouseButton button) {};
	virtual void button_released_mouse_sub(Qt::MouseButton button) {};
	virtual void get_qd_string(void) {};
	virtual void get_fd_string(void) {};
	virtual void get_tape_string(void) {};
	virtual void get_cd_string(void) {};
	virtual void get_bubble_string(void) {};
	void enqueue_key_up(key_queue_t s) {
		key_up_code_fifo->write(s.code);
		key_up_mod_fifo->write(s.mod);
		key_up_repeat_fifo->write(s.repeat? 1 : 0);
	};
	void enqueue_key_down(key_queue_t s) {
		key_down_code_fifo->write(s.code);
		key_down_mod_fifo->write(s.mod);
		key_down_repeat_fifo->write(s.repeat? 1 : 0);
	};
	void dequeue_key_up(key_queue_t *s) {
		s->code = (uint32_t)key_up_code_fifo->read();
		s->mod  = (uint32_t)key_up_mod_fifo->read();
		if(key_up_repeat_fifo->read() != 0) {
			s->repeat = true;
		} else {
			s->repeat = false;
		}
	};
	void dequeue_key_down(key_queue_t *s) {
		s->code = (uint32_t)key_down_code_fifo->read();
		s->mod  = (uint32_t)key_down_mod_fifo->read();
		if(key_down_repeat_fifo->read() != 0) {
			s->repeat = true;
		} else {
			s->repeat = false;
		}
	};
	bool is_empty_key_down() {
		bool f = key_down_code_fifo->empty();
		f &= key_down_mod_fifo->empty();
		f &= key_down_repeat_fifo->empty();
		return f;
	};
	bool is_empty_key_up() {
		bool f = key_up_code_fifo->empty();
		f &= key_up_mod_fifo->empty();
		f &= key_up_repeat_fifo->empty();
		return f;
	};
	void clear_key_queue() {
		key_up_code_fifo->clear();
		key_up_mod_fifo->clear();
		key_up_repeat_fifo->clear();

		key_down_code_fifo->clear();
		key_down_mod_fifo->clear();
		key_down_repeat_fifo->clear();
	};
	virtual void set_romakana(void) { };
public:
	EmuThreadClassBase(META_MainWindow *rootWindow, USING_FLAGS *p, QObject *parent = 0);
	~EmuThreadClassBase();
	virtual void run() {};
	void set_tape_play(bool);
	void resize_screen(int sw, int sh, int stw, int sth);
	void sample_access_drv(void);
	virtual bool now_debugging() { return false; };
public slots:
	void doExit(void);
	
	void do_reset();
	void do_special_reset();
	void do_load_state(QString name);
	void do_save_state(QString name);
	void do_update_config();
	void do_start_record_sound();
	void do_stop_record_sound();
	void do_start_record_video(int fps);
	void do_stop_record_video();
	void do_update_volume_level(int num, int level);
	void do_update_volume_balance(int num, int level);

	void button_pressed_mouse(Qt::MouseButton);
	void button_released_mouse(Qt::MouseButton);
	void do_key_down(uint32_t vk, uint32_t mod, bool repeat);
	void do_key_up(uint32_t vk, uint32_t mod);
signals:
	int message_changed(QString);
	int window_title_changed(QString);
	int sig_draw_thread(bool);
	int quit_draw_thread(void);
	int sig_screen_aspect(int);
	int sig_screen_size(int, int);
	int sig_finished(void);
	int call_emu_thread(EMU *);
	int sig_check_grab_mouse(bool);
	int sig_mouse_enable(bool);
	int sig_update_recent_disk(int);
	
	int sig_change_osd(int, int, QString);
	int sig_change_access_lamp(int, int, QString);
	int sig_change_virtual_media(int, int, QString);
	
	int sig_update_recent_bubble(int);
	int sig_change_osd_bubble(int, QString);
	int sig_set_grid_vertical(int, bool);
	int sig_set_grid_horizonal(int, bool);
	int sig_send_data_led(quint32);
	int sig_resize_screen(int, int);
	int sig_resize_uibar(int, int);
	int sig_is_enable_mouse(bool);
	int sig_debugger_input(QString);
	int sig_quit_debugger();
	int sig_romakana_mode(bool);
	int sig_set_access_lamp(int, bool);
};

QT_END_NAMESPACE

#endif
