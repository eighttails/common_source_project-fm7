/*
 * Common source code project -> FM-7 -> Display -> Vram access
 * (C) 2015 K.Ohta <whatisthis.sowhat _at_ gmail.com>
 * History:
 *  Sep 27, 2015 : Split from display.cpp .
 */

#include "vm.h"
#include "fm7_display.h"
#if defined(_FM77L4)
#include "../hd46505.h"
#endif

extern config_t config;

namespace FM7 {

void DISPLAY::clear_display(int dmode, int w, int h)
{
#if defined(FIXED_FRAMEBUFFER_SIZE)
	if((dmode != DISPLAY_MODE_8_400L) && (dmode != DISPLAY_MODE_1_400L)) {
		h = h * 2;
	}
#endif
	for(int yy = 0; yy < h; yy++) {
		scrntype_t *p;
		p = emu->get_screen_buffer(yy);
		if(p != NULL) {
			memset(p, 0x00, sizeof(scrntype_t) * w);
		}
	}
}

void DISPLAY::draw_window(int dmode, int y, int begin, int bytes, bool window_inv, bool scan_line)
{
	_render_command_data_t cmd;
	bool use_cmd = false;
	int xzoom = 1;
	uint32_t _offset_base = 0x4000;
	int planes;
	int shift;
	int width;
	switch(dmode) {
	case DISPLAY_MODE_8_200L:
		_offset_base = 0x4000;
		use_cmd = true;
		planes = 3;
		shift = 5;
		width = 80;
		break;
#if defined(_FM77AV_VARIANTS)
	case DISPLAY_MODE_4096:
		_offset_base = 0x2000;
		xzoom = 2;
		planes = 12;
		shift = 5;
		width = 40;
		break;
#  if defined(_FM77AV40EX) || defined(_FM77AV40SX) || defined(_FM77AV40)
	case DISPLAY_MODE_8_400L:
		_offset_base = 0x8000;
		use_cmd = true;
		planes = 3;
		shift = 5;
		width = 80;
		break;
#    if defined(_FM77AV40EX) || defined(_FM77AV40SX)
	case DISPLAY_MODE_256k:
		_offset_base = 0x2000;
		xzoom = 2;
		planes = 20;
		shift = 5;
		width = 40;
		break;
#    endif
#  endif
#endif
	default:
		return;
		break;
	}
	if(use_cmd) {
		memset(&cmd, 0x00, sizeof(_render_command_data_t));
#if defined(USE_GREEN_DISPLAY)
		if(use_green_monitor) {
			cmd.palette = dpalette_pixel_green;
		} else {
			cmd.palette = dpalette_pixel;
		}
#else
		cmd.palette = dpalette_pixel;
#endif				
		if(!multimode_dispflags[0]) cmd.is_render[0] = true;
		if(!multimode_dispflags[1]) cmd.is_render[1] = true;
		if(!multimode_dispflags[2]) cmd.is_render[2] = true;
		cmd.bit_trans_table[0] = (_bit_trans_table_t*)(&(bit_trans_table_2[0][0])); // B
		cmd.bit_trans_table[1] = (_bit_trans_table_t*)(&(bit_trans_table_1[0][0])); // R
		cmd.bit_trans_table[2] = (_bit_trans_table_t*)(&(bit_trans_table_0[0][0])); // G
		for(int i = 0; i < 3; i++) {
			cmd.data[i] = gvram_shadow;
			cmd.baseaddress[i] = (i * _offset_base) + yoff_d;
			cmd.voffset[i] = y * width;
		}
		cmd.xzoom = xzoom;
		cmd.addrmask = _offset_base - 1;
		cmd.addrmask2 = _offset_base - 1;
		cmd.render_width = bytes;
		cmd.begin_pos = begin;
		cmd.shift = shift;
	}
	scrntype_t *p;
	scrntype_t *pp;
	if(dmode == DISPLAY_MODE_8_400L) {
#if defined(_FM77AV40EX) || defined(_FM77AV40SX) || defined(_FM77AV40)
		p = emu->get_screen_buffer(y);
		if(p == NULL) return;
		Render8Colors_Line(&cmd, &(p[cmd.begin_pos * 8]), NULL, false);
#endif
	} else {
#if !defined(FIXED_FRAMEBUFFER_SIZE)
		p = emu->get_screen_buffer(y);
		pp = NULL;
#else
		p = emu->get_screen_buffer(y << 1);
		pp = emu->get_screen_buffer((y << 1) + 1);
#endif
		if(p == NULL) return;
		switch(dmode) {
		case DISPLAY_MODE_8_200L:
			{
				if(pp != NULL) {
					Render8Colors_Line(&cmd, &(p[cmd.begin_pos * 8]), &(pp[cmd.begin_pos * 8]), scan_line);
				} else {
					Render8Colors_Line(&cmd, &(p[cmd.begin_pos * 8]), NULL, false);
				}
			}
			break;
#if defined(_FM77AV_VARIANTS)
		case DISPLAY_MODE_4096:
			{
				uint32_t mask = 0x000;
				if(!multimode_dispflags[0]) mask = 0x00f;
				if(!multimode_dispflags[1]) mask = mask | 0x0f0;
				if(!multimode_dispflags[2]) mask = mask | 0xf00;
				if(pp != NULL) pp = &(pp[begin]);
				p = &(p[begin]);
				uint32_t yoff = y * 40 + begin;
				for(int x = begin; x < (begin + bytes); x++) {
					GETVRAM_4096(yoff, p, pp, mask, window_inv, scan_line);
#    if defined(FIXED_FRAMEBUFFER_SIZE)
					p += 16;
					if(pp != NULL) pp += 16;
#    else
					p += 8;
#    endif
					yoff++;
				}
			}
			break;
#    if defined(_FM77AV40EX) || defined(_FM77AV40SX) || defined(_FM77AV40)
		case DISPLAY_MODE_256k:
			{
				uint32_t yoff = y * 40;
				if(pp != NULL) pp = &(pp[begin]);
				p = &(p[begin]);
				for(int x = begin; x < (begin + bytes); x++) {
					GETVRAM_256k(yoff + x, p, pp, scan_line);
#      if defined(FIXED_FRAMEBUFFER_SIZE)
					p += 16;
					if(pp != NULL) pp += 16;
#      else
					p += 8;
#      endif
				}
			}
			break;
#    endif
#  endif
		default:
			break;
		}
	}			
}

#if defined(_FM77L4)
void DISPLAY::draw_77l4_400l(bool ff)
{
	bool renderf = false;
	uint32_t naddr;
	uint8_t bitcode;
	uint8_t charcode;
	uint8_t attr_code;
	scrntype_t on_color;
	int xlim, ylim;
	bool do_green;
	uint8_t *regs = l4crtc->get_regs();
	cursor_start = (int)(regs[10] & 0x1f);
	cursor_end = (int)(regs[11] & 0x1f);
	cursor_type = (int)((regs[10] & 0x60) >> 5);
	text_xmax = (int)((uint16_t)regs[1] << 1);
	text_lines = (int)((regs[9] & 0x1f) + 1);
	text_ymax = (int)(regs[6] & 0x7f);
	int yoff = 0;
	scrntype_t *p;
	for(int y =0; y < 400; y+= 8) {
		renderf = false;
		if((y & 0x0f) == 0) {
			for(int yy = 0; yy < 16; yy++) renderf |= vram_draw_table[y + yy];
			renderf = renderf | ff;
			if(renderf) {
				for(int yy = 0; yy < 16; yy++) vram_draw_table[y + yy] = true;
			}
		}
		if(use_green_monitor) {
			for(int yy = 0; yy < 8; yy++) {
				if(!(vram_draw_table[y + yy] | ff)) continue;
				vram_draw_table[y + yy] = false;
				p = emu->get_screen_buffer(y + yy);
				if(p == NULL) continue;
				yoff = (y + yy) * 80;
				for(int x = 0; x < 10; x++) {
					for(int ii = 0; ii < 8; ii++) {
						GETVRAM_1_400L_GREEN(yoff + ii, p);
						p += 8;
					}
					yoff += 8;
				}
			}
			do_green = true;
		} else {
			for(int yy = 0; yy < 8; yy++) {
				if(!(vram_draw_table[y + yy] | ff)) continue;
				vram_draw_table[y + yy] = false;
				p = emu->get_screen_buffer(y + yy);
				if(p == NULL) continue;
				yoff = (y + yy) * 80;
				for(int x = 0; x < 10; x++) {
					for(int ii = 0; ii < 8; ii++) {
						GETVRAM_1_400L(yoff + ii, p);
						p += 8;
					}
					yoff += 8;
				}
			}
			do_green = false;
		}
		// Draw Text
		if(renderf) {
			bool reverse;
			bool display_char;
			int raster;
			bool cursor_rev;
			uint8_t bitdata;
			if(text_width40) {
				xlim = 40;
			} else {
				xlim = 80;
			}
				
			for(int x = 0; x < xlim; x++) {
				naddr = (text_start_addr.w.l + ((y / text_lines) * text_xmax + x) * 2) & 0x0ffe;
				charcode = text_vram[naddr];
				attr_code = text_vram[naddr + 1];
						
				on_color = GETVRAM_TEXTCOLOR(attr_code, do_green);
					
				display_char = ((attr_code & 0x10) == 0);
				reverse = ((attr_code & 0x08) != 0);
					
				for(int yy = 0; yy < 16; yy++) {
					raster = y % text_lines;
					bitdata = 0x00;
					p = emu->get_screen_buffer(y + yy);
					if(p == NULL) continue;
					if((raster < 16) && (display_char || text_blink)) {
						bitdata = subsys_cg_l4[(uint32_t)charcode * 16 + (uint32_t)raster];
					}
					cursor_rev = false;
					if((naddr == (uint32_t)(cursor_addr.w.l)) && (cursor_type != 1) &&
					   (text_blink || (cursor_type == 0))) {
						if((raster >= cursor_start) && (raster <= cursor_end)) {
							cursor_rev = true;
						}
					}
					bitdata = GETVRAM_TEXTPIX(bitdata, reverse, cursor_rev);
					if(bitdata != 0) {
						if(text_width40) {
							scrntype_t *pp = &(p[x * 2]); 
							for(int ii = 0; ii < 8; ii++) {
								if((bitdata & 0x80) != 0) {
									p[0] = on_color;
									p[1] = on_color;
								}
								bitdata <<= 1;
								p += 2;
							}										
						} else {
							scrntype_t *pp = &(p[x * 2]); 
							for(int ii = 0; ii < 8; ii++) {
								if((bitdata & 0x80) != 0) {
									p[0] = on_color;
								}
								bitdata <<= 1;
								p += 1;
							}										
						}
					}
				}
			}
		}
	}
}
#endif

void DISPLAY::draw_screen()
{
	uint16_t wx_begin = -1, wx_end = -1, wy_low = 1024, wy_high = -1;
	bool scan_line = config.scan_line;
	bool ff = force_update;
	int dmode = display_mode;
	yoff_d = 0;
#if defined(_FM77AV40EX) || defined(_FM77AV40SX)
	{
		wx_begin = window_xbegin;
		wx_end   = window_xend;
		wy_low   = window_low;
		wy_high  = window_high;
		bool _flag = window_opened; 
		if((wx_begin < wx_end) && (wy_low < wy_high)) {
			window_opened = true;
		} else {
			window_opened = false;
		}
		if(_flag != window_opened) {
			vram_wrote_shadow = true;
		}
	}
#endif
#if defined(_FM77AV_VARIANTS)
	yoff_d2 = 0;
	yoff_d1 = 0;
#else
	//if(!(vram_wrote_shadow)) return;
	yoff_d1 = yoff_d2 = offset_point;
#endif
	int ylines;
	int xpixels;
	switch(dmode) {
	case DISPLAY_MODE_8_200L:
		xpixels = 640;
		ylines = 200;
		break;
	case DISPLAY_MODE_1_400L:
	case DISPLAY_MODE_8_400L:
		xpixels = 640;
		ylines = 400;
		break;
	default:
		xpixels = 320;
		ylines = 200;
		break;
	}
# if !defined(FIXED_FRAMEBUFFER_SIZE)
	emu->set_vm_screen_size(xpixels, ylines, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH_ASPECT, WINDOW_HEIGHT_ASPECT);
# endif
	emu->set_vm_screen_lines(ylines);
	if(!crt_flag) {
		if(crt_flag_bak) {
			clear_display(dmode, xpixels, ylines);
		}
		crt_flag_bak = crt_flag;
		if(ff) force_update = false;
		return;
	}
	crt_flag_bak = crt_flag;
	if(!(vram_wrote_shadow | ff)) return;
	vram_wrote_shadow = false;

	int wpixels = xpixels >> 3;
#if defined(_FM77L4)
	if(dmode == DISPLAY_MODE_1_400L) {
		draw_77l4_400l(ff);
		if(ff) force_update = false;
		return;
	}
#endif	
	for(int y = 0;  y < ylines; y += 8) {
		for(int yy = 0; yy < 8; yy++) {
			if(!(vram_draw_table[y + yy] | ff)) continue;
			vram_draw_table[y + yy] = false;
#  if defined(_FM77AV40EX) || defined(_FM77AV40SX) || defined(_FM77AV40)
				int dpage;
				dpage = vram_display_block;
				bool window_inv = false;
#    if defined(_FM77AV40EX) || defined(_FM77AV40SX)
				if((window_opened && (wy_low <= (y + yy)) && (wy_high > (y + yy)))
				   && (dmode != DISPLAY_MODE_256k)) {
					if((wx_begin > 0) && (wx_begin < wx_end) && (wx_begin < wpixels)) {
						yoff_d = (dpage != 0) ? 0x18000 : 0x00000;
						if(display_page_bak == 1) yoff_d += 0xc000;
						draw_window(dmode, yy + y, 0, wx_begin,
									false, scan_line);
						yoff_d = (dpage != 0) ? 0x00000 : 0x18000;
						if(display_page_bak == 1) yoff_d += 0xc000;
						draw_window(dmode, yy + y,
									wx_begin, ((wx_end >= wpixels) ? wpixels : wx_end) - wx_begin,
									true, scan_line);
						if(wx_end < wpixels) {
							yoff_d = (dpage != 0) ? 0x18000 : 0x00000;
							if(display_page_bak == 1) yoff_d += 0xc000;
							draw_window(dmode, yy + y, wx_end,  wpixels - wx_end,
										false, scan_line);
						}
					} else {
						yoff_d = (dpage != 0) ? 0x00000 : 0x18000;
						if(display_page_bak == 1) yoff_d += 0xc000;
						draw_window(dmode, yy + y, 0, wx_end,
									false, scan_line);
						if(wx_end < wpixels) {
							yoff_d = (dpage != 0) ? 0x18000 : 0x00000;
							if(display_page_bak == 1) yoff_d += 0xc000;
							draw_window(dmode, yy + y, wx_end , wpixels - wx_end,
										true, scan_line);
						}
					}						
				} else
#    endif
				{
					
					yoff_d = (dpage != 0) ? 0x18000 : 0x00000;
					if(display_page_bak == 1) yoff_d += 0xc000;
					draw_window(dmode, yy + y, 0, wpixels, false, scan_line);
				}
				// Copy line
#elif defined(_FM77AV_VARIANTS)
				yoff_d = 0;
				if(display_page_bak == 1) yoff_d += 0xc000;
				draw_window(dmode, yy + y, 0, wpixels, false, scan_line);
#else
				yoff_d = 0;
				draw_window(dmode, yy + y, 0, wpixels, false, scan_line);
#endif
		}
	}
	if(ff) force_update = false;
	return;
}


bool DISPLAY::screen_update(void)
{
	if(crt_flag) {
		bool f = screen_update_flag;
		screen_update_flag = false;
		return f;
	} else {
		if(crt_flag_bak) return true;
	}
	return false;
}

void DISPLAY::reset_screen_update(void)
{
	screen_update_flag = false;
}

void DISPLAY::CopyDrawnData(scrntype_t* src, scrntype_t* dst, int width, bool scan_line)
{
	if(dst == NULL) return;
	if(src == NULL) return;
#if defined(_RGB555) || defined(_RGBA565)
	static const int shift_factor = 2;
#else // 24bit
	static const int shift_factor = 3;
#endif
	scrntype_vec8_t* vsrc = (scrntype_vec8_t*)src;
	scrntype_vec8_t* vdst = (scrntype_vec8_t*)dst;
	__DECL_ALIGNED(32) scrntype_vec8_t tmp_dd;
	__DECL_ALIGNED(32) scrntype_vec8_t sline;
	
	if(scan_line) {
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 8; i++) {
			sline.w[i] = (scrntype_t)RGBA_COLOR(31, 31, 31, 255);
		}
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < width; i++) {
			tmp_dd.v = vsrc[i].v;
			tmp_dd.v = tmp_dd.v >> shift_factor;
			tmp_dd.v = tmp_dd.v & sline.v;
			vdst[i].v = tmp_dd.v;
		}
	} else {
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < width; i++) {
			tmp_dd.v = vsrc[i].v;
			vdst[i].v = tmp_dd.v;
		}
	}
}


