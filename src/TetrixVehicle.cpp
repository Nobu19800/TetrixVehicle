// -*- C++ -*-
/*!
 * @file  TetrixVehicle.cpp
 * @brief Tetrix Vehicle
 * @date $Date$
 *
 * @author 宮本　信彦　n-miyamoto@aist.go.jp
 * 産業技術総合研究所　ロボットイノベーション研究センター
 * ロボットソフトウエアプラットフォーム研究チーム
 *
 * $Id$
 */

#include "TetrixVehicle.h"

// Module specification
// <rtc-template block="module_spec">
static const char* tetrixvehicle_spec[] =
  {
    "implementation_id", "TetrixVehicle",
    "type_name",         "TetrixVehicle",
    "description",       "Tetrix Vehicle",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "Controller",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.wheelRadius", "0.08",
    "conf.default.wheelDistance", "0.34",
    "conf.default.portNum", "1",
    "conf.default.rot_dir_left_motor", "1",
    "conf.default.rot_dir_right_motor", "-1",
    "conf.default.GearRatio", "3.0",

    // Widget
    "conf.__widget__.wheelRadius", "text",
    "conf.__widget__.wheelDistance", "text",
    "conf.__widget__.portNum", "radio",
    "conf.__widget__.rot_dir_left_motor", "radio",
    "conf.__widget__.rot_dir_right_motor", "radio",
    "conf.__widget__.GearRatio", "text",
    // Constraints
    "conf.__constraints__.portNum", "(1,2,3,4)",
    "conf.__constraints__.rot_dir_left_motor", "(-1, 1)",
    "conf.__constraints__.rot_dir_right_motor", "(-1,1)",

    "conf.__type__.wheelRadius", "double",
    "conf.__type__.wheelDistance", "double",
    "conf.__type__.portNum", "int",
    "conf.__type__.rot_dir_left_motor", "int",
    "conf.__type__.rot_dir_right_motor", "int",
    "conf.__type__.GearRatio", "double",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
TetrixVehicle::TetrixVehicle(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_target_velocityIn("target_velocity", m_target_velocity),
    m_update_positionIn("update_position", m_update_position),
    m_positionOut("position", m_position)

    // </rtc-template>
{
	m_driver_obj = NULL;
}

/*!
 * @brief destructor
 */
TetrixVehicle::~TetrixVehicle()
{
}



RTC::ReturnCode_t TetrixVehicle::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("target_velocity", m_target_velocityIn);
  addInPort("update_position", m_update_positionIn);
  // Set OutPort buffer
  addOutPort("position", m_positionOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("wheelRadius", m_wheelRadius, "0.08");
  bindParameter("wheelDistance", m_wheelDistance, "0.34");
  bindParameter("portNum", m_portNum, "1");
  bindParameter("rot_dir_left_motor", m_rot_dir_left_motor, "1");
  bindParameter("rot_dir_right_motor", m_rot_dir_right_motor, "-1");
  bindParameter("GearRatio", m_GearRatio, "3.0");
  // </rtc-template>
  
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onFinalize()
{
	if(m_driver_obj)
	{
		delete m_driver_obj;
		close(m_fd);
	}
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onActivated(RTC::UniqueId ec_id)
{
	if(m_driver_obj == NULL)
	{
		std::string i2c_dev_fn = "/dev/i2c-" + coil::otos<int>(m_portNum+2);
		if ((m_fd = open(i2c_dev_fn.c_str(), O_RDWR)) < 0) {
			return RTC::RTC_ERROR;
		}
		m_driver_obj = new HiTechnic_Motor_Driver(m_fd);
		m_driver_obj->setSlave();

	}

	//m_driver_obj->setMode(Mode_Constant_Speed, 1);
	//m_driver_obj->setMode(Mode_Constant_Speed, 2);

	m_driver_obj->setMode(Mode_Power_Only, 1);
	m_driver_obj->setMode(Mode_Power_Only, 2);

	last_time = coil::gettimeofday();
	m_target_velocity.data.vx = 0;
	m_target_velocity.data.va = 0;

	current_x = 0;
	current_y = 0;
	current_a = 0;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onExecute(RTC::UniqueId ec_id)
{
	if(m_target_velocityIn.isNew())
	{
		m_target_velocityIn.read();
		float r = m_wheelRadius / 2.0;
		float d = m_wheelDistance / 2.0;
	
		float vx = m_target_velocity.data.vx;
		float va = m_target_velocity.data.va;

		float right_motor_speed = (vx + va*d) / r * m_GearRatio;
		float left_motor_speed = (vx - va*d) / r * m_GearRatio;

		if (m_rot_dir_right_motor==-1) right_motor_speed = -right_motor_speed;
		if (m_rot_dir_left_motor == -1)left_motor_speed = -left_motor_speed;

		//std::cout << right_motor_speed << "\t" << left_motor_speed << std::endl;

		m_driver_obj->setSpeed(right_motor_speed,1);
		m_driver_obj->setSpeed(left_motor_speed,2);
	}

	if(m_update_positionIn.isNew())
	{
		m_update_positionIn.read();
		current_x = m_update_position.data.position.x;
		current_y = m_update_position.data.position.y;
		current_a = m_update_position.data.heading;
	}

	coil::TimeValue current_time = coil::gettimeofday();
	double diff_time = (double)(current_time - last_time);

	if(diff_time <= 0.5)
	{
		current_x += m_target_velocity.data.vx*diff_time;
		current_a += m_target_velocity.data.va*diff_time;
	}

	m_position.data.position.x = current_x;
	m_position.data.position.y = current_y;
	m_position.data.heading = current_a;
	setTimestamp(m_position);
	m_positionOut.write();

	last_time = current_time;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t TetrixVehicle::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}



extern "C"
{
 
  void TetrixVehicleInit(RTC::Manager* manager)
  {
    coil::Properties profile(tetrixvehicle_spec);
    manager->registerFactory(profile,
                             RTC::Create<TetrixVehicle>,
                             RTC::Delete<TetrixVehicle>);
  }
  
};


