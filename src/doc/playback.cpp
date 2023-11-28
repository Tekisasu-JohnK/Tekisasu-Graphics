// Aseprite Document Library
// Copyright (C) 2021-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "doc/playback.h"

#include "base/remove_from_container.h"
#include "doc/frame.h"
#include "doc/sprite.h"
#include "doc/tag.h"

#include <limits>

#define PLAY_TRACE(...) // TRACEARGS

namespace doc {

[[maybe_unused]]
static const char* mode_to_string(Playback::Mode mode)
{
  switch (mode) {
    case Playback::PlayAll: return "PlayAll";
    case Playback::PlayInLoop: return "PlayInLoop";
    case Playback::PlayWithoutTagsInLoop: return "PlayWithoutTagsInLoop";
    case Playback::PlayOnce: return "PlayOnce";
    case Playback::Stopped: return "Stopped";
  }
  return "";
}

Playback::PlayTag::PlayTag(const Tag* tag, int parentForward)
  : tag(tag)
  , forward(parentForward * (tag->aniDir() == AniDir::FORWARD ||
                             tag->aniDir() == AniDir::PING_PONG ? 1: -1))
{
  if (tag->repeat() > 0) {
    repeat = tag->repeat();
  }
  // Repeat=0 is a "infinite repeat", but we'll play the tag just
  // once.
  else {
    if (tag->aniDir() == AniDir::PING_PONG ||
        tag->aniDir() == AniDir::PING_PONG_REVERSE) {
      repeat = 2;
    }
    else {
      repeat = 1;
    }
  }
}

Playback::Playback(const Sprite* sprite,
                   const TagsList& tags,
                   const frame_t frame,
                   const Mode playMode,
                   const Tag* tag)
  : m_sprite(sprite)
  , m_tags(tags)
  , m_initialFrame(frame)
  , m_frame(frame)
  , m_playMode(playMode)
{
  PLAY_TRACE("--Playback-- tag=", (tag ? tag->name(): ""), "mode=", mode_to_string(m_playMode));

  // Go to the first frame of the animation or active frame tag
  if (playMode == Mode::PlayOnce) {
    if (tag) {
      m_frame = (tag->aniDir() == AniDir::REVERSE ||
                 tag->aniDir() == AniDir::PING_PONG_REVERSE ?
                 tag->toFrame():
                 tag->fromFrame());

      addTag(tag, false, 1);
    }
    else {
      m_frame = 0;
    }
  }
  else if (playMode == Mode::PlayInLoop) {
    if (tag) {
      addTag(tag, false, 1);

      // Loop the given tag in the constructor infite times
      m_playing.back()->repeat = std::numeric_limits<int>::max();
    }
  }

  if (m_sprite)
    handleEnterFrame(frame, true);
}

Playback::Playback(const Sprite* sprite,
                   const frame_t frame,
                   const Mode playMode,
                   const Tag* tag)
  : Playback(sprite,
             (sprite ? sprite->tags().getInternalList(): TagsList()),
             frame,
             playMode,
             tag)
{
}

frame_t Playback::nextFrame(frame_t frameDelta)
{
  PLAY_TRACE("  Playback::nextFrame { frame=", m_frame, "+", frameDelta);

  int step = (frameDelta > 0 ? +1: -1);

  while (frameDelta != 0 && m_playMode != Stopped) {
    bool move = handleExitFrame(step);
    if (move)
      handleMoveFrame(step);
    handleEnterFrame(step, false);

    frameDelta -= step;
  }

  PLAY_TRACE("  } =", m_frame,
             "(tag=", (tag() ? tag()->name(): "nullptr"),
             ", repeat=", (!m_playing.empty() ? m_playing.back()->repeat: -1), ")");
  return m_frame;
}

void Playback::stop()
{
  if (m_playMode == Mode::PlayAll ||
      m_playMode == Mode::PlayOnce) {
    m_frame = m_initialFrame;
  }
  m_playMode = Mode::Stopped;
}

Tag* Playback::tag() const
{
  return (!m_playing.empty() ? const_cast<Tag*>(m_playing.back()->tag): nullptr);
}

void Playback::removeReferencesToTag(Tag* tag)
{
  base::remove_from_container(m_tags, tag);
  base::remove_from_container(m_played, tag);

  for (auto it=m_playing.begin(); it!=m_playing.end(); ) {
    std::unique_ptr<PlayTag>& playTag = *it;
    if (playTag->tag == tag)
      it = m_playing.erase(it);
    else
      ++it;
  }
}

void Playback::handleEnterFrame(const frame_t frameDelta, const bool firstTime)
{
  PLAY_TRACE("    handleEnterFrame", m_frame, "+", frameDelta);

  switch (m_playMode) {

    case PlayAll:
    case PlayInLoop: {
      const Tag* tag = this->tag();
      const frame_t frame = m_frame;
      const int forward = getParentForward();

      for (const Tag* t : m_tags) {
        if (t->contains(frame)) {
          // Ignored tags that were played
          if (m_played.find(t) != m_played.end()) {
            continue;
          }

          if (tag &&
              (tag->toFrame() < t->toFrame() ||
               tag->fromFrame() > t->fromFrame())) {
            // Cascade
            addTag(t, true, 1);
          }
          else {
            addTag(t, false, forward);
            if (!firstTime)
              goToFirstTagFrame(t);
          }
        }
      }
      break;
    }

    case PlayWithoutTagsInLoop:
    case PlayOnce:
      // Do nothing
      break;

  }
}

bool Playback::handleExitFrame(const frame_t frameDelta)
{
  PLAY_TRACE("    handleExitFrame", m_frame, "+", frameDelta);

  switch (m_playMode) {

    case PlayAll:
    case PlayInLoop: {
      auto tag = this->tag();
      if (tag && tag->contains(m_frame)) {
        ASSERT(!m_playing.empty());
        [[maybe_unused]]
        int forward = m_playing.back()->forward;

        PLAY_TRACE("tag aniDir=", (int)tag->aniDir(),
                   "range=", (int)tag->fromFrame(), (int)tag->toFrame(),
                   "forward=", forward);

        if ((tag->aniDir() == AniDir::FORWARD ||
             tag->aniDir() == AniDir::REVERSE)
            && (frameDelta > 0 && m_frame == lastTagFrame(tag))) {
          decrementRepeat(frameDelta);
          return false;
        }
        // Change ping-pong direction
        else if ((tag->aniDir() == AniDir::PING_PONG ||
                  tag->aniDir() == AniDir::PING_PONG_REVERSE)
                 && m_frame == lastTagFrame(tag)) {
          PLAY_TRACE("    Changing direction frame=", m_frame,
                     " forward=", forward, "->", -forward);

          // Changing the direction of the ping-pong animation
          m_playing.back()->invertForward();
          return decrementRepeat(frameDelta);
        }
        else if (m_playMode == PlayInLoop) {
          if (frameDelta < 0 && m_frame == firstTagFrame(tag)) {
            PLAY_TRACE("    Going to last frame=", lastTagFrame(tag),
                      " (PlayInLoop) frame=", m_frame, " forward=", forward);

            m_frame = lastTagFrame(tag);
            return false;
          }
          break;
        }
      }

      if (frameDelta > 0 && m_frame == m_sprite->lastFrame()) {
        if (m_playMode == PlayInLoop) {
          PLAY_TRACE("    Going back to frame=0 (PlayInLoop)", m_frame,
                     m_sprite->lastFrame());
          m_frame = 0;
          return false;
        }
        else {
          PLAY_TRACE("    Stop animation (PlayAll)");
          stop();
          return false;
        }
      }
      else if (frameDelta < 0 && m_frame == 0) {
        if (m_playMode == PlayInLoop) {
          PLAY_TRACE("    Going back to frame=last frame (PlayInLoop)");
          m_frame = m_sprite->lastFrame();
          return false;
        }
        else {
          PLAY_TRACE("    Stop animation in first frame (PlayAll)");
          stop();
          return false;
        }
      }
      break;
    }

    case PlayWithoutTagsInLoop:
      // Do nothing
      break;

    case PlayOnce: {
      if (auto tag = this->tag()) {
        ASSERT(m_playing.size() == 1);
        int forward = m_playing.back()->forward;

        if ((tag->aniDir() == AniDir::FORWARD && m_frame == tag->toFrame()) ||
            (tag->aniDir() == AniDir::REVERSE && m_frame == tag->fromFrame()) ||
            (tag->aniDir() == AniDir::PING_PONG && m_frame == tag->fromFrame() && forward < 0) ||
            (tag->aniDir() == AniDir::PING_PONG_REVERSE && m_frame == tag->toFrame() && forward > 0)) {
          stop();
          return false;
        }
        else if ((tag->aniDir() == AniDir::PING_PONG &&
                  m_frame == tag->toFrame() && forward > 0)
                 || (tag->aniDir() == AniDir::PING_PONG_REVERSE &&
                     m_frame == tag->fromFrame() && forward < 0)) {
          PLAY_TRACE("    Changing direction frame=", m_frame,
                     " forward=", forward, "->", -forward);

          // Changing the direction of the ping-pong animation
          m_playing.back()->invertForward();
        }
      }
      else if ((frameDelta > 0 && m_frame == m_sprite->lastFrame()) ||
               (frameDelta < 0 && m_frame == 0)) {
        stop();
        return false;
      }
      break;
    }

  }

  return true;
}

void Playback::handleMoveFrame(const frame_t frameDelta)
{
  PLAY_TRACE("    handleMoveFrame", m_frame, "+", frameDelta);

  switch (m_playMode) {

    case PlayWithoutTagsInLoop: {
      ASSERT(m_playing.empty());

      frame_t first = 0;
      frame_t last = m_sprite->lastFrame();
      m_frame += frameDelta;
      if (m_frame < 0) m_frame = last;
      if (m_frame > last) m_frame = first;
      break;
    }

    case PlayAll:
    case PlayInLoop:
    case PlayOnce: {
      m_frame += frameDelta * getParentForward();
      break;
    }

  }
}

void Playback::addTag(const Tag* tag,
                      const bool rewind,
                      const int forward)
{
  auto playTag = std::make_unique<PlayTag>(tag, forward);

  PLAY_TRACE("    addTag", tag->name(),
             "rewind", rewind,
             "new playTag forward", playTag->forward);

  if (rewind) {
    playTag->rewind = true;

    // Delay the deletion of currentPlayTag to this new tag
    PlayTag* currentPlayTag = m_playing.back().get();
    PlayTag* delayed = currentPlayTag;

    while (delayed->delayedDelete)
      delayed = delayed->delayedDelete;
    delayed->delayedDelete = playTag.get();
    for (const Tag* otherTag : delayed->removeThese)
      playTag->removeThese.push_back(otherTag);
    playTag->removeThese.push_back(delayed->tag);
    delayed->removeThese.clear();

    auto it = m_playing.end(),
      begin = m_playing.begin();
    --it;
    ASSERT(it->get() == currentPlayTag);
    while (it != begin) {
      if ((*it)->tag == delayed->tag)
        break;
      --it;
    }

    m_playing.insert(it, std::move(playTag));
  }
  else {
    m_playing.push_back(std::move(playTag));
  }
  m_played.insert(tag);
}

void Playback::removeLastTagFromPlayed()
{
  PlayTag* playTag = m_playing.back().get();

  for (auto otherTag : playTag->removeThese) {
    auto it = m_played.find(otherTag);
    ASSERT(it != m_played.end());
    if (it != m_played.end())
      m_played.erase(it);
  }

  auto it = m_played.find(playTag->tag);
  ASSERT(it != m_played.end());
  if (it != m_played.end())
    m_played.erase(it);
}

bool Playback::decrementRepeat(const frame_t frameDelta)
{
  while (true) {
    Tag* tag = this->tag();
    PLAY_TRACE("    Decrement tag", tag->name(),
               "repeat", m_playing.back()->repeat, "-1");

    if (m_playing.back()->repeat > 1) {
      --m_playing.back()->repeat;
      goToFirstTagFrame(tag);

      PLAY_TRACE("    Repeat tag", tag->name(), " frame=", m_frame,
                 "repeat=", m_playing.back()->repeat,
                 "forward=", m_playing.back()->forward);
      return true;
    }
    else {
      // Remove tag from played
      if (!m_playing.back()->delayedDelete) {
        PLAY_TRACE("    Removing played tag", tag->name());
        removeLastTagFromPlayed();
      }
      else {
        PLAY_TRACE("    Delaying the removal of played tag", tag->name());
      }

      // Delete and remove PlayTag
      m_playing.pop_back();

      // Forward direction of the parent tag
      int forward = (m_playing.empty() ? +1: m_playing.back()->forward);
      bool rewind = (m_playing.empty() ? false: m_playing.back()->rewind);

      // New frame outside the tag
      frame_t newFrame;
      if (rewind) {
        newFrame = firstTagFrame(m_playing.back()->tag);
      }
      else {
        newFrame = (frameDelta * forward < 0 ? tag->fromFrame()-1: tag->toFrame()+1);
      }

      PLAY_TRACE("    After tag", tag->name(),
                 "possible new frame=", newFrame,
                 "forward", forward);

      if (newFrame < 0 || newFrame > m_sprite->lastFrame()) {
        if (m_playMode == PlayAll) {
          stop();
          return false;
        }
        if (newFrame < 0)
          newFrame = m_sprite->lastFrame();
        else if (newFrame > m_sprite->lastFrame())
          newFrame = 0;
      }

      m_frame = newFrame;

      if (auto newTag = this->tag()) {
        if (newTag->contains(m_frame)) {
          PLAY_TRACE("    Back to tag", newTag->name(), "frame=", m_frame);
          return false;
        }
        else {
          // Now we try to decrement this tag repeat counter...
        }
      }
      else {
        // Special case where a ping-pong animation ends in the 1st
        // frame and we are playing in loop mode, so starting the
        // animation again should continue in the 2nd frame
        if (m_playing.empty() &&
            m_playMode == PlayInLoop &&
            (tag->aniDir() == AniDir::PING_PONG ||
             tag->aniDir() == AniDir::PING_PONG_REVERSE) &&
            tag->fromFrame() == 0 &&
            tag->toFrame() == m_sprite->lastFrame()) {
          PLAY_TRACE("    Re-adding ping-pong tag", tag->name(), "frame=", m_frame);
          addTag(tag, false, getParentForward());
          return false;
        }
        else {
          PLAY_TRACE("    Going outside the tag", tag->name(), "frame=", m_frame);
          return false;
        }
      }
    }
  }
}

frame_t Playback::firstTagFrame(const Tag* tag)
{
  ASSERT(tag);
  ASSERT(!m_playing.empty());
  int forward = m_playing.back()->forward;
  return (forward < 0 ? tag->toFrame():
                        tag->fromFrame());
}

frame_t Playback::lastTagFrame(const Tag* tag)
{
  ASSERT(tag);
  ASSERT(!m_playing.empty());
  int forward = m_playing.back()->forward;
  return (forward > 0 ? tag->toFrame():
                        tag->fromFrame());
}

void Playback::goToFirstTagFrame(const Tag* tag)
{
  ASSERT(tag);
  m_frame = firstTagFrame(tag);
  PLAY_TRACE("    Go to first frame of tag", tag->name(), "frame=", m_frame);
}

int Playback::getParentForward() const
{
  if (m_playing.empty())
    return 1;
  else
    return m_playing.back()->forward;
}

} // namespace doc
