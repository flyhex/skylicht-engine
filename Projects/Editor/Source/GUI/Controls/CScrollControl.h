/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/
#pragma once

#include "CBase.h"
#include "CScrollBar.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CScrollControl : public CBase
			{
			protected:
				CScrollBar *m_vertical;
				CScrollBar *m_horizontal;

				bool m_canScrollV;
				bool m_canScrollH;

				float m_widthScrollExpand;
				float m_heightScrollExpand;
			public:
				CScrollControl(CBase *parent);

				~CScrollControl();

				virtual void layout();

				virtual void onChildBoundsChanged(const SRect& oldChildBounds, CBase* child);

				virtual bool onMouseWheeled(int delta);

				virtual void onScrollBarV(CBase *base);

				virtual void onScrollBarH(CBase *base);

				void scrollToItem(CBase *item);

				void showScrollBar(bool h, bool v);

				void enableScroll(bool h, bool v);

				float getInnerWidth();

				float getInnerHeight();

				inline CScrollBar* getVerticalSroll()
				{
					return m_vertical;
				}

				inline CScrollBar* getHorizontalSroll()
				{
					return m_horizontal;
				}

				void scrollVerticalOffset(float offset);

				void scrollHorizontalOffset(float offset);

			protected:
				void updateScrollBar();

			};
		}
	}
}