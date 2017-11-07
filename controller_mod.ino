#include "Nintendo.h"

//THIS CODE IS NOT AT ALL NEAR BEING DONE, there is lots of code that is left commented out for testing purposes, and the existing code works sporadically at best.

//TODO should make header file for the functions
//test the new code
//fix jc_upsmash code
void reset_report(Gamecube_Report_t *report);

// Define a Gamecube Controller
CGamecubeController GamecubeController(7);
CGamecubeConsole GamecubeConsole(8);

Gamecube_Report_t last_report = GamecubeController.getReport();

int mode_pins[] = {9,10,11,12,13};
int curr_mode = 0;

//I forget how to do OOP in C++ lol, using C style structs here
typedef struct _listptr {
  struct _listptr *next;
  Gamecube_Report_t *report;
} listptr;

typedef enum {
  Fox,
  Falco,
  Falcon,
  Marth,
  GW
} mode;

listptr *next_action = NULL;
listptr *last_action = NULL;

void setup() {
  for (int i = 0; i < 5; i++)
  {
    pinMode(mode_pins[i], OUTPUT);
  }
 
  // Start debug serial
  while (!Serial);
  Serial.begin(115200);

}
void senddownB() {
        auto data = defaultGamecubeData;
        reset_report(&data.report);
        data.report.b = 1;
        data.report.yAxis=21;
        if (!GamecubeConsole.write(data))
        { 
          Serial.println(F("Error writing Gamecube controller.2"));
        }
  
}

void sendY() {
          auto data = defaultGamecubeData;
        reset_report(&data.report);
        data.report.y = 1;
        if (!GamecubeConsole.write(data))
        { 
          Serial.println(F("Error writing Gamecube controller.1"));
        }
}

void sendUpsmash() {
  auto data = defaultGamecubeData;
  reset_report(&data.report);
  data.report.cyAxis = 255;
  if (!GamecubeConsole.write(data))
  { 
    Serial.println(F("Error writing Gamecube controller.1"));
  }
}

void sendNone() {
            auto data = defaultGamecubeData;
        reset_report(&data.report);
        if (!GamecubeConsole.write(data))
        { 
          Serial.println(F("Error writing Gamecube controller.1"));
        }
}

void fox_shine() {
//method 1: works on dolphin
        //auto time = millis();
        //while ((millis() - time) < (1000/60))
        //{
        //  sendY();
        //}
        
        //while ((millis() - time) < (2700/60))
        //{
        //  sendNone();
        //}
        
        //while ((millis() - time) < (7000/60))
        //{
        //  senddownB();
        //}
      //method 2: works on builds gamecube
      //maybe want to remove 1 none from the start and add one to the end or something

      
        sendY();
        sendY();
        sendNone();
        sendNone();
        sendNone();
//        
        senddownB();
        senddownB();
        senddownB();
        sendNone();
        sendNone();

          //method 3: testing
//          auto time = millis();
//          sendY();
//          while ((millis() - time) < (1000/60))
//          {
//         
//          }
//          sendNone();
//          sendNone();
//          //sendNone();
//          
//          while ((millis() - time) < (2700/60))
//          {
//            
//          }
//          senddownB();
//          senddownB();
//          senddownB();
//          senddownB();
//          while ((millis() - time) < (7000/60))
//          {
//            
//          }
//        
}

void falco_shine() {
  return;
}
void loop() {
        //if there's a queued action, do that action first
  if (next_action)
  {
        Serial.println("there's an action queued");
        auto report = *dequeue_action();
        auto data = defaultGamecubeData;
        reset_report(&data.report);
        data.report.y = 1;
        if (!GamecubeConsole.write(data))
        { 
          Serial.println(F("Error writing Gamecube controller.0"));
        }
        //Serial.println(report.y);
  }
    // Try to read the controller data
  else if (GamecubeController.read())
  {
    auto status = GamecubeController.getStatus();
    if (!check_controller(status))
      return;
    
    auto report = GamecubeController.getReport();

    if (report.r)
    {
      if (report.dleft && !last_report.dleft){
        prev_mode();
      }
      else if (report.dright && !last_report.dright) {
        next_mode();
      }

      if (report.b)
      { 
        switch(curr_mode) {
          case Fox:
            fox_shine();
            break;
          case Falco:
            falco_shine();
          default:
            break;
        }
        
        
       
        return;
      }
      if (report.a)
      {
        //jc_upsmash();
        sendY();
        sendY();
        sendNone();
        sendNone();
        //sendNone();
        sendUpsmash();
        sendUpsmash();
        sendUpsmash();
      }

      last_report = report;
      reset_report(&report);
    }
    else
    {
      last_report = report;
    }
   
      //might need to do this without getting the status/report (maybe the report resets fields or something?)
      if (!GamecubeConsole.write(report))
      {
        Serial.println(F("Error writing Gamecube controller."));
      }
      else
      {
        //Serial.println("wrote report");
      }
  }
  else
  {
    Serial.println(F("Error reading Gamecube controller."));
  }
}


