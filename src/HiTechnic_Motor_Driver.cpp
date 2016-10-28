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

#include "HiTechnic_Motor_Driver.h"



HiTechnic_Motor_Driver::HiTechnic_Motor_Driver(int &fd, unsigned char addr)
{

	_fd = &fd;
	slave_addr = addr;

	


	

	
}


HiTechnic_Motor_Driver::~HiTechnic_Motor_Driver()
{

}


std::string HiTechnic_Motor_Driver::getVersion()
{
	setSlave();


	unsigned char reg = 0x00;



	unsigned char data[8];
	read_data(reg, 8, data);

	return std::string((char*)data);
}


std::string HiTechnic_Motor_Driver::getManufacturer()
{
	setSlave();


	unsigned char reg = 0x08;



	unsigned char data[8];
	read_data(reg, 8, data);

	return std::string((char*)data);
}


std::string HiTechnic_Motor_Driver::getSensorType()
{
	setSlave();


	unsigned char reg = 0x10;



	unsigned char data[8];
	read_data(reg, 8, data);

	return std::string((char*)data);
}


void HiTechnic_Motor_Driver::setMode(HiTechnic_Motor_Driver_Mode mode, int motor_num)
{
	setSlave();

	unsigned char reg;
	if(motor_num == 1)reg = 0x44;
	else reg = 0x47;

	unsigned char data[1] = {mode};
	write_data(reg, 1, data);
}

void HiTechnic_Motor_Driver::setPower(int power, int motor_num)
{
	setSlave();


	unsigned char reg;
	if(motor_num == 1)reg = 0x45;
	else reg = 0x46;

	//if(power < 0)power += 256;

	unsigned char data[1] = {(unsigned char)power};
	write_data(reg, 1, data);
}

void HiTechnic_Motor_Driver::setPosition(double pos, int motor_num, int count_per_rot)
{
	int target = (pos / (2.0 * M_PI)) * count_per_rot;

	unsigned char reg;
	if(motor_num == 1)reg = 0x40;
	else reg = 0x48;

	//if(power < 0)power += 256;

	unsigned char data[4];
	data[0] = target;
	data[1] = target >> 8;
	data[2] = target >> 16;
	data[3] = target >> 24;
	write_data(reg, 4, data);
}

void HiTechnic_Motor_Driver::setSpeed(double speed, int motor_num)
{
	int target = (speed*180.0/M_PI) / 10.0;
	if(target > 127)target = 127;
	else if(target < -127)target = -127;
	//std::cout << target << std::endl;
	setPower(target, motor_num);
}

double HiTechnic_Motor_Driver::getEncorderValue(int motor_num, int count_per_rot)
{
	setSlave();



	unsigned char reg;
	if(motor_num == 1)reg = 0x4c;
	else reg = 0x50;



	unsigned char data[4];
	read_data(reg, 4, data);
	
	int angle = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
	return (double)angle * (2 * M_PI) / (double)count_per_rot;
}

double HiTechnic_Motor_Driver::getBatteryVoltage()
{
	setSlave();



	unsigned char reg = 0x54;



	unsigned char data[2];
	read_data(reg, 2, data);
	
	short vol = ((data[0] << 8) | data[1]);
	return (double)vol;
}

int HiTechnic_Motor_Driver::getGearRatio(int motor_num)
{
	setSlave();

	unsigned char reg;
	if(motor_num == 1)reg = 0x59;
	else reg = 0x5d;

	unsigned char data[1];
	read_data(reg, 1, data);
	return data[0];
}

void HiTechnic_Motor_Driver::setPCofficient(int coff, int motor_num)
{
	setSlave();

	unsigned char reg;
	if(motor_num == 1)reg = 0x57;
	else reg = 0x5b;

	unsigned char data[1] = {(unsigned char)coff};
	write_data(reg, 1, data);
}

void HiTechnic_Motor_Driver::setICofficient(int coff, int motor_num)
{
	setSlave();

	unsigned char reg;
	if(motor_num == 1)reg = 0x58;
	else reg = 0x5c;

	unsigned char data[1] = {(unsigned char)coff};
	write_data(reg, 1, data);
}

void HiTechnic_Motor_Driver::setDCofficient(int coff, int motor_num)
{
	setSlave();

	unsigned char reg;
	if(motor_num == 1)reg = 0x59;
	else reg = 0x5d;

	unsigned char data[1] = {(unsigned char)coff};
	write_data(reg, 1, data);
}


bool HiTechnic_Motor_Driver::setSlave()
{
	if (ioctl((*_fd), I2C_SLAVE, slave_addr) < 0) {
                return false;
        }
	return true;
}


bool HiTechnic_Motor_Driver::read_data(unsigned char reg, int num, unsigned char* data)
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
bool HiTechnic_Motor_Driver::write_data(unsigned char reg, int num, unsigned char* data)
{
	for(int i=0;i < num;i++)
	{
		unsigned char buf[2] = {reg, data[i]};
		write((*_fd), buf, 2);
	}
	return true;
}
