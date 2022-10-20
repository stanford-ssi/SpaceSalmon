#pragma once
#include "../../slate/slate_utils.h"

#include <array>
#include <functional>

template <typename... Ts>
class SlateRegistry
{
private:
    const std::array<std::reference_wrapper<const Slate>, sizeof...(Ts)> list;

public:
    SlateRegistry(Ts const &...xs) : list({(std::ref<const Slate>(xs), ...)})
    {
    }

    void parse(uint16_t offset, float value)
    {
        for (auto elem : list)
        {
            Slate &test = elem.get();
            test.parseCMD(offset, value);
        }
    }
};

// A BUNCH OF OLD STUFF that I used to learn C++ lol
// we could make a factory that make objects?
// template <typename... Ts>
// SlateRegistry2<Ts> makeSR(Ts const &...xs){
//     return SlateRegistry2<Ts>(xs);
// }

// c++17 has type deduction from constructors

// std::array<std::reference_wrapper<const Slate>, sizeof...(Ts)>

// for(const auto t : {xs...}){}
// SlateRegistry2(Ts const&... xs):list({xs...}){

// template <size_t i>
// class SlateRegistry
// {
// private:
//     const std::array<std::reference_wrapper<Slate>, i> list;

// public:
//     SlateRegistry(std::array<std::reference_wrapper<Slate>, i> slates) : list(slates){};

//     void parse(uint16_t offset, float value)
//     {
//         for (auto elem : list)
//         {
//             Slate &test = elem.get();
//             test.parseCMD(offset, value);
//         }
//     }
// };