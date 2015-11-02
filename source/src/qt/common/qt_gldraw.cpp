/*
 * qt_gldraw.cpp
 * (c) 2011 K.Ohta <whatisthis.sowhat@gmail.com>
 * Modified to Common Source code Project, License is changed to GPLv2.
 * 
 */


#include "emu.h"


#include <QtGui>
#if defined(USE_BUTTON)
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QRect>
#endif
//#include <SDL/SDL.h>
#if defined(_WINDOWS) || defined(Q_OS_WIN32)
#include <GL/gl.h>
#include <GL/glext.h>
#else
# if !defined(_USE_GLAPI_QT5_4) || !defined(_USE_GLAPI_QT5_1)  
#  include <GL/glx.h>
#  include <GL/glxext.h>
# endif
#endif
#include <GL/glu.h>

#undef _USE_OPENCL
#ifdef _USE_OPENCL
//# include "agar_glcl.h"
#endif


#ifdef USE_OPENMP
#include <omp.h>
#endif //_OPENMP
#include "qt_gldraw.h"
#include "agar_logger.h"


void GLDrawClass::SetBrightRGB(float r, float g, float b)
{
	fBrightR = r;
	fBrightG = g;
	fBrightB = b;
}




#ifdef _USE_OPENCL
extern class GLCLDraw *cldraw;
extern void InitContextCL(void);
#endif

void GLDrawClass::drawGridsHorizonal(void)
{
	//req_draw_grids_vert = false;
	extfunc->glLineWidth(0.1f);
	{
		QVector4D c;
		c = QVector4D(0.0, 0.0, 0.0, 1.0);
		grids_shader->setUniformValue("color", c);
	}
	if(vertex_grid_horizonal->isCreated()) {
		extfunc->glDisable(GL_TEXTURE_2D);
		vertex_grid_horizonal->bind();
		extfunc->glEnableVertexAttribArray(0);
		extfunc->glEnable(GL_VERTEX_ARRAY);
		extfunc->glDrawArrays(GL_LINES, 0, vert_lines);
		vertex_grid_horizonal->release();
	}
}

void GLDrawClass::drawGridsVertical(void)
{
	  //req_draw_grids_horiz = false;
	extfunc->glLineWidth(0.5f);
	{
		QVector4D c;
		c = QVector4D(0.0, 0.0, 0.0, 1.0);
		grids_shader->setUniformValue("color", c);
	}
	if(vertex_grid_vertical->isCreated()) {
		extfunc->glDisable(GL_TEXTURE_2D);
		extfunc->glClear(GL_COLOR_BUFFER_BIT);
		vertex_grid_vertical->bind();
		extfunc->glEnable(GL_VERTEX_ARRAY);
		extfunc->glDrawArrays(GL_LINES, 0, horiz_pixels);
		vertex_grid_vertical->release();
	}
}


void GLDrawClass::drawGrids(void)
{
	extfunc->glDisable(GL_TEXTURE_2D);
	extfunc->glEnable(GL_DEPTH_TEST);
	extfunc->glDisable(GL_BLEND);
#if 0	
	extfunc->glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
#endif	
	if(gl_grid_horiz && (vert_lines > 0) && (glHorizGrids != NULL) && req_draw_grids_vert) {
		this->drawGridsHorizonal();
	}

	if(gl_grid_vert && (horiz_pixels > 0) && (glVertGrids != NULL) && req_draw_grids_horiz) {
		this->drawGridsVertical();
	}
}
void GLDrawClass::adjustBrightness()
{

	QVector4D c;
	c = QVector4D(fBrightR, fBrightG, fBrightB, 1.0);
	main_shader->setUniformValue("color", c);
}

