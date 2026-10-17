// Author: Jinsong Zhao
// Mail:zhaojs-c@online.sh.cn

#ifndef __I_GUI_FONT_VECTOR_H_INCLUDED__
#define __I_GUI_FONT_VECTOR_H_INCLUDED__
#ifdef _IRR_COMPILE_WITH_FREETYPE_
#include "IGUIFont.h"

namespace irr
{
namespace gui
{

//! Font interface.
class IGUIFontVector : public IGUIFont
{
public:

	//! Destructor
	virtual ~IGUIFontVector() {}

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() const { return EGFT_VECTOR; }

	virtual IGUIFontVector* getNewSizeFont(u32 width,u32 height) = 0;
	//virtual void setTransform(u32 width) const = 0;
};

} // end namespace gui
} // end namespace irr
#endif
#endif

