#ifndef __LAVA_MESH__
#define __LAVA_MESH__

#ifdef LAVA_USE_ASSIMP

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/mesh.h>

#include <lava/api.h>

#include "../vulkan.hpp"

namespace lava
{
  namespace extras
  {
    struct Vertex
    {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texCoord;

      static vk::VertexInputBindingDescription getBindingDescription( )
      {
        vk::VertexInputBindingDescription bindingDescription = {};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof( Vertex );
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
      }

      static std::array< vk::VertexInputAttributeDescription, 2 > getAttributeDescriptions( )
      {
        std::array< vk::VertexInputAttributeDescription, 2 > attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof( Vertex, position );

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof( Vertex, normal );

        //attributeDescriptions[2].binding = 0;
        //attributeDescriptions[2].location = 2;
        //attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        //attributeDescriptions[2].offset = offsetof( Vertex, texCoord );

        //attributeDescriptions[3].binding = 0;
        //attributeDescriptions[3].location = 3;
        //attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        //attributeDescriptions[3].offset = offsetof( Vertex, color );

        return attributeDescriptions;
      }
    };
    class Mesh
    {
    public:
      LAVA_API
      Mesh( const aiMesh *mesh );
    public:
      uint32_t numVertices;
      uint32_t numIndices;
      std::vector< Vertex > vertices;
      std::vector< uint32_t > indices;
    };
  }
}

#endif

#endif /* __LAVA_MESH__ */
