#include "ButtonManager.h"
#include "PotentiometerManager.h"
#include "debug.h"

// midi settings
#define MIDI_VELOCITY       100
#define MIDI_CHANNEL        0
#define NOTES_PER_OCTAVE    7
#define BASS_NOTE_MODIFIER  -24
#define PITCH_MIDDLE_C      24  // midi note for C
#define BASS_NOTE_NUMBER    7
#define ALTERNATE_MODES     3

// ARPEGIATOR
class ArpeggiatorMachine {
private:
  int chordMode_;
  int chordRootNote_;
  int chordOctave_;
  int chordNumber_;
  Midi &midi_;
  int alternateNr_;

  int previousPitchesOct_[NUM_BUTTONS][OCTAVE_COUNT];

  // Musical parameters

  const byte NOTE_PITCHES_WIDE[MODE_COUNT][NOTES_PER_OCTAVE * 2] = {
    { 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23 },  // Major
    { 0, 2, 3, 5, 7, 8, 11, 12, 14, 15, 17, 19, 20, 23 },  // Minor Harmonic
    { 0, 2, 3, 5, 7, 9, 11, 12, 14, 15, 17, 19, 21, 23 },  // Minor Melodic
    { 0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22 },  // Minor Natural (eolien)
    { 0, 2, 3, 5, 7, 9, 10, 12, 14, 15, 17, 19, 21, 22 },  // Dorian (Celtic)
    { 0, 2, 4, 5, 7, 9, 10, 12, 14, 16, 17, 19, 21, 22 },  // Mixolydien (folk blues)
    { 0, 1, 3, 5, 7, 8, 10, 12, 13, 15, 17, 19, 20, 22 },  // Phrygien (metal)
    { 0, 1, 4, 5, 7, 8, 10, 12, 13, 16, 17, 19, 20, 22 }   // Phrygien Dominant (metal)
  };

  const int FIFTHS_SEQUENCE[SEMITONES_PER_OCTAVE] = { 0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5 };

public:
  ArpeggiatorMachine()           {    chordNumber_ = 1;     }
  void initMidiOut(Midi &m)      {    midi_ = m;            }
  void setChordMode(int v)       {    chordMode_ = v;       }
  void setChordRootNote(int v)   {    chordRootNote_ = v;   }
  void setChordOctave(int v)     {    chordOctave_ = v;     }
  void setChordNumber(int v)     {    chordNumber_ = v;     }
  void setChordAlternate(int b)  {    alternateNr_ = b;    }
  int  getChordOctave()          {    return chordOctave_;  }

  int computeModifierToSwichMinorMajorChords(int noteNum)
  {
    int mod = 0;
    if(noteNum == 2)
    {
      int distanceOfThird= NOTE_PITCHES_WIDE[chordMode_][2 + chordNumber_ - 1] - NOTE_PITCHES_WIDE[chordMode_][0 + chordNumber_ - 1];
      
      DEBUGVAL("distanceOfThird = ",distanceOfThird)

      int modifiers[] = {0, 0, 0, 1, -1, 0, 0, 0};
      mod = modifiers[distanceOfThird];
 
    }
    DEBUGVAL2("mod = ", mod,"noteNum = ", noteNum)
    return mod;
  }

  int computePitch(int noteNum, int octave) {
    int bassNoteModifier = 0;
    int alternateModifier = 0;

    if (noteNum == BASS_NOTE_NUMBER) bassNoteModifier = BASS_NOTE_MODIFIER;  // reduces 2 octaves

    switch(alternateNr_ % ALTERNATE_MODES)
    {
      case 1: alternateModifier = computeModifierToSwichMinorMajorChords(noteNum); break;
      case 2: alternateModifier = 1; break;
    }
    
    //DEBUGVAL("alternateNr_ = ", alternateNr_)

    return PITCH_MIDDLE_C
           + NOTE_PITCHES_WIDE[chordMode_][noteNum + chordNumber_ - 1]
           + FIFTHS_SEQUENCE[chordRootNote_]
           + octave * 12
           + bassNoteModifier
           + alternateModifier;
  }

  int computePitch(int noteNum) {
    return computePitch(noteNum, chordOctave_);
  }

  void playNoteMidi(int noteNum, int pitch, bool pressed, int octave) {
    if (pressed) 
    {
      // is that note key still ringing? stop it
      if (previousPitchesOct_[noteNum][octave] > 0)
        midi_.noteOff(0, previousPitchesOct_[noteNum][octave], 0);

      midi_.noteOn(MIDI_CHANNEL, pitch, MIDI_VELOCITY);
      previousPitchesOct_[noteNum][octave] = pitch;

      //DEBUGVAL3("playNoteMidi note ON pitch", previousPitchesOct_[noteNum][octave], "noteNum", noteNum, "octave", octave)
    } 
    else 
    {
      //DEBUGVAL3("playNoteMidi note OFF previousPitches_", previousPitchesOct_[noteNum][octave], "noteNum", noteNum, "octave", octave)
      
      if (previousPitchesOct_[noteNum][octave] > 0)
      {
        midi_.noteOff(MIDI_CHANNEL, previousPitchesOct_[noteNum][octave], 0);
        previousPitchesOct_[noteNum][octave] = 0;
      }
    }

    midi_.flush();
  }

  void playNoteMidiFromButton(int noteNum, bool pressed) {
    playNoteMidi(noteNum, computePitch(noteNum), pressed, chordOctave_);
  }
};
