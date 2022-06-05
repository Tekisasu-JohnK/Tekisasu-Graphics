// Aseprite Document Library
// Copyright (c) 2019-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "doc/tileset.h"

#include "doc/primitives.h"
#include "doc/remap.h"
#include "doc/sprite.h"

#include <memory>

#define TS_TRACE(...) // TRACE(__VA_ARGS__)

namespace doc {

Tileset::Tileset(Sprite* sprite,
                 const Grid& grid,
                 const tileset_index ntiles)
  : Object(ObjectType::Tileset)
  , m_sprite(sprite)
  , m_grid(grid)
  , m_tiles(ntiles)
{
  // The origin of tileset grids must be 0,0 (the origin is then
  // specified by each cel position)
  ASSERT(grid.origin() == gfx::Point(0, 0));

  // TODO at the moment retrieving a tileset from the clipboard use no
  //      sprite, but in the future we should save a whole sprite in the
  //      clipboard
  //ASSERT(sprite);

  for (tile_index ti=0; ti<ntiles; ++ti) {
    ImageRef tile = makeEmptyTile();
    m_tiles[ti] = tile;
    hashImage(ti, tile);
  }
}

// static
Tileset* Tileset::MakeCopyWithSameImages(const Tileset* tileset)
{
  std::unique_ptr<Tileset> copy(
    new Tileset(tileset->sprite(),
                tileset->grid(),
                tileset->size()));
  copy->setName(tileset->name());
  for (tile_index ti=0; ti<copy->size(); ++ti) {
    ImageRef image = tileset->get(ti);
    ASSERT(image);
    copy->set(ti, image);
  }
  return copy.release();
}

// static
Tileset* Tileset::MakeCopyCopyingImages(const Tileset* tileset)
{
  std::unique_ptr<Tileset> copy(
    new Tileset(tileset->sprite(),
                tileset->grid(),
                tileset->size()));
  copy->setName(tileset->name());
  for (tile_index ti=0; ti<copy->size(); ++ti) {
    ImageRef image = tileset->get(ti);
    ASSERT(image);
    // TODO can we avoid making a copy of this image
    copy->set(ti, ImageRef(Image::createCopy(image.get())));
  }
  return copy.release();
}

int Tileset::getMemSize() const
{
  int size = sizeof(Tileset) + m_name.size();
  for (auto& img : const_cast<Tileset*>(this)->m_tiles) {
    ASSERT(img);
    size += img->getMemSize();
  }
  return size;
}

void Tileset::resize(const tile_index ntiles)
{
  int oldSize = m_tiles.size();
  m_tiles.resize(ntiles);
  for (tile_index ti=oldSize; ti<ntiles; ++ti)
    m_tiles[ti] = makeEmptyTile();
}

void Tileset::remap(const Remap& remap)
{
  Tiles tmp = m_tiles;

  // The notile cannot be remapped
  ASSERT(remap[0] == 0);

  for (tile_index ti=1; ti<size(); ++ti) {
    TS_TRACE("m_tiles[%d] = tmp[%d]\n", remap[ti], ti);

    ASSERT(remap[ti] >= 0);
    ASSERT(remap[ti] < m_tiles.size());
    if (remap[ti] >= 0 &&
        remap[ti] < m_tiles.size()) {
      ASSERT(remap[ti] != notile);

      m_tiles[remap[ti]] = tmp[ti];
    }
  }

  rehash();
}

void Tileset::set(const tile_index ti,
                  const ImageRef& image)
{
  ASSERT(image);
  ASSERT(image->width() == m_grid.tileSize().w);
  ASSERT(image->height() == m_grid.tileSize().h);

#if _DEBUG
  if (ti == notile && !is_empty_image(image.get())) {
    TRACEARGS("Warning: setting tile 0 with a non-empty image");
  }
#endif

  removeFromHash(ti, false);

  preprocess_transparent_pixels(image.get());
  m_tiles[ti] = image;

  if (!m_hash.empty())
    hashImage(ti, image);
}

tile_index Tileset::add(const ImageRef& image)
{
  ASSERT(image);
  ASSERT(image->width() == m_grid.tileSize().w);
  ASSERT(image->height() == m_grid.tileSize().h);

  preprocess_transparent_pixels(image.get());
  m_tiles.push_back(image);

  const tile_index newIndex = tile_index(m_tiles.size()-1);
  if (!m_hash.empty())
    hashImage(newIndex, image);
  return newIndex;
}

void Tileset::insert(const tile_index ti,
                     const ImageRef& image)
{
  ASSERT(image);
  ASSERT(image->width() == m_grid.tileSize().w);
  ASSERT(image->height() == m_grid.tileSize().h);

#if _DEBUG
  if (ti == notile && !is_empty_image(image.get())) {
    TRACEARGS("Warning: inserting tile 0 with a non-empty image");
  }
#endif

  ASSERT(ti >= 0 && ti <= m_tiles.size()+1);
  preprocess_transparent_pixels(image.get());
  m_tiles.insert(m_tiles.begin()+ti, image);

  if (!m_hash.empty()) {
    // Fix all indexes in the hash that are greater than "ti"
    for (auto& it : m_hash)
      if (it.second >= ti)
        ++it.second;

    // And now we can add the new image with the "ti" index
    hashImage(ti, image);
  }
}

void Tileset::erase(const tile_index ti)
{
  ASSERT(ti >= 0 && ti < size());
  // TODO check why this doesn't work
  //removeFromHash(ti, true);

  m_tiles.erase(m_tiles.begin()+ti);
  rehash();
}

ImageRef Tileset::makeEmptyTile()
{
  ImageSpec spec = m_sprite->spec();
  spec.setSize(m_grid.tileSize());
  return ImageRef(Image::create(spec));
}

void Tileset::setExternal(const std::string& filename,
                          const tileset_index& tsi)
{
  m_external.filename = filename;
  m_external.tileset = tsi;
}

bool Tileset::findTileIndex(const ImageRef& tileImage,
                            tile_index& ti)
{
  ASSERT(tileImage);
  if (!tileImage) {
    ti = notile;
    return false;
  }

  auto& h = hashTable(); // Don't use m_hash directly in case that
                         // we've to regenerate the hash table.

  auto it = h.find(tileImage);
  if (it != h.end()) {
    ti = it->second;
    return true;
  }
  else {
    ti = notile;
    return false;
  }
}

void Tileset::notifyTileContentChange(const tile_index ti)
{
#if 0 // TODO Try to do less work

  ASSERT(ti >= 0 && ti < size());

  // If two or more tiles are exactly the same, they will have the
  // same hash, so the m_hash table can be smaller than the m_tiles
  // array.
  if (m_hash.size() < m_tiles.size()) {
    // Count how many hash elements are poiting to this tile index
    int tilesWithSameHash = 0;
    for (auto item : m_hash)
      if (item.second == ti)
        ++tilesWithSameHash;

    // In this case we re-generate the whole hash table because one or
    // more tiles tile are using the hash of the modified tile.
    if (tilesWithSameHash >= 2) {
      rehash();
      return;
    }
  }

  // In other case we can do a fast-path, just removing and
  // re-adding the tile to the hash table.
  removeFromHash(ti, false);
  if (!m_hash.empty())
    hashImage(ti, m_tiles[ti]);

#else // Regenerate the whole hash map (at the moment this is the
      // only way to make it work correctly)

  (void)ti;                     // unused

  if (ti >= 0 && ti < m_tiles.size() && m_tiles[ti])
    preprocess_transparent_pixels(m_tiles[ti].get());

  rehash();

#endif
}

void Tileset::notifyRegenerateEmptyTile()
{
  if (size() == 0)
    return;

  ImageRef image = get(doc::notile);
  if (image)
    doc::clear_image(image.get(), image->maskColor());
  rehash();
}

void Tileset::removeFromHash(const tile_index ti,
                             const bool adjustIndexes)
{
  auto end = m_hash.end();
  for (auto it=m_hash.begin(); it!=end; ) {
    if (it->second == ti) {
      it = m_hash.erase(it);
      end = m_hash.end();
    }
    else {
      if (adjustIndexes && it->second > ti)
        --it->second;
      ++it;
    }
  }
}

#ifdef _DEBUG
void Tileset::assertValidHashTable()
{
  // And empty hash table means that we've to re-generate it when it's
  // needed (when findTileIndex() is used).
  if (m_hash.empty())
    return;

  // If two or more tiles are exactly the same, they will have the
  // same hash, so the m_hash table can be smaller than the m_tiles
  // array.
  if (m_hash.size() < m_tiles.size()) {
    for (tile_index ti=0; ti<tile_index(m_tiles.size()); ++ti) {
      auto it = m_hash.find(m_tiles[ti]);
      ASSERT(it != m_hash.end());

      // If the hash doesn't match, it is because other tile is equal
      // to this one.
      if (it->second != ti) {
        ASSERT(is_same_image(it->first.get(), m_tiles[it->second].get()));
      }
    }
  }
  else if (m_hash.size() == m_tiles.size()) {
    for (tile_index ti=0; ti<tile_index(m_tiles.size()); ++ti) {
      auto it = m_hash.find(m_tiles[ti]);
      ASSERT(it != m_hash.end());
      ASSERT(it->second == ti);
    }
  }
  else {
    ASSERT(false && "The hash table cannot contain more tiles than the tileset");
  }
}
#endif

void Tileset::hashImage(const tile_index ti,
                        const ImageRef& tileImage)
{
  if (m_hash.find(tileImage) == m_hash.end())
    m_hash[tileImage] = ti;
}

void Tileset::rehash()
{
  // Clear the hash table, we'll lazy-rehash it when
  // hashTable()/findTileIndex() is used.
  m_hash.clear();
}

TilesetHashTable& Tileset::hashTable()
{
  if (m_hash.empty()) {
    // Re-hash/create the whole hash table from scratch
    tile_index ti = 0;
    for (auto tile : m_tiles)
      hashImage(ti++, tile);
  }
  return m_hash;
}

} // namespace doc
