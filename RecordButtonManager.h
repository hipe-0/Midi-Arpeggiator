
#define LED_PIN                10  // digital 10
#define BUTTON_RECORD_PIN      12  // digital 12


class RecordButtonManager
{
  private:
  int  previousRecordButton_;
  int  recordButton_;
  int  recordState_;
  int  previousState_;
  int  futureState_;
  Led  recordLed_;
  bool isSynched_;
  bool isPending_;
  long pressedTime_;

  #define RECORD_STOP               0 
  #define RECORD_RECORDING_OR_PLAY  1 
  #define RECORD_RECORDING          2 
  #define RECORD_PLAY               3 
  #define RECORD_OVERDUB_OR_STOP    4 
  #define RECORD_OVERDUB            5 
    
  #define DELAY_LONG_PRESS_MS   800

  public:
  RecordButtonManager() 
  { 
    recordState_   = RECORD_STOP;
    previousState_ = RECORD_STOP;
    futureState_   = RECORD_STOP;
    recordLed_.initPin(LED_PIN);  
    pinMode(BUTTON_RECORD_PIN, INPUT_PULLUP);
    isSynched_ = false;
    isPending_ = false;
  }

  void setSynch(bool b)  { isSynched_ = b;  }

  void manageRecordButton()
  {
    // statebox of button "Record"
    previousRecordButton_ = recordButton_;
    previousState_        = recordState_;
    
    // read hardware
    recordButton_ = digitalRead(BUTTON_RECORD_PIN);
    bool pressed = (recordButton_ == LOW);

    if(previousRecordButton_ != recordButton_) // has the record button changed state ?
    {
      if(pressed) 
      {
        pressedTime_ = millis();
    
        switch(recordState_ )
        {
          case RECORD_STOP:                  futureState_ = RECORD_RECORDING_OR_PLAY; isPending_=true;  recordLed_.initTime();  break;
          case RECORD_PLAY:                  futureState_ = RECORD_OVERDUB_OR_STOP;   isPending_=true;  recordLed_.initTime();  break;
        }
      }
      else // if button released then switch to play mode
      {
        switch(recordState_)
        {
          case RECORD_RECORDING:
          case RECORD_OVERDUB:
          case RECORD_RECORDING_OR_PLAY:     futureState_ = RECORD_PLAY;   isPending_=true;    break;
          case RECORD_OVERDUB_OR_STOP:       futureState_ = RECORD_STOP;   isPending_=true;    break;
        }
      }
    }
    
    if(isSynched_ && isPending_)
    {
      recordState_ = futureState_;
      isPending_ = false;
    }

    bool isLongPress = (millis() - pressedTime_ >= DELAY_LONG_PRESS_MS) && pressed;
    if(isLongPress)
    {
       switch(recordState_)
        {
          case RECORD_RECORDING_OR_PLAY:    recordState_ = RECORD_RECORDING;  break;
          case RECORD_OVERDUB_OR_STOP  :    recordState_ = RECORD_OVERDUB;    break;
        }
    }
    
  
    // manage led
    switch( recordState_ ) 
    {
      case RECORD_PLAY: recordLed_.turnOn(); break;
      case RECORD_RECORDING: 
      case RECORD_RECORDING_OR_PLAY: 
      case RECORD_OVERDUB: 
      case RECORD_OVERDUB_OR_STOP: recordLed_.blink(); break;
      case RECORD_STOP: recordLed_.turnOff(); break;
    }

    if(recordState_ != previousState_)
    {
      switch(recordState_)
      {
        case RECORD_STOP :              Serial.println("RecordState RECORD_STOP");              break;
        case RECORD_PLAY :              Serial.println("RecordState RECORD_PLAY");              break;
        case RECORD_RECORDING :         Serial.println("RecordState RECORD_RECORDING");         break;
        case RECORD_RECORDING_OR_PLAY : Serial.println("RecordState RECORD_RECORDING_OR_PLAY"); break;
        case RECORD_OVERDUB :           Serial.println("RecordState RECORD_OVERDUB");           break;
        case RECORD_OVERDUB_OR_STOP :   Serial.println("RecordState RECORD_OVERDUB_OR_STOP");   break;
        default: Serial.print("RecordState_ "); Serial.println(recordState_);  
      }
    }
    /*if(recordButton != previousRecordButton)
    {
      Serial.print("recordButton ");
      Serial.println(recordButton);
    }*/
  }

  int getState()         { return recordState_;  }
  int getPreviousState() { return previousState_;}

  bool isRecordingOrOverdub()
  {
    return  recordState_ == RECORD_RECORDING 
          || recordState_ == RECORD_RECORDING_OR_PLAY
          || recordState_ == RECORD_OVERDUB
          || recordState_ == RECORD_OVERDUB_OR_STOP;
  }

  bool isPlayingMusic()
  {
    return recordState_ == RECORD_PLAY 
    || recordState_ ==  RECORD_OVERDUB_OR_STOP 
    || recordState_ ==  RECORD_OVERDUB;
  }

  void turnOnLedManually()
  {
    recordLed_.turnOn();
  }
    
  } ;
