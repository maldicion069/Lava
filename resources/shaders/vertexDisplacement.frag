#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in float noise;
layout(binding = 1) uniform sampler2D tExplosion;

layout (location = 0) out vec4 fragColor;

float random( vec3 scale, float seed ){
  return fract( sin( dot( gl_FragCoord.xyz + seed, scale ) ) * 43758.5453 + seed ) ;
}

void main( )
{
  // get a random offset
  float r = .01 * random( vec3( 12.9898, 78.233, 151.7182 ), 0.0 );
  // lookup vertically in the texture, using noise and offset
  // to get the right RGB colour
  vec2 tPos = vec2( 0, 1.3 * noise + r );
  vec4 color = texture( tExplosion, tPos );

  fragColor = vec4( color.rgb, 1.0 );
  fragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
}