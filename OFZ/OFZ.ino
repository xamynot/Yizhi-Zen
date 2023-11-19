
//One finger zen v1.3

#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin8192_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Smooth.h>
#include <AutoMap.h>
#include <AutoRange.h>


Oscil <8192, AUDIO_RATE> aOscil(SIN8192_DATA);;

// for triggering the envelope



ADSR <AUDIO_RATE, AUDIO_RATE> envelope;
EventDelay noteDelay;
boolean note_is_on = true;

int n = 0;

void setup(){

   Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
 // Serial.begin(115200);



// noteDelay.set(2000); // 2 second countdown
  startMozzi(640);
}

unsigned int duration, attack, decay, sustain, release_ms;


void updateControl(){

  int freq_set;
  int carrier_freq;
  int piezo_value = mozziAnalogRead(2);

  int pitch_value = mozziAnalogRead(0);

  int speed_rate = mozziAnalogRead(1);
  if(noteDelay.ready()){
      int range_ = mozziAnalogRead(3)/100;
  int range_fix;
     
    if(piezo_value < 10)
    {     //threshold1
      piezo_value = 0;
    }
      if(piezo_value > 500)
    {     //threshold1_high
      piezo_value = piezo_value / 2;
    }

      // choose envelope levels
    //  byte attack_level = piezo_value*10;
    //  byte decay_level = piezo_value*10;
      envelope.setADLevels(4000,piezo_value);

    // generate a random new adsr time parameter value in milliseconds
     unsigned int new_value = piezo_value;
     
     // randomly choose one of the adsr parameters and set the new value
  /*   switch (rand(4)){
       case 0:
       attack = new_value*10;
       break;

       case 1:
       decay = new_value*10;
       break;

       case 2:
       sustain = new_value*10;
       break;

       case 3:
       release_ms = new_value*10;
       break;
     }*/
     envelope.setTimes(0,piezo_value*2,piezo_value*2,piezo_value*2);
     envelope.noteOn();
    int scale[10] ={523,587,659,784,880,1046,1174,1318,1568,1760};
 /*   int scale2[9] ={523,587,659,784,880,1046,1174,1318,1568};
    int scale3[8] ={523,587,659,784,880,1046,1174,1318};
    int scale4[7] ={523,587,659,784,880,1046,1174,1318};
    int scale4[7] ={523,587,659,784,880,1046,1174,1318};*/

      range_fix = piezo_value%10;

      
      if(range_fix >range_)
      {
        range_fix = range_; 
      }
      Serial.println(range_fix);
       int freq_ = scale[range_fix];
       
       int freq_set = freq_/(rand(2)+1)*(rand(2)+1)*(rand(2)+1)/(rand(2)+1)/(rand(2)+1)*(pitch_value/40+1);
      aOscil.setFreq(freq_set);
    

     

/*
     // print to screen
     Serial.print("midi_note\t"); Serial.println(midi_note);
     Serial.print("attack_level\t"); Serial.println(attack_level);
     Serial.print("decay_level\t"); Serial.println(decay_level);
     Serial.print("attack\t\t"); Serial.println(attack);
     Serial.print("decay\t\t"); Serial.println(decay);
     Serial.print("sustain\t\t"); Serial.println(sustain);
     Serial.print("release\t\t"); Serial.println(release_ms);
     Serial.println();
*/
    noteDelay.start(piezo_value+(speed_rate>>5));
  }
}


AudioOutput_t updateAudio(){
  envelope.update();
  int sig_= envelope.next() * aOscil.next();
  return MonoOutput::fromNBit(16,sig_);
}


void loop(){
  audioHook(); // required here
}