#if defined(_FM77L4)
scrntype_t DISPLAY::GETVRAM_TEXTCOLOR(uint8_t attr, bool do_green)
{
	int color = attr & 0x07;
	int r, g, b;

	static const int green_g_table[16] = {0, 24, 48, 64, 80, 96, 112, 128,
										  140, 155, 175, 186, 210, 220, 240, 255};
	if(do_green) {
		if((attr & 0x20) != 0) color += 8;
		r = b = 0;
		g = green_g_table[color];
		if(color >= 10) {
			r = (color - 9) * 16;
			b = (color - 9) * 16;
		}
	} else {
		if((attr & 0x20) != 0) {
			g = ((color & 4) != 0) ? 255 : 0;
			r = ((color & 2) != 0) ? 255 : 0;
			b = ((color & 1) != 0) ? 255 : 0;
		} else {
			g = ((color & 4) != 0) ? 128 : 0;
			r = ((color & 2) != 0) ? 128 : 0;
			b = ((color & 1) != 0) ? 128 : 0;
		}
	}
	return RGBA_COLOR(r, g, b, 255);
}

uint8_t DISPLAY::GETVRAM_TEXTPIX(uint8_t bitdata, bool reverse, bool cursor_rev)
{
	uint8_t ret = bitdata;
	if(reverse) {
		ret = (uint8_t)(~ret);
	}
	if(cursor_rev) {
	    ret = (uint8_t)(~ret);
	}
	return ret;
}