#if defined(USE_BUTTON)
void GLDrawClass::drawButtons()
{
	int i;
	GLfloat Vertexs[4][3];
	updateButtonTexture();
	extfunc->glEnable(GL_TEXTURE_2D);
	extfunc->glEnable(GL_DEPTH_TEST);
	for(i = 0; i < MAX_BUTTONS; i++) {
		if(vertex_button[i]->isCreated()) {
			extfunc->glClear(GL_COLOR_BUFFER_BIT);
			extfunc->glEnable(GL_TEXTURE_2D);
			extfunc->glEnable(GL_DEPTH_TEST);
			//extfunc->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			vertex_button->bind();
			buffer_button_vertex[i]->bind();
			VertexTexCoord_t vt;
			vt[0].x =  fButtonX[i];
			vt[0].y =  fButtonY[i];
			vt[0].z =  -0.2f;
			vt[0].s = 0.0f;
			vt[0].t = 1.0f;
		
			vt[1].x =  fButtonX[i] + fButtonWidth[i];
			vt[1].y =  fButtonY[i];
			vt[1].z =  -0.2f;
			vt[1].s = 1.0f;
			vt[1].t = 1.0f;
		   
			vt[2].x =  fButtonX[i] + fButtonWidth[i];
			vt[2].y =  fButtonY[i] - fButtonHeight[i];
			vt[2].z =  -0.2f;
			vt[2].s = 1.0f;
			vt[2].t = 0.0f;
		   
			vt[3].x =  fButtonX[i];
			vt[3].y =  fButtonY[i] - fButtonHeight[i];
			vt[3].z =  -0.2f;
			vt[3].s = 0.0f;
			vt[3].t = 0.0f;
		   
			buffer_bitmap_vertex->write(0, vt, 4 * sizeof(VertexTexCoord_t));
			bitmap_shader->bind();
			{
				QVector4D c;
				c = QVector4D(1.0, 1.0, 0.0, 1.0);
				button_shader->setUniformValue("color", c);
			}
			extfunc->glActiveTexture(GL_TEXTURE0);
# if defined(_USE_GLAPI_QT5_4)
			uButtonTextureID[i]->bind();
# else
			extfunc->glBindTexture(GL_TEXTURE_2D, uButtonTextureID[i]);
# endif
			//smoosing = config.use_opengl_filters;
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			button_shader->setUniformValue("a_texture", 0);
			button_shader->enableAttributeArray("texcoord");
			button_shader->enableAttributeArray("vertex");
			extfunc->glEnableVertexAttribArray(0);
			extfunc->glEnableVertexAttribArray(1);
			extfunc->glEnable(GL_VERTEX_ARRAY);
			
			extfunc->glDrawArrays(GL_POLYGON, 0, 4);
			buffer_button_vertex[i]->release();
			vertex_button->release();
		
			button_shader->release();
			extfunc->glBindTexture(GL_TEXTURE_2D, 0);
			extfunc->glDisable(GL_TEXTURE_2D);
		}
	}
}
#endif
#if defined(_USE_GLAPI_QT5_4)

# ifdef USE_BITMAP
void GLDrawClass::drawBitmapTexture(void)
{
	extfunc->glEnable(GL_TEXTURE_2D);
	if(uBitmapTextureID->isCreated()) {
		uBitmapTextureID->bind();

		extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#if 0		
		extfunc->glBegin(GL_POLYGON);
		extfunc->glTexCoord2f(TexCoords[0][0], TexCoords[0][1]);
		extfunc->glVertex3f(BitmapVertexs[0][0], BitmapVertexs[0][1], BitmapVertexs[0][2]);
	
		extfunc->glTexCoord2f(TexCoords[1][0], TexCoords[1][1]);
		extfunc->glVertex3f(BitmapVertexs[1][0], BitmapVertexs[1][1], BitmapVertexs[1][2]);
	 
		extfunc->glTexCoord2f(TexCoords[2][0], TexCoords[2][1]);
		extfunc->glVertex3f(BitmapVertexs[2][0], BitmapVertexs[2][1], BitmapVertexs[2][2]);
		
		extfunc->glTexCoord2f(TexCoords[3][0], TexCoords[3][1]);
		extfunc->glVertex3f(BitmapVertexs[3][0], BitmapVertexs[3][1], BitmapVertexs[3][2]);
		extfunc->glEnd();
#else
		{
			QVector4D c;
			c = QVector4D(1.0, 1.0, 1.0, 1.0);
			bitmap_shader->setUniformValue("color", c);
		}
		if(vertex_bitmap->isCreated()) {
			main_shader->setUniformValue("texture", 0);
			//main_shader->setUniformValue("v_texcoord", *texture_texcoord);
			extfunc->glClear(GL_COLOR_BUFFER_BIT);
			vertex_bitmap->bind();
			extfunc->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			vertex_bitmap->release();
		}
#endif		
		uBitmapTextureID->release();
	}
	extfunc->glDisable(GL_TEXTURE_2D);
}
# endif	

