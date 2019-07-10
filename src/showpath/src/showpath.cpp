#include <ros/ros.h>
#include <ros/console.h>
#include <nav_msgs/Path.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_broadcaster.h>
#include <tf/tf.h>
#include <nav_msgs/Odometry.h>

ros::Time current_time; 
ros::Publisher path_pub_odom;
ros::Publisher path_pub_noise_odom;
nav_msgs::Path path_odom;
nav_msgs::Path path_noise_odom;


void drawPath(const nav_msgs::Odometry& odom, nav_msgs::Path& path, ros::Publisher pub, std::string frame_id)
{
    current_time = ros::Time::now();
    geometry_msgs::PoseStamped this_pose_stamped;
    this_pose_stamped.pose = odom.pose.pose;
    this_pose_stamped.header.stamp = current_time;
    this_pose_stamped.header.frame_id = frame_id;
    path.poses.push_back(this_pose_stamped);
    pub.publish(path);
}

void odomCallback(const nav_msgs::Odometry& odom_data)
{
    drawPath(odom_data, path_odom, path_pub_odom, "odom");
}

void nodomCallback(const nav_msgs::Odometry& odom_data)
{
    drawPath(odom_data, path_noise_odom, path_pub_noise_odom, "noise_odom");
}

main (int argc, char **argv)
{
    ros::init (argc, argv, "showpath");
    ROS_INFO("Initialize success....");
    ROS_INFO("Listen the topic 'trajectoryOdom' show trajectory for odom");
    ROS_INFO("Listen the topic 'trajectoryNoiseOdom' show trajectory for noise_odom");

    ros::NodeHandle ph;
    path_pub_odom = ph.advertise<nav_msgs::Path>("trajectoryOdom",1, true);
    path_pub_noise_odom = ph.advertise<nav_msgs::Path>("trajectoryNoiseOdom",1, true);
    ros::Subscriber sub_odom = ph.subscribe("odom", 1000, odomCallback );
    ros::Subscriber sub_noise_odom = ph.subscribe("noise_odom", 1000, nodomCallback );

    current_time = ros::Time::now();  
    path_odom.header.stamp=current_time;
    path_odom.header.frame_id="odom";
    path_noise_odom.header.stamp=current_time;
    path_noise_odom.header.frame_id="odom";

    ros::spin();

    return 0;
}