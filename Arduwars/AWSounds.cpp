// Music and beeps on the Arduboy's speaker.
//
// The Arduboy's speaker sits between two pins, so two tones can play at once.
// Voice 0 plays the tune on pin 2, voice 1 the bass and the snare on pin 1.
// Voice 2 plays beeps on pin 2. While a beep plays, the tune keeps running silently.
//
// Pin 2 (timer 4) plays a thin pulse, on a quarter of the time, like a Game Boy lead.
// Pin 1 (timer 3, BeepPin1 of Arduboy2) plays a full square wave for the bass,
// and random pitches for the snare.
// BeepPin2 is not used: Ardens plays it wrong and it cut into the other pin.
//
// A timer interrupt steps the voices about 1000 times a second,
// so the music runs on while the game draws, waits or thinks.
// The SOUND switch mutes both pins (arduboy.audio), the music runs on silently.

#include <Arduboy2.h>
#include "AWSounds.h"

// Note names, from e3 up to a5 ("s" is sharp). R is a rest.
// Lower case, because A3 to A5 are Arduino pin names.
// The bass voice plays every note one octave lower than written.
// The tune voice reaches from b3 up.
enum : uint8_t {
  R, e3, f3, fs3, g3, gs3, a3, as3, b3,
  c4, cs4, d4, ds4, e4, f4, fs4, g4, gs4, a4, as4, b4,
  c5, cs5, d5, ds5, e5, f5, fs5, g5, gs5, a5,
  SN // a snare hit, bass voice only
};
constexpr uint8_t SD = a5; // a soft, low drum hit, bass voice only

// Beeper counts for the notes above: 1,000,000 / frequency - 1.
const uint16_t pitches[] PROGMEM = {
  6066, 5726, 5404, 5101, 4815, 4544, 4289, 4049,
  3821, 3607, 3404, 3213, 3033, 2862, 2702, 2550, 2407, 2272, 2144, 2024,
  1910, 1803, 1702, 1606, 1516, 1431, 1350, 1275, 1203, 1135
};

// Note lengths in sixteenths: 16th, 8th, dotted 8th, quarter, dotted quarter, half, dotted half, whole.
enum : uint8_t { L16, L8, L8d, L4, L4d, L2, L2d, L1 };
const uint8_t lengths[] PROGMEM = {1, 2, 3, 4, 6, 8, 12, 16};

// One note is one byte: pitch and length. A 0 byte ends the list,
// so a rest is never a sixteenth long.
#define N(pitch, length) uint8_t((pitch) << 3 | (length))

// Our own music, in D minor, a military march. Both themes share one motif:
// "da-da DA-da DA DA", climbing from A to F, then falling back to D.

