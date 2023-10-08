
class SynchManager
{
  private:
  int previousSynchValue_ ;
  int synchValue_;
  long startRecordingDate_;
  long synchCounter_;
  long recordingDuration_;
  long recordingSyncCount_;
  long previousSynchTime_;
  long recordPlayHead_;
  float idealPlayHead_;
  long unsyncMs_;
  bool isSynching_;

  public:

  void setSynchValue( int v ) 
  { 
    previousSynchValue_ = synchValue_; 
    synchValue_ = v;
    
    if (synchValue_ == 0) isSynching_ = false;
  }

  void startRecordTimer()
  {
    startRecordingDate_ = millis();
    synchCounter_  = 0;
    idealPlayHead_ = 0.0;
    isSynching_ = true;
  }

  void setRecordPlayHeadMs( long ph)
  {
    recordPlayHead_ = ph;
  }

  void storeRecordLength()
  {
    long m = millis();
    recordingDuration_ = m - startRecordingDate_;
    recordingSyncCount_= synchCounter_;
    idealPlayHead_     = 0.0;

    DEBUGVAL2("storeRecordLength recordingDuration_", recordingDuration_, "recordingSyncCount_", recordingSyncCount_);
  }

  void stop()
  {
    isSynching_    = false;
    idealPlayHead_ = 0;
  }


  void compute()
  {
 
    //DEBUGVAL2("isSynching_", isSynching_, "synchValue_", synchValue_)

    if(isSynching_)
    {
      int deltaSync= synchValue_ - previousSynchValue_;
      if ( deltaSync < 0 ) deltaSync += 24;

      if(deltaSync > 0)    // synch signal is received
      {
        long t             =  millis();
        int deltaMs        =  t - previousSynchTime_;
        previousSynchTime_ =  t;
        synchCounter_      += deltaSync;

        /*       
        float bpm = 0.0;
        if (deltaMs > 0)
          bpm = (float) deltaSync * (1000.0 / 24.0 * 60.0) / (float) deltaMs ;

          DEBUGVAL3 ( "deltaSync", deltaSync, "deltaMs", deltaMs, "bpm", bpm )
        */

        if (recordingDuration_ > 0 && recordingSyncCount_ > 0)
        {
          float idealRecordingTickMs = 0;
          if (recordingSyncCount_ > 0)      idealRecordingTickMs = (float) recordingDuration_ / (float) recordingSyncCount_ ;

          idealPlayHead_ += idealRecordingTickMs * deltaSync;
          
          if (idealPlayHead_ > recordingDuration_) idealPlayHead_ -=recordingDuration_;

          long unsyncTemp = idealPlayHead_ - recordPlayHead_ ;
          unsyncMs_ = limit_( unsyncTemp , -10, 10);

          //DEBUGVAL3 ( "idealPlayHead_", idealPlayHead_, "recordPlayHead_", recordPlayHead_, "unsyncTemp", unsyncTemp)
        }
        else
          unsyncMs_ = 0;

      }
      else
        unsyncMs_ = 0;
    }
    else
      unsyncMs_ = 0;

  }

  long getUnSyncMs()
  {
    return  unsyncMs_;
  }

private:
  int limit_ (int u, int min, int max)
  {
    if(u<min) u = min;
    if(u>max) u = max;
    return u;
  }

};