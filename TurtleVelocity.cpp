#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h> // boolean 타입의 메시지 파일 헤더
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <turtlesim/Pose.h>

class TurtleVelocity 
{
private:
    ros::NodeHandle nh;
    ros::Publisher turtle_pub;
    ros::Subscriber turtle_postion_sub;
    geometry_msgs::Twist vel_msg;
    turtlesim::Pose pose_msg;
    ros::Subscriber trigger_sub;
    const float PI = 3.14159; // PI값은 고정값이어야 하기에 상수처리함
    //float pose_x = 5.544444561004639 , pose_y = 5.544444561004639 , pose_theta = 5.544444561004639; // 현재 위치값
    float pose_x , pose_y, pose_theta ; // 현재 위치값
    bool is_pose_updated = false; // 위치 값이 업데이트되었는지 확인하는 플래그
public:
    // 생성자
    TurtleVelocity() 
    {
        // Publisher 초기화
        turtle_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
        turtle_postion_sub = nh.subscribe("/turtle1/pose",1, &TurtleVelocity::coordinate, this);
        /*&MyRobot::callback는 "MyRobot 클래스의 callback 멤버 함수의 주소"를 의미
          클래스의 멤버 함수는 특정 객체에 속해 있어서, 함수 포인터만으로는 어떤 객체의 함수인지 알 수 없음
          this는 현재 객체를 가리키는 포인터
          콜백 함수가 호출될 때 어떤 객체의 멤버 변수를 수정할지 알려주는 역할
          this 없이는 콜백 함수가 어떤 객체의 멤버에 접근해야 하는지 알 수 없음*/
        trigger_sub = nh.subscribe("driver_start", 1, &TurtleVelocity::msgCallback, this);
    }

    // linear 속도 설정 함수
    void drawCrown() 
    {
        float goal_x, goal_y , goal_theta,  difference_x, difference_y, distance;

        goal_x = 7.544444561004639;
        goal_y = 5.544444561004639;
        goal_theta = 1.5708;
        difference_x = goal_x - pose_x;
        difference_y = goal_y - pose_y;
        distance = std::sqrt(difference_x * difference_x + difference_y * difference_y); // 유클리드 거리

        ROS_INFO("2. goal_x: %f, goal_y: %f, goal_theta:%f, pose_x: %f, pose_y: %f, pose_theta: %f", goal_x, goal_y, goal_theta, pose_x, pose_y, pose_theta);
        ROS_INFO("3. difference_x: %f, difference_y: %f", difference_x, difference_y);
        ROS_INFO("4. distance :%f", distance);

        vel_msg.linear.x = 0.5;
        vel_msg.angular.z = 0.0;
        
        turtle_pub.publish(vel_msg);
        
        // 멈추기
        if(distance < 0.05)
        {
            vel_msg.linear.x = 0.0;
            vel_msg.angular.z = 0.0;
            turtle_pub.publish(vel_msg);

            ros::Duration(2.0).sleep(); // 시작 전 잠시 대기

        }
        

        //vel_msg.angular.z = PI / 2;

        
    }

    void coordinate(const turtlesim::Pose::ConstPtr& coordinate) // pose의 구조체에서 x,y값을 가져옴
    {
        ros::Rate loop_rate(10);  // 10Hz 루프
        ROS_INFO("5. x:%f y:%f theta:%f lin_ver:%f ang_ver:%f ", coordinate -> x, coordinate ->y , coordinate -> theta, coordinate -> linear_velocity ,coordinate -> angular_velocity);
        // vel_msg.linear.x = coordinate -> x;
        // vel_msg.angular.z = coordinate -> theta;

        pose_x = coordinate -> x;
        pose_y = coordinate -> y;
        pose_theta = coordinate -> theta;

        is_pose_updated = true; // 업데이트 완료 표시
        ROS_INFO("6.Updated pose_x: %f, pose_y: %f, pose_theta: %f", pose_x, pose_y, pose_theta);
        ROS_INFO("==================================================================== \n");

        ros::spinOnce(); // 한번하는데 계속 물려있다..
        loop_rate.sleep();

        drawCrown();

        // printf("x :%f", pose_msg.x);
    }

    void msgCallback(const std_msgs::Bool::ConstPtr& msg)
    {
       ROS_INFO("1. receive msg = %d", msg->data); // data 메시지를 표현

       bool testing = msg -> data;
       //bool testing =  true;
       
       if(testing)
       {
        if(is_pose_updated)
        {   
            drawCrown(); // 함수를 먼저 호출 했는데 coordinate가 먼저 실행이 되는가?
    
        }
        else
        {
            ROS_WARN("Pose is not updated yet. Waiting...");
        }
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
    ros::Rate loop_rate(10);  // 10Hz 루프

    // 클래스 인스턴스 생성
    TurtleVelocity turtle;

    while(ros::ok())
    {
        ros::spin();
    }
    loop_rate.sleep(); // 위에서 정한 루프 주기에 따라 슬립에 들어간다.


    return 0;
}