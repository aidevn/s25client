// $Id: glArchivItem_Bitmap.h 6921 2010-12-22 08:19:30Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.
#ifndef GLARCHIVITEM_BITMAP_INCLUDED
#define GLARCHIVITEM_BITMAP_INCLUDED

#pragma once

#include "colors.h"

#ifdef _MSC_VER
typedef unsigned long long uint64_t;
#endif

/// Basisklasse f�r GL-Bitmapitems.
class glArchivItem_Bitmap : public virtual libsiedler2::baseArchivItem_Bitmap
{
public:
	/// Konstruktor von @p glArchivItem_Bitmap.
	glArchivItem_Bitmap(void);
	/// Kopiekonstruktor von @p glArchivItem_Bitmap.
	glArchivItem_Bitmap(const glArchivItem_Bitmap *item);

	/// Destruktor von @p glArchivItem_Bitmap.
	virtual ~glArchivItem_Bitmap(void);

	/// Erzeugt und zeichnet die Textur.
	virtual void Draw(short dst_x, short dst_y, short dst_w = 0, short dst_h = 0, short src_x = 0, short src_y = 0, short src_w = 0, short src_h = 0, const unsigned int color = COLOR_WHITE, const unsigned int unused = COLOR_WHITE);

	/// liefert das GL-Textur-Handle.
	unsigned int GetTexture();
	/// L�scht die GL-Textur (z.B f�rs Neuerstellen)
	virtual void DeleteTexture();
	/// Setzt den Texturfilter auf einen bestimmten Wert.
	virtual void setFilter(unsigned int filter);

protected:
	/// Erzeugt die Textur.
	virtual void GenerateTexture();
	/// Zeichnet einen Vertex inkl Texturkoordinaten.
	inline void DrawVertex(float x, float y, float tx, float ty)
	{
		glTexCoord2f(tx/tex_width, ty/tex_height);
		glVertex2f(x, y);
	}
protected:
	unsigned int texture; ///< Das GL-Textur-Handle
	unsigned int filter;  ///< Der aktuell gew�hlte Texturfilter

	typedef std::map<uint64_t, unsigned int> calllistmap;
	typedef std::map<uint64_t, calllistmap> calllistmapmap;

	calllistmapmap calllists;
};

#endif // !GLARCHIVITEM_BITMAP_INCLUDED
