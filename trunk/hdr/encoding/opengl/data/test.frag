uniform sampler2D EncodingPattern;
const float pattern_scale = 1.0 / 4.0;

uniform sampler2D LSBPattern;


vec3 encode(float data, vec3 color) {
 /* Get the encoded data corresponding to this
   * pixel in screen coordinates. We look this
   * up from an RGB texture's channels:
   *   R: Bit mask
   *   B: Alignment pattern
   */
  vec4 pattern = texture2D(EncodingPattern, gl_FragCoord.xy * pattern_scale);

  bool data_bit = fract(data / pattern.r) >= 0.5;

  /* Encode data in the least significant bits: one data bit per pixel,
   * a checksum bit, and an alignment bit.
   */
  vec3 lsb = vec3(data_bit ? 1.0 : 0.0,
		  data_bit ? pattern.b : (1 - pattern.b),
		  pattern.b);

  /* Use another lookup table to quantize the image and set LSB values.
   */
  return vec3(texture2D(LSBPattern, vec2(color.r, lsb.r)).r,
	      texture2D(LSBPattern, vec2(color.g, lsb.g)).r,
	      texture2D(LSBPattern, vec2(color.b, lsb.b)).r);
}


void main(void) {
  float gradient = floor(gl_FragCoord.x / 4.0) * 4.0;
  float data = saturate(gradient / 640.0);

  gl_FragColor = vec4(encode(data, vec3(gl_Color)), 1.0);
}
