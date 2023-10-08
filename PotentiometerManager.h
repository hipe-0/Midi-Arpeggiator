// PIN numbers as soldered on the Arduino micro board
// Analog pins
const int POT_MODE_PIN          = 0;  // analog A0 input
const int POT_ROOT_NOTE_PIN     = 1;  // analog A1 input
const int POT_OCTAVE_PIN        = 2;  // analog A2 input
const int POT_CHORD_NUMBER_PIN  = 3;  // analog A3 input, actually not a pot, but provide same voltage as a pot
const int VALUE_STABILIZER      = 2; // filters chaotic values
const int SEMITONES_PER_OCTAVE  = 12;

#define MODE_COUNT              8
#define OCTAVE_COUNT            5


class PotentiometerManager
{
	private:
	int currentPots[4]; // store  state of all pots
	
	// store  chord state
	int chordMode_;
	int chordRootNote_;
	int chordOctave_;
  int chordNumber_;
  int transientChordNumber_;
  long transientTime_;
  bool isTransient_;
  int extraPresses_;
 
	public:
	PotentiometerManager() { chordNumber_=1;transientChordNumber_=1;extraPresses_=0; }
	
	void readPotentiometers()
	{
	  for(int i = 0; i<4 ; i++)
	  {
      int val = analogRead(i);

      if( abs(val - currentPots[i]) >= VALUE_STABILIZER )  // only changes of more that 2 points are recorded to avoid glitches
        currentPots[i] = val;
	  }
	  
	  chordMode_     = (int) (mapfloat(currentPots[ POT_MODE_PIN      ], 0, 1023, 0, MODE_COUNT - 1 ));
	  chordRootNote_ = (int) (mapfloat(currentPots[ POT_ROOT_NOTE_PIN ], 0, 1023, 0, SEMITONES_PER_OCTAVE - 1));
	  chordOctave_   = (int) (mapfloat(currentPots[ POT_OCTAVE_PIN    ], 0, 1023, 0, OCTAVE_COUNT - 1)) ;

    computeChordButtons_();
  }


	long mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
	{
	  return round( (x - in_min) * (out_max - out_min) / (float) (in_max - in_min) + out_min );
	}
	
	int getChordMode()        { return chordMode_;     }
	int getChordRootNote()    { return chordRootNote_; }
	int getChordOctave()      { return chordOctave_;   }
	int getChordNumber()      { return chordNumber_;   }
  int getExtraChordPress()  { return extraPresses_;  } 

private:
	void computeChordButtons_()
  {
    int transientOld = transientChordNumber_;

    #define CHORD_TRANSIENT_DURATION 25
    #define NO_PRESS_NUMBER 8
//    #define DEAD_ZONE 20
    
    int potLevels[] = { 0, 50, 120, 330, 440, 610, 750, 970, 1023 };

	  int val = currentPots[POT_CHORD_NUMBER_PIN];
    for(int i=1 ; i < 9 ; i++)
      if ( val >= potLevels[i-1]  &&  val <= potLevels[i] ) 
        transientChordNumber_ = i;

    long d = millis()-transientTime_;
    /*
    Serial.print("transientChordNumber_ = ");
    Serial.print(transientChordNumber_);
    Serial.print("  chordNumber_ = ");
    Serial.print(chordNumber_);
    Serial.print("  val = ");
    Serial.println(val);*/

    if (transientOld != transientChordNumber_)
    {  
      transientTime_ = millis();
      isTransient_ = true;
    }
    else
      if(d > CHORD_TRANSIENT_DURATION && isTransient_ ) 
      {
        isTransient_ = false;
        if(transientChordNumber_!= NO_PRESS_NUMBER) 
        {
          if(chordNumber_ == transientChordNumber_)
          {
            extraPresses_++;
          }
          else
          {
            chordNumber_ = transientChordNumber_;
            isTransient_ = false;
            extraPresses_ = 0;
          }

          
          if(extraPresses_ > 0)
          {
            Serial.print("Alternate chord ");
            Serial.println(extraPresses_);
          }
          else
            Serial.println("Standard chord");
          
        }
      }

/*
    { // serial plotter for 
      Serial.print("-10,");
      Serial.print(val);
      Serial.println(",1030");
    }*/

  }

};
