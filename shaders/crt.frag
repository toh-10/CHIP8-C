#version 330 core

#define BLUR_SAMPLES 15
#define pi 3.141592
#define pow2(x) (x * x)

in vec2 iuv;
out vec3 color;
uniform sampler2D myTextureSampler;
uniform float iTime;
uniform vec2 iResolution;

float rand(vec2 uv) {
  return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 white_noise(vec2 uv) {
  uv *= sin(iTime);
  float r = rand(uv);
  vec3 noise = vec3(r);

  return noise;
}

vec3 scan_line(vec3 col, vec2 uv) {
  float scanLineColor = sin(iTime * 10 + uv.y * 500) / 2 + 0.5;
  col *= 0.1 + clamp(scanLineColor + 0.5, 0, 1) * 0.5;

  float _ScanLineSpeed = 50;
  float _ScanLineTail = 1.8;
  float tail =
      clamp((fract(uv.y + iTime * _ScanLineSpeed) - 1 + _ScanLineTail) /
                min(_ScanLineTail, 1),
            0, 1);

  return col *= tail;
}

vec2 screenDistort(vec2 uv, float distortion) {
  uv -= vec2(.5, .5);
  uv *= vec2(pow(length(uv), distortion));
  uv += vec2(.5, .5);

  return uv;
}

vec3 cell(vec2 uv) {
  float hres = 70., vres = 40.;
  vec2 cell, local;

  cell.x = floor(uv.x * hres) / hres;
  cell.y = floor(uv.y * vres) / vres;

  local.x = fract(uv.x * hres);
  local.y = fract(uv.y * vres);

  float mask_pix =
      1. - max(smoothstep(0.75, 0.8, local.x), smoothstep(0.75, 0.8, local.y));

  vec3 mask_sub;
  mask_sub.r = smoothstep(0.251, 0.25, local.x);
  mask_sub.b = smoothstep(0.50, 0.501, local.x);
  mask_sub.g = smoothstep(0.25, 0.251, local.x) - mask_sub.b;

  return mask_pix * mask_sub;
}

float gaussian(float x, float sigma) {
  return (1.0 / sqrt(2.0 * pi * pow2(sigma))) *
         exp(-(pow2(x) / (2.0 * pow2(sigma))));
}

vec3 blur(sampler2D sp, vec2 uv, vec2 scale) {
  vec4 col = vec4(0.0);
  float accum, weight, offset;

  for (int i = -BLUR_SAMPLES / 2; i < BLUR_SAMPLES / 2; ++i) {
    offset = float(i);
    weight = gaussian(offset, sqrt(float(BLUR_SAMPLES)));
    col += texture(sp, uv + scale * offset) * weight;
    accum += weight;
  }

  return (col / accum).xyz;
}

vec3 bloom(vec3 col, vec2 uv) {
  vec2 ps = vec2(1.0) / iResolution.xy;
  col *= blur(myTextureSampler, uv, vec2(0.0, ps.y));
  col *= blur(myTextureSampler, uv, vec2(ps.x, 0.0));

  return col;
}

vec3 overlay(vec3 src, vec3 dst) {
  return vec3((dst.r <= 0.5) ? (2.0 * src.r * dst.r)
                             : (1.0 - 2.0 * (1.0 - dst.r) * (1.0 - src.r)),
              (dst.g <= 0.5) ? (2.0 * src.g * dst.g)
                             : (1.0 - 2.0 * (1.0 - dst.g) * (1.0 - src.g)),
              (dst.b <= 0.5) ? (2.0 * src.b * dst.b)
                             : (1.0 - 2.0 * (1.0 - dst.b) * (1.0 - src.b)));
}

vec3 vignette(vec3 col, vec2 uv) {
  float vigAmt = 3. + .3 * sin(iTime + 5. * cos(iTime * 5.));
  float vignette = (1. - vigAmt * (uv.y - .5) * (uv.y - .5)) *
                   (1. - vigAmt * (uv.x - .5) * (uv.x - .5));
  col *= vignette;
  col *= (12. + mod(uv.y * 30. + iTime, 1.)) / 13.;

  return col;
}

void main() {
  vec2 uv = screenDistort(iuv, 0.03);
  vec3 col = texture(myTextureSampler, uv).rgb;

  col = mix(col, overlay(col, bloom(col, uv)), 0.8);
  col = mix(col, cell(uv), 0.01);
  col += mix(col, white_noise(uv), 0.05);
  col += scan_line(col, uv);
  col = vignette(col, uv);

  color = col;
}
