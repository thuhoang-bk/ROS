# include "ros/ros.h"
# include "geometry_msgs/Twist.h"
# include "turtlesim/Pose.h"

//using namespace std;

ros::Publisher velocity_publisher;
ros::Subscriber pose_subscriber;
turtlesim::Pose turtlesim_pose;     //Pose messages

const double PI = 3.14156265359;

void move(double speed, double distance, bool isForward);
void rotate(double angular_speed, double relative_angle, bool clockwise);
double deg2rad(double angle_deg);
double set_orientation(double angle_rad);
void pose_callback(const turtlesim::Pose::ConstPtr & pose_message);
double get_distance(double x1, double y1, double x2, double y2);
void move_goal(turtlesim::Pose goal_pose, double distance_err);
void grid_move();
void spiral_move();

//MAIN
int main(int argc, char **argv){
  ros::init(argc, argv, "turtlesim_cleaner_node");
  ros::NodeHandle nh;
  velocity_publisher = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
  pose_subscriber = nh.subscribe("/turtle1/pose", 10, pose_callback);

  double speed, distance;
  bool isForward;
  double angular_speed, angle;
  bool clockwise;

  /* std::cout << "---START TESTING---" << std::endl;
  std::cout << "Speed (cm/s): ";
  std::cin >> speed;
  std::cout << "Distance (cm): ";
  std::cin >> distance;
  std::cout << "isForward? ";
  std::cin >> isForward;
  move(speed, distance, isForward);

  std::cout << "Angular velocity (deg/s): ";
  std::cin >> angular_speed;
  std::cout << "Desired angle (deg): ";
  std::cin >> angle;
  std::cout << "isClockwise? ";
  std::cin >> clockwise;
  rotate(deg2rad(angular_speed), deg2rad(angle), clockwise); */

  /*set_orientation(deg2rad(120));
  ros::Rate loop_rate(0.5);
  loop_rate.sleep();
  set_orientation(deg2rad(-60));
  loop_rate.sleep();
  set_orientation(deg2rad(0));*/

  ros::Rate loop_rate(1);
  // turtlesim::Pose goal_pose;
  // goal_pose.x = 10;
  // goal_pose.y = 10;
  // goal_pose.theta = 0;  //radian
  // move_goal(goal_pose, 0.01);
  // loop_rate.sleep();

  spiral_move();

  ros::spin();
  return 0;
}

//MOVE_STRAIGHT
void move(double speed, double distance, bool isForward){
  geometry_msgs::Twist vel_msg;
  if (isForward)
    vel_msg.linear.x = abs(speed);
  else
    vel_msg.linear.x = -abs(speed);
  vel_msg.linear.y = 0;
  vel_msg.linear.z = 0;
  vel_msg.angular.x = 0;
  vel_msg.angular.y = 0;
  vel_msg.angular.z = 0;

  double t0 = ros::Time::now().toSec();
  double t1;
  ros::Rate loop_rate(100); //loop-messages per sec - higher = more precise

  do {
    velocity_publisher.publish(vel_msg);
    t1 = ros::Time::now().toSec();
    ros::spinOnce();
    loop_rate.sleep();
  } while (t1-t0 < distance/speed);

  vel_msg.linear.x = 0;
  velocity_publisher.publish(vel_msg);
}

//ROTATE
void rotate(double angular_speed, double relative_angle, bool clockwise){
  geometry_msgs::Twist vel_msg;
  vel_msg.linear.x = 0;
  vel_msg.linear.y = 0;
  vel_msg.linear.z = 0;
  vel_msg.angular.x = 0;
  vel_msg.angular.y = 0;
  if (clockwise)
    vel_msg.angular.z = -abs(angular_speed);
  else
    vel_msg.angular.z = abs(angular_speed);

  double t0 = ros::Time::now().toSec();
  double current_angle = 0.0;
  ros::Rate loop_rate(100); //loop/messages per sec - 100 msg/s

  do {
    velocity_publisher.publish(vel_msg);
    double t1 = ros::Time::now().toSec();
    current_angle = angular_speed*(t1 - t0);
    ros::spinOnce();
    loop_rate.sleep();
  } while (current_angle < relative_angle);

  vel_msg.angular.z = 0;
  velocity_publisher.publish(vel_msg);
}

