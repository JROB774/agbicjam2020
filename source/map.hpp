#ifndef MAP_HPP
#define MAP_HPP

GLOBAL constexpr int TILE_W = 16;
GLOBAL constexpr int TILE_H = 16;

enum TileType { TILE_EMPTY, TILE_SOLID };

struct Tile
{
    TileType type;
    int xoff,yoff; // In tiles!
    bool invis = false;
};

struct Map
{
    Image background;
    Image foreground;
    Image tileset;
    int w,h;
    std::vector<Tile> tiles;
    bool has_background;
    bool has_foreground;
    // Entities
    std::vector<Spike> spikes;
    std::vector<SmallBone> sbones;
    std::vector<BigBone> lbones;
    std::vector<BreakableBlock> bblocks;
    std::vector<CrushBoi> cboi;
    std::vector<SpitBoy> spitboys;
    std::vector<WalkBoy> walkboys;
    std::vector<ChargeBoy> chargeboys;
};

INTERNAL void LoadMap (Map& map, std::string file_name);
INTERNAL void FreeMap (Map& map);

INTERNAL void UpdateMap (Map& map, float dt);

INTERNAL void DrawMapBackground    (Map& map);
INTERNAL void DrawMapBackEntities  (Map& map, float dt);
INTERNAL void DrawMapFrontEntities (Map& map, float dt);
INTERNAL void DrawMapFrontTiles    (Map& map);
INTERNAL void DrawMapForeground    (Map& map);

INTERNAL void ResetMap ();

INTERNAL Tile& GetMapTile (Map& map, int x, int y);

#endif /* MAP_HPP */