void DISPLAY::GETVRAM_1_400L(int yoff, scrntype_t *p)
{
	uint8_t pixel;
	if(p == NULL) return;
	yoff_d = yoff & 0x7fff;
	pixel = gvram_shadow[yoff_d];
	uint16_vec8_t *ppx = (uint16_vec8_t *)(&(bit_trans_table_0[pixel][0]));
	__DECL_ALIGNED(16) uint16_vec8_t tmp_d;
	__DECL_ALIGNED(32) scrntype_vec8_t tmp_dd;
	scrntype_vec8_t *vp = (scrntype_vec8_t *)p;

	tmp_d.v = ppx->v;
	tmp_d.v = tmp_d.v >> 5;
	
__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 8; i++) {
		tmp_dd.w[i] = dpalette_pixel[tmp_d.w[i]];
	}

	vp->v = tmp_dd.v;
}

void DISPLAY::GETVRAM_1_400L_GREEN(int yoff, scrntype_t *p)
{
	uint8_t pixel;
	if(p == NULL) return;
	yoff_d = yoff & 0x7fff;
	pixel = gvram_shadow[yoff_d];
	uint16_vec8_t *ppx = (uint16_vec8_t *)(&(bit_trans_table_0[pixel][0]));
	__DECL_ALIGNED(16) uint16_vec8_t tmp_d;
	__DECL_ALIGNED(32) scrntype_vec8_t tmp_dd;
	scrntype_vec8_t *vp = (scrntype_vec8_t *)p;

	tmp_d.v = ppx->v;
	tmp_d.v = tmp_d.v >> 5;
	
__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 8; i++) {
		tmp_dd.w[i] = dpalette_pixel_green[tmp_d.w[i]];
	}
	vp->v = tmp_dd.v;

}
#endif


