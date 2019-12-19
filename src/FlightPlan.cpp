#include "FlightPlan.hpp"
#include "main.hpp"

FlightPlan::FlightPlan(){
    state = Waiting;
    state_timer = 0;
}

void FlightPlan::update(AltFilter& filter){

    float velocity = filter.getVelocity();
    float altitude = filter.getAltitude() - pad_alts[0]; //convert ASL to AGL!

    state_timer++;

    switch (state)
    {
    case Waiting:
        if(state_timer > 200 && velocity > 100.0){ //launch!
            state = Flight;
            state_timer = 0;
            sys.pyro.arm();
            logState();
            filter.logState();
        }else{
            pad_alt_counter++;
            if(pad_alt_counter >= 1000){
                pad_alt_counter = 0;
                pad_alts[0] = pad_alts[1];
                pad_alts[1] = filter.getAltitude();
            }
        }
        break;
    case Flight:
        if(state_timer > 200 && velocity < 0.0){ //apogee!
            state = Falling;
            state_timer = 0;
            logState();
            filter.logState();
        }
        break;
    case Falling:
        if(state_timer > 500 && velocity > -5.0){ //stopped falling
            state = Landed;
            state_timer = 0;
            sys.pyro.disarm();
            logState();
            filter.logState();
        }

        break;
    case Landed:
        //do nothing
        break;
    }

    for (uint8_t i = 0; i < (sizeof(eventList)/sizeof(FlightEvent)); i++)
    {
        FlightEvent e = eventList[i];
        
        if(event_done[i] == false){

            if(state == e.state &&  //state is right
               ((e.velCond == VelNone) || (e.velCond == VelLess && velocity < e.velocity) || (e.velCond == VelMore && velocity > e.velocity)) && //check velocity threshold
               ((e.altCond == AltNone) || (e.altCond == AltLess && altitude < e.altitude) || (e.altCond == AltMore && altitude > e.altitude)))   //check altitude threshold
            {
                if(xTaskGetTickCount() > event_timer){
                    event_timer = xTaskGetTickCount() + e.time;
                    event_done[i] = true;

                    sys.pyro.fire(e.squib);

                    StaticJsonDocument<500> event_json;
                    event_json["squib"] = (uint8_t) e.squib;
                    event_json["tick"] = xTaskGetTickCount();
                    sys.tasks.logger.logJSON(event_json, "event");
                }
            }   
        }
    }

    if(xTaskGetTickCount() - print_timer > 500){
        print_timer = xTaskGetTickCount();
        logState();
    }    
}

void FlightPlan::logState(){
    StaticJsonDocument<500> json;
    json["state"] = (uint8_t) state;
    json["pad_alt"] = pad_alts[0];
    json["tick"] = xTaskGetTickCount();
    json["timer"] = state_timer;
    sys.tasks.logger.logJSON(json, "flight_state");
}

//TODO: Strongly Typed Enums, and an implicit conversion to strings for logging
void FlightPlan::dumpConfig(){
    StaticJsonDocument<1000> flightplan_json;
    JsonArray events_json = flightplan_json.createNestedArray("events");
    uint8_t num_events = sizeof(eventList)/sizeof(FlightEvent);
    flightplan_json["tick"] = xTaskGetTickCount();
    flightplan_json["count"] = num_events;

    for (uint8_t i = 0; i < num_events; i++)
    {
        JsonObject event_json = events_json.createNestedObject();
        event_json["state"] = (uint8_t) eventList[i].state;

        if(eventList[i].velCond != VelNone){
            event_json["velCond"] = (uint8_t) eventList[i].velCond;
            event_json["velocity"] = eventList[i].velocity;
        }

        if(eventList[i].altCond != AltNone){
            event_json["altCond"] = (uint8_t) eventList[i].altCond;
            event_json["altitude"] = eventList[i].altitude;
        }

        if(eventList[i].action == BlowSquib){
            event_json["squib"] = (uint8_t) eventList[i].squib;
        }
    }
    sys.tasks.logger.logJSON(flightplan_json, "flightplan");
}