
class Led
{
  private:
  long initT_ = 0;
  int  pin_   = -1;
  
  public:
  void initPin(int newPin) { pin_ = newPin; }
  
  // LED management
  void turnOn()  { digitalWrite(pin_, HIGH); }
  void turnOff() { digitalWrite(pin_, LOW); }
  
  void initTime()  {   initT_= millis();  }
  void blink() 
  { 
    const int WAIT_LED_CYCLE_MILLISECOND = 200;
    
    int t = (millis()-initT_) / WAIT_LED_CYCLE_MILLISECOND;
    if ( t % 2 == 0 ) 
      digitalWrite(pin_, LOW); 
    else
      digitalWrite(pin_, HIGH); 
  }
};
