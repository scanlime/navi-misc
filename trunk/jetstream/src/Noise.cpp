/*
 * Noise.cpp - A library of noise functions
 *
 *  References:
 *     http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
 *     http://www.robo-murito.net/code/perlin-noise-math-faq.html
 *
 * Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include "Noise.h"
#include "Util.h"


void PerlinNoise3::set(int seed, int octaves, float persistence, int logTableSize) {
  gradTable = std::vector<Vector3>(1<<logTableSize);
  tableMask = (1 << logTableSize) - 1;
  this->seed = seed;
  this->octaves = octaves;
  this->persistence = persistence;

  /* Initialize the random gradient table with random unit vectors.
   * Note that these aren't uniformly distributed!
   * We could start with an arbitrary vector and perform a random
   * rotation to get uniformly distributed vectors, but this is good enough.
   */
  for (int i=0; i<gradTable.size(); i++) {
    float a = noise(seed++);
    float b = noise(seed++);
    float c = noise(seed++);
    gradTable[i] = Vector3(a,b,c);
    gradTable[i].normalize();
  }

#if 0 /* Debug code - write a slice of the noise pattern to disk */
  FILE *f = fopen("noisetest.pnm","wb");
  fprintf(f,"P6\n640 960\n255\n");
  for (int y=0;y<480;y++)
    for (int x=0;x<640;x++) {
      Vector3 v((x-320)/80.0, (y-240)/80.0, 0);
      float val = getValue(v);
      fputc((int)(127+96*val), f);
      fputc((int)(127+96*val), f);
      fputc((int)(127+96*val), f);
    }
  for (int y=0;y<480;y++)
    for (int x=0;x<640;x++) {
      Vector3 v((x-320)/80.0, (y-240)/80.0, 0);
      Vector3 grad = getGradient(v);
      fputc((int)(127+127*grad[0]), f);
      fputc((int)(127+127*grad[1]), f);
      fputc((int)(127+127*grad[2]), f);
    }
  fclose(f);
#endif
}

inline float PerlinNoise3::noise(int n) {
  n = (n<<13) ^ n;
  return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);   
}

inline Vector3 &PerlinNoise3::tableNoise3(int x, int y, int z) {
  return gradTable[ (x + y*95 + z*127) & tableMask ];
}

float PerlinNoise3::smoothNoise3(const Vector3 &point) {
  int ip0 = (int) floor(point[0]);
  int ip1 = (int) floor(point[1]);
  int ip2 = (int) floor(point[2]);  

  /* Get a random gradient for each */
  Vector3 &g000 = tableNoise3(ip0  ,ip1  ,ip2  );
  Vector3 &g001 = tableNoise3(ip0  ,ip1  ,ip2+1);
  Vector3 &g010 = tableNoise3(ip0  ,ip1+1,ip2  );
  Vector3 &g011 = tableNoise3(ip0  ,ip1+1,ip2+1);
  Vector3 &g100 = tableNoise3(ip0+1,ip1  ,ip2  );
  Vector3 &g101 = tableNoise3(ip0+1,ip1  ,ip2+1);
  Vector3 &g110 = tableNoise3(ip0+1,ip1+1,ip2  );
  Vector3 &g111 = tableNoise3(ip0+1,ip1+1,ip2+1);

  /* Get a vector from each grid point to the point we're evaluating */
  Vector3 v000(point[0] - (ip0  ), point[1] - (ip1  ), point[2] - (ip2  ));
  Vector3 v001(point[0] - (ip0  ), point[1] - (ip1  ), point[2] - (ip2+1));
  Vector3 v010(point[0] - (ip0  ), point[1] - (ip1+1), point[2] - (ip2  ));
  Vector3 v011(point[0] - (ip0  ), point[1] - (ip1+1), point[2] - (ip2+1));
  Vector3 v100(point[0] - (ip0+1), point[1] - (ip1  ), point[2] - (ip2  ));
  Vector3 v101(point[0] - (ip0+1), point[1] - (ip1  ), point[2] - (ip2+1));
  Vector3 v110(point[0] - (ip0+1), point[1] - (ip1+1), point[2] - (ip2  ));
  Vector3 v111(point[0] - (ip0+1), point[1] - (ip1+1), point[2] - (ip2+1));

  /* Dot products to calculate the influence of each grid point */
  float i000 = g000 * v000;
  float i001 = g001 * v001;
  float i010 = g010 * v010;
  float i011 = g011 * v011;
  float i100 = g100 * v100;
  float i101 = g101 * v101;
  float i110 = g110 * v110;
  float i111 = g111 * v111;

  /* Cubic interpolation in three dimensions */
  float r0   = v000[0];
  float r1   = v000[1];
  float r2   = v000[2];
  float s0   = 3*r0*r0 - 2*r0*r0*r0;
  float s1   = 3*r1*r1 - 2*r1*r1*r1;
  float s2   = 3*r2*r2 - 2*r2*r2*r2;
  float ax00 = i000 + s0*(i100-i000);
  float ax01 = i001 + s0*(i101-i001);
  float ax10 = i010 + s0*(i110-i010);
  float ax11 = i011 + s0*(i111-i011);
  float ay0  = ax00 + s1*(ax10-ax00);
  float ay1  = ax01 + s1*(ax11-ax01);
  float az   = ay0 + s2*(ay1-ay0);

  return az;
}

