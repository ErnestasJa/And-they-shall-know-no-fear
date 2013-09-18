/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    Mark Page
*/


#pragma once

#include "api_swrender.h"
#include "../Core/Math/vec2.h"
#include "../Core/Math/vec4.h"
#include "../Core/Math/mat4.h"

namespace clan
{
/// \addtogroup clanSWRender_Display clanSWRender Display
/// \{

class PixelPipeline;
class PixelCommand;

/// \brief Interface for commands participating in drawing primitives
class SoftwareProgram
{
//!Operations
public:
	virtual ~SoftwareProgram() { }

	virtual int get_attribute_count() const = 0;
	virtual int get_attribute_index(const std::string &name) const = 0;
	virtual Vec4f get_attribute_default(int index) { return Vec4f(0.0f, 0.0f, 1.0f, 1.0f); }
	virtual void set_uniform(int location, const Vec4f &vec) = 0;
	virtual void set_uniform_matrix(int location, const Mat4f &mat) = 0;

	virtual PixelCommand *draw_triangle(PixelPipeline *pipeline, const std::vector<Vec4f> &attribute_values) = 0;
	virtual PixelCommand *draw_sprite(PixelPipeline *pipeline, const std::vector<Vec4f> &attribute_values) = 0;
	virtual PixelCommand *draw_line(PixelPipeline *pipeline, const std::vector<Vec4f> &attribute_values) = 0;
};

}

/// \}
