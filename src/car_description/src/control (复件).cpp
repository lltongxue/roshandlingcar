#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Point.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <math.h> 
using namespace std;

//新建了一个命名空间，这样car_x;car_y;vector等都可以全局使用了
namespace carposition
{
    float car_x = 0;
    float car_y = 0;
    float car_xita = 0;
    vector<float> car_pos(3);

    float path_x = 0;
    float path_y = 0;
    float path_xita = 0;
    vector<float> path_pos(3);

    float control_speed = 0;
    float control_turn = 0;
}
using namespace carposition;
void getcarposition(const nav_msgs::Odometry msg)
{
  //ROS_INFO("I heard: [%s]", msg.position.c_str());
    car_x = msg.pose.pose.position.x;
    car_y = msg.pose.pose.position.y;
    car_xita = msg.pose.pose.orientation.z;
    car_pos.push_back(car_x);
    car_pos.push_back(car_y);
    car_pos.push_back(car_xita);

}

void getcpathposition(const geometry_msgs::Point msg)
{
    path_x = msg.x;
    path_y = msg.y;
    path_xita = msg.z;
    path_pos.push_back(path_x);
    path_pos.push_back(path_y);
    path_pos.push_back(path_xita);
}

//控制的计算方法放在这里面。
void control_algorithm(vector<float> car, vector<float> path)
{
    float L;
    float e_xita;
    L = sqrt((car[1]-path[1])*(car[1]-path[1])+(car[2]-path[2])*(car[2]-path[2]));
    e_xita= atan((path[2]/path[1]))-car[3];
    if (L > 1.5)
    
        control_speed = control_speed + 0.2;
    else if(L <= 1.5)
        control_speed = L*2;   
    
    if (e_xita>0)
        control_turn = 0.2;
    else if (e_xita<0)
        control_turn = -0.2;
    cout<<"path1:"<<path[1]<<endl;
    cout<<"path2:"<<path[2]<<endl;

}   
int main(int argc, char **argv)
{

    ros::init(argc, argv, "control");
    ros::NodeHandle n;

    ros::AsyncSpinner spinner(4); // Use 4 threads
    spinner.start();
    ros::Subscriber subcar = n.subscribe("odom", 1000, getcarposition);
    ros::Subscriber subpath = n.subscribe("ref_path_topic", 1000, getcpathposition);
    
    ros::waitForShutdown();

    ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
    ros::Rate loop_rate(100); //发布频率为每秒10次


    while (ros::ok())
    {
        control_algorithm(car_pos, path_pos);
        if(control_speed > 3)
            control_speed = 3;
        // 创建输出的速度话题数据
        geometry_msgs::Twist vel;
        vel.linear.x=control_speed;
        vel.linear.y=0;
        vel.linear.z=0;
        vel.angular.x=control_turn;
        vel.angular.y=0;
        vel.angular.z=0;

        //发布数据
        vel_pub.publish(vel);


    }
    /*
    * 告诉 master 我们要订阅 chatter 话题上的消息。
    * 当有消息发布到这个话题时，ROS 就会调用 chatterCallback() 函数。
    * 第二个参数是队列大小，以防我们处理消息的速度不够快，当缓存达到 1000 条消息后，
    * 再有新的消息到来就将开始丢弃先前接收的消息。
    *
    * NodeHandle::subscribe() 返回 ros::Subscriber 对象,
    * 你必须让它处于活动状态直到你不再想订阅该消息。
    * 当这个对象销毁时，它将自动退订 chatter 话题的消息。
    */
    ros::spin();
    /*ros::spin() 进入自循环，可以尽可能快的调用消息回调函数。*/
    loop_rate.sleep();
    return 0;
}