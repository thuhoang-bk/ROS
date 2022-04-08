#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>

double x=0, y=0, th=0, vx=0, vy=0, vth=0;

void state_callback(const geometry_msgs::Twist::ConstPtr& state_msg) {
  x = state_msg->linear.x;
  y = state_msg->linear.y;
  th = state_msg->linear.z;

  vx = state_msg->angular.x;
  vy = 0;
  vth = state_msg->angular.y;
}

int main(int argc, char** argv){
  ros::init(argc, argv, "odometry_publisher");

  ros::NodeHandle n;
  ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 100);
  ros::Subscriber state_sub = n.subscribe("robot_state", 1000, state_callback);
  tf::TransformBroadcaster odom_broadcaster;

  ros::Time current_time;

  ros::Rate r(100.0);
  while(n.ok()){

    ros::spinOnce();               // check for incoming messages

    current_time = ros::Time::now();

    //---first, we'll publish the transform over tf
    //since all odometry is 6DOF we'll need a quaternion created from yaw
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = current_time;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = x;
    odom_trans.transform.translation.y = y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = odom_quat;

    //send the transform
    odom_broadcaster.sendTransform(odom_trans);

    //---next, we'll publish the odometry message over ROS
    nav_msgs::Odometry odom;
    odom.header.stamp = current_time;
    odom.header.frame_id = "odom";

    //set the position
    odom.pose.pose.position.x = x;
    odom.pose.pose.position.y = y;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = odom_quat;

    //set the velocity
    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = vx;
    odom.twist.twist.linear.y = 0;
    odom.twist.twist.angular.z = vth;

    //publish the message
    odom_pub.publish(odom);

    r.sleep();
  }
}
