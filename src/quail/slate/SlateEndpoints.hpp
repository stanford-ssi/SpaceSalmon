#pragma once

#include <string>
#include <typeinfo>
#include "SlateKey.hpp"
#include "ArduinoJson.h"
#include "../config.h"

template <typename T>
class EndPoint : public SlateKey<T>{
    public:
        EndPoint(const std::string id, const std::string quailID, T init, bool editable, const std::string desc = "none") : 
        SlateKey<T>(id, init), quailID(quailID), editable(editable), desc(desc) {}

        EndPoint() : SlateKey<T>(NO_QUAIL_ID, (T) false), quailID(NO_QUAIL_ID), editable(false) {}

        // SlateKeys own mutexes which cannot be copied (assignment is deleted)
        // need to manually define assignment for all descendants
        EndPoint<T> &operator=(EndPoint<T> src) {
            SlateKey<T>::operator=(src);
            this->id = src.id;
            this->quailID = src.quailID;
            this->editable = src.editable;
            this->desc = src.desc;
            return *this;
        }

        EndPoint<T> &operator<<(const JsonVariant src) override {
            if (editable) SlateKey<T>::operator<<(src);
            return *this;
        }

        EndPoint<T> &operator<<(const T &in) {
            SlateKey<T>::operator<<(in);
            return *this;
        }

        void metadump(JsonVariant dst) override {
            JsonObject obj = dst.createNestedObject(this->id);
            obj["valu"] = this->get();
            obj["desc"] = desc;
            obj["unit"] = typeid(this->get()).name();
            obj["qpin"] = quailID;
            obj["edit"] = editable;
        }

    private:
        std::string quailID;
        std::string desc;
        bool editable;
};

class EndSensor : public EndPoint<float>{
    public:
        EndSensor(const std::string id, const std::string quailID, float init, const std::string desc) :
        EndPoint(id, quailID, init, false, desc) {}

        EndSensor() : EndSensor(NO_QUAIL_ID, NO_QUAIL_ID, 0.0, "unknown") {}

        EndSensor &operator=(EndSensor src) {
            EndPoint<float>::operator=(src);
            return *this;
        }
};

class EndDerived : public EndSensor{
    public:
        EndDerived(const std::string id, float init) :
        EndSensor(id, NO_QUAIL_ID, init, "derived") {}

        EndDerived() : EndDerived(NO_QUAIL_ID, 0.0) {}

        EndDerived &operator=(EndDerived src) {
            EndSensor::operator=(src);
            return *this;
        }
};

template <typename T>
class EndActuator : public EndPoint<T>{
    public:
        EndActuator(const std::string id, const std::string quailID) :
        EndPoint<T>(id, quailID, (T)false, true) {}

        EndActuator() : EndActuator<T>(NO_QUAIL_ID, NO_QUAIL_ID) {}

        EndActuator<T> &operator=(EndActuator<T> src) {
            EndPoint<T>::operator=(src);
            return *this;
        }
};