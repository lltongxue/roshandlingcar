#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Point.h"
//坐标转换用的
#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/Quaternion.h"
#include "tf/transform_datatypes.h"
#include "tf/LinearMath/Matrix3x3.h"
#include "nav_msgs/Path.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <math.h> 
using namespace std;

//新建了一个命名空间，这样 car_x;car_y;vector等都可以全局使用了
namespace carposition
{
    float car_x = 0;
    float car_y = 0;
    float car_xita = 0;

    float path_x = 0;
    float path_y = 0;
    float path_xita = 0;

    float control_speed = 0;
    float control_turn = 0;
}
//调用全局变量
using namespace carposition;




void getcarposition(const nav_msgs::Odometry msg)
{
  //ROS_INFO("I heard: [%s]", msg.position.c_str());
    car_x = msg.pose.pose.position.x;
    car_y = msg.pose.pose.position.y;
    //car_xita = msg.pose.pose.orientation.z;
//四元数转换成欧拉角
    geometry_msgs::Quaternion tf_msg;
    tf_msg.x=msg.pose.pose.orientation.x;
    tf_msg.y=msg.pose.pose.orientation.y;
    tf_msg.z=msg.pose.pose.orientation.z;
    tf_msg.w=msg.pose.pose.orientation.w;
    // the incoming geometry_msgs::Quaternion is transformed to a tf::Quaterion
    tf::Quaternion quat;
    tf::quaternionMsgToTF(tf_msg, quat);
    // the tf::Quaternion has a method to acess roll pitch and yaw
    double roll, pitch, yaw;
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    car_xita = (float)yaw;
    //cout << "car_xita" << car_xita <<endl;

}


void getpathposition(const nav_msgs::Path msg)
{
    path_x = msg.poses[20].pose.position.x;
    path_y = msg.poses[20].pose.position.y;
    //四元数转换成欧拉角
    geometry_msgs::Quaternion tf_msg;
    tf_msg.x=msg.poses[20].pose.orientation.x;
    tf_msg.y=msg.poses[20].pose.orientation.y;
    tf_msg.z=msg.poses[20].pose.orientation.z;
    tf_msg.w=msg.poses[20].pose.orientation.w;
    // the incoming geometry_msgs::Quaternion is transformed to a tf::Quaterion
    tf::Quaternion quat;
    tf::quaternionMsgToTF(tf_msg, quat);
    // the tf::Quaternion has a method to acess roll pitch and yaw
    double roll, pitch, yaw;
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    path_xita = (float)yaw;
 
}

/*******************控制的计算方法放在这里面。************************/

void control_algorithm()
{
    float L;
    float e_xita;
    float xita;
    float dx;
    float dy;
    L = sqrt((car_x-path_x)*(car_x-path_x)+(car_y-path_y)*(car_y-path_y));
    xita= atan(((path_y-car_y)/(path_x-car_x+0.000001)));
    
    dx = path_x - car_x;
    dy = path_y - car_y;


    //速度控制

    if (L > 1 && control_speed<=2)
        control_speed = control_speed + 0.005;
    else if(L <= 1 && L>=0.1)
        control_speed = L*2; 
    else if(L < 0.1)
    {
        control_speed = 0;
	control_turn = 0;
    }


    //角速度控制

     if (dx>=0 && dy>=0)
        xita = xita;
    else if (dx<0)
        xita = 3.141592654 + xita;
    else if (dx>0 && dy<0)
        xita = 2*3.141592654+xita;
    if (xita>3.1415926)
        xita = xita-6.2831853;       


    e_xita = xita-car_xita;
    
    if (e_xita>0)
        control_turn = e_xita*5;
    else if (e_xita<0)
        control_turn = e_xita*5;
    
    //速度约束
    float control_speed_max = 2;
    float control_turn_max = 1;
    if (control_speed > control_speed_max)
        control_speed = control_speed_max;
    if (control_turn > control_turn_max)
        control_turn = control_turn_max;
    if (control_turn< -control_turn_max )
        control_turn= -control_turn_max;
  //  cout<<"L"<<L<<endl;
  //  cout<<"xita"<<e_xita<<endl;
    //cout<<"path1:"<<path_x<<endl;
    //cout<<"path2:"<<path_y<<endl;

}   

int main(int argc, char **argv)
{

    ros::init(argc, argv, "control");
    ros::NodeHandle n;
    ros::Subscriber subcar = n.subscribe("odom", 1000, getcarposition);
    ros::Subscriber subpath = n.subscribe("/move_base/TrajectoryPlannerROS/local_plan", 1000, getpathposition);
    ros::AsyncSpinner spinner(2); // Use 4 threads,开启多线程模式，这样同时启动两个订阅器不会出现数据丢失

    ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
    ros::Rate loop_rate(100); //发布频率为每秒100次


    while (ros::ok())
    {
        control_algorithm();
       // cout << "this" << endl;
        // 创建输出的速度话题数据
        geometry_msgs::Twist vel;
        vel.linear.x=control_speed;
        vel.linear.y=0;
        vel.linear.z=0;
        vel.angular.x=0;
        vel.angular.y=0;
        vel.angular.z=control_turn;

        //发布数据
        vel_pub.publish(vel);
        spinner.start();
  
        //ros::spin();
        ros::spinOnce(); //对于publish，将会在被调用的那一时间点调用所有等待的回调函数.
        loop_rate.sleep();
    }


    
    /*ros::spin() 进入自循环，可以尽可能快的调用消息回调函数。*/
    ros::waitForShutdown();
    return 0;
    
}
