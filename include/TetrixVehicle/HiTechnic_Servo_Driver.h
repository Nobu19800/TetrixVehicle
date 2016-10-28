#ifndef HiTechnic_Servo_Driver_H
#define HiTechnic_Servo_Driver_H


#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_HITECHNIC_SERVO_DRIVER 0x01




class HiTechnic_Servo_Driver
{
public:
	HiTechnic_Servo_Driver(int &fd, unsigned char addr = DEFAULT_HITECHNIC_SERVO_DRIVER);
	~HiTechnic_Servo_Driver();
	std::string getVersion();
	std::string getManufacturer();
	std::string getSensorType();
	void setStepTime(double st);
	double getStatus();
	void setPWMEnable(bool enable);
	void setPosition(double angle, int motor_num);
	
	bool setSlave();
	bool read_data(unsigned char reg, int num, unsigned char* data);
	bool write_data(unsigned char reg, int num, unsigned char* data);

private:
	int *_fd;
	unsigned char slave_addr;
};


#endif