void reset_report(Gamecube_Report_t *report) {
  report->start = 0;
  report->y = 0;
  report->x = 0;
  report->b = 0;
  report->a = 0;
  report->l = 0;
  report->r = 0;
  report->dup = 0;
  report->ddown = 0;
  report->dright = 0;
  report->dleft = 0;
  report->xAxis = 127;
  report->yAxis = 122;
  report->cxAxis = 133;
  report->cyAxis = 132;
  report->left = 45;
  report->right = 0;
}

Gamecube_Report_t make_empty_report() {
  //GamecubeController.read();
  //auto report = GamecubeController.getReport();
  auto data = defaultGamecubeData;
  reset_report(&data.report);
  data.origin = GamecubeController.getOrigin();
  return data.report;
}
//
//void queue_action(Gamecube_Report_t *report) {
//  //should fix the memory leak here
//  listptr *action = (listptr *)malloc(sizeof(listptr));
//  action->next = next_action;
//  action->report = report;
//  next_action = action;
//  if (next_action->next == NULL)
//  {
//    last_action = next_action;
//  }
//}

Gamecube_Report_t *dequeue_action() {
  if (!next_action)
    return NULL;
    
  auto report = next_action->report;
  //Serial.println(report->y);
  next_action = next_action->next;
  if (next_action == NULL)
  {
    last_action = NULL;
  }
  return report;
}

void append_action(Gamecube_Report_t *report) {
  //should fix the memory leak here
  listptr *action = (listptr *)malloc(sizeof(listptr));
  action->next = NULL;
  action->report = report;
  
  if (!next_action) //if next_action is null
  {
    Serial.println("inserting next action");
    next_action = action;
  }
  else 
  {
    //if there's a next_action there's also a last_action
    last_action->next = action;
  }
  
  last_action = action;
  Serial.println(next_action->report->y);
}

void jc_upsmash() {
//  auto report = make_empty_report();
//  report.y = 1;
//  if (!GamecubeConsole.write(report)) //send jump
//  {
//    Serial.println("failed jump");
//  }
//  delay(1000/60);
//  report = make_empty_report();
//  report.cyAxis = 250;
//  if (!GamecubeConsole.write(report)) //send jump
//  {
//    Serial.println("failed us");
//  }
//  delay(2000/60);
}

void repeat_append_action(Gamecube_Report_t *report, int amount)
{
  for (int i = 0; i < amount; i++)
  {
    append_action(report);
  }
}

void jc_shine() {
  if (!next_action)
  {
    auto report = make_empty_report();
    report.y = 1;
    append_action(&report);

    //should probably update to use the repeated action appending if it doesn't introduce bugs.
    report = make_empty_report();
    append_action(&report);

    report = make_empty_report();
    append_action(&report);

    report = make_empty_report();
    append_action(&report);

    report = make_empty_report();
    report.b = 1;
    report.yAxis = 21;
    append_action(&report);

    report = make_empty_report();
    append_action(&report);
    
    report = make_empty_report();
    append_action(&report);
    
    report = make_empty_report();
    append_action(&report);
  }
}


void change_mode(int shift) {
  digitalWrite(mode_pins[curr_mode], LOW);
  curr_mode = (curr_mode + shift + 5) % 5;
  digitalWrite(mode_pins[curr_mode], HIGH);
}

void next_mode() {
  change_mode(1);
}

void prev_mode() {
  change_mode(-1);
}

bool check_controller(Gamecube_Status_t &gc_status){
  switch (gc_status.device) {
    case NINTENDO_DEVICE_GC_NONE:
      Serial.println(F("No Gamecube Controller found!"));
      return false;
  }
  return true;
}



//some old buggy code
//    report = make_empty_report();
//    report.y = 1;
//    if (!GamecubeConsole.write(report)) //send jump
//    {
//      Serial.println("failed jump");
//    }
//    delay(2000/60);
//    
//    report = make_empty_report();
//    report.yAxis = 21;
//    report.b = 1;
//    if (!GamecubeConsole.write(report)) //send down + b
//    {
//      Serial.println("failed down b2");
//    }
//    delay(1000/60);
