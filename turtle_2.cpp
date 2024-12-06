#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h> // boolean 타입의 메시지 파일 헤더
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <turtlesim/Pose.h>

bool testing;
float pose_x , pose_y, pose_theta ; // 현재 위치값

class turtle_2
{
private:
    ros::NodeHandle nh;
    ros::Publisher turtle_pub;
    ros::Subscriber turtle_position_sub;
    geometry_msgs::Twist vel_msg;
    turtlesim::Pose pose_msg;
    ros::Subscriber trigger_sub;
   
public:
    turtle_2()
    {
        turtle_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
        trigger_sub = nh.subscribe("driver_start", 1, &turtle_2::msgCallback, this);
        turtle_position_sub = nh.subscribe("/turtle1/pose",1, &turtle_2::coordinate, this);
    }

    void msgCallback(const std_msgs::Bool::ConstPtr& msg)
    {
        //ROS_INFO("1. receive msg = %d", msg->data); // data 메시지를 표현
        testing = msg -> data;
    }

    void coordinate(const turtlesim::Pose::ConstPtr& coordinate) // pose의 구조체에서 x,y값을 가져옴 
    {  
        pose_x = coordinate -> x;
        pose_y = coordinate -> y;
        pose_theta = coordinate -> theta;
    }

    void go_command(float goal_x, float goal_y)
    {
        float difference_x, difference_y, distance;

        difference_x = goal_x - pose_x;
        difference_y = goal_y - pose_y;
        distance = std::sqrt(difference_x * difference_x + difference_y * difference_y); // 유클리드 거리

        vel_msg.linear.x = 1.0;
        vel_msg.angular.z = 0.0;
        turtle_pub.publish(vel_msg);

        if(distance < 0.05)
        {
            vel_msg.linear.x = 0.0;
            vel_msg.angular.z = 0.0;
            turtle_pub.publish(vel_msg);
        }
    }
};

int main(int argc, char** argv) 
{
    // 노드 초기화
    ros::init(argc, argv, "turtle_velocity_controller");
    ros::Time::init();

    // // ROS 시간 사용
    // ros::Time current_time = ros::Time::now();
   
    // 루프 주기를 설정한다. "10" 이라는 것은 10hz를 말하는 것으로 0.1초 간격으로 처리가 반복된다.
    ros::Rate loop_rate(1);  // 10Hz 루프

    turtle_2 turtle;

    while (ros::ok())
    {
        ros::spinOnce();  // 콜백 함수 실행
        loop_rate.sleep();

        if(testing) // testing이 true일 때만 실행
        {
            //ROS_INFO("Trigger is ON");
            //printf("pose_x: %f, pose_y: %f, pose_theta: %f \n\r",  pose_x, pose_y, pose_theta);
            turtle.go_command(7.544444561004639,5.544444561004639);
        }
        else
        {
            ROS_INFO("Trigger is OFF");
        }

        
    }

    return 0;
}
