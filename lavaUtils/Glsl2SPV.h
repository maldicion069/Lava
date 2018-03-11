#ifndef __LAVA_UTILS_GLSL2SPV__
#define __LAVA_UTILS_GLSL2SPV__

#include <lava/lava.h>
#include <lavaUtils/api.h>
#include <vector>

#include <glslang/SPIRV/GlslangToSpv.h>

namespace lava
{
	namespace utility
	{
    class GLSLToSPIRVCompiler
    {
    public:
      GLSLToSPIRVCompiler( void );
      ~GLSLToSPIRVCompiler( void );

      std::vector<uint32_t> compile( vk::ShaderStageFlagBits stage,
        const std::string& source ) const;

    private:
      TBuiltInResource  _resource;
    };
  	std::vector< uint32_t > compileGLSLToSPIRV( 
  		vk::ShaderStageFlagBits stage, const std::string& source );
	}
}

#endif /* __LAVA_UTILS_GLSL2SPV__ */