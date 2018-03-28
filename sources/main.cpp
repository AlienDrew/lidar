#include "application.h"

int main(int argc, char* argv[])
{
    core::Application app(argc, argv, "LIDAR");
    return app.exec();
}