// The menu theme: the motif on the tune, over a galloping bass with a snare.
// 12 bars: motif, answer, motif higher, answer, a calm part.
const uint8_t themeMelody[] PROGMEM = {
  N(a4,L8d), N(a4,L16), N(d5,L8d), N(d5,L16), N(e5,L4), N(f5,L4),
  N(e5,L8d), N(d5,L16), N(c5,L8), N(a4,L8), N(d5,L2),
  N(a4,L8d), N(a4,L16), N(d5,L8d), N(d5,L16), N(e5,L4), N(f5,L4),
  N(g5,L8d), N(f5,L16), N(e5,L8), N(cs5,L8), N(a4,L2),
  N(d5,L8d), N(d5,L16), N(f5,L8d), N(f5,L16), N(g5,L4), N(a5,L4),
  N(g5,L8d), N(f5,L16), N(e5,L8), N(c5,L8), N(f5,L2),
  N(a4,L8d), N(a4,L16), N(d5,L8d), N(d5,L16), N(e5,L4), N(f5,L4),
  N(e5,L8d), N(d5,L16), N(cs5,L8), N(e5,L8), N(a4,L2),
  N(f4,L4), N(a4,L4), N(d5,L2),
  N(e4,L4), N(g4,L4), N(c5,L2),
  N(d4,L4), N(f4,L4), N(as4,L2),
  N(cs5,L4d), N(e5,L8), N(a4,L2),
  0
};
// The bass gallops on each chord's root and its octave, with a snare on beats 2 and 4.
// Chords, one per bar: D C D A | Bb F D A | D C Bb A
const uint8_t themeBass[] PROGMEM = {
  N(d4,L8), N(d4,L16), N(d4,L16), N(SN,L8), N(d5,L8), N(d4,L8), N(d4,L8), N(SN,L8), N(d5,L8),
  N(c4,L8), N(c4,L16), N(c4,L16), N(SN,L8), N(c5,L8), N(c4,L8), N(c4,L8), N(SN,L8), N(c5,L8),
  N(d4,L8), N(d4,L16), N(d4,L16), N(SN,L8), N(d5,L8), N(d4,L8), N(d4,L8), N(SN,L8), N(d5,L8),
  N(a3,L8), N(a3,L16), N(a3,L16), N(SN,L8), N(a4,L8), N(a3,L8), N(a3,L8), N(SN,L8), N(a4,L8),
  N(as3,L8), N(as3,L16), N(as3,L16), N(SN,L8), N(as4,L8), N(as3,L8), N(as3,L8), N(SN,L8), N(as4,L8),
  N(f3,L8), N(f3,L16), N(f3,L16), N(SN,L8), N(f4,L8), N(f3,L8), N(f3,L8), N(SN,L8), N(f4,L8),
  N(d4,L8), N(d4,L16), N(d4,L16), N(SN,L8), N(d5,L8), N(d4,L8), N(d4,L8), N(SN,L8), N(d5,L8),
  N(a3,L8), N(a3,L16), N(a3,L16), N(SN,L8), N(a4,L8), N(a3,L8), N(a3,L8), N(SN,L8), N(a4,L8),
  N(d4,L8), N(d4,L16), N(d4,L16), N(SN,L8), N(d5,L8), N(d4,L8), N(d4,L8), N(SN,L8), N(d5,L8),
  N(c4,L8), N(c4,L16), N(c4,L16), N(SN,L8), N(c5,L8), N(c4,L8), N(c4,L8), N(SN,L8), N(c5,L8),
  N(as3,L8), N(as3,L16), N(as3,L16), N(SN,L8), N(as4,L8), N(as3,L8), N(as3,L8), N(SN,L8), N(as4,L8),
  N(a3,L8), N(a3,L16), N(a3,L16), N(SN,L8), N(a4,L8), N(a3,L8), N(a3,L8), N(SN,L8), N(a4,L8),
  0
};

// The game theme: only the deep bass, so it does not pull you out of your turn.
// The motif plays low, then the bass marches with a soft drum. 12 bars.
const uint8_t gameBass[] PROGMEM = {
  N(a4,L8d), N(a4,L16), N(d5,L8d), N(d5,L16), N(e5,L4), N(f5,L4),
  N(e5,L8d), N(d5,L16), N(c5,L8), N(a4,L8), N(d5,L2),
  N(d4,L8), N(d4,L16), N(d4,L16), N(SD,L8), N(d4,L8), N(d4,L8), N(d4,L16), N(d4,L16), N(SD,L8), N(a3,L8),
  N(d4,L8), N(d4,L16), N(d4,L16), N(SD,L8), N(d4,L8), N(d4,L8), N(d4,L16), N(d4,L16), N(SD,L8), N(a3,L8),
  N(a4,L8d), N(a4,L16), N(d5,L8d), N(d5,L16), N(e5,L4), N(f5,L4),
  N(g5,L8d), N(f5,L16), N(e5,L8), N(cs5,L8), N(a4,L2),
  N(d4,L8), N(d4,L16), N(d4,L16), N(SD,L8), N(d4,L8), N(d4,L8), N(d4,L16), N(d4,L16), N(SD,L8), N(a3,L8),
  N(a3,L8), N(a3,L16), N(a3,L16), N(SD,L8), N(a3,L8), N(a3,L8), N(a3,L16), N(a3,L16), N(SD,L8), N(e3,L8),
  N(d4,L8d), N(d4,L16), N(f4,L8d), N(f4,L16), N(g4,L4), N(a4,L4),
  N(g4,L8d), N(f4,L16), N(e4,L8), N(c4,L8), N(f4,L2),
  N(as3,L8), N(as3,L16), N(as3,L16), N(SD,L8), N(as3,L8), N(as3,L8), N(as3,L16), N(as3,L16), N(SD,L8), N(f3,L8),
  N(a3,L8), N(a3,L16), N(a3,L16), N(SD,L8), N(a3,L8), N(a3,L8), N(a3,L16), N(a3,L16), N(SD,L8), N(e3,L8),
  0
};