#if defined(_FM77AV_VARIANTS)
void DISPLAY::GETVRAM_4096(int yoff, scrntype_t *p, scrntype_t *px,
						   uint32_t mask,
						   bool window_inv,
						   bool scan_line)
{
	uint32_t b3, r3, g3;
	uint8_t  bb[4], rr[4], gg[4];
	__DECL_ALIGNED(16) uint16_vec8_t pixels;
	__DECL_ALIGNED(16) const uint16_t __masks[8] = {(uint16_t)mask, (uint16_t)mask, (uint16_t)mask, (uint16_t)mask, (uint16_t)mask, (uint16_t)mask, (uint16_t)mask, (uint16_t)mask};
	scrntype_t b, r, g;
	uint32_t idx;;
	scrntype_t pixel;
# if defined(_FM77AV40EX) || defined(_FM77AV40SX)
	int dpage = vram_display_block;
# endif
	if(p == NULL) return;
	
	yoff_d1 = yoff;
	yoff_d2 = yoff;
# if defined(_FM77AV40EX) || defined(_FM77AV40SX)
	if(window_inv) {
		if(dpage == 0) {
			dpage = 1;
		} else {
			dpage = 0;
		}
	}
	if(dpage != 0) {
		yoff_d1 += 0x18000;
		yoff_d2 += 0x18000;
	}
# endif
	bb[0] = gvram_shadow[yoff_d1];
	bb[1] = gvram_shadow[yoff_d1 + 0x02000];
	rr[0] = gvram_shadow[yoff_d1 + 0x04000];
	rr[1] = gvram_shadow[yoff_d1 + 0x06000];
	gg[0] = gvram_shadow[yoff_d1 + 0x08000];
	gg[1] = gvram_shadow[yoff_d1 + 0x0a000];
		
	bb[2] = gvram_shadow[yoff_d2 + 0x0c000];
	bb[3] = gvram_shadow[yoff_d2 + 0x0e000];
	rr[2] = gvram_shadow[yoff_d2 + 0x10000];
	rr[3] = gvram_shadow[yoff_d2 + 0x12000];
	gg[2] = gvram_shadow[yoff_d2 + 0x14000];
	gg[3] = gvram_shadow[yoff_d2 + 0x16000];

	uint16_t *p0, *p1, *p2, *p3;
#if !defined(FIXED_FRAMEBUFFER_SIZE)
	__DECL_ALIGNED(sizeof(scrntype_t) * 8) scrntype_t tmp_dd[8];
#else
	__DECL_ALIGNED(sizeof(scrntype_t) * 8) scrntype_t tmp_dd[16];
#endif
	__DECL_ALIGNED(16) uint16_vec8_t tmp_g, tmp_r, tmp_b;
	__v8hi *vp0, *vp1, *vp2, *vp3;
	// G
	vp0 = (__v8hi*)(&(bit_trans_table_0[gg[0]][0]));
	vp1 = (__v8hi*)(&(bit_trans_table_1[gg[1]][0]));
	vp2 = (__v8hi*)(&(bit_trans_table_2[gg[2]][0]));
	vp3 = (__v8hi*)(&(bit_trans_table_3[gg[3]][0]));
	tmp_g.v = *vp0;
	tmp_g.v = tmp_g.v | *vp1;
	tmp_g.v = tmp_g.v | *vp2;
	tmp_g.v = tmp_g.v | *vp3;
	// R
	vp0 = (__v8hi*)(&(bit_trans_table_0[rr[0]][0]));
	vp1 = (__v8hi*)(&(bit_trans_table_1[rr[1]][0]));
	vp2 = (__v8hi*)(&(bit_trans_table_2[rr[2]][0]));
	vp3 = (__v8hi*)(&(bit_trans_table_3[rr[3]][0]));
	tmp_r.v = *vp0;
	tmp_r.v = tmp_r.v | *vp1;
	tmp_r.v = tmp_r.v | *vp2;
	tmp_r.v = tmp_r.v | *vp3;

	// B
	vp0 = (__v8hi*)(&(bit_trans_table_0[bb[0]][0]));
	vp1 = (__v8hi*)(&(bit_trans_table_1[bb[1]][0]));
	vp2 = (__v8hi*)(&(bit_trans_table_2[bb[2]][0]));
	vp3 = (__v8hi*)(&(bit_trans_table_3[bb[3]][0]));
	tmp_b.v = *vp0;
	tmp_b.v = tmp_b.v | *vp1;
	tmp_b.v = tmp_b.v | *vp2;
	tmp_b.v = tmp_b.v | *vp3;
	
	__v8hi *mp = (__v8hi*)__masks;
	tmp_g.v = tmp_g.v << 4;
	tmp_b.v = tmp_b.v >> 4;
	pixels.v = tmp_b.v;
	pixels.v = pixels.v | tmp_r.v;
	pixels.v = pixels.v | tmp_g.v;
	pixels.v = pixels.v & *mp;
	

	scrntype_vec8_t *vp = (scrntype_vec8_t*)p;
	scrntype_vec8_t *dp = (scrntype_vec8_t*)tmp_dd;
#if !defined(FIXED_FRAMEBUFFER_SIZE)
__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 8; i++) {
		tmp_dd[i] = analog_palette_pixel[pixels[i]];
	}
	vp->v = dp->v;
