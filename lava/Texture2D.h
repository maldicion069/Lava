/**
 * Copyright (c) 2017 - 2018, Lava
 * All rights reserved.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef __LAVA_TEXTURE2D__
#define __LAVA_TEXTURE2D__

#include "includes.hpp"

#include "CommandBuffer.h"
#include "Queue.h"
#include "Texture.h"

#include <lava/api.h>

namespace lava
{
  class Texture2D: public Texture
  {
  public:
    LAVA_API
    Texture2D( const std::shared_ptr<Device>& device, const std::string& filename,
      const std::shared_ptr<CommandPool>& cmdPool, 
      const std::shared_ptr<Queue>& queue, vk::Format format,
      vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eSampled,
      vk::ImageLayout imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
      bool forceLinear = false );
    LAVA_API
    Texture2D( const std::shared_ptr<Device>& device, 
      vk::ArrayProxy<void*> data/*, short nChannels*/,
      const std::shared_ptr<CommandPool>& cmdPool,
      const std::shared_ptr<Queue>& queue, vk::Format format,
      vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eSampled,
      vk::ImageLayout imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
      bool forceLinear = false );
  private:
    void createTexture( void* data, uint32_t width, uint32_t height, 
      short nChannels, const std::shared_ptr<CommandPool>& cmdPool,
      const std::shared_ptr<Queue>& queue, vk::Format format,
      vk::ImageUsageFlags imageUsageFlags, vk::ImageLayout imageLayout,
      bool forceLinear );
  };
}

#endif /* __LAVA_TEXTURE2D__ */