#pragma once

class Pyro
{
  public:
    typedef enum {
        SquibNone,
        SquibA,
        SquibB,
        SquibC
    } PyroChannel;

    virtual void arm();
    virtual void disarm();

    //fire some squibs!
    virtual bool fire(PyroChannel channel);

    //read the status
    virtual void getStatus();
};