#else
__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 8; i++) {
		tmp_dd[i * 2] = tmp_dd[i * 2 + 1] = analog_palette_pixel[pixels.w[i]];;
	}
	scrntype_vec8_t *vpx = (scrntype_vec8_t*)px;
	__DECL_ALIGNED(32) scrntype_vec8_t vmask;
__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 2; i++) {
		vp[i].v = dp[i].v;
	}
	if(scan_line) {
/* Fancy scanline */
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 2; i++) {
#if defined(_RGB888) || defined(_RGBA888)
			dp[i].v = dp[i].v >> 3;
#elif defined(_RGB555)
			dp[i].v = dp[i].v >> 2;
#elif defined(_RGB565)
			dp[i].v = dp[i].v >> 2;
#endif
		}
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 8; i++) {
			vmask.w[i] = (const scrntype_t)RGBA_COLOR(31, 31, 31, 255);
		}
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 2; i++) {
			dp[i].v = dp[i].v & vmask.v; 
			vpx[i].v = dp[i].v;
		}
	} else {
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 2; i++) {
			vpx[i].v = dp[i].v;
		}
	}
#endif	
}
#endif

#if defined(_FM77AV40) || defined(_FM77AV40EX) || defined(_FM77AV40SX)

void DISPLAY::GETVRAM_256k(int yoff, scrntype_t *p, scrntype_t *px, bool scan_line)
{
	uint32_t b3, r3, g3;
	uint32_t b4, r4, g4;
	uint32_t btmp, rtmp, gtmp;
	
	scrntype_t b, r, g;
	scrntype_t pixel;
	uint32_t _bit;
	int _shift;
	int cp;
	if(p == NULL) return;
	
	r3 = g3 = b3 = 0;
	r4 = g4 = b4 = 0;
	r = g = b = 0;
	
	yoff_d1 = yoff;
	yoff_d2 = yoff;

	uint8_t  bb[8], rr[8], gg[8];

	__DECL_ALIGNED(16) uint16_vec8_t _btmp;
	__DECL_ALIGNED(16) uint16_vec8_t _rtmp;
	__DECL_ALIGNED(16) uint16_vec8_t _gtmp;
	uint16_vec8_t *vp0, *vp1, *vp2, *vp3, *vp4, *vp5;
#if !defined(FIXED_FRAMEBUFFER_SIZE)
	__DECL_ALIGNED(sizeof(scrntype_t) * 8) scrntype_t tmp_dd[8];
#else
	__DECL_ALIGNED(sizeof(scrntype_t) * 8) scrntype_t tmp_dd[16];
#endif
//	if(mask & 0x01) {
	if(!multimode_dispflags[0]) {
		// B
		bb[0] = gvram_shadow[yoff_d1];
		bb[1] = gvram_shadow[yoff_d1 + 0x02000];
		
		bb[2] = gvram_shadow[yoff_d2 + 0x0c000];
		bb[3] = gvram_shadow[yoff_d2 + 0x0e000];
	
		bb[4] = gvram_shadow[yoff_d1 + 0x18000];
		bb[5] = gvram_shadow[yoff_d1 + 0x1a000];
		
		vp0 = (uint16_vec8_t*)(&(bit_trans_table_0[bb[0]][0]));
		vp1 = (uint16_vec8_t*)(&(bit_trans_table_1[bb[1]][0]));
		vp2 = (uint16_vec8_t*)(&(bit_trans_table_2[bb[2]][0]));
		vp3 = (uint16_vec8_t*)(&(bit_trans_table_3[bb[3]][0]));
		vp4 = (uint16_vec8_t*)(&(bit_trans_table_4[bb[4]][0]));
		vp5 = (uint16_vec8_t*)(&(bit_trans_table_5[bb[5]][0]));
		_btmp.v = vp0->v;
		_btmp.v = _btmp.v | vp1->v;
		_btmp.v = _btmp.v | vp2->v;
		_btmp.v = _btmp.v | vp3->v;
		_btmp.v = _btmp.v | vp4->v;
		_btmp.v = _btmp.v | vp5->v;
	} else {
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 8; i++) {
			_btmp.w[i] = 0;
		}
	}
	if(!multimode_dispflags[1]) {
		//if(mask & 0x02) {
		// R
		rr[0] = gvram_shadow[yoff_d1 + 0x04000];
		rr[1] = gvram_shadow[yoff_d1 + 0x06000];
		
		rr[2] = gvram_shadow[yoff_d2 + 0x10000];
		rr[3] = gvram_shadow[yoff_d2 + 0x12000];
	
		rr[4] = gvram_shadow[yoff_d1 + 0x1c000];
		rr[5] = gvram_shadow[yoff_d1 + 0x1e000];
		
		vp0 = (uint16_vec8_t*)(&(bit_trans_table_0[rr[0]][0]));
		vp1 = (uint16_vec8_t*)(&(bit_trans_table_1[rr[1]][0]));
		vp2 = (uint16_vec8_t*)(&(bit_trans_table_2[rr[2]][0]));
		vp3 = (uint16_vec8_t*)(&(bit_trans_table_3[rr[3]][0]));
		vp4 = (uint16_vec8_t*)(&(bit_trans_table_4[rr[4]][0]));
		vp5 = (uint16_vec8_t*)(&(bit_trans_table_5[rr[5]][0]));
		_rtmp.v = vp0->v;
		_rtmp.v = _rtmp.v | vp1->v;
		_rtmp.v = _rtmp.v | vp2->v;
		_rtmp.v = _rtmp.v | vp3->v;
		_rtmp.v = _rtmp.v | vp4->v;
		_rtmp.v = _rtmp.v | vp5->v;
	} else {
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 8; i++) {
			_rtmp.w[i] = 0;
		}
	}
	if(!multimode_dispflags[2]) {
		//if(mask & 0x04) {
		// G
		gg[0] = gvram_shadow[yoff_d1 + 0x08000];
		gg[1] = gvram_shadow[yoff_d1 + 0x0a000];
		
		gg[2] = gvram_shadow[yoff_d2 + 0x14000];
		gg[3] = gvram_shadow[yoff_d2 + 0x16000];
	
		gg[4] = gvram_shadow[yoff_d1 + 0x20000];
		gg[5] = gvram_shadow[yoff_d1 + 0x22000];
		
		vp0 = (uint16_vec8_t*)(&(bit_trans_table_0[gg[0]][0]));
		vp1 = (uint16_vec8_t*)(&(bit_trans_table_1[gg[1]][0]));
		vp2 = (uint16_vec8_t*)(&(bit_trans_table_2[gg[2]][0]));
		vp3 = (uint16_vec8_t*)(&(bit_trans_table_3[gg[3]][0]));
		vp4 = (uint16_vec8_t*)(&(bit_trans_table_4[gg[4]][0]));
		vp5 = (uint16_vec8_t*)(&(bit_trans_table_5[gg[5]][0]));
		_gtmp.v = vp0->v;
		_gtmp.v = _gtmp.v | vp1->v;
		_gtmp.v = _gtmp.v | vp2->v;
		_gtmp.v = _gtmp.v | vp3->v;
		_gtmp.v = _gtmp.v | vp4->v;
		_gtmp.v = _gtmp.v | vp5->v;
	} else {
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 8; i++) {
			_gtmp.w[i] = 0;
		}
	}

	scrntype_vec8_t* vpp = (scrntype_vec8_t*)p;
	scrntype_vec8_t* dp = (scrntype_vec8_t*)tmp_dd;
