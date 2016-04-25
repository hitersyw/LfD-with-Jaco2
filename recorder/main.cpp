#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <Kinova.API.CommLayerUbuntu.h>
#include <KinovaTypes.h>
#include <stdio.h>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;


int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}


int main(int argc, char *argv[])
{
    cout << endl << endl;
    cout << "===================================================" << endl;
    cout << "===== A Demonstration Recorder for Jaco2 arm  =====" << endl;
    cout << "===================================================" << endl;
    cout << "code: Reza Ahmadzadeh (IRIM, 2016)." << endl;
    cout << "structure: i j1 j2 j3 j4 j5 j6 x y z thetax thetay thetaz" << endl;
    cout << "{Position, Angle} : {mm, deg}" << endl << endl;
    if ( argc != 2 ) {// argc should be 2 for correct execution
        // We print argv[0] assuming it is the program name
        cout << "Usage: "<< argv[0] <<" <filename>" << endl;
        cout << "Error: a filename has to be provided, for instance [demo_1.dat]" << endl  << endl << endl;
        return 0;
    }
    else {
        // We assume argv[1] is a filename to open
        cout << "recording in a file saved as: " << argv[1] << endl;
        }


    int result;
    //AngularPosition dataCommand;
    AngularPosition dataPosition;
    CartesianPosition cartPosition;

    //Handle for the library's command layer.
    void * commandLayer_handle;

    //Function pointers to the functions we need
    int (*MyInitAPI)();
    int (*MyCloseAPI)();
    //int (*MyGetAngularCommand)(AngularPosition &);
    int (*MyGetAngularPosition)(AngularPosition &);
    int (*MyGetCartesianPosition)(CartesianPosition &);
    int (*MyGetDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);
    int (*MySetActiveDevice)(KinovaDevice device);

    //We load the library
    commandLayer_handle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);

    //We load the functions from the library (Under Windows, use GetProcAddress)
    MyInitAPI = (int (*)()) dlsym(commandLayer_handle,"InitAPI");
    MyCloseAPI = (int (*)()) dlsym(commandLayer_handle,"CloseAPI");
    //MyGetAngularCommand = (int (*)(AngularPosition &)) dlsym(commandLayer_handle,"GetAngularCommand");
    MyGetAngularPosition = (int (*)(AngularPosition &)) dlsym(commandLayer_handle,"GetAngularPosition");
    MyGetCartesianPosition = (int (*) (CartesianPosition &)) dlsym(commandLayer_handle,"GetCartesianPosition");
    MyGetDevices = (int (*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) dlsym(commandLayer_handle,"GetDevices");
    MySetActiveDevice = (int (*)(KinovaDevice devices)) dlsym(commandLayer_handle,"SetActiveDevice");

    //If the was loaded correctly
    if((MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MyGetCartesianPosition == NULL) || (MyGetAngularPosition == NULL)
       || (MySetActiveDevice == NULL) || (MyGetDevices == NULL))
    {
        cout << "* * *  ERROR: initialization failed!  * * *" << endl;
    }
    else
    {
        cout << "Initialization Completed." << endl << endl;

        result = (*MyInitAPI)();

        // lets open a file
        ofstream outfile;
        outfile.open(argv[1]); //("trajectory.dat");
        cout << "an empty file has been created to record the trajectory" << endl;


        cout << "Initialization's result :" << result << endl;

        KinovaDevice list[MAX_KINOVA_DEVICE];

        int devicesCount = MyGetDevices(list, result);
        if (devicesCount == 0)
        {
            cout << "\n WARNING: The robot is off or is not in the loop!" << endl;
            return 0;
        }

        for(int i = 0; i < devicesCount; i++)
        {
            cout << "Found a robot on the USB bus (" << list[i].SerialNumber << ") (" << list[i].DeviceType << ")" << endl;

            //Setting the current device as the active device.
            MySetActiveDevice(list[i]);

            int pointCounter = 0;
            while(!kbhit())
            {
                // (*MyGetAngularCommand)(dataCommand);
                (*MyGetAngularPosition)(dataPosition);
                (*MyGetCartesianPosition)(cartPosition);

                outfile << pointCounter << "  " << dataPosition.Actuators.Actuator1 << "  " << dataPosition.Actuators.Actuator2 << "  " << dataPosition.Actuators.Actuator3 << "  "
                        << dataPosition.Actuators.Actuator4 << "  " << dataPosition.Actuators.Actuator5 << "  " << dataPosition.Actuators.Actuator6 << "  "
                        << cartPosition.Coordinates.X << "  " << cartPosition.Coordinates.Y << "  " << cartPosition.Coordinates.Z << "  "
                        << cartPosition.Coordinates.ThetaX << "  " << cartPosition.Coordinates.ThetaY << "  " << cartPosition.Coordinates.ThetaZ << endl;
                cout << pointCounter << "  " << dataPosition.Actuators.Actuator1 << "  " << dataPosition.Actuators.Actuator2 << "  " << dataPosition.Actuators.Actuator3 << "  "
                     << dataPosition.Actuators.Actuator4 << "  " << dataPosition.Actuators.Actuator5 << "  " << dataPosition.Actuators.Actuator6 << "  "
                     << cartPosition.Coordinates.X << "  " << cartPosition.Coordinates.Y << "  " << cartPosition.Coordinates.Z << "  "
                     << cartPosition.Coordinates.ThetaX << "  " << cartPosition.Coordinates.ThetaY << "  " << cartPosition.Coordinates.ThetaZ << endl;
                pointCounter++;
            }
        }

        outfile.close();

        cout << endl << "Closing API...!" << endl;
        result = (*MyCloseAPI)();
    }

    dlclose(commandLayer_handle);

    return 0;
}
