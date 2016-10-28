#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream> 

#define _USE_MATH_DEFINES
#include <math.h>

#include "HiTechnic_Servo_Driver.h"

#define MAX_ANGLE (M_PI/2.0)
#define NEUTRAL_POSITION 120
#define MAX_PULSE_WIDTH 120



HiTechnic_Servo_Driver::HiTechnic_Servo_Driver(int &fd, unsigned char addr)
{

	_fd = &fd;
	slave_addr = addr;
}


HiTechnic_Servo_Driver::~HiTechnic_Servo_Driver()
{

}


std::string HiTechnic_Servo_Driver::getVersion()
{
	setSlave();


	unsigned char reg = 0x00;



	unsigned char data[8];
	read_data(reg, 8, data);

	return std::string((char*)data);
}


std::string HiTechnic_Servo_Driver::getManufacturer()
{
	setSlave();


	unsigned char reg = 0x08;



	unsigned char data[8];
	read_data(reg, 8, data);

	return std::string((char*)data);
}


std::string HiTechnic_Servo_Driver::getSensorType()
{
	setSlave();


	unsigned char reg = 0x10;



	unsigned char data[8];
	read_data(reg, 8, data);

	return std::string((char*)data);
}


void HiTechnic_Servo_Driver::setStepTime(double st)
{
	setSlave();

}



double HiTechnic_Servo_Driver::getStatus()
{
	setSlave();

	return 0;

}

void HiTechnic_Servo_Driver::setPWMEnable(bool enable)
{
	setSlave();

	unsigned char reg = 0x48;
	


	unsigned char data[1];

	if(enable)data[0] = 0x00;
	else data[0] = 0xff;

	write_data(reg, 1, data);
}

void HiTechnic_Servo_Driver::setPosition(double angle, int motor_num)
{
	setSlave();

	unsigned char reg = 0x42 + (motor_num-1);
	

	short p_angle = (angle / MAX_ANGLE * NEUTRAL_POSITION) + MAX_PULSE_WIDTH;

	unsigned char data[1] = {(unsigned char)p_angle};
	write_data(reg, 1, data);
}


bool HiTechnic_Servo_Driver::setSlave()
{
	if (ioctl((*_fd), I2C_SLAVE, slave_addr) < 0) {
                return false;
        }
	return true;
}


bool HiTechnic_Servo_Driver::read_data(unsigned char reg, int num, unsigned char* data)
{

	for(int i=0;i < num;i++)
	{
		unsigned char p_reg = reg + i;
		unsigned char buf[1] = {p_reg};
		write((*_fd), buf, 1);
		read((*_fd), buf, 1);
		data[i] = buf[0];

	}

	return true;
}
bool HiTechnic_Servo_Driver::write_data(unsigned char reg, int num, unsigned char* data)
{
	for(int i=0;i < num;i++)
	{
		unsigned char buf[2] = {reg, data[i]};
		write((*_fd), buf, 2);
	}

	return true;
}
