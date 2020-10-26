#ifndef TERRAIN_SETTINGS_H_
#define TERRAIN_SETTINGS_H_

struct TerrainSettings
{
    int chunk_dimensions = 32;
    int max_terrain_height = 512;
    int generation_range = 16;
    int scale = 4.f;
};


#endif