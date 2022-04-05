#pragma once
#include "Container.hpp"
#include "SlateKey.hpp"
#include "Array.hpp"
#include <functional>

template <typename T>
class EndPoint : public SlateKey<T>{
    public:
        EndPoint(const std::string id, const std::string quailID, T init, bool editable) : 
        SlateKey<T>(id, init), quailID(quailID), editable(editable) {}

        void metadump(JsonVariant dst) {
            JsonObject obj = dst.createNestedObject(this->id);
            obj[this->id] = this->get();
            // obj["Type"] = typeid(this->get()).name();
            obj["Quail ID"] = quailID;
            obj["Editable"] = editable;
        }

        EndPoint<T> &operator=(EndPoint<T> src) {
            SlateKey<T>::operator=(src);
            this->id = src.id;
            this->quailID = src.quailID;
            this->editable = src.editable;
            return *this;
        }

    private:
        std::string quailID;
        bool editable;
};


template <typename T>
class EndActuator : public EndPoint<T>{
    public:
        EndActuator(const std::string id, const std::string quailID) :
        EndPoint<T>(id, quailID, (T)false, true) {}

        EndActuator() : EndPoint<T>("", "", (T)false, true) {}

        EndActuator<T> &operator=(EndActuator<T> src) {
            EndPoint<T>::operator=(src);
            return *this;
        }
};

class Igniter : public Container<2> {
    public:
        EndActuator<bool> arm;
        EndActuator<bool> state;
        Igniter(const std::string id, const std::string quailID) : Container(id, {
            std::ref(arm),
            std::ref(state)
        }), quailID(quailID) {
            arm = EndActuator<bool>(id, quailID);
            state = EndActuator<bool>(id, quailID);
        };

    private:
        std::string quailID;
};

class Slate : public Container<3>
{
public:
    Array<Igniter, 2> squib = Array<Igniter, 2>("squib", {
        Igniter("1", "E1"),
        Igniter("2", "E2")
    });

    // class Squib : public Container<2>
    // {
    // public:
    //     EndActuator<bool> arm = EndActuator<bool>("arm", "E1");
    //     Array<SlateKey<bool>,8> fire = Array<SlateKey<bool>,8>("fire",{
    //         SlateKey<bool>("1", false),
    //         SlateKey<bool>("2", false),
    //         SlateKey<bool>("3", false),
    //         SlateKey<bool>("4", false),
    //         SlateKey<bool>("5", false),
    //         SlateKey<bool>("6", false),
    //         SlateKey<bool>("7", false),
    //         SlateKey<bool>("8", false)
    //     });
    //     Squib(const std::string id) : Container(id, {std::ref(arm),std::ref(fire)}){};
    // } squib = Squib("squib");
    
    Array<SlateKey<bool>,8> solenoid = Array<SlateKey<bool>,8>("solenoid",{
        SlateKey<bool>("1", false),
        SlateKey<bool>("2", false),
        SlateKey<bool>("3", false),
        SlateKey<bool>("4", false),
        SlateKey<bool>("5", false),
        SlateKey<bool>("6", false),
        SlateKey<bool>("7", false),
        SlateKey<bool>("8", false)
    });

    Array<SlateKey<float>,4> adc_in = Array<SlateKey<float>,4>("adc_in",{
        SlateKey<float>("1", 0.0),
        SlateKey<float>("2", 0.0),
        SlateKey<float>("3", 0.0),
        SlateKey<float>("4", 0.0)
    });
    
    Slate(const std::string id) : Container(id, {std::ref(squib), std::ref(solenoid), std::ref(adc_in)}){};
};