// The end tunes start with the motif.
// The win turns it to D major, climbs over B flat and C, and holds a high A over a snare roll.
const uint8_t winMelody[] PROGMEM = {
  N(a4,L8d), N(a4,L16), N(d5,L8d), N(d5,L16), N(e5,L4), N(fs5,L4),
  N(d5,L8d), N(d5,L16), N(f5,L4), N(e5,L8d), N(e5,L16), N(g5,L4),
  N(fs5,L8d), N(fs5,L16), N(a5,L2d),
  0
};
const uint8_t winBass[] PROGMEM = {
  N(d4,L8), N(d4,L16), N(d4,L16), N(SN,L8), N(d5,L8), N(d4,L8), N(d4,L8), N(SN,L8), N(a3,L8),
  N(as3,L8), N(as3,L8), N(SN,L8), N(as3,L8), N(c4,L8), N(c4,L8), N(SN,L8), N(c4,L8),
  N(d4,L8), N(SN,L16), N(SN,L16), N(SN,L16), N(SN,L16), N(SN,L16), N(SN,L16), N(d4,L2),
  0
};
// The loss breaks off after the motif and sighs down to a low D.
// It plays slower, at the game theme's speed.
const uint8_t loseMelody[] PROGMEM = {
  N(a4,L8d), N(a4,L16), N(d5,L8d), N(d5,L16), N(c5,L2),
  N(as4,L4), N(a4,L4), N(f4,L4), N(e4,L4),
  N(d4,L2),
  0
};
const uint8_t loseBass[] PROGMEM = {
  N(d4,L2), N(a3,L2), N(as3,L2), N(a3,L2), N(d4,L2),
  0
};

// Short sounds, in the order of the Sound names in AWSounds.h.
// A sixteenth is 15 ms here. The tune voice reaches down to b3.
const uint8_t soundConfirm[] PROGMEM   = { N(e5,L16), N(a5,L16), 0 };
const uint8_t soundBack[] PROGMEM      = { N(a4,L16), N(e4,L16), 0 };
const uint8_t soundHit[] PROGMEM       = { N(c5,L16), N(g4,L16), N(d4,L16), N(b3,L16), 0 };
const uint8_t soundDestroyed[] PROGMEM = { N(a4,L4), N(f4,L4), N(d4,L4), N(b3,L2), 0 };
const uint8_t soundThump[] PROGMEM     = { N(b3,L8), 0 };
const uint8_t soundCaptured[] PROGMEM  = { N(d5,L8), N(fs5,L8), N(a5,L4), 0 };
const uint8_t * const sounds[] PROGMEM = {
  soundConfirm, soundBack, soundHit, soundDestroyed, soundThump, soundCaptured
};
constexpr uint8_t SoundUnit = 15;

// Timer ticks (about 1 ms each) per sixteenth. 111 is 132 beats a minute, 131 is 112.
constexpr uint8_t MenuTempo = 111;
constexpr uint8_t GameTempo = 131;

// How long a snare hit and a soft drum hiss, in ticks.
constexpr uint8_t SnareTicks = 40;
constexpr uint8_t SoftTicks = 30;

struct Voice {
  const uint8_t *pos;  // next note, nullptr when the voice is silent
  const uint8_t *loop; // where to go on at the end, nullptr to stop
  uint8_t unit;        // ticks per sixteenth
  uint8_t ticks;       // ticks left in this sixteenth
  uint8_t left;        // sixteenths left in this note
};
static Voice voices[3];
static uint8_t snare;       // ticks left in the snare hit
static bool softDrum;       // the hit hisses lower
static uint8_t noise = 1;   // random numbers for the snare

// Plays a square wave on pin 1. The counter restarts, so a higher note never waits
// for the counter to run past the old, longer count.
static void bassTone(uint16_t count){
  TCNT3 = 0;
  BeepPin1::tone(count);
}

// Volume: the game theme plays at half volume. A speaker pin can only be on or off,
// so pin 2 switches 62,500 times a second, too fast to hear. How long it stays on
// in each switch sets how far the speaker moves: a level from 0 to 255, 128 is the middle.
// Timer 3 then flips that level up and down at the note's pitch (its interrupt below),
// and the drums set random levels. The tune voice is silent in the game theme.
constexpr uint8_t QuietVolume = 64; // 128 would be full volume
static bool quietBass;

// Pin 2 as a volume knob, the bass note's level flips start on timer 3. Count 0 is silent.
static void quietTone(uint16_t count){
  TIMSK3 = 0;
  // a beep keeps pin 2 until it ends
  if (!voices[2].pos){
    TCCR4B = _BV(CS40); // no clock divide, top 255: 62,500 waves a second
    TC4H = 0;
    OCR4C = 255;
    OCR4A = 128;
    TCCR4A = _BV(COM4A1) | _BV(PWM4A);
  }
  if (count){
    TCCR3A = 0; // timer 3 keeps time only, pin 1 stays off
    OCR3A = count;
    TCNT3 = 0;
    TIMSK3 = _BV(OCIE3A);
  }
}

