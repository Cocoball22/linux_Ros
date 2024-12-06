#include "ros/ros.h" // ROS의 기본 기능을 사용하기 위한 헤더
#include <geometry_msgs/Twist.h> // 로봇의 선속도, 각속도 메시지 타입
#include <sstream>
#include <std_msgs/Bool.h> // boolean 타입의 메시지
#include <math.h>

class TurtleCrownController 
{
private:
    ros::NodeHandle nh_; // ROS 시스템과의 통신을 위한 핸들
    ros::Publisher turtle_pub_;  // 거북이 제어 명령을 발행할 퍼블리셔
    const float PI = 3.14159; // PI값은 고정값이어야 하기에 상수처리함
    
    // 거북이 이동 명령 발행
    void moveCommand(float linear_x, float angular_z, float duration)
    {

        // linear_x: 직선 속도 (양수: 전진, 음수: 후진)
        // angular_z: 회전 속도 (양수: 반시계, 음수: 시계방향)
        // duration: 해당 동작을 수행할 시간

        ROS_INFO("Publishing: linear_x = %.4f, angular_z = %.4f", linear_x, angular_z);

        geometry_msgs::Twist vel_msg; // 로봇의 속도 명령을 담는 메시지 객체

        // vel_msg의 구조: 속도: 미터/초 회전속도: 라디안/초
        // 1. linear (선속도) - 직선 운동
        //    vel_msg.linear.x  - x축 방향 선속도 (전진/후진) 
        //    vel_msg.linear.y  - y축 방향 선속도 (좌/우 이동)
        //    vel_msg.linear.z  - z축 방향 선속도 (상/하 이동)

        // 2. angular (각속도) - 회전 운동
        //    vel_msg.angular.x - x축 중심 회전
        //    vel_msg.angular.y - y축 중심 회전
        //    vel_msg.angular.z - z축 중심 회전 (평면상의 좌/우 회전)

        // 2D 환경의 터틀봇의 경우 주로 사용하는 값:
        // vel_msg.linear.x  - 전진/후진 속도
        // vel_msg.angular.z - 좌/우 회전 속도


        // roundf(): 부동소수점 반올림 함수로 정밀도 보정
        vel_msg.linear.x = roundf(linear_x*10000) / 10000;
        vel_msg.angular.z = angular_z;
        
        ros::Time start_time = ros::Time::now();
        ros::Rate rate(10); // 10Hz로 제어 주기 설정 (0.1초마다 한 번씩 실행)
        
        while (ros::Time::now() - start_time < ros::Duration(duration))
        {
            turtle_pub_.publish(vel_msg);
            rate.sleep();
        }
        
        // 정지
        vel_msg.linear.x = 0;
        vel_msg.angular.z = 0;
        turtle_pub_.publish(vel_msg);
    }
    
public:
    TurtleCrownController()
    {
        // Publisher 초기화 - cmd_vel 토픽으로 발행
        turtle_pub_ = nh_.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
        ROS_INFO("Crown Pattern Controller initialized");
    }
    
    // 왕관 모양 그리기
    void drawCrown()
    {
        // 회전은 PI 180도에서 90도는 /2 45도는 /4 
        // 시계방향 -값 시계방향 +값
        ros::Duration(1.0).sleep(); // 시작 전 잠시 대기
        
        // 기본 설정
        float base_linear_speed = 2.0;
        float duration_curve = 1.0;
        float duration_straight = 2.0;
        
        // 왕관의 베이스 (직선) 그리기
        moveCommand(1, 0, 4); // 직선
        ros::Duration(2.0).sleep(); // 2초 대기
        moveCommand(0, PI/20, 10.0); // 시계 반대방향
        
        // 첫 번째 봉우리
        // moveCommand(base_linear_speed, 0, duration_straight); // 직선
        // ros::Duration(2.0).sleep(); // 2초 대기
        // moveCommand(0, (PI/1.2), duration_curve); // 시계 반대선 120도 회전 (180/1.5 = 120도)
        // moveCommand(base_linear_speed, 0, duration_straight); // 직선
        
        // // 두 번째 봉우리 (중앙, 큰 봉우리)
        // moveCommand(0,-0.7,2.0); // 시계 방향
        // moveCommand(base_linear_speed, 0, 2.0);
        // moveCommand(0,1.15,2.0);
        // moveCommand(base_linear_speed, 0, 1.8);
        
        // // 마무리 직선
        // moveCommand(0,0.8,2.0); // 시계 반대
        // moveCommand(base_linear_speed, 0, 1.7);
        
        ROS_INFO("Crown pattern completed!");
    }

    void circle()
    {
        ros::Duration(1.0).sleep(); // 시작 전 잠시 대기
        moveCommand(2.0, 1.0, 5.5);
        //ros::Duration(1.0).sleep(); // 시작 전 잠시 대기
        moveCommand(2.0, -1.0, 4);
    }
};

int main(int argc, char **argv)
{
     // ROS 노드 초기화
    // argc, argv: 명령행 인자를 받아서 ROS 시스템 초기화
    // "driver_controller": 이 노드의 이름
    ros::init(argc,argv, "driver_controller"); // 노드명 초기화
    
    ros::NodeHandle n;  // ROS 시스템과의 통신을 위한 핸들 생성
    
     // 퍼블리셔 선언, ros_tutorials_topic 패키지의 MsgTutorial 메시지 파일을 이용한
     // 퍼블리셔 
    // "dirver_start"라는 토픽으로 Bool 타입 메시지를 발행
    // 10은 메시지 큐의 크기
    ros::Publisher test_pub = n.advertise<std_msgs::Bool>("driver_start", 10);

    ros::Rate loop_rate(10); // 10hz로 설정

    TurtleCrownController controller;

    std_msgs::Bool msg;
    msg.data = true;

    //토픽 발행
    test_pub.publish(msg);

    // 잠시 대기 후 왕관 그리기 시작
        ros::Duration(1.0).sleep();
        controller.drawCrown();
        //controller.ciurcle();

//     while(ros::ok())
//     {
        
//     }

}