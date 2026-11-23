#ifndef AWSOUNDS_H
#define AWSOUNDS_H

// Music and beeps on the Arduboy's built-in beeper. See AWSounds.cpp.

// Call once after arduboy.audio.begin().
void soundBegin();

// The title theme. Does nothing if it already plays.
void playMenuTheme();

// The deep bass loop during a match.
void playGameTheme();

// The tune at the end of a match: a fanfare for a win, a slow fall for a loss.
void playEndTune(bool won);

// Short sounds. They cut into the tune for a moment.
enum : uint8_t { SoundConfirm, SoundBack, SoundHit, SoundDestroyed, SoundThump, SoundCaptured };
void playSound(uint8_t sound);

#endif
