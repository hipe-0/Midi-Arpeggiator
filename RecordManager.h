
#define MAX_NOTE_COUNT 60  // WARNING memory usage
 
// RECORD
class RecordManager
{
  private:
  long recordStartTime_;
  long playStartTime_;
  long duration_;
  long lastPlayHead_;

  long     noteTime_          [MAX_NOTE_COUNT];
  uint8_t  noteNumber_        [MAX_NOTE_COUNT];
  bool     noteIsPressed_     [MAX_NOTE_COUNT];
  uint8_t  noteOctave_        [MAX_NOTE_COUNT];
  int      noteCount_;
  
  long     noteTimeTemp_      [MAX_NOTE_COUNT];
  uint8_t  noteNumberTemp_    [MAX_NOTE_COUNT];
  bool     noteIsPressedTemp_ [MAX_NOTE_COUNT];
  uint8_t  noteOctaveTemp_    [MAX_NOTE_COUNT];
  int      noteCountTemp_;
  bool     isPlaying;

  ButtonManager      * press_;
  ArpeggiatorMachine * arp_;

  public:
  RecordManager() { }
  void initPresses   ( ButtonManager * p)      { press_ = p; }
  void initArpPointer( ArpeggiatorMachine * a) { arp_ = a;   }

  void startRecording()
  {  
    noteCountTemp_   = 0;
    lastPlayHead_    = 0;
    // do not reset noteCount_ here as this recording may be cancelled if a stop command is entered by user (button released)
    recordStartTime_ = millis();
    playStartTime_   = recordStartTime_;
    isPlaying        = true;

    addCurrentPresses_();
    Serial.println("startRecording");
  }
  

  void startPlaying()
  {
    playStartTime_ = millis();
    lastPlayHead_  = 0;
    isPlaying      = true;
  }
  
  void stop()
  {
    isPlaying     = false;
    lastPlayHead_ = 0;
  }

  void copyRecordedNotesToTemporary_()
  {
    for (int i=0; i< noteCountTemp_; i++)
      {
        noteTimeTemp_[i]      = noteTime_     [i];
        noteNumberTemp_[i]    = noteNumber_   [i];
        noteIsPressedTemp_[i] = noteIsPressed_[i];
        noteOctaveTemp_[i]    = noteOctave_   [i];
      }
    
    noteCountTemp_ = noteCount_;
  }
  

  void storeRecording()
  {
    // Copy notes from temp
    for (int i=0; i< noteCountTemp_; i++)
      {
        noteTime_[i]      = noteTimeTemp_     [i];
        noteNumber_[i]    = noteNumberTemp_   [i];
        noteIsPressed_[i] = noteIsPressedTemp_[i];
        noteOctave_[i]    = noteOctaveTemp_   [i];
      }
    
    noteCount_ = noteCountTemp_;
    duration_ = millis() - recordStartTime_;
    playStartTime_ = millis(); 

    Serial.print  ("storeRecording noteCount_ = ");
    Serial.println  ( noteCount_ );
  }


  void cancelOnGoingRecording()
  {
    noteCountTemp_=0;
    Serial.println("cancelOnGoingRecording");
  }


  void startOverdubing()
  {  
    copyRecordedNotesToTemporary_();
    recordStartTime_=playStartTime_;
    
    addCurrentPresses_();
    isPlaying = true;
  }

  
  void storeOverdubing()
  {
    // reset all notes recorded
    noteCount_ = 0;
  
    // Copy all notes from temp
    for (int i=0; i< noteCountTemp_; i++)
      if(noteCount_ < MAX_NOTE_COUNT-1)
        {
          noteTime_     [noteCount_] = noteTimeTemp_     [i] % duration_;
          noteNumber_   [noteCount_] = noteNumberTemp_   [i];
          noteIsPressed_[noteCount_] = noteIsPressedTemp_[i];
          noteOctave_   [noteCount_] = noteOctaveTemp_   [i];
          noteCount_++;
        }
    Serial.print  ("storeOverdubing noteCount_ = ");
    Serial.println  ( noteCount_ );
    isPlaying = true;
  }

/*
  void addDummyNote_()
  {
    noteTime_     [noteCount_] = 0;
    noteNumber_   [noteCount_] = 0;
    noteIsPressed_[noteCount_] = true;
    noteOctave_   [noteCount_] = 0;
    noteCount_++;

    noteTime_     [noteCount_] = 100;
    noteNumber_   [noteCount_] = 0;
    noteIsPressed_[noteCount_] = false;
    noteOctave_   [noteCount_] = 0;
    noteCount_++;

    Serial.print  ("addDummyNote_ noteCount_ = ");
    Serial.println  ( noteCount_ );
  }
*/

/*
bool isInsideExistingNoteTemp_(int targetNote, int targetOctave)
{
  long playHead = millis() - recordStartTime_;
  long previousNoteTime = 0 ;
  int  previousNote = -1;

  // searching for the previous pressed note before targetNote
  for (int i=0; i< noteCountTemp_; i++)
    if(noteNumberTemp_[i] == targetNote &&  noteOctaveTemp_[i] == targetOctave && noteIsPressedTemp_[i])
      if(noteTimeTemp_[i] > previousNoteTime && noteTimeTemp_[i] < playHead ) 
      {
        previousNoteTime = noteTimeTemp_[i];
        previousNote = i;
      }

  // searching for the next released note after targetNote
  long nextNoteTime = 99999999 ;
  int  nextNote = -1;

  for (int i=0; i< noteCountTemp_; i++)
    if(noteNumberTemp_[i] == targetNote &&  noteOctaveTemp_[i] == targetOctave && !noteIsPressedTemp_[i])
      if(noteTimeTemp_[i] < nextNoteTime && noteTimeTemp_[i] > playHead ) 
      {
        nextNoteTime = noteTimeTemp_[i];
        nextNote = i;
      }

}*/

