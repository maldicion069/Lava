#include "VulkanRenderAPI.h"
using namespace vklava;

int main( )
{
  VulkanRenderAPI vr;
  vr.initialize( );
  vr.run( );
  vr.cleanup( );
  system( "PAUSE" );
  return 0;
}