/*

   Midi Arpeggiator USB device

   Created: 2023-09-17
   Target device: Arduino Micro

   Author: Hippolyte Mounier


*/

#define NODEBUG

#include "debug.h"
#include "Led.h"
#include "Midi.h"
#include "ArpeggiatorMachine.h"
#include "RecordButtonManager.h"
#include "RecordManager.h"
#include "SynchManager.h"

ButtonManager        buttons;
PotentiometerManager pots;
Midi                 midi;
RecordButtonManager  recordButton;
RecordManager        record;
ArpeggiatorMachine   arp;
SynchManager         sync;

void setup() 
{
  Serial.begin(115200);
  DEBUGTXT("Setup was started")
  
  arp.initMidiOut      (midi);
  record.initPresses   (&buttons);
  record.initArpPointer(&arp);

  recordButton.turnOnLedManually(); delay(100);
}


void loop() 
{
  // Read hardware
  buttons.readAll();
  pots.readPotentiometers();
  midi.readMidiSynch();
  int s = midi.getSynchSignals();

  sync.setSynchValue(s);
  sync.setRecordPlayHeadMs(record.computePlayHeadMs());
  sync.compute();
  
  long u = sync.getUnSyncMs();  
  if (recordButton.isPlayingMusic()) record.shiftPlayHeadByMs(u);

  // manage hardware
  arp.setChordMode      ( pots.getChordMode()     );
  arp.setChordRootNote  ( pots.getChordRootNote() );
  arp.setChordOctave    ( pots.getChordOctave()   );
  arp.setChordNumber    ( pots.getChordNumber()   );
  arp.setChordAlternate ( pots.getExtraChordPress() );

  changeReadOrRecordStates();

  playAndRecordNotesFromButtons();

  if (s == 1) recordButton.turnOnLedManually();
  
  if ( recordButton.isPlayingMusic()) record.playRecordedNotes();
}


void changeReadOrRecordStates()
{
  int s = midi.getSynchSignals();
  recordButton.setSynch( s==0 || s==1 );
  recordButton.manageRecordButton();
  int state     = recordButton.getState();
  int prevState = recordButton.getPreviousState();

  if( state != prevState ) // change of state ?
  {
    switch( state )
    {
      case RECORD_RECORDING_OR_PLAY:     
        record.startRecording();
        sync.startRecordTimer();
        break;
      case RECORD_OVERDUB_OR_STOP:       
        record.startOverdubing();
        break;
      case RECORD_PLAY:
        if (prevState==RECORD_RECORDING) 
        {
          record.storeRecording();
          sync.storeRecordLength();
          record.startPlaying();
        }
        
        if (prevState==RECORD_OVERDUB) 
          record.storeOverdubing(); 

        if (prevState==RECORD_RECORDING_OR_PLAY || prevState==RECORD_OVERDUB_OR_STOP) 
          record.cancelOnGoingRecording();

        break;
      case RECORD_STOP:                  
        midi.killAllNotes(); 
        
        if (prevState==RECORD_RECORDING_OR_PLAY || prevState==RECORD_OVERDUB_OR_STOP) 
          record.cancelOnGoingRecording();

        record.stop();
        sync.stop();
        break;
    }
  }

}

int initialButtonPressOctave[8];

void playAndRecordNotesFromButtons()
{
  for (int i = 0; i < buttons.nbButtons(); i++)
  {
    if ( buttons.hasChanged(i) )
    {
      int octave = arp.getChordOctave();
      if(buttons.isPressed(i))
        initialButtonPressOctave[i] = octave;
      else
        octave = initialButtonPressOctave[i];
      
      //DEBUGVAL("playAndRecordNotesFromButtons octave = ", octave)
        
      //DEBUGVAL2("playAndRecordNotesFromButtons i = ",  i, " has changed ",  buttons.isPressed(i) )

      arp.playNoteMidi(i, arp.computePitch(i, octave), buttons.isPressed(i), octave);

      if(recordButton.isRecordingOrOverdub())
        record.recordNoteTemp(buttons.isPressed(i), i, octave);
    }
  }
}