  void recordNoteTemp(bool pressed, int targetNote_, int octave)
  {
    if( noteCountTemp_ < MAX_NOTE_COUNT)
    {
      noteTimeTemp_      [noteCountTemp_] = millis() - recordStartTime_ ;
      noteIsPressedTemp_ [noteCountTemp_] = pressed;
      noteNumberTemp_    [noteCountTemp_] = targetNote_;
      noteOctaveTemp_    [noteCountTemp_] = octave;
      noteCountTemp_ ++;
      
      //DEBUGVAL2    ("Record noteTimeTemp_", noteTimeTemp_[noteCountTemp_], "Record noteCountTemp_", noteCountTemp_ );
    }
  }


  long computePlayHeadMs()
  {
    if (isPlaying)
      return (millis() - playStartTime_);
    else
      return 0;
  }


  void shiftPlayHeadByMs(long ms)
  {
    const long DEAD_ZONE_MS = 20;
    long ph = computePlayHeadMs();

    if (isPlaying)
    {
      if (ph > 0 + DEAD_ZONE_MS && ph < duration_ - DEAD_ZONE_MS) 
      {
        //DEBUGVAL("shiftPlayHeadByMs ms", ms)
        playStartTime_ -= ms;
      }
    }
  }


  void playRecordedNotes()
  {
    //Serial.print  ("playRecordedNotes noteCount_ = ");
    //Serial.println  ( noteCount_ );

    if (duration_ > 0)
    {
      long playHead = computePlayHeadMs();
      bool isLooped=false;

      if (playHead > duration_) // normally playhead goes increasing, unless it starts again to zero 
      { 
        playHead      -= duration_;

        DEBUGTXT("recording has looped!")
        isLooped       = true;
        playStartTime_ = millis();
      }
  
      for (int i=0; i< noteCount_; i++)
      {
        
        bool betweenLastAndPlayhead = noteTime_[i] >= lastPlayHead_ && noteTime_[i] <  playHead ;
        bool afterLastorbeforePlayhead = noteTime_[i] <  playHead || noteTime_[i] >= lastPlayHead_;

        DEBUGVAL("noteTime_[i]", noteTime_[i])
        
        // time to sound a note?
        if( (!isLooped && betweenLastAndPlayhead) || (isLooped && afterLastorbeforePlayhead))
        {
          int pitch = arp_->computePitch(noteNumber_[i], noteOctave_[i]);
  
          arp_->playNoteMidi ( noteNumber_[i], pitch, noteIsPressed_[i], noteOctave_[i] );
        }

      }
      
      lastPlayHead_ = playHead;
    }
  }


private:
  void addCurrentPresses_()
  {
    int c=0;
    // records already pressed keys at the start of the recording
    for( int i=0; i < NUM_BUTTONS; i++)
    {      
      if( press_->isPressed(i) ) // a button is already pressed, let's record it.
      { 
        c++;
        recordNoteTemp( true, i, arp_->getChordOctave() );
      }
    }

    Serial.print("addCurrentPresses count of presses = ");
    Serial.println(c);
  }

/*
  void finishNotesAtEndOfRecording_()
  {
    int lastNoteTimePressed [NUM_BUTTONS] [OCTAVE_COUNT];
    int lastNoteTimeReleased[NUM_BUTTONS] [OCTAVE_COUNT];
    int nbAdded = 0;

    //clear tables
    for (int n=0; n<NUM_BUTTONS; n++)
      for (int o=0; o<OCTAVE_COUNT; o++)
      {
        lastNoteTimePressed [n][o] = -1;
        lastNoteTimeReleased[n][o] = -1;
      }

    for (int i=0; i< noteCount_; i++)
      if(noteCount_ < MAX_NOTE_COUNT-1)
        {
          int t1 = lastNoteTimePressed[noteNumber_[i]][noteOctave_[i]];
          int t2 = lastNoteTimeReleased[noteNumber_[i]][noteOctave_[i]];
          if ( noteTime_[i] > t1 )
            if( noteIsPressed_[i] )
              lastNoteTimePressed[noteNumber_[i]][noteOctave_[i]] = noteTime_[i];
          if ( noteTime_[i] > t2 )
            if( !noteIsPressed_[i] )
              lastNoteTimeReleased[noteNumber_[i]][noteOctave_[i]] = noteTime_[i];
        }

    for (int n=0; n<NUM_BUTTONS; n++)
      for (int o=0; o<OCTAVE_COUNT; o++)
      {
        if ( lastNoteTimePressed[n][o] > -1 
              && lastNoteTimeReleased[n][o] <= lastNoteTimePressed[n][o] ) // a note is pressed but not released ?
        {
        if(noteCount_ < MAX_NOTE_COUNT-1)
          { // let's add the release notes at end of recording
            noteTime_     [noteCount_] = duration_-1;
            noteNumber_   [noteCount_] = n;
            noteIsPressed_[noteCount_] = false;
            noteOctave_   [noteCount_] = o;
            noteCount_++; nbAdded++;
          }
        }
      }
      Serial.print    ("finishNotesAtEndOfRecording_ addedReleases = ");
      Serial.println  ( nbAdded );

  }
*/
};
