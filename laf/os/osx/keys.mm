// LAF OS Library
// Copyright (C) 2015-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

// Uncomment this to log how scancodes are generated
#define KEY_TRACE(...)

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/osx/keys.h"

#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h> // TIS functions

namespace os {

static KeyScancode from_char_to_scancode(int chr)
{
  static KeyScancode map[] = {
    kKeyNil,        // 0 = 00 = NUL
    kKeyNil,        // 1 = 01 = STX
    kKeyNil,        // 2 = 02 = SOT
    kKeyNil,        // 3 = 03 = ETX
    kKeyNil,        // 4 = 04 = EOT
    kKeyNil,        // 5 = 05 = ENQ
    kKeyNil,        // 6 = 06 = ACK
    kKeyNil,        // 7 = 07 = BEL
    kKeyBackspace,  // 8 = 08 = BS
    kKeyNil,        // 9 = 09 = HT
    kKeyNil,        // 10 =0A = LF
    kKeyNil,        // 11 =0B = VT
    kKeyNil,        // 12 =0C = FF
    kKeyNil,        // 13 =0D = CR
    kKeyNil,        // 14 =0E = SO
    kKeyNil,        // 15 =0F = SI
    kKeyNil,        // 16 =10 = DLE
    kKeyNil,        // 17 =11 = DC1
    kKeyNil,        // 18 =12 = DC2
    kKeyNil,        // 19 =13 = DC3
    kKeyNil,        // 20 =14 = DC4
    kKeyNil,        // 21 =15 = NAK
    kKeyNil,        // 22 =16 = SYN
    kKeyNil,        // 23 =17 = ETB
    kKeyNil,        // 24 =18 = CAN
    kKeyNil,        // 25 =19 = EM
    kKeyNil,        // 26 =1A = SUB
    kKeyNil,        // 27 =1B = ESC
    kKeyNil,        // 28 =1C = FS
    kKeyNil,        // 29 =1D = GS
    kKeyNil,        // 30 =1E = RS
    kKeyNil,        // 31 =1F = US
    kKeySpace,      // 32 =20 = Space
    kKeyNil,        // 33 =21 = !
    kKeyNil,        // 34 =22 = "
    kKeyNil,        // 35 =23 = #
    kKeyNil,        // 36 =24 = $
    kKeyNil,        // 37 =25 = %
    kKeyNil,        // 38 =26 = &
    kKeyQuote,      // 39 =27 = '
    kKeyNil,        // 40 = 28 = (
    kKeyNil,        // 41 = 29 = )
    kKeyNil,        // 42 = 2A = *
    kKeyNil,        // 43 = 2B = +
    kKeyComma,      // 44 = 2C = ,
    kKeyMinus,      // 45 = 2D = -
    kKeyStop,       // 46 = 2E = .
    kKeySlash,      // 47 = 2F = /
    kKey0,          // 48 = 30 = 0
    kKey1,          // 49 = 31 = 1
    kKey2,          // 50 = 32 = 2
    kKey3,          // 51 = 33 = 3
    kKey4,          // 52 = 34 = 4
    kKey5,          // 53 = 35 = 5
    kKey6,          // 54 = 36 = 6
    kKey7,          // 55 = 37 = 7
    kKey8,          // 56 = 38 = 8
    kKey9,          // 57 = 39 = 9
    kKeyColon,      // 58 = 3A = :
    kKeySemicolon,  // 59 = 3B = ;
    kKeyNil,        // 60 = 3C = <
    kKeyEquals,     // 61 = 3D = =
    kKeyNil,        // 62 = 3E = >
    kKeyNil,        // 63 = 3F = ?
    kKeyNil,        // 64 = 40 = @
    kKeyA,          // 65 = 41 = A
    kKeyB,          // 66 = 42 = B
    kKeyC,          // 67 = 43 = C
    kKeyD,          // 68 = 44 = D
    kKeyE,          // 69 = 45 = E
    kKeyF,          // 70 = 46 = F
    kKeyG,          // 71 = 47 = G
    kKeyH,          // 72 = 48 = H
    kKeyI,          // 73 = 49 = I
    kKeyJ,          // 74 = 4A = J
    kKeyK,          // 75 = 4B = K
    kKeyL,          // 76 = 4C = L
    kKeyM,          // 77 = 4D = M
    kKeyN,          // 78 = 4E = N
    kKeyO,          // 79 = 4F = O
    kKeyP,          // 80 = 50 = P
    kKeyQ,          // 81 = 51 = Q
    kKeyR,          // 82 = 52 = R
    kKeyS,          // 83 = 53 = S
    kKeyT,          // 84 = 54 = T
    kKeyU,          // 85 = 55 = U
    kKeyV,          // 86 = 56 = V
    kKeyW,          // 87 = 57 = W
    kKeyX,          // 88 = 58 = X
    kKeyY,          // 89 = 59 = Y
    kKeyZ,          // 90 = 5A = Z
    kKeyOpenbrace,  // 91 = 5B = [
    kKeyBackslash,  // 92 = 5C = backslash
    kKeyClosebrace, // 93 = 5D = ]
    kKeyCircumflex, // 94 = 5E = ^
    kKeyNil,        // 95 = 5F = _
    kKeyBackquote,  // 96 = 60 = `
    kKeyA,          // 97 = 61 = a
    kKeyB,          // 98 = 62 = b
    kKeyC,          // 99 = 63 = c
    kKeyD,          // 100 = 64 = d
    kKeyE,          // 101 = 65 = e
    kKeyF,          // 102 = 66 = f
    kKeyG,          // 103 = 67 = g
    kKeyH,          // 104 = 68 = h
    kKeyI,          // 105 = 69 = i
    kKeyJ,          // 106 = 6A = j
    kKeyK,          // 107 = 6B = k
    kKeyL,          // 108 = 6C = l
    kKeyM,          // 109 = 6D = m
    kKeyN,          // 110 = 6E = n
    kKeyO,          // 111 = 6F = o
    kKeyP,          // 112 = 70 = p
    kKeyQ,          // 113 = 71 = q
    kKeyR,          // 114 = 72 = r
    kKeyS,          // 115 = 73 = s
    kKeyT,          // 116 = 74 = t
    kKeyU,          // 117 = 75 = u
    kKeyV,          // 118 = 76 = v
    kKeyW,          // 119 = 77 = w
    kKeyX,          // 120 = 78 = x
    kKeyY,          // 121 = 79 = y
    kKeyZ,          // 122 = 7A = z
    kKeyOpenbrace,  // 123 = 7B = {
    kKeyBackslash,  // 124 = 7C = |
    kKeyClosebrace, // 125 = 7D = }
    kKeyTilde,      // 126 = 7E = ~
    kKeyNil,        // 127 = 7F = DEL
  };

  if (chr >= 0 && chr < sizeof(map) / sizeof(map[0])) {
    // Converts an ASCII character into a os::KeyScancode
    return map[chr];
  }
  else
    return kKeyNil;
}

static KeyScancode from_keycode_to_scancode(UInt16 keyCode)
{
  // Converts macOS virtual key code into a os::KeyScancode
  static KeyScancode map[256] = {
                    // 0x00
    kKeyA,          // 0x00 - kVK_ANSI_A
    kKeyS,          // 0x01 - kVK_ANSI_S
    kKeyD,          // 0x02 - kVK_ANSI_D
    kKeyF,          // 0x03 - kVK_ANSI_F
    kKeyH,          // 0x04 - kVK_ANSI_H
    kKeyG,          // 0x05 - kVK_ANSI_G
    kKeyZ,          // 0x06 - kVK_ANSI_Z
    kKeyX,          // 0x07 - kVK_ANSI_X
    kKeyC,          // 0x08 - kVK_ANSI_C
    kKeyV,          // 0x09 - kVK_ANSI_V
    kKeyNil,        // 0x0A - kVK_ISO_Section
    kKeyB,          // 0x0B - kVK_ANSI_B
    kKeyQ,          // 0x0C - kVK_ANSI_Q
    kKeyW,          // 0x0D - kVK_ANSI_W
    kKeyE,          // 0x0E - kVK_ANSI_E
    kKeyR,          // 0x0F - kVK_ANSI_R
                    // 0x10
    kKeyY,          // 0x10 - kVK_ANSI_Y
    kKeyT,          // 0x11 - kVK_ANSI_T
    kKey1,          // 0x12 - kVK_ANSI_1
    kKey2,          // 0x13 - kVK_ANSI_2
    kKey3,          // 0x14 - kVK_ANSI_3
    kKey4,          // 0x15 - kVK_ANSI_4
    kKey6,          // 0x16 - kVK_ANSI_6
    kKey5,          // 0x17 - kVK_ANSI_5
    kKeyEquals,     // 0x18 - kVK_ANSI_Equal
    kKey9,          // 0x19 - kVK_ANSI_9
    kKey7,          // 0x1A - kVK_ANSI_7
    kKeyMinus,      // 0x1B - kVK_ANSI_Minus
    kKey8,          // 0x1C - kVK_ANSI_8
    kKey0,          // 0x1D - kVK_ANSI_0
    kKeyClosebrace, // 0x1E - kVK_ANSI_RightBracket
    kKeyO,          // 0x1F - kVK_ANSI_O
                    // 0x20
    kKeyU,          // 0x20 - kVK_ANSI_U
    kKeyOpenbrace,  // 0x21 - kVK_ANSI_LeftBracket
    kKeyI,          // 0x22 - kVK_ANSI_I
    kKeyP,          // 0x23 - kVK_ANSI_P
    kKeyEnter,      // 0x24 - kVK_Return
    kKeyL,          // 0x25 - kVK_ANSI_L
    kKeyJ,          // 0x26 - kVK_ANSI_J
    kKeyQuote,      // 0x27 - kVK_ANSI_Quote
    kKeyK,          // 0x28 - kVK_ANSI_K
    kKeySemicolon,  // 0x29 - kVK_ANSI_Semicolon
    kKeyBackslash,  // 0x2A - kVK_ANSI_Backslash
    kKeyComma,      // 0x2B - kVK_ANSI_Comma
    kKeySlash,      // 0x2C - kVK_ANSI_Slash
    kKeyN,          // 0x2D - kVK_ANSI_N
    kKeyM,          // 0x2E - kVK_ANSI_M
    kKeyStop,       // 0x2F - kVK_ANSI_Period
                    // 0x30
    kKeyTab,        // 0x30 - kVK_Tab
    kKeySpace,      // 0x31 - kVK_Space
    kKeyNil,        // 0x32 - kVK_ANSI_Grave
    kKeyBackspace,  // 0x33 - kVK_Delete
    kKeyNil,        // 0x34 - ?
    kKeyEsc,        // 0x35 - kVK_Escape
    kKeyNil,        // 0x36 - ?
    kKeyCommand,    // 0x37 - kVK_Command
    kKeyLShift,     // 0x38 - kVK_Shift
    kKeyCapsLock,   // 0x39 - kVK_CapsLock
    kKeyAlt,        // 0x3A - kVK_Option
    kKeyLControl,   // 0x3B - kVK_Control
    kKeyRShift,     // 0x3C - kVK_RightShift
    kKeyAltGr,      // 0x3D - kVK_RightOption
    kKeyRControl,   // 0x3E - kVK_RightControl
    kKeyNil,        // 0x3F - kVK_Function
                    // 0x40
    kKeyNil,        // 0x40 - kVK_F17
    kKeyNil,        // 0x41 - kVK_ANSI_KeypadDecimal
    kKeyNil,        // 0x42 - ?
    kKeyAsterisk,   // 0x43 - kVK_ANSI_KeypadMultiply
    kKeyNil,        // 0x44 - ?
    kKeyPlusPad,    // 0x45 - kVK_ANSI_KeypadPlus
    kKeyNil,        // 0x46 - ?
    kKeyDelPad,     // 0x47 - kVK_ANSI_KeypadClear
    kKeyNil,        // 0x48 - kVK_VolumeUp
    kKeyNil,        // 0x49 - kVK_VolumeDown
    kKeyNil,        // 0x4A - kVK_Mute
    kKeySlashPad,   // 0x4B - kVK_ANSI_KeypadDivide
    kKeyEnterPad,   // 0x4C - kVK_ANSI_KeypadEnter
    kKeyNil,        // 0x4D - ?
    kKeyMinusPad,   // 0x4E - kVK_ANSI_KeypadMinus
    kKeyNil,        // 0x4F - kVK_F18
                    // 0x50
    kKeyNil,        // 0x50 - kVK_F19
    kKeyEqualsPad,  // 0x51 - kVK_ANSI_KeypadEquals
    kKey0Pad,       // 0x52 - kVK_ANSI_Keypad0
    kKey1Pad,       // 0x53 - kVK_ANSI_Keypad1
    kKey2Pad,       // 0x54 - kVK_ANSI_Keypad2
    kKey3Pad,       // 0x55 - kVK_ANSI_Keypad3
    kKey4Pad,       // 0x56 - kVK_ANSI_Keypad4
    kKey5Pad,       // 0x57 - kVK_ANSI_Keypad5
    kKey6Pad,       // 0x58 - kVK_ANSI_Keypad6
    kKey7Pad,       // 0x59 - kVK_ANSI_Keypad7
    kKeyNil,        // 0x5A - kVK_F20
    kKey8Pad,       // 0x5B - kVK_ANSI_Keypad8
    kKey9Pad,       // 0x5C - kVK_ANSI_Keypad9
    kKeyYen,        // 0x5D - kVK_JIS_Yen
    kKeyNil,        // 0x5E - kVK_JIS_Underscore
    kKeyNil,        // 0x5F - kVK_JIS_KeypadComma
                    // 0x60
    kKeyF5,         // 0x60 - kVK_F5
    kKeyF6,         // 0x61 - kVK_F6
    kKeyF7,         // 0x62 - kVK_F7
    kKeyF3,         // 0x63 - kVK_F3
    kKeyF8,         // 0x64 - kVK_F8
    kKeyF9,         // 0x65 - kVK_F9
    kKeyNil,        // 0x66 - kVK_JIS_Eisu
    kKeyF11,        // 0x67 - kVK_F11
    kKeyKana,       // 0x68 - kVK_JIS_Kana
    kKeyNil,        // 0x69 - kVK_F13
    kKeyNil,        // 0x6A - kVK_F16
    kKeyNil,        // 0x6B - kVK_F14
    kKeyNil,        // 0x6C - ?
    kKeyF10,        // 0x6D - kVK_F10
    kKeyNil,        // 0x6E - ?
    kKeyF12,        // 0x6F - kVK_F12
                    // 0x70
    kKeyNil,        // 0x70 - ?
    kKeyNil,        // 0x71 - kVK_F15
    kKeyNil,        // 0x72 - kVK_Help
    kKeyHome,       // 0x73 - kVK_Home
    kKeyPageUp,     // 0x74 - kVK_PageUp
    kKeyDel,        // 0x75 - kVK_ForwardDelete
    kKeyF4,         // 0x76 - kVK_F4
    kKeyEnd,        // 0x77 - kVK_End
    kKeyF2,         // 0x78 - kVK_F2
    kKeyPageDown,   // 0x79 - kVK_PageDown
    kKeyF1,         // 0x7A - kVK_F1
    kKeyLeft,       // 0x7B - kVK_LeftArrow
    kKeyRight,      // 0x7C - kVK_RightArrow
    kKeyDown,       // 0x7D - kVK_DownArrow
    kKeyUp,         // 0x7E - kVK_UpArrow
    kKeyNil         // 0x7F - ?
  };

  if (keyCode >= 0 && keyCode < sizeof(map) / sizeof(map[0])) {
    // Converts macOS virtual key into a os::KeyScancode
    return map[keyCode];
  }
  else
    return kKeyNil;
}

KeyScancode scancode_from_nsevent(NSEvent* event)
{
#if 1
  // For keys that are not in the numpad we try to get the scancode
  // converting the first char in NSEvent.characters to a
  // scancode.
  if ((event.modifierFlags & NSEventModifierFlagNumericPad) == 0) {
    KeyScancode code;

    // It looks like getting the first "event.characters" char is the
    // only way to get the correct Cmd+letter combination on "Dvorak -
    // QWERTY Cmd" keyboard layout.
    NSString* chars = event.characters;
    if (chars.length > 0) {
      int chr = [chars characterAtIndex:chars.length-1];

      // Avoid activating space bar modifier. E.g. pressing
      // Ctrl+Alt+Shift+S on "Spanish ISO" layout generates a
      // whitespace ' ', and we prefer the S scancode instead of the
      // space bar scancode.
      if (chr != 32) {
        code = from_char_to_scancode(chr);
        if (code != kKeyNil) {
          KEY_TRACE("scancode_from_nsevent %d -> %d (characters)\n",
                    (int)chr, (int)code);
          return code;
        }
      }
    }

    chars = event.charactersIgnoringModifiers;
    if (chars.length > 0) {
      int chr = [chars characterAtIndex:chars.length-1];
      code = from_char_to_scancode(chr);
      if (code != kKeyNil) {
        KEY_TRACE("scancode_from_nsevent %d -> %d (charactersIgnoringModifiers)\n",
                  (int)chr, (int)code);
        return code;
      }
    }
  }
#else // Don't use this code, it reports scancodes always as QWERTY
      // and doesn't work for Dvorak or AZERTY layouts.
  {
    CFStringRef strRef = get_unicode_from_key_code(
      event.keyCode,
      event.modifierFlags & NSCommandKeyMask);

    if (strRef) {
      KeyScancode code = kKeyNil;

      int length = CFStringGetLength(strRef);
      if (length > 0) {
        // Converts the first unicode char into a macOS virtual key
        UInt16 chr = CFStringGetCharacterAtIndex(strRef, length-1);
        code = from_char_to_scancode(chr);
        if (code != kKeyNil) {
          KEY_TRACE("scancode_from_nsevent %d -> %d (get_unicode_from_key_code)\n",
                    (int)chr, (int)code);
        }
      }

      CFRelease(strRef);
      if (code != kKeyNil) {
        return code;
      }
    }
  }
#endif

  KeyScancode code = from_keycode_to_scancode(event.keyCode);
  KEY_TRACE("scancode_from_nsevent %d -> %d (keyCode)\n",
            (int)event.keyCode, (int)code);
  return code;
}

// Based on code from:
// * http://stackoverflow.com/questions/22566665/how-to-capture-unicode-from-key-events-without-an-nstextview
// * http://stackoverflow.com/questions/12547007/convert-key-code-into-key-equivalent-string
// * http://stackoverflow.com/questions/8263618/convert-virtual-key-code-to-unicode-string
// * MacKeycodeAndModifiersToCharacter() function from Chromium
//
// If "deadKeyState" is = nullptr, it doesn't process dead keys.
CFStringRef get_unicode_from_key_code(const UInt16 keyCode,
                                      const NSEventModifierFlags modifierFlags,
                                      UInt32* deadKeyState)
{
  // The "TISCopyCurrentKeyboardInputSource()" doesn't contain
  // kTISPropertyUnicodeKeyLayoutData (returns nullptr) when the input
  // source is Japanese (Romaji/Hiragana/Katakana).

  //TISInputSourceRef inputSource = TISCopyCurrentKeyboardInputSource();
  TISInputSourceRef inputSource = TISCopyCurrentKeyboardLayoutInputSource();
  CFDataRef keyLayoutData = (CFDataRef)TISGetInputSourceProperty(inputSource, kTISPropertyUnicodeKeyLayoutData);
  const UCKeyboardLayout* keyLayout =
    (keyLayoutData ? (const UCKeyboardLayout*)CFDataGetBytePtr(keyLayoutData): nullptr);

  UInt32 deadKeyStateWrap = (deadKeyState ? *deadKeyState: 0);
  UniChar output[4];
  UniCharCount length;

  // Convert NSEvent modifiers to format UCKeyTranslate accepts. See
  // docs on UCKeyTranslate for more info.
  int unicode_modifiers = 0;
  if (modifierFlags & NSEventModifierFlagShift)
    unicode_modifiers |= shiftKey;
  if (modifierFlags & NSEventModifierFlagCapsLock)
    unicode_modifiers |= alphaLock;
  // if (modifierFlags & NSEventModifierFlagControl)
  //   unicode_modifiers |= controlKey;
  if (modifierFlags & NSEventModifierFlagOption)
    unicode_modifiers |= optionKey;
  // if (modifierFlags & NSEventModifierFlagCommand)
  //   unicode_modifiers |= cmdKey;
  UInt32 modifier_key_state = (unicode_modifiers >> 8) & 0xFF;

  // Reference here:
  // https://developer.apple.com/reference/coreservices/1390584-uckeytranslate?language=objc
  UCKeyTranslate(
    keyLayout,
    keyCode,
    kUCKeyActionDown,
    modifier_key_state,
    LMGetKbdType(),
    (deadKeyState ? 0: kUCKeyTranslateNoDeadKeysMask),
    &deadKeyStateWrap,
    sizeof(output) / sizeof(output[0]),
    &length,
    output);

  if (deadKeyState)
    *deadKeyState = deadKeyStateWrap;

  CFRelease(inputSource);
  return CFStringCreateWithCharacters(kCFAllocatorDefault, output, length);
}

} // namespace os