ISR(TIMER3_COMPA_vect){
  if (!voices[2].pos) OCR4A = (OCR4A > 128) ? 128 - QuietVolume : 128 + QuietVolume;
}

static void setPitch(uint8_t voice, uint8_t pitch){
  uint16_t count = (pitch && pitch < SN) ? pgm_read_word(pitches + pitch - 1) : 0;
  if (voice == 1){
    snare = 0;
    softDrum = pitch == SD;
    if (pitch == SN) snare = SnareTicks;
    else if (softDrum) snare = SoftTicks;
    // twice the count plays one octave lower
    count = (pitch && !snare) ? count * 2 + 1 : 0;
    if (quietBass) quietTone(count);
    else if (count) bassTone(count);
    else if (!snare) BeepPin1::noTone();
    return;
  }
  // the tune stays quiet while a beep plays
  if (voice == 0 && voices[2].pos) return;
  if (pitch){
    // Timer 4 counts 4 times slower than the beeper count, up to top.
    // The pin is on until a quarter of top. New values start with the next wave.
    uint16_t top = count >> 2;
    TCCR4B = _BV(CS42) | _BV(CS41) | _BV(CS40); // divide by 64
    TC4H = top >> 8;
    OCR4C = top;
    TC4H = 0;
    OCR4A = top >> 2;
    TCCR4A = _BV(COM4A1) | _BV(PWM4A);
  }
  else TCCR4A = 0;
}

// Timer 0 runs Arduino's millis() on its overflow. We use its compare B, about 976 times a second.
ISR(TIMER0_COMPB_vect){
  if (snare){
    // a new random pitch every tick sounds like a hiss
    noise ^= noise << 3;
    noise ^= noise >> 5;
    noise ^= noise << 1;
    uint8_t left = --snare;
    if (quietBass) { if (!voices[2].pos) OCR4A = left ? (noise & (QuietVolume - 1)) + 128 - QuietVolume / 2 : 128; }
    else if (left) bassTone(softDrum ? noise * 8 + 800 : noise * 2 + 120);
    else BeepPin1::noTone();
  }
  for (uint8_t i = 0; i < 3; i++){
    Voice &v = voices[i];
    if (!v.pos) continue;
    if (--v.ticks){
      // a short gap at the end of each note keeps repeated notes apart
      if (v.left == 1 && v.ticks == (v.unit >> 3)) setPitch(i, 0);
      continue;
    }
    v.ticks = v.unit;
    if (--v.left) continue;
    uint8_t note = pgm_read_byte(v.pos++);
    if (!note){
      v.pos = v.loop;
      if (!v.pos) { setPitch(i, 0); continue; }
      note = pgm_read_byte(v.pos++);
    }
    v.left = pgm_read_byte(lengths + (note & 7));
    setPitch(i, note >> 3);
  }
}

static void startVoice(uint8_t i, const uint8_t *notes, const uint8_t *loop, uint8_t unit){
  noInterrupts();
  Voice &v = voices[i];
  v.pos = notes;
  v.loop = loop;
  v.unit = unit;
  v.ticks = 1;
  v.left = 1;
  // stop the note the voice played before
  setPitch(i, 0);
  interrupts();
}

void soundBegin(){
  BeepPin1::begin();
  // timer 4: fast PWM, divide by 64 clock prescale
  TCCR4D = 0;
  TCCR4B = _BV(CS42) | _BV(CS41) | _BV(CS40);
  OCR0B = 128;
  TIMSK0 |= _BV(OCIE0B);
}

void playMenuTheme(){
  if (voices[0].loop == themeMelody) return;
  quietBass = false;
  TIMSK3 = 0;
  startVoice(0, themeMelody, themeMelody, MenuTempo);
  startVoice(1, themeBass, themeBass, MenuTempo);
}

void playGameTheme(){
  // stop the menu bass on pin 1 first, then the bass moves to pin 2 at half volume
  startVoice(1, nullptr, nullptr, GameTempo);
  quietBass = true;
  startVoice(0, nullptr, nullptr, GameTempo);
  startVoice(1, gameBass, gameBass, GameTempo);
}

void playSound(uint8_t sound){
  startVoice(2, (const uint8_t *)pgm_read_ptr(sounds + sound), nullptr, SoundUnit);
}

void playEndTune(bool won){
  uint8_t tempo = won ? MenuTempo : GameTempo;
  quietBass = false;
  TIMSK3 = 0;
  startVoice(0, won ? winMelody : loseMelody, nullptr, tempo);
  startVoice(1, won ? winBass : loseBass, nullptr, tempo);
}
