#include <ros/ros.h>    
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>

#define PI 3.14159265

void cmd_velCallback(const geometry_msgs::Twist::ConstPtr& msg);
void poseCallback(const geometry_msgs::Pose2D::ConstPtr& msg);
void update(float dt);

ros::WallTime last_command_time;
float lin_vel, ang_vel;
geometry_msgs::Pose2D pose;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "modelsim");
    ros::NodeHandle nh;
    
    ros::Time current_time = ros::Time::now();
    ros::Time last_time = ros::Time::now();

    ros::Subscriber cmd_vel_sub = nh.subscribe("robot/cmd_vel", 100, cmd_velCallback);
    ros::Subscriber pose_sub = nh.subscribe("robot/pose", 100, poseCallback);
    ros::Publisher pose_pub = nh.advertise<geometry_msgs::Pose2D>("robot/pose", 100);
    ros::Rate r(100);
    while(ros::ok())
    {
        current_time = ros::Time::now();
        update((current_time-last_time).toSec());
        pose_pub.publish(pose);
        last_time = current_time;
        ros::spinOnce();
        r.sleep();
    }
    return 0;
}

void cmd_velCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
    last_command_time = ros::WallTime::now();
    lin_vel = msg->linear.x;
    ang_vel = msg->angular.z;
}

void poseCallback(const geometry_msgs::Pose2D::ConstPtr& msg)
{
    pose = *msg;
}

void update(float dt)
{
    if(ros::WallTime::now() - last_command_time > ros::WallDuration(0.5))
    {
        lin_vel = 0.0;
        ang_vel = 0.0;
    }

    pose.theta += ang_vel*dt;
    pose.theta -= 2*PI * std::floor((pose.theta+PI)/(2*PI));
    pose.x += cos(pose.theta)*lin_vel*dt;
    pose.y += sin(pose.theta)*lin_vel*dt;

    static tf::TransformBroadcaster odom_br;
    geometry_msgs::TransformStamped odom_tr;
    odom_tr.header.stamp = ros::Time::now();
    odom_tr.header.frame_id = "map";
    odom_tr.child_frame_id = "base_link";

    odom_tr.transform.translation.x = pose.x;
    odom_tr.transform.translation.y = pose.y;
    odom_tr.transform.translation.z = 0.0;
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(pose.theta);
    odom_tr.transform.rotation = odom_quat;

    odom_br.sendTransform(odom_tr);
}