#if !defined(FIXED_FRAMEBUFFER_SIZE)
__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 8; i++) {
		tmp_dd[i] = RGB_COLOR(_rtmp.w[i], _gtmp.w[i], _btmp.w[i]);
	}
	vpp->v = dp->v;
#else
__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 8; i++) {
		tmp_dd[i * 2] = tmp_dd[i * 2 + 1] = RGB_COLOR(_rtmp.w[i], _gtmp.w[i], _btmp.w[i]);
	}

__DECL_VECTORIZED_LOOP
	for(int i = 0; i < 2; i++) {
		vpp[i].v = dp[i].v;
	}
	scrntype_vec8_t* vpx = (scrntype_vec8_t*)px;
	if(scan_line) {
/* Fancy scanline */
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 2; i++) {
#if defined(_RGB888) || defined(_RGBA888)
			dp[i].v = dp[i].v >> 3;
#elif defined(_RGB555)
			dp[i].v = dp[i].v >> 2;
#elif defined(_RGB565)
			dp[i].v = dp[i].v >> 2;
#endif
		}
		__DECL_ALIGNED(32) scrntype_vec8_t scanline_data;
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 8; i++) {
			scanline_data.w[i] = RGBA_COLOR(31, 31, 31, 255);
		}
__DECL_VECTORIZED_LOOP
		for(int i = 0; i < 2; i++) {
			dp[i].v = dp[i].v & scanline_data.v;
			vpx[i].v = dp[i].v;
		}
	} else {
		for(int i = 0; i < 2; i++) {
			vpx[i].v = dp[i].v;
		}
	}
#endif	
}
#endif

}
