#include "AltimeterTask.hpp"

extern const char *build_version;

AltimeterTask::AltimeterTask(uint8_t priority) : Task(priority, "Altimeter") {}

void AltimeterTask::activity()
{
    char str[150];

    snprintf(str, sizeof(str), "Altimeter Started\nBuild Version: %s", build_version);
    sys.tasks.logger.log(str);

    TickType_t lastStatusTime = xTaskGetTickCount();

    OneBattery battery(sys.adc0);

    while (true)
    {

        vTaskDelayUntil(&lastStatusTime, 1000);

        digitalWrite(ALT_LED, true);

        StaticJsonDocument<1000> status_json;

        status_json["tick"] = xTaskGetTickCount();

        uint32_t runtime;
        TaskStatus_t tasks[15];
        uint8_t count = uxTaskGetSystemState(tasks, 15, &runtime);

        JsonObject tasks_json = status_json.createNestedObject("tasks");

        for (uint8_t i = 0; i < count; i++)
        {
            float percent = ((float)tasks[i].ulRunTimeCounter) / ((float)runtime) * 100.0;
            tasks_json[tasks[i].pcTaskName] = percent;
        }

        OneBattery::cell_voltage_t voltage = battery.readVoltage();
        sys.tasks.alt.battData.post(voltage);

        JsonObject bat_json = status_json.createNestedObject("bat");
        bat_json["main"] = voltage.cellMain;
        bat_json["bkp"] = voltage.cellBackup;

        status_json["log"] = sys.tasks.logger.isLoggingEnabled();
        
        bool armed;
        sys.armed.get(armed);
        status_json["armed"] = armed;

        JsonArray pyro_json = status_json.createNestedArray("pyro");

        bool pyroA = sys.pyro.getStatus(Pyro::SquibA);
        bool pyroB = sys.pyro.getStatus(Pyro::SquibB);

        pyro_json.add(pyroA);
        pyro_json.add(pyroB);

        #ifdef CHOVY
            //sys.pyrosquib.getResistance();

            bool pyroC = sys.pyro.getStatus(Pyro::SquibC);
            bool pyroD = sys.pyro.getStatus(Pyro::SquibD);
            pyro_json.add(pyroC);
            pyro_json.add(pyroD);
        #endif

        

        sys.tasks.logger.logJSON(status_json, "status");

        digitalWrite(ALT_LED, false);
    }
}