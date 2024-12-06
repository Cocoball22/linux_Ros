// ROS와 필요한 메시지, 서비스 타입들을 위한 헤더 파일들
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>  // 속도 제어를 위한 메시지 타입
#include <std_msgs/Bool.h>        // boolean 타입의 메시지
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <turtlesim/Pose.h>       // 터틀의 위치 정보를 위한 메시지 타입
#include <turtlesim/Kill.h>       // 터틀 삭제를 위한 서비스 타입
#include <turtlesim/Spawn.h>      // 새 터틀 생성을 위한 서비스 타입
#include <std_srvs/Empty.h>       // 파라미터가 없는 서비스 호출을 위한 타입

// 전역 변수들
bool testing = false;             // 그리기 작업 시작 여부를 제어하는 플래그
float pose_x, pose_y, pose_theta; // 터틀의 현재 위치와 각도
float PI = 3.141592;             // 원주율 상수

class turtle_time
{
private:
    ros::NodeHandle nh;           // ROS 시스템과의 통신을 위한 핸들
    ros::Publisher turtle_pub;     // 터틀 속도 명령을 발행하기 위한 퍼블리셔
    ros::Subscriber turtle_position_sub; // 터틀의 위치 정보를 구독하기 위한 서브스크라이버
    ros::Subscriber trigger_sub;   // 그리기 작업 시작 명령을 구독하기 위한 서브스크라이버

    // 서비스 클라이언트들
    ros::ServiceClient kill_client;   // 터틀 삭제를 위한 서비스 클라이언트
    ros::ServiceClient spawn_client;  // 새 터틀 생성을 위한 서비스 클라이언트
    ros::ServiceClient reset_client;  // 시뮬레이션 초기화를 위한 서비스 클라이언트

    geometry_msgs::Twist vel_msg;     // 속도 명령 메시지
    turtlesim::Pose pose_msg;         // 위치 정보 메시지

public:
    // 생성자: 퍼블리셔, 서브스크라이버, 서비스 클라이언트들을 초기화
    turtle_time()
    {
        // 터틀에게 속도 명령을 보내기 위한 퍼블리셔 설정
        turtle_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
        
        // 그리기 시작 명령과 터틀 위치 정보를 받기 위한 서브스크라이버들 설정
        trigger_sub = nh.subscribe("driver_start", 100, &turtle_time::msgCallback, this);
        turtle_position_sub = nh.subscribe("/turtle1/pose", 1, &turtle_time::coordinate, this);

        // 터틀 관리를 위한 서비스 클라이언트들 초기화
        kill_client = nh.serviceClient<turtlesim::Kill>("/kill");
        spawn_client = nh.serviceClient<turtlesim::Spawn>("/spawn");
        reset_client = nh.serviceClient<std_srvs::Empty>("/reset");
    }

    // 그리기 시작 명령을 처리하는 콜백 함수
    void msgCallback(const std_msgs::Bool::ConstPtr& msg)
    {
        ROS_INFO("receive msg = %d", msg->data);
        testing = msg->data;
        printf("test_2 : %d\n\r", testing);
    }

    // 터틀의 현재 위치 정보를 처리하는 콜백 함수
    void coordinate(const turtlesim::Pose::ConstPtr& coordinate)
    {
        pose_x = coordinate->x;
        pose_y = coordinate->y;
        pose_theta = coordinate->theta;
    }

    // 지정된 이름의 터틀을 삭제하는 함수
    void killTurtle(const std::string& turtle_name) 
    {
        turtlesim::Kill srv;
        srv.request.name = turtle_name;
        
        if (kill_client.call(srv)) {
            ROS_INFO("Killed turtle: %s", turtle_name.c_str());
        }
    }

    // 시뮬레이션을 초기 상태로 리셋하는 함수
    void resetTurtle() 
    {
        std_srvs::Empty srv;
        if (reset_client.call(srv)) {
            ROS_INFO("reset turtle");
        }
    }

    // 새로운 터틀을 생성하는 함수
    void spawnTurtle(float x, float y, float theta, const std::string& name) 
    {
        turtlesim::Spawn srv;
        srv.request.x = x;
        srv.request.y = y;
        srv.request.theta = theta;
        srv.request.name = name;
        
        if (spawn_client.call(srv)) {
            ROS_INFO("Spawned turtle: %s", srv.response.name.c_str());
        }
    }

    // 왕관 모양을 그리는 함수
    void drawCrown()
    {
        // 일련의 직선 이동과 회전 동작을 수행하여 왕관 모양을 그림
        go_command(1.0,0.0,4.0);      // 직진
        ros::Duration(2.0).sleep();    // 2초 대기
        go_command(0,(PI*0.25),2.0);   // 45도 회전
        // ... (이하 동일한 패턴으로 계속)
    }

    // 원을 그리는 함수
    void drawcircle()
    {
        // 선속도와 각속도를 조절하여 원 모양을 그림
        go_command(PI*0.0625,PI*0.03125,64);  // 작은 원
        ros::Duration(2.0).sleep();
        // ... (다른 크기의 원들)
    }

    // 지정된 시간 동안 특정 속도로 이동하는 함수
    void go_command(float goal_x, float goal_z, float duration)
    {
        ros::Time start_time = ros::Time::now();
        
        // 선속도와 각속도 설정
        vel_msg.linear.x = goal_x;     // 전진/후진 속도
        vel_msg.angular.z = goal_z;    // 회전 속도
        
        // 지정된 시간 동안 속도 명령 발행
        while (ros::Time::now() - start_time < ros::Duration(duration))
        {
            turtle_pub.publish(vel_msg);
        }
        
        // 이동 완료 후 정지
        vel_msg.linear.x = 0;
        vel_msg.angular.z = 0;
        turtle_pub.publish(vel_msg);
    }

    // 터틀을 정지시키는 함수
    void stop_command()
    {
        printf("stop\n\r");
        vel_msg.linear.x = 0.0;
        vel_msg.angular.z = 0.0;
        turtle_pub.publish(vel_msg);
        ros::Duration(2.0).sleep();
    }
};

int main(int argc, char** argv) 
{
    // ROS 노드 초기화
    ros::init(argc, argv, "turtle_velocity_controller");
    ros::Time::init();
    ros::Rate rate(10);           // 10Hz로 루프 실행
    turtle_time turtle;           // 터틀 제어 객체 생성

    printf("main: %d \n\r",testing);

    // 메인 루프
    while(ros::ok())              // ROS가 실행 중인 동안
    {
        printf(" in while test : %d\n\r", testing);
        if(testing)               // 그리기 명령을 받았다면
        {
            turtle.drawCrown();   // 왕관 그리기
            ros::Duration(2.0);
            turtle.resetTurtle(); // 시뮬레이션 리셋
            turtle.drawcircle();  // 원 그리기
            testing = false;      // 그리기 완료
        }
        
        ros::spinOnce();         // 콜백 함수 처리
        rate.sleep();            // 지정된 주기 유지
    }
    
    return 0;
}