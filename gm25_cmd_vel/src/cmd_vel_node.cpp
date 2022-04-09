#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

// publish once per sec, robot forward 1s, backward 1s, forward 1s ...
int main(int argc, char **argv){
  ros::init(argc, argv, "cmd_vel_node");
  ros::NodeHandle nh;
  ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 10);

  ros::Time current_time, last_time;
  geometry_msgs::Twist vel_msg;
  int c = -1;
  ros::Rate pub_rate(100); //Hz
  while (ros::ok()){
    current_time = ros::Time::now();
    if (current_time.toSec() - last_time.toSec() > 1){
      c = -c;
      last_time = current_time;
      // while (ros::Time::now().toSec() - current_time.toSec() < 0.0001){
      //   pub.publish(vel_msg);
      //   ros::spinOnce();
      // } //pub for 100us
    }
    vel_msg.linear.x = 10*c;
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;
    vel_msg.angular.z = 0;
    pub.publish(vel_msg);
    ros::spinOnce();
    pub_rate.sleep();
  }
}
