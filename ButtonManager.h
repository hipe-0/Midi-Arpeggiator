#define   NUM_BUTTONS         8  // digital buttons on Arduino

const int BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 11};

class ButtonManager
{
	private:
	// store last buttons notes pressed
	bool    isPressedButtons_ [NUM_BUTTONS];
	bool    previousButtons_  [NUM_BUTTONS];

	public:
	
	ButtonManager() 
	{ 
		for (int i = 0; i < NUM_BUTTONS; i++)
			pinMode(BUTTON_PINS[i], INPUT_PULLUP);
	}

  
	int nbButtons() { return NUM_BUTTONS; }
	
	void readAll()
	{
	  for (int i = 0; i < NUM_BUTTONS; i++)
	  {
  		previousButtons_[i] = isPressedButtons_[i];
  		isPressedButtons_[i] = (digitalRead(BUTTON_PINS[i]) == LOW);
  /*
      if( isPressed(i) )
      {
        Serial.print  ("readAll i = ");
        Serial.print  ( i );
        Serial.print  (" ");
        Serial.println  ( isPressedButtons[i] );
      }*/
	  }
	}


	bool hasChanged(int i) { return isPressedButtons_[i] != previousButtons_[i]; }
	bool isPressed(int i)  { return isPressedButtons_[i]; }

};
