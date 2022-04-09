---turtlesim_cleaner---
  "turtle sim: 1 turtle follow programmed trajectory"

  roslaunch turtlesim_cleaner cleaner.launch

---laser_scanner---
  "sick tim 320, publish laser_scan messages"

  roscore
  rosrun laser_scanner laser_publisher

---learning_tf---
  "2 turtle in turtlesim, turtle 2 follow turtle1"

  roslaunch learning_tf start_demo.launch

---my_urdf---
  "Spawn a blue-rectangular box to rviz"

  roslaunch my_urdf rviz_spawn.launch

---gm25_cmd_vel---
  "A node publish to /cmd_vel topic"

  roscore
  rosrun gm25_cmd_vel cmd_vel_node

---gm25_odom---
  "Publish odometry from gm25 motors, connect to arduino and power 2 motors first"

  roslaunch gm25_odom gm25_odom.launch
