#pragma once

#include <string>
#include <typeinfo>
#include "SlateKey.hpp"
#include "ArduinoJson.h"

template <typename T>
class EndPoint : public SlateKey<T>{
    public:
        EndPoint(const std::string id, const std::string quailID, T init, bool editable) : 
        SlateKey<T>(id, init), quailID(quailID), editable(editable) {}

        using SlateKey<T>::operator=;

        void metadump(JsonVariant dst) {
            JsonObject obj = dst.createNestedObject(this->id);
            obj[this->id] = this->get();
            // obj["Type"] = typeid(this->get()).name();
            obj["Quail ID"] = quailID;
            obj["Editable"] = editable;
        }

    private:
        const std::string quailID;
        bool editable;
};

class EndSensor : public EndPoint<float>{
    public:
        EndSensor(const std::string id, const std::string quailID, float init) :
        EndPoint(id, quailID, init, false) {}

        using EndPoint<float>::SlateKey::operator=;
};

class EndDerived : public EndSensor{
    public:
        EndDerived(const std::string id, float init) :
        EndSensor(id, NO_QUAIL_ID, init) {}

        using EndSensor::EndPoint<float>::SlateKey::operator=;
};

template <typename T>
class EndActuator : public EndPoint<T>{
    public:
        EndActuator(const std::string id, const std::string quailID) :
        EndPoint<T>(id, quailID, (T)false, true) {}

        using EndPoint<T>::SlateKey::operator=;
};