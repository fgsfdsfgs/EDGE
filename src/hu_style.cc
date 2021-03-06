//----------------------------------------------------------------------------
//  EDGE Heads-up-display Style code
//----------------------------------------------------------------------------
// 
//  Copyright (c) 2004-2009  The EDGE Team.
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------

#include "system/i_defs.h"
#include "hu_style.h"
#include "hu_draw.h"

#include "dm_defs.h"
#include "dm_state.h"
#include "r_local.h"
#include "r_colormap.h"
#include "r_draw.h"
#include "r_modes.h"
#include "r_image.h"

// Edge has lots of style
style_container_c hu_styles;


style_c::style_c(styledef_c *_def) : def(_def), bg_image(NULL)
{
	for (int T = 0; T < styledef_c::NUM_TXST; T++)
		fonts[T] = NULL;
}

style_c::~style_c()
{
	/* nothing to do */
}


void style_c::Load()
{
	if (def->bg.image_name.c_str())
	{
		const char *name = def->bg.image_name.c_str();

		bg_image = W_ImageLookup(name, INS_Flat, ILF_Null);

		if (! bg_image)
			bg_image = W_ImageLookup(name, INS_Graphic);
	}

	for (int T = 0; T < styledef_c::NUM_TXST; T++)
	{
		if (def->text[T].font)
			fonts[T] = hu_fonts.Lookup(def->text[T].font);
	}
}


//-CA 
//CONSBK is for drawing the console! not drawbackground for generic drawing!
void style_c::DrawBackground(int x, int y, int w, int h, int align)
{
	if (w == 0)
		x = 0, w = SCREENWIDTH;

	if (h == 0)
		y = 0, h = SCREENHEIGHT;

	float alpha = PERCENT_2_FLOAT(def->bg.translucency);

	if (alpha < 0.02)
		return;

	if (! bg_image)
	{
		if (def->bg.colour != RGB_NO_VALUE)
			HUD_SolidBox(x, y, w, h, def->bg.colour);
		return;
	}

	float right = IM_RIGHT(bg_image);
	float top   = IM_TOP(bg_image);

	if (def->special & SYLSP_Tiled)
	{
		float y_scale = def->bg.scale;
		float x_scale = def->bg.aspect * y_scale;

		x_scale *= (float)SCREENWIDTH  / 320.0f;
		y_scale *= (float)SCREENHEIGHT / 200.0f;

		RGL_DrawImage(x, y, w, h, bg_image,
				0.0f, align ? (1.0f - top * h / IM_HEIGHT(bg_image) / y_scale) : 0.0f,
				right  * w / IM_WIDTH(bg_image)  / x_scale,
				align ? 1.0f : (top * h / IM_HEIGHT(bg_image) / y_scale),
				NULL);
	}
	else if (def->special & SYLSP_TiledNoScale)
	{
		RGL_DrawImage(x, y, w, h, bg_image,
				0.0f, align ? (1.0f - top * h / IM_HEIGHT(bg_image)) : 0.0f,
				right  * w / IM_WIDTH(bg_image),
				align ? 1.0f : (top * h / IM_HEIGHT(bg_image)),
				NULL, alpha);
	}
	else
	{
		RGL_DrawImage(x, y, w, h, bg_image, 0.0f, 0.0f,
					  right, top, NULL, alpha);
	}
}

// ---> style_container_c class

//
// style_container_c::CleanupObject()
//
void style_container_c::CleanupObject(void *obj)
{
	style_c *a = *(style_c**)obj;

	if (a) delete a;
}

//
// style_container_c::Lookup()
//
// Never returns NULL.
//
style_c* style_container_c::Lookup(styledef_c *def)
{
	SYS_ASSERT(def);

	for (epi::array_iterator_c it = GetIterator(0); it.IsValid(); it++)
	{
		style_c *st = ITERATOR_TO_TYPE(it, style_c*);

		if (def == st->def)
			return st;
	}

	style_c *new_st = new style_c(def);

	new_st->Load();
	Insert(new_st);

	return new_st;
}


//
// HL_WriteText
//
// Compatibility crud...
//
void HL_WriteText(style_c *style, int text_type, int x, int y, const char *str, float scale)
{
	HUD_SetFont(style->fonts[text_type]);
	HUD_SetScale(scale * style->def->text[text_type].scale);

	const colourmap_c *colmap = style->def->text[text_type].colmap;

	if (colmap)
		HUD_SetTextColor(V_GetFontColor(colmap));

	HUD_DrawText(x, y, str);

	HUD_SetFont();
	HUD_SetScale();
	HUD_SetTextColor();
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
