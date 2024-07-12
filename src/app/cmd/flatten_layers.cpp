// Aseprite
// Copyright (C) 2019-2022  Igara Studio S.A.
// Copyright (C) 2001-2018  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/cmd/flatten_layers.h"

#include "app/cmd/add_layer.h"
#include "app/cmd/configure_background.h"
#include "app/cmd/copy_rect.h"
#include "app/cmd/move_layer.h"
#include "app/cmd/remove_layer.h"
#include "app/cmd/set_layer_flags.h"
#include "app/cmd/set_layer_name.h"
#include "app/cmd/unlink_cel.h"
#include "app/doc.h"
#include "app/i18n/strings.h"
#include "app/restore_visible_layers.h"
#include "doc/algorithm/shrink_bounds.h"
#include "doc/cel.h"
#include "doc/layer.h"
#include "doc/primitives.h"
#include "doc/sprite.h"
#include "render/render.h"

namespace app {
namespace cmd {

FlattenLayers::FlattenLayers(doc::Sprite* sprite,
                             const doc::SelectedLayers& layers0,
                             const bool newBlend)
  : WithSprite(sprite)
{
  m_newBlendMethod = newBlend;
  doc::SelectedLayers layers(layers0);
  layers.removeChildrenIfParentIsSelected();

  m_layerIds.reserve(layers.size());
  for (auto layer : layers)
    m_layerIds.push_back(layer->id());
}

void FlattenLayers::onExecute()
{
  Sprite* sprite = this->sprite();
  auto doc = static_cast<Doc*>(sprite->document());

  // Set of layers to be flattened.
  bool backgroundIsSel = false;
  SelectedLayers layers;
  for (auto layerId : m_layerIds) {
    doc::Layer* layer = doc::get<doc::Layer>(layerId);
    ASSERT(layer);
    layers.insert(layer);
    if (layer->isBackground())
      backgroundIsSel = true;
  }

  LayerList list = layers.toBrowsableLayerList();
  if (list.empty())
    return;                     // Do nothing

  // Create a temporary image.
  ImageRef image(Image::create(sprite->spec()));

  LayerImage* flatLayer;  // The layer onto which everything will be flattened.
  color_t bgcolor;        // The background color to use for flatLayer.
  bool newFlatLayer = false;

  flatLayer = sprite->backgroundLayer();
  if (backgroundIsSel && flatLayer && flatLayer->isVisible()) {
    // There exists a visible background layer, so we will flatten onto that.
    bgcolor = doc->bgColor(flatLayer);
  }
  else {
    // Create a new transparent layer to flatten everything onto it.
    flatLayer = new LayerImage(sprite);
    ASSERT(flatLayer->isVisible());
    flatLayer->setName(Strings::layer_properties_flattened());
    newFlatLayer = true;
    bgcolor = sprite->transparentColor();
  }

  render::Render render;
  render.setNewBlend(m_newBlendMethod);
  render.setBgOptions(render::BgOptions::MakeNone());

  {
    // Show only the layers to be flattened so other layers are hidden
    // temporarily.
    RestoreVisibleLayers restore;
    restore.showSelectedLayers(sprite, layers);

    // Copy all frames to the background.
    for (frame_t frame(0); frame<sprite->totalFrames(); ++frame) {
      // Clear the image and render this frame.
      clear_image(image.get(), bgcolor);
      render.renderSprite(image.get(), sprite, frame);

      // TODO Keep cel links when possible

      ImageRef cel_image;
      Cel* cel = flatLayer->cel(frame);
      if (cel) {
        if (cel->links())
          executeAndAdd(new cmd::UnlinkCel(cel));

        cel_image = cel->imageRef();
        ASSERT(cel_image);

        executeAndAdd(
          new cmd::CopyRect(cel_image.get(), image.get(),
                            gfx::Clip(0, 0, image->bounds())));
      }
      else {
        gfx::Rect bounds(image->bounds());
        if (doc::algorithm::shrink_bounds(
              image.get(), image->maskColor(), nullptr, bounds)) {
          cel_image.reset(
            doc::crop_image(image.get(), bounds, image->maskColor()));
          cel = new Cel(frame, cel_image);
          cel->setPosition(bounds.origin());
          flatLayer->addCel(cel);
        }
      }
    }
  }

  // Add new flatten layer
  if (newFlatLayer)
    executeAndAdd(new cmd::AddLayer(list.front()->parent(), flatLayer, list.front()));

  // Delete flattened layers.
  for (Layer* layer : layers) {
    // layer can be == flatLayer when we are flattening on the
    // background layer.
    if (layer != flatLayer) {
      executeAndAdd(new cmd::RemoveLayer(layer));
    }
  }
}

} // namespace cmd
} // namespace app
