#ifndef HiTechnic_Motor_Driver_H
#define HiTechnic_Motor_Driver_H

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_HITECHNIC_MOTOR_DRIVER 0x01
#define DEFAULT_COUNT_PER_ROT 1440

enum HiTechnic_Motor_Driver_Mode
{
	Mode_Power_Only = 0b00000000,
	Mode_Constant_Speed = 0b00000001,
	Mode_Position = 0b00000010,
	Mode_Reset_Encorder = 0b00000011
};


class HiTechnic_Motor_Driver
{
public:
	HiTechnic_Motor_Driver(int &fd, unsigned char addr = DEFAULT_HITECHNIC_MOTOR_DRIVER);
	~HiTechnic_Motor_Driver();
	std::string getVersion();
	std::string getManufacturer();
	std::string getSensorType();
	void setMode(HiTechnic_Motor_Driver_Mode mode, int motor_num = 1);
	void setPower(int power, int motor_num = 1);
	void setPosition(double pos, int motor_num = 1, int count_per_rot = DEFAULT_COUNT_PER_ROT);
	void setSpeed(double speed, int motor_num = 1);
	double getEncorderValue(int motor_num = 1, int count_per_rot = DEFAULT_COUNT_PER_ROT);
	double getBatteryVoltage();
	int getGearRatio(int motor_num = 1);
	void setPCofficient(int coff, int motor_num = 1);
	void setICofficient(int coff, int motor_num = 1);
	void setDCofficient(int coff, int motor_num = 1);
	bool setSlave();
	bool read_data(unsigned char reg, int num, unsigned char* data);
	bool write_data(unsigned char reg, int num, unsigned char* data);

private:
	int *_fd;
	unsigned char slave_addr;
};



#endif
