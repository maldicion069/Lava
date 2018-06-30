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

#ifndef __LAVAENGINE_DUMP_VISITOR__
#define __LAVAENGINE_DUMP_VISITOR__

#include "Visitor.h"
#include <string>

#include <lavaEngine/api.h>

namespace lava
{
  namespace engine
  {
    class DumpVisitor :
      public Visitor
    {
    public:
      LAVAENGINE_API
      virtual void traverse( Node *node ) override;
      LAVAENGINE_API
      virtual void visitNode( Node *node ) override;
      LAVAENGINE_API
      virtual void visitGroup( Group *group ) override;
      LAVAENGINE_API
      virtual void visitGeometry( Geometry *geometry ) override;
      LAVAENGINE_API
      virtual void visitCamera( Camera *camera ) override;
      LAVAENGINE_API
      virtual void visitLight( Light *light ) override;
    private:
      void _dumpNode( Node *node, const std::string& type );
      unsigned int _auxLevel = 0;
    };
  }
}

#endif /* __LAVAENGINE_DUMP_VISITOR__ */