void GLDrawClass::drawScreenTexture(void)
{
# ifdef USE_BITMAP
	if(uBitmapTextureID->isCreated()) {
		extfunc->glEnable(GL_BLEND);
		extfunc->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
# endif
	smoosing = config.use_opengl_filters;
	if(uVramTextureID->isCreated()) {
		extfunc->glEnable(GL_TEXTURE_2D);
		uVramTextureID->bind();
#if 1
		main_shader->bind();
		extfunc->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		main_shader->setUniformValue("texture", 0);
		//main_shader->setUniformValue("v_texcoord", *texture_texcoord);
#endif		
		if(!smoosing) {
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		} else {
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		if(vertex_screen->isCreated()) {
			//extfunc->glClear(GL_COLOR_BUFFER_BIT);
			vertex_screen->bind();
			main_shader->setUniformValue("texture", uVramTextureID->textureId());
			int vertex_loc = main_shader->attributeLocation("position");
			main_shader->enableAttributeArray(vertex_loc);
			main_shader->setAttributeArray(vertex_loc, ScreenVertexs, 4);
			
			int texcoord_loc = main_shader->attributeLocation("a_texcoord");
			main_shader->enableAttributeArray(texcoord_loc);
			main_shader->setAttributeArray(texcoord_loc, TexCoords, 4);
		
			extfunc->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			main_shader->disableAttribArray(texcoord_loc);
			main_shader->disableAttribArray(vertex_loc);
			vertex_acreen->release();
		}
		uVramTextureID->release();
		extfunc->glDisable(GL_TEXTURE_2D);
#if 1
		main_shader->release();
#endif		
	}
}
#else // Not _GLAPI_QT_5_4
# ifdef USE_BITMAP
void GLDrawClass::drawBitmapTexture(void)
{
	if(uBitmapTextureID != 0) {
		extfunc->glEnable(GL_TEXTURE_2D);
		extfunc->glEnable(GL_DEPTH_TEST);
		extfunc->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		vertex_bitmap->bind();
		buffer_bitmap_vertex->bind();
		bitmap_shader->bind();
		{
			QVector4D c;
			c = QVector4D(1.0, 1.0, 0.0, 1.0);
			bitmap_shader->setUniformValue("color", c);
		}
		extfunc->glActiveTexture(GL_TEXTURE0);
		extfunc->glBindTexture(GL_TEXTURE_2D, uBitmapTextureID);
		//smoosing = config.use_opengl_filters;
		extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		bitmap_shader->setUniformValue("a_texture", 0);
		bitmap_shader->enableAttributeArray("texcoord");
		bitmap_shader->enableAttributeArray("vertex");
		extfunc->glEnableVertexAttribArray(0);
		extfunc->glEnableVertexAttribArray(1);
		extfunc->glEnable(GL_VERTEX_ARRAY);

		extfunc->glDrawArrays(GL_POLYGON, 0, 4);
		buffer_bitmap_vertex->release();
		vertex_bitmap->release();
		
		bitmap_shader->release();
		extfunc->glBindTexture(GL_TEXTURE_2D, 0);
		extfunc->glDisable(GL_TEXTURE_2D);
	}
}
# endif


void GLDrawClass::drawScreenTexture(void)
{
#ifdef USE_BITMAP
	if(uBitmapTextureID != 0) {
		extfunc->glEnable(GL_BLEND);
		extfunc->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
#else
	extfunc->glDisable(GL_BLEND);
#endif
	
	if(uVramTextureID != 0) {
		extfunc->glEnable(GL_TEXTURE_2D);
		extfunc->glEnable(GL_DEPTH_TEST);
#ifndef USE_BITMAP	   
		extfunc->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif	   
		vertex_screen->bind();
		buffer_screen_vertex->bind();
		main_shader->bind();
		extfunc->glActiveTexture(GL_TEXTURE0);
		extfunc->glBindTexture(GL_TEXTURE_2D, uVramTextureID);
		smoosing = config.use_opengl_filters;
		if(!smoosing) {
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} else {
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			extfunc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		main_shader->setUniformValue("a_texture", 0);
		main_shader->enableAttributeArray("texcoord");
		main_shader->enableAttributeArray("vertex");
		extfunc->glEnableVertexAttribArray(0);
		extfunc->glEnableVertexAttribArray(1);
		extfunc->glEnable(GL_VERTEX_ARRAY);

		extfunc->glDrawArrays(GL_POLYGON, 0, 4);
		buffer_screen_vertex->release();
		vertex_screen->release();
		
		main_shader->release();
		extfunc->glBindTexture(GL_TEXTURE_2D, 0);
		extfunc->glDisable(GL_TEXTURE_2D);
	}
}
#endif

void GLDrawClass::drawUpdateTexture(QImage *p)
{
	if((p != NULL)) {
#if defined(_USE_GLAPI_QT5_4)   
		if(uVramTextureID->isCreated()) {
	  		uVramTextureID->destroy();
			uVramTextureID->create();
		}
		uVramTextureID->setData(*p);
#else
		if(uVramTextureID != 0) {
			extfunc->glBindTexture(GL_TEXTURE_2D, uVramTextureID);
			extfunc->glTexSubImage2D(GL_TEXTURE_2D, 0,
						 0, 0,
						 p->width(), p->height(),
						 GL_RGBA, GL_UNSIGNED_BYTE, p->constBits());
			extfunc->glBindTexture(GL_TEXTURE_2D, 0);
	  		//this->deleteTexture(uVramTextureID);
		} else {
			uVramTextureID = this->bindTexture(*p);
		}
	   
#endif
	}
//#ifdef _USE_OPENCL
}

#if defined(USE_BUTTON)
void GLDrawClass::updateButtonTexture(void)
{
	int i;
   	QImage *img;
   	QPainter *painter;
	QColor col;
	QRect rect;
	QPen *pen;
	if(button_updated) return;
	col.setRgb(0, 0, 0, 255);
	pen = new QPen(col);
	for(i = 0; i < MAX_BUTTONS; i++) {
		img = new QImage(buttons[i].width, buttons[i].height, QImage::Format_RGB32);
		painter = new QPainter(img);
		//painter->setRenderHint(QPainter::Antialiasing, true);
		col.setRgb(255, 255, 255, 255);
		painter->fillRect(0, 0, buttons[i].width, buttons[i].height, col);
		//painter->setPen(pen);
		rect.setWidth(buttons[i].width);
		rect.setHeight(buttons[i].height);
		rect.setX(0);
		rect.setY(0);
		painter->drawText(rect, Qt::AlignCenter, QString::fromUtf8(buttons[i].caption));
#if defined(_USE_GLAPI_QT5_4)   
		if( uButtonTextureID[i]->isCreated()) {
	  		uButtonTextureID[i]->destroy();
			uButtonTextureID[i]->create();
		} else {
			uButtonTextureID[i]->create();
		}
		uButtonTextureID[i]->setData(*img);
#else
		if(uButtonTextureID[i] != 0) {
	  		this->deleteTexture(uButtonTextureID[i]);
		}
		uButtonTextureID[i] = this->bindTexture(*img);;
#endif
		delete painter;
		delete img;
	}
	delete pen;
	button_updated = true;
}
#endif

#if defined(USE_BITMAP)
void GLDrawClass::uploadBitmapTexture(QImage *p)
{
	int i;
	if(p == NULL) return;
	if(!bitmap_uploaded) {
#if defined(_USE_GLAPI_QT5_4)   
		if(uBitmapTextureID->isCreated()) {
	  		uBitmapTextureID->destroy();
			uBitmapTextureID->create();
		}
		uBitmapTextureID->setData(*p);
#else
		if(uBitmapTextureID != 0) {
	  		this->deleteTexture(uBitmapTextureID);
		}
		uBitmapTextureID = this->bindTexture(*p);
#endif	   
		bitmap_uploaded = true;
		crt_flag = true;
	}
}

void GLDrawClass::updateBitmap(QImage *p)
{
	bitmap_uploaded = false;
	uploadBitmapTexture(p);
}
#endif

void GLDrawClass::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	double ww, hh;
	int w, h;
#if !defined(USE_BUTTON)
# ifdef USE_SCREEN_ROTATE
	if(config.rotate_type) {
		int tmp;
		tmp = width;
		width = height;
		height = tmp;
	}
# endif
#endif
	extfunc->glViewport(0, 0, width, height);
	crt_flag = true;

	req_draw_grids_horiz = true;
	req_draw_grids_vert = true;
	
	if(draw_width  < ((horiz_pixels * 4) / 2)) req_draw_grids_horiz = false;
	if(draw_height < ((vert_lines   * 2) / 2))   req_draw_grids_vert = false;
	{
		int i;
		GLfloat yf;
		GLfloat xf;
		GLfloat delta;
		
		yf = -screen_height;
		delta = (2.0f * screen_height) / (float)vert_lines;
		yf = yf - delta * 0.75f;
		if(glHorizGrids != NULL) {
			if(vert_lines > SCREEN_HEIGHT) vert_lines = SCREEN_HEIGHT;
			for(i = 0; i < (vert_lines + 1) ; i++) {
				glHorizGrids[i * 6]     = -screen_width; // XBegin
				glHorizGrids[i * 6 + 3] = +screen_width; // XEnd
				glHorizGrids[i * 6 + 1] = yf; // YBegin
				glHorizGrids[i * 6 + 4] = yf; // YEnd
				glHorizGrids[i * 6 + 2] = 0.1f; // ZBegin
				glHorizGrids[i * 6 + 5] = 0.1f; // ZEnd
				yf = yf + delta;
			}
		}
		if(vertex_grid_horizonal->isCreated()) {
			vertex_grid_horizonal->bind();
			buffer_grid_horizonal->bind();
			buffer_grid_horizonal->write(0, glHorizGrids, (vert_lines + 1) * 6 * sizeof(GLfloat));
			//extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			buffer_grid_horizonal->release();
			vertex_grid_horizonal->release();
		}
		xf = -screen_width; 
		delta = (2.0f * screen_width) / (float)horiz_pixels;
		xf = xf - delta * 0.75f;
		if(glVertGrids != NULL) {
			if(horiz_pixels > SCREEN_WIDTH) horiz_pixels = SCREEN_WIDTH;
			for(i = 0; i < (horiz_pixels + 1) ; i++) {
				glVertGrids[i * 6]     = xf; // XBegin
				glVertGrids[i * 6 + 3] = xf; // XEnd
				glVertGrids[i * 6 + 1] = -screen_height; // YBegin
				glVertGrids[i * 6 + 4] =  screen_height; // YEnd
				glVertGrids[i * 6 + 2] = 0.1f; // ZBegin
				glVertGrids[i * 6 + 5] = 0.1f; // ZEnd
				xf = xf + delta;
			}
			if(vertex_grid_vertical->isCreated()) {
				vertex_grid_vertical->bind();
				buffer_grid_vertical->bind();
				buffer_grid_vertical->write(0, glVertGrids, (horiz_pixels + 1) * 6 * sizeof(GLfloat));
				//extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
				buffer_grid_vertical->release();
				vertex_grid_vertical->release();
			}
		}
	}
	if(vertex_screen->isCreated()) {
		vertexFormat[0].x = -screen_width;
		vertexFormat[0].y = -screen_height;
	   
		vertexFormat[1].x = +screen_width;
		vertexFormat[1].y = -screen_height;
	   
		vertexFormat[2].x = +screen_width;
		vertexFormat[2].y = +screen_height;
	   
		vertexFormat[3].x = -screen_width;
		vertexFormat[3].y = +screen_height;
	   
		vertex_screen->bind();
		buffer_screen_vertex->bind();
		buffer_screen_vertex->write(0, vertexFormat, sizeof(vertexFormat));
		//extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
		buffer_screen_vertex->release();
		vertex_screen->release();
	}

	
#if defined(USE_BITMAP)	
	if(vertex_bitmap->isCreated()) {
		vertexBitmap[0].x = -screen_width;
		vertexBitmap[0].y = -screen_height;
	   
		vertexBitmap[1].x = +screen_width;
		vertexBitmap[1].y = -screen_height;
	   
		vertexBitmap[2].x = +screen_width;
		vertexBitmap[2].y = +screen_height;
	   
		vertexBitmap[3].x = -screen_width;
		vertexBitmap[3].y = +screen_height;
	   
		vertex_bitmap->bind();
		buffer_bitmap_vertex->bind();
		buffer_bitmap_vertex->write(0, vertexFormat, sizeof(vertexFormat));
		//extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		extfunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
		buffer_bitmap_vertex->release();
		vertex_bitmap->release();
	}
#endif
	
	AGAR_DebugLog(AGAR_LOG_DEBUG, "ResizeGL: %dx%d", width , height);
	emit sig_resize_uibar(width, height);
}


/*
 * "Paint" Event handler
 */

void GLDrawClass::paintGL(void)
{
	int i;
	if(!crt_flag) return;
	if(p_emu != NULL) {
		//if(imgptr == NULL) return;
		drawUpdateTexture(imgptr);
		crt_flag = false;
	}
#if 0	
	extfunc->glPushAttrib(GL_TEXTURE_BIT);
	extfunc->glPushAttrib(GL_TRANSFORM_BIT);
	extfunc->glPushAttrib(GL_ENABLE_BIT);
#endif	
#ifdef _USE_OPENCL
	//    InitContextCL();   
#endif

	extfunc->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	extfunc->glEnable(GL_DEPTH_TEST);
	extfunc->glDisable(GL_BLEND);

#ifdef USE_BITMAP
	drawBitmapTexture();
#endif	
#ifdef USE_SCREEN_ROTATE   
	extfunc->glPushMatrix();
#endif   
	/*
	 * VRAMの表示:テクスチャ貼った四角形
	 */
	drawScreenTexture();
	extfunc->glEnable(GL_DEPTH_TEST);
	extfunc->glDisable(GL_BLEND);

	if(set_brightness) {
		adjustBrightness();
	}
	drawGrids();
#if defined(USE_BUTTON)
	drawButtons();
#endif	
#ifdef USE_SCREEN_ROTATE   
	extfunc->glPopMatrix();
#endif   
	extfunc->glDisable(GL_DEPTH_TEST);
#ifdef USE_OPENGL
	//DrawOSDGL(glv);
#endif
#if 0	
	extfunc->glPopAttrib();
	extfunc->glPopAttrib();
	extfunc->glPopAttrib();
#endif	
	extfunc->glFlush();
}

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLDrawClass::GLDrawClass(QWidget *parent)
#if defined(_USE_GLAPI_QT5_4)
  : QOpenGLWidget(parent, Qt::Widget)
#else
  : QGLWidget(parent)
#endif
{
	
#if defined(_USE_GLAPI_QT5_4)   
	uVramTextureID = new QOpenGLTexture(QOpenGLTexture::Target2D);
#else
	uVramTextureID = 0;
#endif
	imgptr = NULL;
	p_emu = NULL;
	extfunc = NULL;
#ifdef USE_BITMAP
# if defined(_USE_GLAPI_QT5_4)   
	uBitmapTextureID = new QOpenGLTexture(QOpenGLTexture::Target2D);
# else
	uBitmapTextureID = 0;
# endif
	bitmap_uploaded = false;
#endif
#ifdef USE_BUTTON
	int i;
	for(i = 0; i < MAX_BUTTONS; i++) {
# if defined(_USE_GLAPI_QT5_4)   
		uButtonTextureID[i] = new QOpenGLTexture(QOpenGLTexture::Target2D);
# else	   
		uButtonTextureID[i] = 0;
# endif	   
		fButtonX[i] = -1.0 + (float)(buttons[i].x * 2) / (float)SCREEN_WIDTH;
		fButtonY[i] = 1.0 - (float)(buttons[i].y * 2) / (float)SCREEN_HEIGHT;
		fButtonWidth[i] = (float)(buttons[i].width * 2) / (float)SCREEN_WIDTH;
		fButtonHeight[i] = (float)(buttons[i].height * 2) / (float)SCREEN_HEIGHT;
	}
	button_updated = false;
	button_drawn = false;
#endif
	req_draw_grids_horiz = false;
	req_draw_grids_vert = false;
	fBrightR = 1.0; // 輝度の初期化
	fBrightG = 1.0;
	fBrightB = 1.0;
	set_brightness = false;
	crt_flag = false;
	smoosing = false;
	
	gl_grid_horiz = false;
	gl_grid_vert = false;
	glVertGrids = NULL;
	glHorizGrids = NULL;

	vert_lines = SCREEN_HEIGHT;
	horiz_pixels = SCREEN_WIDTH;
	enable_mouse = true;
	screen_width = 1.0;
	screen_height = 1.0;
#ifdef _USE_OPENCL
	bInitCL = false;
	nCLGlobalWorkThreads = 10;
	bCLSparse = false; // true=Multi threaded CL,false = Single Thread.
	nCLPlatformNum = 0;
	nCLDeviceNum = 0;
	bCLInteropGL = false;
    //bCLDirectMapping = false;
#endif
	this->setFocusPolicy(Qt::StrongFocus);
	this->initKeyCode();
}

GLDrawClass::~GLDrawClass()
{
//	this->releaseKeyCode();
#if defined(_USE_GLAPI_QT5_4)   
	if(uVramTextureID->isCreated()) {
  		uVramTextureID->destroy();
	}
	delete uVramTextureID;
#else
	this->deleteTexture(uVramTextureID);
#endif     
#ifdef USE_BITMAP
# if defined(_USE_GLAPI_QT5_4)   
	if(uBitmapTextureID->isCreated()) {
  		uBitmapTextureID->destroy();
	}
	delete uBitmapTextureID;
# else   
	if(uBitmapTextureID != 0) {
  		this->deleteTexture(uBitmapTextureID);
	}
# endif 
#endif
#ifdef USE_BUTTON
	int i;
	for(i = 0; i < MAX_BUTTONS; i++) {
# if defined(_USE_GLAPI_QT5_4)   
		if(uButtonTextureID[i]->isCreated()) uButtonTextureID[i]->destroy();
		delete uButtonTextureID[i];
# else
		if(uButtonTextureID[i] != 0) this->deleteTexture(uButtonTextureID[i]);
# endif
	}
#endif
#if 1
	if(buffer_screen_vertex->isCreated()) buffer_screen_vertex->destroy();
	if(vertex_screen->isCreated()) vertex_screen->destroy();
	if(vertex_grid_vertical->isCreated()) vertex_grid_vertical->destroy();
	if(vertex_grid_horizonal->isCreated()) vertex_grid_horizonal->destroy();
# if defined(USE_BITMAP)
	if(vertex_bitmap->isCreated()) vertex_bitmap->destroy();
# endif
# if defined(USE_BUTTON)
	for(i = 0; i < MAX_BUTTONS; i++) {
		if(vertex_button[i]->isCreated()) vertex_button[i]->destroy();
	}
# endif	
#endif
	if(glVertGrids != NULL) free(glVertGrids);
	if(glHorizGrids != NULL) free(glHorizGrids);
	if(extfunc != NULL) delete extfunc;
}

void GLDrawClass::setEmuPtr(EMU *p)
{
	p_emu = p;
}

QSize GLDrawClass::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLDrawClass::sizeHint() const
{
	return QSize(400, 400);
}

QSize GLDrawClass::getCanvasSize(void)
{
	return QSize(this->width(), this->height());
}

QSize GLDrawClass::getDrawSize(void)
{
	return QSize(draw_width, draw_height);
}

