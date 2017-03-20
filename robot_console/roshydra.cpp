#include "roshydra.h"
#include "chydradatamanager.h"
#include "cthreaddata.h"
#include "robot_hydra_id.h"

#include "ros/ros.h"
#include "std_msgs/Float64MultiArray.h"
#include "sensor_msgs/JointState.h"





rosHydra::rosHydra()
{

}

void* rosHydra_main(void* param)
{
    CthreadData* thread_data = (CthreadData*)param;

    ros::init(thread_data->argc,thread_data->argv,"hydraStatePublisher"); // argc,argv not set yet in servo_ui

    ros::NodeHandle nodeHandle;

    ros::Publisher rosHydraPublisher = nodeHandle.advertise<sensor_msgs::JointState>("joint_states",1000);

    ros::Rate loop_rate(100);

    sensor_msgs::JointState rosmsgHydraJointState;

    for(int loop=0; loop<31; loop++){
        rosmsgHydraJointState.name.push_back(joint_hydra_names[loop]);
        rosmsgHydraJointState.position.push_back(0);
    }

    //double dummy[]={0.1, 0.2, 0.3};

    while(ros::ok()){

        //std_msgs::Float64MultiArray msg;


//        rosmsgHydraJointState.position.push_back(0.1);
//        rosmsgHydraJointState.position.push_back(0.2);
//        rosmsgHydraJointState.position.push_back(0.3);

        rosmsgHydraJointState.header.stamp = ros::Time::now();

        rosHydraPublisher.publish(rosmsgHydraJointState);
        ros::spinOnce();

        loop_rate.sleep();
    }

}