// SET_ORIENTATION
double deg2rad(double angle_deg){
  return (angle_deg/180) * PI;
}

double set_orientation(double angle_rad){
  double relative_angle = angle_rad - turtlesim_pose.theta;
  bool clockwise = ((relative_angle < 0)?true:false);
  std::cout << angle_rad << ", " << turtlesim_pose.theta << ", " << relative_angle << ", " << clockwise;
  rotate(abs(relative_angle), abs(relative_angle), clockwise);
}

// SUB_CALLBACK
void pose_callback(const turtlesim::Pose::ConstPtr & pose_message){
  turtlesim_pose.x = pose_message->x;
  turtlesim_pose.y = pose_message->y;
  turtlesim_pose.theta = pose_message->theta;
}

// GO_TO_GOAL
double get_distance(double x1, double y1, double x2, double y2){
  return sqrt(pow(x1 - x2,2) + pow(y1 - y2,2));
}

void move_goal(turtlesim::Pose goal_pose, double distance_err){
  geometry_msgs::Twist vel_msg;
  ros::Rate loop_rate(10);
  do {
    vel_msg.linear.x = 1.5 * get_distance(turtlesim_pose.x, turtlesim_pose.y,
      goal_pose.x, goal_pose.y);
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;
    vel_msg.angular.z = 4*(atan2(goal_pose.y - turtlesim_pose.y,
      goal_pose.x - turtlesim_pose.x) - turtlesim_pose.theta);
    velocity_publisher.publish(vel_msg);

    ros::spinOnce();
    loop_rate.sleep();
  } while (get_distance(turtlesim_pose.x, turtlesim_pose.y,
    goal_pose.x, goal_pose.y) > distance_err);

  std::cout << "end move goal" << std::endl;
  vel_msg.linear.x = 0;
  vel_msg.angular.z = 0;
  velocity_publisher.publish(vel_msg);
}

//GRID_MOVE
void grid_move(){
  ros::Rate loop_rate(0.5);
  turtlesim::Pose goal_pose;
  goal_pose.x = 1;
  goal_pose.y = 1;
  goal_pose.theta = 0;  //radian
  move_goal(goal_pose, 0.01);
  loop_rate.sleep();
  set_orientation(0);
  loop_rate.sleep();

  move(2, 9, true);
  loop_rate.sleep();
  rotate(deg2rad(30), deg2rad(90), false);
  loop_rate.sleep();
  move(2, 9, true);

  rotate(deg2rad(30), deg2rad(90), false);
  loop_rate.sleep();
  move(2, 1, true);
  rotate(deg2rad(30), deg2rad(90), false);
  loop_rate.sleep();
  move(2, 9, true);

  rotate(deg2rad(30), deg2rad(90), true);
  loop_rate.sleep();
  move(2, 1, true);
  rotate(deg2rad(30), deg2rad(90), true);
  loop_rate.sleep();
  move(2, 9, true) ;
}

void spiral_move(){
  geometry_msgs::Twist vel_msg;
  double count = 0;

  double constant_speed = 4;
  double vk = 1;
  double wk = 2;
  double rk = 0.5;
  ros::Rate loop_rate(1);

  do {
    rk += 1.5;
    vel_msg.linear.x = rk;
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;
    vel_msg.angular.z = constant_speed;

    std::cout<<"vel_msg.linear.x = "<<vel_msg.linear.x<<std::endl;
    std::cout<<"vel_msg.angular.z = "<<vel_msg.angular.z<<std::endl;
    velocity_publisher.publish(vel_msg);
    ros::spinOnce();

    loop_rate.sleep();
    //vk = vel_msg.linear.x;
    //wk = vel_msg.angular.z;
    //rk = vk/wk;
    std::cout << rk <<", "<< vk <<", "<< wk << std::endl;

  } while ((turtlesim_pose.x<10.5) && (turtlesim_pose.y<10.5));
  vel_msg.linear.x = 0;
  velocity_publisher.publish(vel_msg);
}
