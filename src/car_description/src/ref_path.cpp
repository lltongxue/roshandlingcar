/*ros/ros.h 是一个实用的头文件，它引用了 ROS 系统中大部分常用的头文件。*/
#include "ros/ros.h"
#include "geometry_msgs/Point.h" 
#include "geometry_msgs/PoseStamped.h"
#include <iostream>
#include <sstream>
using namespace std;

namespace refposition
{
    float vec[4][4]={{7.40303230286,0.322997331619,0.678748061277,0.734371206756},
		     {7.48586177826,2.99050354958,0.999989955039,0.00448216697893},
		     {6.2281832695,1.01640820503,-0.703713737579,0.710483620883},
		     {2.54628753662,0.197164058685,0.999999999946,0}};
    int i = 0;
    bool sendsingel=true;    
}
using namespace refposition;

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "ref_path");
  ros::NodeHandle n;

  ros::Publisher chatter_pub = n.advertise<geometry_msgs::PoseStamped>("move_base_simple/goal", 100);

  ros::Rate loop_rate(1);
  while (ros::ok())
  {
    if(sendsingel==true)
    {
    geometry_msgs::PoseStamped goals;
    goals.header.stamp=ros::Time::now();
    goals.header.frame_id="map";
    goals.pose.position.x = vec[i][0];
    goals.pose.position.y = vec[i][1];
    goals.pose.orientation.z = vec[i][2];
    goals.pose.orientation.w = vec[i][3];

    chatter_pub.publish(goals);
    sendsingel = false;
    }
    ros::spinOnce();

    loop_rate.sleep();

  }


  return 0;
}