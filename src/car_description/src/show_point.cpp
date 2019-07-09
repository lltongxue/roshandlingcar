#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/PoseStamped.h"

#include <sstream>
#include <iostream>
#include <math.h> 
using namespace std ;

namespace Point
{
    float point_x;
    float point_y;

}
using namespace Point ;
void chatterCallback(const geometry_msgs::Point msg)
{
  point_x = msg.x;
  point_y = msg.y;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "show_point");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("ref_path_topic", 1000, chatterCallback);
    ros::Publisher pub = n.advertise<geometry_msgs::PoseStamped>("show_point_topic",1000);
    ros::Rate loop_rate(100);
    while (ros::ok())
     {
    
        geometry_msgs::PoseStamped msg;
       // msg.header.seq = 0;
        //msg.header.stamp = 0;
        msg.header.frame_id = "map";
        msg.pose.position.x = point_x;
        msg.pose.position.y = point_y;
        msg.pose.position.z = 0;
        msg.pose.orientation.x = 0 ;
        msg.pose.orientation.y = 0 ;
        msg.pose.orientation.z = 0 ;
        msg.pose.orientation.w = 1 ;

   
        pub.publish(msg);
        ros::spin();
        ros::spinOnce();

        loop_rate.sleep();
   
      }
   

    return 0;
}