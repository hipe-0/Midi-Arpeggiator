
#include "MIDIUSB.h"
#include "debug.h"

class Midi
{
private:
	int  synchSignals_;
  int  synch_;
	
public:
	MIDI() { synchSignals_=0; synch_ = false;}

	// First parameter is the event type (0x09 = note on, 0x08 = note off).
	// Second parameter is note-on/note-off, combined with the channel.
	// Channel can be anything between 0-15. Typically reported to the user as 1-16.
	// Third parameter is the note number (48 = middle C).
	// Fourth parameter is the velocity (64 = normal, 127 = fastest).

	void noteOn(byte channel, byte pitch, byte velocity) 
	{
	  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
	  MidiUSB.sendMIDI(noteOn);
    //DEBUGVAL("noteOn ",pitch)
	}

	void noteOff(byte channel, byte pitch, byte velocity) 
	{
	  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
	  MidiUSB.sendMIDI(noteOff);
    //DEBUGVAL("noteOff ", pitch)
	}


	void killAllNotes()
	{
	  for (int i = 0; i < 128; i++)
		  noteOff(0, i, 0);
	}
	
	void flush() {  MidiUSB.flush(); }
	
	void readMidiSynch()
	{
		midiEventPacket_t event = MidiUSB.read();
		
		if( event.header == 15) // F
		{
			if( event.byte1 == 250 ) // 0xFA START
      { 
        synch_ = true;
        synchSignals_ = 0;
      }
			
			if( event.byte1 == 248 && synch_ == true) // 0xF8 Timing Clock
				synchSignals_ = synchSignals_ % 24 + 1;

			if( event.byte1 == 252 ) // 0xFC STOP
      {  
        synchSignals_ = 0;
        synch_ = false;
      }
		}
    /*
		if( event.header > 0 )
		{
			Serial.print  ("             readMidiSynch = ");
			Serial.print  ( synchSignals );
     
      Serial.print  ("  midi received = ");
			Serial.print  ( event.header );
			Serial.print  (", ");
			Serial.print  ( event.byte1 );
			Serial.print  (", ");
			Serial.print  ( event.byte2 );
			Serial.print  (", ");
			Serial.println( event.byte3 );
		}
   */
    if( event.header > 0 && synchSignals_ == 1 )
      DEBUGTXT  ("Midi synchSignal")
	}

  // returns >0 if synch is on. value starts at 1 and is increasing after each midi synch signal is received. max value is 24 --> goes back to 1
  int getSynchSignals() { return synchSignals_;}

	// First parameter is the event type (0x0B = control change).
	// Second parameter is the event type, combined with the channel.
	// Third parameter is the control number number (0-119).
	// Fourth parameter is the control value (0-127).
	void controlChange(byte channel, byte control, byte value) 
	{
	  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
	  MidiUSB.sendMIDI(event);
	}
};
