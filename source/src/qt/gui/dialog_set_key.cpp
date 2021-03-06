/*
 * Common Source Project/ Qt
 * (C) 2015 K.Ohta <whatisthis.sowhat _at_ gmail.com>
 *  Qt: Menu->Emulator->Define Strings
 *  History: Feb 24, 2016 : Initial
 */

#include "qt_gldraw.h"
#include "dialog_set_key.h"

#include <QApplication>

CSP_KeySetDialog::CSP_KeySetDialog(QWidget *parent, GLDrawClass *glv) : QWidget(parent)
{
	p_wid = parent;
	p_glv = glv;

	if(glv == NULL) return;

	layout = new QVBoxLayout(this);
	label_head = new QLabel(QApplication::translate("KeySetDialog", "<B>Define Keys</B>", 0));
	layout->addWidget(label_head);

	scroll_area = new QScrollArea(this);
	scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	keycodes_widget = new QWidget(this);
	keycodes_layout = new QGridLayout(keycodes_widget);

	int lim = glv->get_key_table_size();
	int i;
	int j = 0;

	if(lim > KEYDEF_MAXIMUM) lim = KEYDEF_MAXIMUM;
	setup_head_label[0] = new QLabel(QString::fromUtf8("<B>VK</B>"));
	setup_head_label[1] = new QLabel(QApplication::translate("KeySetDialog", "<B>Scan Code</B>", 0));
	keycodes_layout->addWidget(setup_head_label[0], 0, 0);
	keycodes_layout->addWidget(setup_head_label[1], 0, 1);
	
	for(i = 0; i < 256; i++) {
		vk_checked[i] = false;
	}
	for(i = 0; i < KEYDEF_MAXIMUM; i++) {
		vk_map[i] = 0x00;
		setup_combo[i] = NULL;
		setup_label[i] = NULL;
	}
	for(i = 0; i < lim; i++) {
		QString tmps;
		const char *p;
		keydef_table_t *key_table;
		if(j >= KEYDEF_MAXIMUM) break;
		key_table = glv->get_key_table(i);
		if(key_table == NULL) continue;
		if(key_table->vk >= 0xffffffff) break;
		if((key_table->vk < 0) && (key_table->vk >= 256)) continue;
		p = glv->get_key_vk_name(key_table->vk);
		if(p == NULL) continue;
		vk_checked[key_table->vk] = true;
		setup_combo[j] = new CSP_KeySetupCombo(keycodes_widget, i, key_table,
											   glv->get_default_key_table());
		connect(setup_combo[j], SIGNAL(sig_selected(uint32_t, uint32_t)),
				glv, SLOT(do_update_keyboard_scan_code(uint32_t, uint32_t)));
		tmps = QString::fromUtf8("<B>") + QString::fromUtf8(p) + QString::fromUtf8("</B>"); 
		setup_label[j] = new QLabel(tmps);
		keycodes_layout->addWidget(setup_label[j], j + 1, 0);
		keycodes_layout->addWidget(setup_combo[j], j + 1, 1);
		j++;
	}
	
	for(i = 0; i < 256; i++) {
		if(!vk_checked[i]) {
			QString tmps;
			const char *p;
			keydef_table_t key_table_s;

			key_table_s.vk = (uint32_t)i;
			key_table_s.scan = 0;
			
			p = glv->get_key_vk_name((uint32_t)i);
			if(p == NULL) {
				p = (const char *)"VK_Undef";
			}
			setup_combo[j] = new CSP_KeySetupCombo(keycodes_widget, i, &key_table_s,
											   glv->get_default_key_table());
			connect(setup_combo[j], SIGNAL(sig_selected(uint32_t, uint32_t)),
					glv, SLOT(do_update_keyboard_scan_code(uint32_t, uint32_t)));
			tmps = QString::fromUtf8("<B>") + QString::fromUtf8(p) + QString::fromUtf8("</B>"); 
			vk_map[j] = (uint8_t)i;
			setup_label[j] = new QLabel(tmps);
			keycodes_layout->addWidget(setup_label[j], j + 1, 0);
			keycodes_layout->addWidget(setup_combo[j], j + 1, 1);
			j++;
		}
	}		
	keycodes_widget->setLayout(keycodes_layout);
	
	scroll_area->setWidget(keycodes_widget);
	scroll_area->setWidgetResizable(true);

	layout->addWidget(scroll_area);
	if(p_wid == NULL) this->setWindowIcon(QIcon(":/icon_keyboard.png"));
	this->setLayout(layout);
}

CSP_KeySetDialog::~CSP_KeySetDialog()
{
}

void CSP_KeySetDialog::do_update_keyname_table(uint32_t vk, QString name)
{
	if(p_glv == NULL) return;
	for(int i = 0; i < KEYDEF_MAXIMUM; i++) {
		if(setup_combo[i] != NULL) {
			setup_combo[i]->do_update_scan_name(vk, name);
		}
	}
}
