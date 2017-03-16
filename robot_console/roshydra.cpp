#include "roshydra.h"
#include "chydradatamanager.h"
#include "cthreaddata.h"

#include "ros/ros.h"
#include "std_msgs/Float64MultiArray.h"





rosHydra::rosHydra()
{

}

void* rosHydra_main(void* param)
{
    CthreadData* thread_data = (CthreadData*)param;

    ros::init(thread_data->argc,thread_data->argv,"hydraStatePublisher"); // argc,argv not set yet in servo_ui

    ros::NodeHandle nodeHandle;

    ros::Publisher hydraPub = nodeHandle.advertise<std_msgs::Float64MultiArray>("hydra_joint_state",1000);

    ros::Rate loop_rate(100);

    //double dummy[]={0.1, 0.2, 0.3};

    while(ros::ok()){

        std_msgs::Float64MultiArray msg;
        msg.data.clear();
        msg.data.push_back(0.1);
        msg.data.push_back(0.2);
        msg.data.push_back(0.3);

        hydraPub.publish(msg);
        ros::spinOnce();

        loop_rate.sleep();
    }

}
