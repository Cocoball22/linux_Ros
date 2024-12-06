#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h> // boolean 타입의 메시지 파일 헤더
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <turtlesim/Pose.h>
#include <turtlesim/Kill.h>

bool testing = false;  // public 멤버 변수로 선언
float pose_x , pose_y, pose_theta, distance ; // 현재 위치값
float PI = 3.141592;

class turtle_time
{
private:
    ros::NodeHandle nh;
    ros::Publisher turtle_pub;
    ros::Subscriber turtle_position_sub;
    geometry_msgs::Twist vel_msg;
    turtlesim::Pose pose_msg;
    ros::Subscriber trigger_sub;
    ros::ServiceServer kill_sub;
public:
    turtle_time()
    {
        turtle_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
        trigger_sub = nh.subscribe("driver_start", 100 , &turtle_time::msgCallback, this);
        turtle_position_sub = nh.subscribe("/turtle1/pose", 1 , &turtle_time::coordinate, this);
    }

    void msgCallback(const std_msgs::Bool::ConstPtr& msg)
    {
        ROS_INFO("receive msg = %d", msg->data); // data 메시지를 표현
        testing = msg->data;
        printf("test_2 : %d\n\r", testing);
    }

    void coordinate(const turtlesim::Pose::ConstPtr& coordinate) // pose의 구조체에서 x,y값을 가져옴 
    {  
        // ROS_INFO("11111111\n"); // data 메시지를 표현
        pose_x = coordinate -> x;
        pose_y = coordinate -> y;
        pose_theta = coordinate -> theta;
    }

    // 서비스 처리 성공/실패를 반환해야함
    bool killCallback(turtlesim::Kill::Request& request,turtlesim::Kill::Response& response)
    {
        return true;
    }

    // void drawCrown()
    // {
    //     go_command(2.0,0.0,2.0);
    
    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(0,(PI*0.5),1.0);  // pi/2 360??? pi/8 30도 

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(2.0,0.0,1.5);

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(0,(PI*0.66),1.0);  // pi/4 45도 pi/6 대략 30도

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(2.0,0,2.0);

    // while (ros::Time::now() - start_time < ros::Duration(duration))
    // {
    //     turtle_pub.publish(vel_msg);
    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(0,-(PI*0.33),1.0);  // pi/4 45도 pi/6 대략 30도

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(2.0,0,2.0);

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(0,(PI*0.67),1.0);  // pi/2 360??? pi/8 30도 

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(2.0,0.0,1.5);

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(0,(PI*0.5),1.0);  // pi/2 360??? pi/8 30도 

    //     ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    //     go_command(2.0,0.0,2.0);

    // }

    void drawCrown()
    {
        go_command(1.0,0.0,4.0);
    
        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(0,(PI*0.25),2.0);  // pi/2 360??? pi/8 30도 

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(1.0,0.0,3.0);

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(0,(PI*0.33),2.0);  // pi/4 45도 pi/6 대략 30도

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(1.0,0,4.0);

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(0,-(PI*0.11111),3.0);  // pi/4 45도 pi/6 대략 30도

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(1.0,0,4.0);

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(0,(PI*0.33),2.0);  // pi/2 360??? pi/8 30도 

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(1.0,0.0,3.0);

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(0,(PI*0.25),2.0);  // pi/2 360??? pi/8 30도 

        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(1.0,0.0,4.0);

    }

    void drawcircle()
    {
        go_command(PI*0.0625,PI*0.03125,64);
        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(PI*0.0625 ,-PI*0.03125, 32);
        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(PI*0.0625 ,-PI*0.0625, 16);
        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
        go_command(PI*0.0625 ,PI*0.0625, 16);
        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    }

    void go_command(float goal_x, float goal_z, float duration)
    {
        ros::Time start_time = ros::Time::now();
        // ros::Rate rate(10);

        vel_msg.linear.x = goal_x;
        vel_msg.angular.z = goal_z;
        
        // 지정된 duration 동안만 실행
        while (ros::Time::now() - start_time < ros::Duration(duration))
        {
            turtle_pub.publish(vel_msg);
        }
        
        vel_msg.linear.x = 0;
        vel_msg.angular.z = 0;
        turtle_pub.publish(vel_msg);
    }

    void stop_command()
    {
        printf("stop\n\r");
        vel_msg.linear.x = 0.0;
        vel_msg.angular.z = 0.0;
        turtle_pub.publish(vel_msg);
        ros::Duration(2.0).sleep(); // 시작 전 잠시 대기
    }
};

int main(int argc, char** argv) 
{
    // 노드 초기화
    ros::init(argc, argv, "turtle_velocity_controller");
    ros::Time::init();
    ros::Rate rate(10);
    turtle_time turtle;

    printf("main: %d \n\r",testing);

        while(ros::ok()) // roscore가 실행 되는 동안
        {
            printf(" in while test : %d\n\r", testing);
            if(testing)
            {
                turtle.drawCrown();
                // turtle.drawcircle();
            }
            //ros::spin(); // 반복해서 callback을 호출함
            ros::spinOnce();
            rate.sleep();
    
        }

    // ros::spinOnce();
    // rate.sleep();
    
    ros::spin(); // 반복해서 callback을 호출함
    // ros::spinOnce();
    // rate.sleep();

    
    return 0;
}
