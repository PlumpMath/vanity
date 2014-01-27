/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/common.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Provides common types and headers
    for the GL objects.
*/

#pragma once

#include "core.h"
#include "check.h"

namespace gl
{
  using name_t = GLuint;

  enum class buffer_target : GLenum
  {
    array = GL_ARRAY_BUFFER,
    atomic_counter = GL_ATOMIC_COUNTER_BUFFER,
    copy_read = GL_COPY_READ_BUFFER,
    copy_write = GL_COPY_WRITE_BUFFER,
    draw_indirect = GL_DRAW_INDIRECT_BUFFER,
    dispatch_indirect = GL_DISPATCH_INDIRECT_BUFFER,
    element_array = GL_ELEMENT_ARRAY_BUFFER,
    pixel_pack = GL_PIXEL_PACK_BUFFER,
    pixel_unpack = GL_PIXEL_UNPACK_BUFFER,
    query = GL_QUERY_BUFFER,
    shader_storage = GL_SHADER_STORAGE_BUFFER,
    texture = GL_TEXTURE_BUFFER,
    transform_feedback = GL_TRANSFORM_FEEDBACK_BUFFER, 
    uniform = GL_UNIFORM_BUFFER
  };

  enum class texture_target : GLenum
  {
    _1d = GL_TEXTURE_1D,
    _2d = GL_TEXTURE_2D,
    _3d = GL_TEXTURE_3D,
    _1d_array = GL_TEXTURE_1D_ARRAY,
    _2d_array = GL_TEXTURE_2D_ARRAY,
    rectangle = GL_TEXTURE_RECTANGLE,
    cube_map = GL_TEXTURE_CUBE_MAP,
    cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
    buffer = GL_TEXTURE_BUFFER,
    _2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,
    _2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
  };

  enum class texture_format : GLenum
  {
    red = GL_RED,
    rg = GL_RG,
    rgb = GL_RGB,
    bgr = GL_BGR,
    rgba = GL_RGBA,
    bgra = GL_BGRA,
    red_integer = GL_RED_INTEGER,
    rg_integer = GL_RG_INTEGER,
    rgb_integer = GL_RGB_INTEGER,
    bgr_integer = GL_BGR_INTEGER,
    rgba_integer = GL_RGBA_INTEGER,
    bgra_integer = GL_BGRA_INTEGER,
    stencil_index = GL_STENCIL_INDEX,
    depth_component = GL_DEPTH_COMPONENT,
    depth_stencil = GL_DEPTH_STENCIL
  };
}
