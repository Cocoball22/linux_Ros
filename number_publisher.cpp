#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <cmath>

class TurtleCrownController 
{
private:
    ros::NodeHandle nh_;
    ros::Publisher turtle_pub_;
    const double PI = 3.14159265359;
    
    // 거북이 이동 명령 발행
    void moveCommand(double linear_x, double angular_z, double duration)
    {
        geometry_msgs::Twist vel_msg;
        vel_msg.linear.x = linear_x;
        vel_msg.angular.z = angular_z;
        
        ros::Time start_time = ros::Time::now();
        ros::Rate rate(10);
        
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
        ros::Duration(1.0).sleep(); // 시작 전 잠시 대기
        
        // 기본 설정
        double base_linear_speed = 2.0;
        double base_angular_speed = 1.0;
        
        // 왕관의 베이스 (직선) 그리기
        moveCommand(base_linear_speed, 0, 2.0);
        
        // 첫 번째 봉우리
        moveCommand(base_linear_speed, base_angular_speed, 1.5);
        moveCommand(base_linear_speed, -base_angular_speed, 1.5);
        
        // 두 번째 봉우리 (중앙, 큰 봉우리)
        moveCommand(base_linear_speed, base_angular_speed, 2.0);
        moveCommand(base_linear_speed, -base_angular_speed, 2.0);
        
        // 세 번째 봉우리
        moveCommand(base_linear_speed, base_angular_speed, 1.5);
        moveCommand(base_linear_speed, -base_angular_speed, 1.5);
        
        // 마무리 직선
        moveCommand(base_linear_speed, 0, 2.0);
        
        ROS_INFO("Crown pattern completed!");
    }
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "turtle_crown_controller");
    
    TurtleCrownController controller;
    
    // 잠시 대기 후 왕관 그리기 시작
    ros::Duration(1.0).sleep();
    controller.drawCrown();
    
    return 0;
}