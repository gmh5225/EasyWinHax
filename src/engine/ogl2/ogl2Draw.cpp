#include "ogl2Draw.h"
#include "ogl2Font.h"
#include "..\Engine.h"
#include <Windows.h>

namespace hax {

	namespace ogl2 {

		Draw::Draw() : _hGameContext{}, _hHookContext{}, _width{}, _height{}, _isInit{} {}


		Draw::~Draw() {

			if (this->_hHookContext) {
				wglDeleteContext(this->_hHookContext);
			}

		}


		void Draw::beginDraw(Engine* pEngine) {
			const HDC hDc = reinterpret_cast<HDC>(pEngine->pHookArg);

			if (!this->_isInit) {
				this->_hGameContext = wglGetCurrentContext();
				this->_hHookContext = wglCreateContext(hDc);
				
				if (!this->_hGameContext || !this->_hHookContext) return;
				
				this->_isInit = true;
			}

			wglMakeCurrent(hDc, this->_hHookContext);

			GLint viewport[4]{};
			glGetIntegerv(GL_VIEWPORT, viewport);

			if (this->_width == viewport[2] && this->_height == viewport[3]) return;

			this->_width = viewport[2];
			this->_height = viewport[3];
			
			pEngine->setWindowSize(this->_width, this->_height);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, static_cast<double>(this->_width), static_cast<double>(this->_height), 0, 1, -1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glClearColor(0.f, 0.f, 0.f, 1.f);

			return;
		}


		void Draw::endDraw(const Engine* pEngine) const {

			if (!this->_isInit) return;

			wglMakeCurrent(reinterpret_cast<HDC>(pEngine->pHookArg), this->_hGameContext);

			return;
		}

		void Draw::drawTriangleStrip(const Vector2 corners[], UINT count, rgb::Color color) const {
			
			if (!this->_isInit) return;
			
			glColor3ub(UCHAR_R(color), UCHAR_G(color), UCHAR_B(color));
			glBegin(GL_TRIANGLE_STRIP);

			for (UINT i = 0; i < count; i++) {
				glVertex2f(corners[i].x, corners[i].y);
			}

			glEnd();

			return;
        }


		void Draw::drawString(void* pFont, const Vector2* pos, const char* text, rgb::Color color) const {
			
			if (!this->_isInit) return;

			Font* pOgl2Font = reinterpret_cast<Font*>(pFont);
			GLsizei strLen = static_cast<GLsizei>(strlen(text));
			
			if (!pOgl2Font || strLen > 100) return;

			glColor3ub(UCHAR_R(color), UCHAR_G(color), UCHAR_B(color));
			glRasterPos2f(pos->x, pos->y);
			glPushAttrib(GL_LIST_BIT);
			glListBase(pOgl2Font->displayLists - 32);
			glCallLists(strLen, GL_UNSIGNED_BYTE, text);
			glPopAttrib();

            return;
		}

	}

}