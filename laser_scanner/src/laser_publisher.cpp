#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"

#include <iostream>
#include <string>
#include <math.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

using namespace std;


int main(int argc, char **argv) {

  	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_device_handle *dev_handle; //a device handle
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values
	ssize_t cnt; //holding number of devices in list
	r = libusb_init(&ctx); //initialize the library for the session we just declared
	if(r < 0) {
		cout<<"Init Error "<<r<<endl; //there was an error
		return 1;
	}
	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation

	cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
	if(cnt < 0) {
		cout<<"Get Device Error"<<endl; //there was an error
		return 1;
	}
	cout<<cnt<<" Devices in list."<<endl;

	dev_handle = libusb_open_device_with_vid_pid(ctx, 0x19A2, 0x5001); //these are vendorID and productID I found for my usb device
	if(dev_handle == NULL)
		cout<<"Cannot open device"<<endl;
	else
		cout<<"Device Opened"<<endl;
	libusb_free_device_list(devs, 1); //free the list, unref the devices in it

	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
		cout<<"Kernel Driver Active"<<endl;
		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
			cout<<"Kernel Driver Detached!"<<endl;
	}
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) {
		cout<<"Cannot Claim Interface"<<endl;
		return 1;
	}
	cout<<"Claimed Interface"<<endl;


	//IMPORTANT
	unsigned char datastart[] = {   0x02, 0x73, 0x45, 0x49, 0x20, 0x38, 0x33, 0x20, 0x31, 0x03,
					0x02, 0x73, 0x52, 0x49, 0x20, 0x31, 0x39, 0x31, 0x03, 

					0x02, 0x73, 0x45, 0x49, 0x20, 0x37, 0x44, 0x20, 0x31, 0x03,
					0x02, 0x73, 0x52, 0x49, 0x20, 0x31, 0x36, 0x36, 0x03, 

					0x02, 0x73, 0x45, 0x49, 0x20, 0x35, 0x42, 0x20, 0x31, 0x03,
					0x02, 0x73, 0x52, 0x49, 0x20, 0x45, 0x37, 0x03   };

	int actual; //used to find out how many bytes were written or read

	cout<<"Datastart->"<<datastart[2]<<"<-"<<endl; //just to see the data we want to write
	cout<<"Writing Datastart..."<<endl;
	r = libusb_bulk_transfer(dev_handle, 0x02, datastart, 56, &actual, 0); //the device had 2 endpoints: 0x02-out and 0x81-in
	if(r == 0 && actual == 56) //we wrote the 56 bytes successfully
		cout<<"Writing Successful!"<<endl;
	else
		cout<<"Write Error"<<endl;

	//ROS
  	ros::init(argc, argv, "laser_publisher");
 	ros::NodeHandle n;
  	ros::Publisher laser_pub = n.advertise<sensor_msgs::LaserScan>("scan", 100);
  	ros::Rate loop_rate(15);

  	while (ros::ok()) {
	
		unsigned char* data_recv = new unsigned char[0x600];
		r = libusb_bulk_transfer(dev_handle, 0x81, data_recv, 0x600, &actual, 350);
		if (r==0) {
			cout << "Reading sucessful!" << endl;
			cout << data_recv << endl;
			cout << actual << endl;
		}
		else {
			cout << "Reading error" << endl;
			cout << "r= " << r << endl;
		}
		
		if (actual < 600) continue;				//CRITICAL LINE

		string data_str((char*)data_recv);
		delete[] data_recv;

		//tokenize the data_str and check if data is valid
		int count=0;
		size_t pos = 0;
		string token[0x600];
		int data[0x600];

		while ((pos = data_str.find(" "))!=string::npos) {
    			token[count] = data_str.substr(0, pos);
    			data_str.erase(0, pos + 1);
			count++;
		}
		token[count] = data_str;
		//convert if data is valid
		int i=0;
		while (token[i] != "DIST1") ++i;		
		if (i+6 <= count-270) {
			for (int j=i+6; j<=i+276; ++j)
				data[j-i-6] = stoul(token[j], nullptr, 16);
			ROS_INFO("HELLO");
			cout << "---OK---" <<endl;

			//ROS
			sensor_msgs::LaserScan msg;
    			for (int i=0; i<=270; ++i) {
				if (data[i] > 0) msg.ranges.push_back(float(data[i])/1000);
				else msg.ranges.push_back(float(4.0));
			}
			
			ros::Time scan_time = ros::Time::now();

			msg.header.frame_id = "laser_link";
			msg.header.stamp = scan_time;
			msg.angle_min = -M_PI / 4;
			msg.angle_max = 5*M_PI / 4;
			msg.angle_increment = M_PI / 180;
			msg.scan_time = float(1) / 15;
			msg.time_increment = (float(1)/15) / 271;
			msg.range_min = 0.05;
			msg.range_max = 5;

    			ROS_INFO("MSG");
			for (int i=0; i<=270; ++i) cout << msg.ranges[i] << " ";
			cout << endl << endl << endl;
    			laser_pub.publish(msg);
    			ros::spinOnce();
    			loop_rate.sleep();
		}
	}

	r = libusb_release_interface(dev_handle, 0); //release the claimed interface
	if(r!=0) {
		cout<<"Cannot Release Interface"<<endl;
		return 1;
	}
	cout<<"Released Interface"<<endl;

	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the session

  	return 0;
}