float PerlinNoise3::getValue(const Vector3 &point) {
  float result = 0;
  float fundamental = 1;
  float amplitude = 1;

  /* Calculate each octave */
  for (int i=0; i<octaves; i++) {
    result += smoothNoise3(point * fundamental) * amplitude;
    fundamental *= 2;
    amplitude *= persistence;
  }

  return result;
}

Vector3 PerlinNoise3::smoothNoiseGradient3(const Vector3 &point) {
  Vector3 result;

  int ip0 = (int) floor(point[0]);
  int ip1 = (int) floor(point[1]);
  int ip2 = (int) floor(point[2]);  

  /* Get a random gradient for each */
  Vector3 &g000 = tableNoise3(ip0  ,ip1  ,ip2  );
  Vector3 &g001 = tableNoise3(ip0  ,ip1  ,ip2+1);
  Vector3 &g010 = tableNoise3(ip0  ,ip1+1,ip2  );
  Vector3 &g011 = tableNoise3(ip0  ,ip1+1,ip2+1);
  Vector3 &g100 = tableNoise3(ip0+1,ip1  ,ip2  );
  Vector3 &g101 = tableNoise3(ip0+1,ip1  ,ip2+1);
  Vector3 &g110 = tableNoise3(ip0+1,ip1+1,ip2  );
  Vector3 &g111 = tableNoise3(ip0+1,ip1+1,ip2+1);

  /* Get a vector from each grid point to the point we're evaluating */
  Vector3 v000(point[0] - (ip0  ), point[1] - (ip1  ), point[2] - (ip2  ));
  Vector3 v001(point[0] - (ip0  ), point[1] - (ip1  ), point[2] - (ip2+1));
  Vector3 v010(point[0] - (ip0  ), point[1] - (ip1+1), point[2] - (ip2  ));
  Vector3 v011(point[0] - (ip0  ), point[1] - (ip1+1), point[2] - (ip2+1));
  Vector3 v100(point[0] - (ip0+1), point[1] - (ip1  ), point[2] - (ip2  ));
  Vector3 v101(point[0] - (ip0+1), point[1] - (ip1  ), point[2] - (ip2+1));
  Vector3 v110(point[0] - (ip0+1), point[1] - (ip1+1), point[2] - (ip2  ));
  Vector3 v111(point[0] - (ip0+1), point[1] - (ip1+1), point[2] - (ip2+1));
  
  /* Dot products to calculate the influence of each grid point */
  float i000 = g000 * v000;
  float i001 = g001 * v001;
  float i010 = g010 * v010;
  float i011 = g011 * v011;
  float i100 = g100 * v100;
  float i101 = g101 * v101;
  float i110 = g110 * v110;
  float i111 = g111 * v111;
  
  /* Cubic interpolation in three dimensions */
  float r0   = v000[0];
  float r1   = v000[1];
  float r2   = v000[2];
  float s0   = 3*r0*r0 - 2*r0*r0*r0;
  float s1   = 3*r1*r1 - 2*r1*r1*r1;
  float s2   = 3*r2*r2 - 2*r2*r2*r2;
  float ax00 = i000 + s0*(i100-i000);
  float ax01 = i001 + s0*(i101-i001);
  float ax10 = i010 + s0*(i110-i010);
  float ax11 = i011 + s0*(i111-i011);
  float ay0  = ax00 + s1*(ax10-ax00);
  float ay1  = ax01 + s1*(ax11-ax01);
  
  /* Partial derivative with respect to x */
  Vector3 v_dx(1,0,0);
  float s0_dx   = 6*(r0 - r0*r0);
  float s1_dx   = 0;
  float s2_dx   = 0;
  float i000_dx = g000 * v_dx;
  float i001_dx = g001 * v_dx;
  float i010_dx = g010 * v_dx;
  float i011_dx = g011 * v_dx;
  float i100_dx = g100 * v_dx;
  float i101_dx = g101 * v_dx;
  float i110_dx = g110 * v_dx;
  float i111_dx = g111 * v_dx;
  float ax00_dx = i000_dx + s0_dx*(i100-i000) + s0*(i100_dx-i000_dx);
  float ax01_dx = i001_dx + s0_dx*(i101-i001) + s0*(i101_dx-i001_dx);
  float ax10_dx = i010_dx + s0_dx*(i110-i010) + s0*(i110_dx-i010_dx);
  float ax11_dx = i011_dx + s0_dx*(i111-i011) + s0*(i111_dx-i011_dx);
  float ay0_dx  = ax00_dx + s1*(ax10_dx-ax00_dx) + s1_dx*(ax10-ax00);
  float ay1_dx  = ax01_dx + s1*(ax11_dx-ax01_dx) + s1_dx*(ax11-ax01);
  result[0]     = ay0_dx + s2*(ay1_dx-ay0_dx) + s2_dx*(ay1-ay0);

  /* Partial derivative with respect to y */
  Vector3 v_dy(0,1,0);
  float s0_dy   = 0;
  float s1_dy   = 6*(r0 - r0*r0);
  float s2_dy   = 0;
  float i000_dy = g000 * v_dy;
  float i001_dy = g001 * v_dy;
  float i010_dy = g010 * v_dy;
  float i011_dy = g011 * v_dy;
  float i100_dy = g100 * v_dy;
  float i101_dy = g101 * v_dy;
  float i110_dy = g110 * v_dy;
  float i111_dy = g111 * v_dy;
  float ax00_dy = i000_dy + s0_dy*(i100-i000) + s0*(i100_dy-i000_dy);
  float ax01_dy = i001_dy + s0_dy*(i101-i001) + s0*(i101_dy-i001_dy);
  float ax10_dy = i010_dy + s0_dy*(i110-i010) + s0*(i110_dy-i010_dy);
  float ax11_dy = i011_dy + s0_dy*(i111-i011) + s0*(i111_dy-i011_dy);
  float ay0_dy  = ax00_dy + s1*(ax10_dy-ax00_dy) + s1_dy*(ax10-ax00);
  float ay1_dy  = ax01_dy + s1*(ax11_dy-ax01_dy) + s1_dy*(ax11-ax01);
  result[1]     = ay0_dy + s2*(ay1_dy-ay0_dy) + s2_dy*(ay1-ay0);

  /* Partial derivative with respect to z */
  Vector3 v_dz(0,0,1);
  float s0_dz   = 0;
  float s1_dz   = 0;
  float s2_dz   = 6*(r0 - r0*r0);
  float i000_dz = g000 * v_dz;
  float i001_dz = g001 * v_dz;
  float i010_dz = g010 * v_dz;
  float i011_dz = g011 * v_dz;
  float i100_dz = g100 * v_dz;
  float i101_dz = g101 * v_dz;
  float i110_dz = g110 * v_dz;
  float i111_dz = g111 * v_dz;
  float ax00_dz = i000_dz + s0_dz*(i100-i000) + s0*(i100_dz-i000_dz);
  float ax01_dz = i001_dz + s0_dz*(i101-i001) + s0*(i101_dz-i001_dz);
  float ax10_dz = i010_dz + s0_dz*(i110-i010) + s0*(i110_dz-i010_dz);
  float ax11_dz = i011_dz + s0_dz*(i111-i011) + s0*(i111_dz-i011_dz);
  float ay0_dz  = ax00_dz + s1*(ax10_dz-ax00_dz) + s1_dz*(ax10-ax00);
  float ay1_dz  = ax01_dz + s1*(ax11_dz-ax01_dz) + s1_dz*(ax11-ax01);
  result[2]     = ay0_dz + s2*(ay1_dz-ay0_dz) + s2_dz*(ay1-ay0);

  return result;
}

Vector3 PerlinNoise3::getGradient(const Vector3 &point) {
  Vector3 result(0,0,0);
  float fundamental = 1;
  float amplitude = 1;

  /* Calculate each octave */
  for (int i=0; i<octaves; i++) {
    result += smoothNoiseGradient3(point * fundamental) * amplitude;
    fundamental *= 2;
    amplitude *= persistence;
  }
  return result;
}


/* The End */

