/* -------------------------------------
 *  A Simple Trajectory Executer
 *
 *
 *
 * set singlePointTest for sending a single point to the module
 * set incrementalTest for sending points in an incremental loop
 * input the file including trajectory information recorded using the recorder module in the repo
 *
 *
 * <<< SINCE THERE IS NO OBSTACLE AVOIDANCE MONITORING THE ROBOT USING THIS MODULE CAN BE DANGEREOUS >>>
 *
 *
 *
 * Reza Ahmadzadeh (IRIM, April-2016)
 *
 * -------------------------------------*/

#include <iostream>
#include <dlfcn.h>
#include <KinovaTypes.h>
#include <Kinova.API.CommLayerUbuntu.h>
#include <stdio.h>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <string>
#include <sstream>

using namespace std;
int main()
{
        int result;
        bool singlePointTest, incrementalTest, trajectoryTest, homeTorqueTest, homePoseTest;
        CartesianPosition data;
        AngularPosition angles;
        cout << "Executing trajectories using SendBasicTrajectory" << endl;
        //Handle for the library's command layer.
        void * commandLayer_handle;
        //Function pointers to the functions we need
        int (*MyInitAPI)();
        int (*MyCloseAPI)();
        int (*MySendBasicTrajectory)(TrajectoryPoint command);
        int (*MyStartControlAPI)();
        int (*MyMoveHome)();
        int (*MyGetCartesianCommand)(CartesianPosition &);
        int (*MyGetCartesianPosition)(CartesianPosition &);
        int (*MyGetAngularPosition)(AngularPosition &);

        //We load the library (Under Windows, use the function LoadLibrary)
        commandLayer_handle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);
        //We load the functions from the library (Under Windows, use GetProcAddress)
        MyInitAPI = (int (*)()) dlsym(commandLayer_handle,"InitAPI");
        MyCloseAPI = (int (*)()) dlsym(commandLayer_handle,"CloseAPI");
        MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(commandLayer_handle,"SendBasicTrajectory");
        MyStartControlAPI = (int (*)()) dlsym(commandLayer_handle,"StartControlAPI");
        MyGetCartesianCommand = (int (*)(CartesianPosition &)) dlsym(commandLayer_handle,"GetCartesianCommand");
        MyGetCartesianPosition = (int (*)(CartesianPosition &)) dlsym(commandLayer_handle,"GetCartesianPosition");
        MyGetAngularPosition = (int(*)(AngularPosition &)) dlsym(commandLayer_handle,"GetAngularPosition");
        MyMoveHome = (int (*)()) dlsym(commandLayer_handle,"MoveHome");

        singlePointTest = false;
        incrementalTest = false;
        trajectoryTest = false;
        homeTorqueTest = true;
        homePoseTest = false;
        //If the was loaded correctly
        if((MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MySendBasicTrajectory == NULL) || (MyStartControlAPI == NULL) || 
        (MyMoveHome == NULL) || (MyGetCartesianCommand == NULL) || (MyGetCartesianPosition == NULL))
        {
                cout << "Unable to initialize the command layer." << endl;
        }
        else
        {
                result = (*MyInitAPI)();

                result = (*MyGetCartesianPosition)(data);
                cout << " Home: [ " << data.Coordinates.X << "," << data.Coordinates.Y << "," << data.Coordinates.Z << " ]" << endl;
                usleep(3000);
                TrajectoryPoint trajectoryPoint;
                if (homeTorqueTest)
                {
                    trajectoryPoint.InitStruct();
                    trajectoryPoint.Position.Type = ANGULAR_POSITION;
                    trajectoryPoint.Position.Actuators.Actuator1 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator2 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator3 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator4 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator5 = 0.0f;
                    trajectoryPoint.Position.Actuators.Actuator6 = 90.0f;
                    (*MySendBasicTrajectory)(trajectoryPoint);
                    result = (*MyGetAngularPosition)(angles);
                    cout << " Target: [ " << angles.Actuators.Actuator1 << "," << angles.Actuators.Actuator2 << "," << angles.Actuators.Actuator3 << "," << angles.Actuators.Actuator4 << "," << angles.Actuators.Actuator5 << "," << angles.Actuators.Actuator6 << " ]" << endl;
                    usleep(3000);
                }

                if (homePoseTest)
                {
                    trajectoryPoint.InitStruct();
                    trajectoryPoint.Position.Type = ANGULAR_POSITION;
                    trajectoryPoint.Position.Actuators.Actuator1 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator2 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator3 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator4 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator5 = 180.0f;
                    trajectoryPoint.Position.Actuators.Actuator6 = 180.0f;
                    (*MySendBasicTrajectory)(trajectoryPoint);
                    result = (*MyGetAngularPosition)(angles);
                    cout << " Target: [ " << angles.Actuators.Actuator1 << "," << angles.Actuators.Actuator2 << "," << angles.Actuators.Actuator3 << "," << angles.Actuators.Actuator4 << "," << angles.Actuators.Actuator5 << "," << angles.Actuators.Actuator6 << " ]" << endl;
                    usleep(3000);
                }


                if (singlePointTest)
                {
                    MyMoveHome();
                    //We prepare the virtual joystick command that will be sent to the robotic arm.
                    trajectoryPoint.InitStruct();
                    trajectoryPoint.Position.Type = CARTESIAN_POSITION;
                    trajectoryPoint.Position.CartesianPosition.X = 0.214f;
                    trajectoryPoint.Position.CartesianPosition.Y = -0.465f;
                    trajectoryPoint.Position.CartesianPosition.Z = 0.497f;
                    //We set the orientation part of the position (unit is RAD)
                    trajectoryPoint.Position.CartesianPosition.ThetaX = 1.56f;
                    trajectoryPoint.Position.CartesianPosition.ThetaY = 0.81f;
                    trajectoryPoint.Position.CartesianPosition.ThetaZ = 0.04f;
                    (*MySendBasicTrajectory)(trajectoryPoint);
                    //result = (*MyGetCartesianCommand)(data);
                    result = (*MyGetCartesianPosition)(data);
                    cout << " Target: [ " << data.Coordinates.X << "," << data.Coordinates.Y << "," << data.Coordinates.Z << " ]" << endl;
                    usleep(3000);

                    trajectoryPoint.InitStruct();
                    trajectoryPoint.Position.Type = CARTESIAN_POSITION;
                    trajectoryPoint.Position.CartesianPosition.X = data.Coordinates.X;
                    trajectoryPoint.Position.CartesianPosition.Y = data.Coordinates.Y;
                    trajectoryPoint.Position.CartesianPosition.Z = data.Coordinates.Z;
                    trajectoryPoint.Position.CartesianPosition.ThetaX = data.Coordinates.ThetaX;
                    trajectoryPoint.Position.CartesianPosition.ThetaY = data.Coordinates.ThetaY;
                    trajectoryPoint.Position.CartesianPosition.ThetaZ = data.Coordinates.ThetaZ;
                    (*MySendBasicTrajectory)(trajectoryPoint);
                    //result = (*MyGetCartesianCommand)(data);
                    result = (*MyGetCartesianPosition)(data);
                    cout << " Home: [ " << data.Coordinates.X << "," << data.Coordinates.Y << "," << data.Coordinates.Z << " ]" << endl;
                    usleep(3000);
                }

                if (incrementalTest)
                {
                    MyMoveHome();
                    trajectoryPoint.InitStruct();
                    trajectoryPoint.Position.Type = CARTESIAN_POSITION;
                    // get the current pose of the end-effector
                    trajectoryPoint.Position.CartesianPosition.X = data.Coordinates.X;
                    trajectoryPoint.Position.CartesianPosition.Y = data.Coordinates.Y;
                    trajectoryPoint.Position.CartesianPosition.Z = data.Coordinates.Z;
                    trajectoryPoint.Position.CartesianPosition.ThetaX = data.Coordinates.ThetaX;
                    trajectoryPoint.Position.CartesianPosition.ThetaY = data.Coordinates.ThetaY;
                    trajectoryPoint.Position.CartesianPosition.ThetaZ = data.Coordinates.ThetaZ;


                    for (int ii=0;ii<20;ii++)
                    {
                        trajectoryPoint.Position.CartesianPosition.X += 0.005f;
                        (*MySendBasicTrajectory)(trajectoryPoint);
                        cout << ii << " [ " << trajectoryPoint.Position.CartesianPosition.X << "," << trajectoryPoint.Position.CartesianPosition.Y << "," << trajectoryPoint.Position.CartesianPosition.Z << " ] " << endl;
                    }

                    for (int ii=0;ii<20;ii++)
                    {
                        trajectoryPoint.Position.CartesianPosition.Y -= 0.005f;
                        trajectoryPoint.Position.CartesianPosition.X -= 0.005f;
                        (*MySendBasicTrajectory)(trajectoryPoint);
                        cout << ii << " [ " << trajectoryPoint.Position.CartesianPosition.X << "," << trajectoryPoint.Position.CartesianPosition.Y << "," << trajectoryPoint.Position.CartesianPosition.Z << " ] " << endl;
                    }

                    for (int ii=0;ii<40;ii++)
                    {
                        trajectoryPoint.Position.CartesianPosition.Z += 0.005f;
                        trajectoryPoint.Position.CartesianPosition.Y -= 0.005f;
                        (*MySendBasicTrajectory)(trajectoryPoint);
                        cout << ii << " [ " << trajectoryPoint.Position.CartesianPosition.X << "," << trajectoryPoint.Position.CartesianPosition.Y << "," << trajectoryPoint.Position.CartesianPosition.Z << " ] " << endl;
                    }

                    for (int ii=0;ii<20;ii++)
                    {
                        trajectoryPoint.Position.CartesianPosition.X -= 0.005f;
                        (*MySendBasicTrajectory)(trajectoryPoint);
                        cout << ii << " [ " << trajectoryPoint.Position.CartesianPosition.X << "," << trajectoryPoint.Position.CartesianPosition.Y << "," << trajectoryPoint.Position.CartesianPosition.Z << " ] " << endl;
                    }

                    for (int ii=0;ii<50;ii++)
                    {
                        trajectoryPoint.Position.CartesianPosition.Z -= 0.005f;
                        trajectoryPoint.Position.CartesianPosition.Y += 0.005f;
                        (*MySendBasicTrajectory)(trajectoryPoint);
                        cout << ii << " [ " << trajectoryPoint.Position.CartesianPosition.X << "," << trajectoryPoint.Position.CartesianPosition.Y << "," << trajectoryPoint.Position.CartesianPosition.Z << " ] " << endl;
                    }
                }

                //getchar();
                //ifstream inputFile ("traj2.txt");
                if (trajectoryTest)
                {
                    MyMoveHome();
                    usleep(10000);
                    cout << "Enter the filename to execute the trajectory from: " << endl;
                    string fileName;
                    getline(cin, fileName);
                    ifstream ifs;
                    ifs.open(fileName.c_str());
                    if (!ifs.is_open())
                        cout << "no such file! try again." << endl;
                    int count;
                    float j1,j2,j3,j4,j5,j6,x,y,z,tx,ty,tz;

                    while (ifs.is_open() &&  ifs >> count >> j1 >> j2 >> j3 >> j4 >> j5 >> j6 >> x >> y >> z >> tx >> ty >> tz)
                    {
                      // printf("%i\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", count,j1,j2,j3,j4,j5,j6,x,y,z,tx,ty,tz);
                      trajectoryPoint.InitStruct();
                      trajectoryPoint.Position.Type = CARTESIAN_POSITION;
                      // get the current pose of the end-effector
                      trajectoryPoint.Position.CartesianPosition.X = x;
                      trajectoryPoint.Position.CartesianPosition.Y = y;
                      trajectoryPoint.Position.CartesianPosition.Z = z;
                      trajectoryPoint.Position.CartesianPosition.ThetaX = tx;
                      trajectoryPoint.Position.CartesianPosition.ThetaY = ty;
                      trajectoryPoint.Position.CartesianPosition.ThetaZ = tz;
                      (*MySendBasicTrajectory)(trajectoryPoint);
                      usleep(3000);
                      printf("%i\t%f\t%f\t%f\t%f\t%f\t%f\t\n", count,x,y,z,tx,ty,tz);
                    }

                    ifs.close();
                }
                result = (*MyCloseAPI)();

        }
        return 0;
}

