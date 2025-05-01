#include <iostream>

#include <sigslot/signal.hpp>

void f() { std::cout << "free function" << std::endl; }

struct o
{
    void operator()() { std::cout << "function object" << std::endl; }
};

void fArg(float &x)
{
    std::cout << "Adding 2 to x which is " << x << std::endl;
    x += 2.0f;
}

struct oArg
{
    void operator()(float &x)
    {
        std::cout << "Adding 2 to x which is " << x << std::endl;
        x += 2.0f;
    }
};

int main()
{
    // Without arguments
    auto lambda = []()
    { std::cout << "lambda\n"; };

    sigslot::signal<> sigNoArg;

    sigNoArg.connect(f);
    sigNoArg.connect(o());
    sigNoArg.connect(lambda);

    sigNoArg();

    // With argument
    auto lambdaArg = [](float &x)
    {
        std::cout << "Adding 2 to x which is " << x << std::endl;
        x += 2.0f;
    };

    float arg = 1.23f;
    sigslot::signal<float &> sig;

    sig.connect(fArg);
    sig.connect(oArg());
    sig.connect(lambdaArg);

    std::cout << "x = " << arg << std::endl;
    sig(arg);
    std::cout << "x = " << arg << std::endl;

    return 0;
}
