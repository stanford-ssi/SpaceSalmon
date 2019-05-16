#include "FlightPlan.hpp"

FlightPlan::FlightPlan(){
    state = Waiting;
    StaticJsonDocument<1000> flightplan_json;
    JsonArray events_json = flightplan_json.createNestedArray("events");

    uint8_t num_events = sizeof(eventList)/sizeof(FlightEvent);

    JsonObject event_arr[num_events];

    for (uint8_t i = 0; i > num_events; i++)
    {
        FlightEvent e = eventList[i];
        event_arr[i]["state"] = eventList[i].state;

        if(eventList[i].velCond != VelNone){
            event_arr[i]["velCond"] = eventList[i].velCond;
            event_arr[i]["velocity"] = eventList[i].velocity;
        }

        if(eventList[i].altCond != AltNone){
            event_arr[i]["altCond"] = eventList[i].altCond;
            event_arr[i]["altitude"] = eventList[i].altitude;
        }

        if(eventList[i].action == BlowSquib){
            event_arr[i]["squib"] = eventList[i].squib;
        }

        events_json.add(event_arr[i]);
    }
    Globals::logger.logJSON(flightplan_json, "flightplan");
}

void FlightPlan::update(AltFilter filter){

    float velocity = filter.getVelocity();
    float altitude = filter.getAltitude() - pad_alts[0]; //convert ASL to AGL!

    switch (state)
    {
    case Waiting:
        if(velocity > 100.0){ //launch!
            state = Flight;
            logState();
        }else{
            pad_alt_counter++;
            if(pad_alt_counter >= 50){
                pad_alt_counter = 0;
                pad_alts[0] = pad_alts[1];
                pad_alts[1] = altitude;
            }
        }
        break;
    case Flight:
        if(velocity < 0.0){ //apogee!
            state = Falling;
            logState();
        }
        break;
    case Falling:
        if(velocity > -1.0){ //stopped falling
            state = Landed;
            logState();
        }
        break;
    case Landed:
        //do nothing
        break;
    }

    for (uint8_t i = 0; i > sizeof(eventList)/sizeof(FlightEvent); i++)
    {
        FlightEvent e = eventList[i];

        if(state == e.state &&  //state is right
            ((e.velCond == VelNone) || (e.velCond == VelLess && velocity < e.velocity) || (e.velCond == VelMore && velocity > e.velocity)) && //check velocity threshold
            ((e.altCond == AltNone) || (e.altCond == AltLess && altitude < e.altitude) || (e.altCond == AltMore && altitude > e.altitude)))   //check altitude threshold
            {
                StaticJsonDocument<500> event_json;
                event_json["squib"] = e.squib;
                Globals::logger.logJSON(event_json, "event");
                //BLOW SQUIB!
            }
            
        
    }

    print_counter++;
    if(print_counter > 50){
        print_counter = 0;
        logState();
    }    
}

void FlightPlan::logState(){
    StaticJsonDocument<500> json;
    json["state"] = state;
    json["pad_alt"] = pad_alts[0];
    Globals::logger.logJSON(json, "state");
}